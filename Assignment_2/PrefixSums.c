#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

/**
 * Devise a serial algorithm for computing the n prefix sums of the array with n elements
 */
void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);
void Read_vector(double local_vec[], int local_n, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_vec[], int local_n, int n, char title[],
                  int my_rank, MPI_Comm comm);
void Prefix_Sum(double local_vec[], double local_sum[], int local_n, MPI_Comm comm);

int main()
{
    int n, local_n;
    double *local_vec1, *local_sum;
    int comm_sz, my_rank;
    MPI_Comm comm;
    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    Read_n(&n, &local_n, my_rank, comm_sz, comm);

    local_vec1 = (double *)malloc(local_n * sizeof(double));
    local_sum = (double *)malloc(local_n * sizeof(double));

    Read_vector(local_vec1, local_n, my_rank, comm_sz, comm);
    Print_vector(local_vec1, local_n, n, "\nEntered Vector:", my_rank, comm);

    Prefix_Sum(local_vec1, local_sum, local_n, comm);
    Print_vector(local_sum, local_n, n, "Sum:", my_rank, comm);

    MPI_Finalize();
    return 0;
}

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm)
{
    if (my_rank == 0)
    {
        printf("Enter vector size (n):\n");
        scanf("%d", n_p);
    }

    MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
    *local_n_p = *n_p / comm_sz;
}

void Read_vector(double local_vec[], int local_n, int my_rank, int comm_sz, MPI_Comm comm)
{
    // MPI_Datatype block_type;
    // MPI_Type_contiguous(1, MPI_DOUBLE, &block_type);
    // MPI_Type_commit(&block_type);

    double *a = NULL;

    if (my_rank == 0)
    {
        a = malloc(local_n * comm_sz * sizeof(double));
        printf("Enter the vector\n");

        for (int i = 0; i < local_n * comm_sz; i++)
            scanf("%lf", &a[i]);

        MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec, local_n, MPI_DOUBLE, 0, comm);
        //MPI_Scatter(a, 1, block_type, local_vec, local_n, MPI_DOUBLE, 0, comm);
        free(a);
    }
    else
    {
        MPI_Scatter(NULL, local_n, MPI_DOUBLE, local_vec, local_n, MPI_DOUBLE, 0, comm);
        // MPI_Scatter(NULL, 1, block_type, local_vec, local_n, MPI_DOUBLE, 0, comm);
    }

    //MPI_Type_free(&block_type);
}

void Print_vector(double local_vec[], int local_n, int n, char title[],
                  int my_rank, MPI_Comm comm)
{

    // MPI_Datatype block_type;
    // MPI_Type_contiguous(1, MPI_DOUBLE, &block_type);
    // MPI_Type_commit(&block_type);

    double *a = NULL;

    if (my_rank == 0)
    {
        a = malloc(n * sizeof(double));
        MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
        // MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, 1, block_type, 0, comm);

        printf("%s\n", title);
        for (int i = 0; i < n; i++)
            printf("%d ", (int)a[i]);
        printf("\n\n");
        free(a);
    }
    else
    {
        MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
        //MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, 1, block_type, 0, comm);
    }

    //MPI_Type_free(&block_type);
}

void Prefix_Sum(double local_vec1[], double local_sum[], int local_n, MPI_Comm comm)
{
    //double prefix_sum = 0.0;
    MPI_Scan(local_vec1, local_sum, local_n, MPI_DOUBLE, MPI_SUM, comm);

    // for (int i = 0; i < local_n; i++)
    // {
    //     local_sum[i] = prefix_sum + local_sum[i];
    //     prefix_sum = local_sum[i];
    // }
}