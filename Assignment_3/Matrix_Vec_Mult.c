#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/**
 * Implement matrix-vector multiplication using MPI. You can have process 0 read the matrix A and the vector x
 * Assume the matrix is square of order n and that n is evenly divisible by comm_sz.
 * INPUT: n – the dimension for A and x. (You can use a 𝑛2 vector to denote the 𝑛 × 𝑛 matrix)
 * OUTPUT: The result vector 𝑦 = 𝑨𝑥.
 */

void Read_n(int *m_p, int *local_m_p, int *n_p, int *local_n_p, int my_rank, int comm_sz, MPI_Comm comm);
void Read_matrix(double local_A[], int m, int local_m, int n, int my_rank, MPI_Comm comm);
void Read_vector(double local_vec[], int n, int local_n, int my_rank, MPI_Comm comm);
void Print_matrix(double local_A[], int m, int local_m, int n, int my_rank, MPI_Comm comm);
void Print_vector(double local_vec[], int n, int local_n, char title[], int my_rank, MPI_Comm comm);
void Mat_vect_mult(double local_A[], double local_x[], double local_y[], int local_m, int n, int local_n, MPI_Comm comm);

int main(void)
{
    double *local_A;
    double *local_x;
    double *local_y;
    int m, local_m, n, local_n;
    int my_rank, comm_sz;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    Read_n(&m, &local_m, &n, &local_n, my_rank, comm_sz, comm);

    local_A = malloc(local_m * n * sizeof(double));
    local_x = malloc(local_n * sizeof(double));
    local_y = malloc(local_m * sizeof(double));

    Read_matrix(local_A, m, local_m, n, my_rank, comm);
    Print_matrix(local_A, m, local_m, n, my_rank, comm);

    Read_vector(local_x, n, local_n, my_rank, comm);
    Print_vector(local_x, n, local_n, "\nEntered Vector:", my_rank, comm);

    Mat_vect_mult(local_A, local_x, local_y, local_m, n, local_n, comm);
    Print_vector(local_y, m, local_m, "\nProduct:", my_rank, comm);

    free(local_A);
    free(local_x);
    free(local_y);

    MPI_Finalize();
    return 0;
}

void Read_n(int *m_p, int *local_m_p, int *n_p, int *local_n_p, int my_rank, int comm_sz, MPI_Comm comm)
{
    if (my_rank == 0)
    {
        printf("Enter the number of rows/columns\n");
        int temp;
        scanf("%d", &temp);
        *m_p = *n_p = temp;
    }
    MPI_Bcast(m_p, 1, MPI_INT, 0, comm);
    MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

    *local_m_p = *m_p / comm_sz;
    *local_n_p = *n_p / comm_sz;
}

void Read_matrix(double local_A[], int m, int local_m, int n, int my_rank, MPI_Comm comm)
{
    double *A = NULL;
    int i, j;

    if (my_rank == 0)
    {
        A = malloc(m * n * sizeof(double));

        printf("Enter the matrix \n");
        for (i = 0; i < m; i++)
        {
            for (j = 0; j < n; j++)
            {
                scanf("%lf", &A[i * n + j]);
            }
        }
        MPI_Scatter(A, local_m * n, MPI_DOUBLE, local_A, local_m * n, MPI_DOUBLE, 0, comm);
        free(A);
    }
    else
    {
        MPI_Scatter(A, local_m * n, MPI_DOUBLE, local_A, local_m * n, MPI_DOUBLE, 0, comm);
    }
}

void Read_vector(double local_vec[], int n, int local_n, int my_rank, MPI_Comm comm)
{
    double *vec = NULL;
    int i;

    if (my_rank == 0)
    {
        vec = malloc(n * sizeof(double));

        printf("Enter the vector \n");
        for (i = 0; i < n; i++)
        {
            scanf("%lf", &vec[i]);
        }

        MPI_Scatter(vec, local_n, MPI_DOUBLE, local_vec, local_n, MPI_DOUBLE, 0, comm);
        free(vec);
    }
    else
    {
        MPI_Scatter(vec, local_n, MPI_DOUBLE, local_vec, local_n, MPI_DOUBLE, 0, comm);
    }
}

void Print_matrix(double local_A[], int m, int local_m, int n, int my_rank, MPI_Comm comm)
{
    double *A = NULL;
    int i, j;

    if (my_rank == 0)
    {
        A = malloc(m * n * sizeof(double));

        MPI_Gather(local_A, local_m * n, MPI_DOUBLE, A, local_m * n, MPI_DOUBLE, 0, comm);
        printf("\nThe matrix \n");
        for (i = 0; i < m; i++)
        {
            for (j = 0; j < n; j++)
            {
                printf("%.0lf ", A[i * n + j]);
            }
            printf("\n");
        }
        printf("\n");
        free(A);
    }
    else
    {
        MPI_Gather(local_A, local_m * n, MPI_DOUBLE, A, local_m * n, MPI_DOUBLE, 0, comm);
    }
}

void Print_vector(double local_vec[], int n, int local_n, char title[], int my_rank, MPI_Comm comm)
{
    double *vec = NULL;
    int i;

    if (my_rank == 0)
    {
        vec = malloc(n * sizeof(double));

        MPI_Gather(local_vec, local_n, MPI_DOUBLE, vec, local_n, MPI_DOUBLE, 0, comm);

        printf("%s\n", title);

        for (i = 0; i < n; i++)
        {
            printf("%.0lf ", vec[i]);
        }
        printf("\n");
        free(vec);
    }
    else
    {
        MPI_Gather(local_vec, local_n, MPI_DOUBLE, vec, local_n, MPI_DOUBLE, 0, comm);
    }
}

void Mat_vect_mult(double local_A[], double local_x[], double local_y[], int local_m, int n, int local_n, MPI_Comm comm)
{
    double *x;
    int local_i, j;

    x = malloc(n * sizeof(double));

    MPI_Allgather(local_x, local_n, MPI_DOUBLE, x, local_n, MPI_DOUBLE, comm);

    for (local_i = 0; local_i < local_m; local_i++)
    {
        local_y[local_i] = 0.0;
        for (j = 0; j < n; j++)
        {
            local_y[local_i] += local_A[local_i * n + j] * x[j];
        }
    }
    free(x);
}