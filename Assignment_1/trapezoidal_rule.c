#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

double Trap(double left_endpt, double right_endpt, int trap_count, double base_len);
double f(double x);
void Read(double *a, double *b, int *n, int my_rank, int comm_sz, MPI_Comm comm);

int main(void)
{
    // Removed the value of n
    int my_rank, comm_sz, n, local_n;
    // Removed the value of a and b
    double a, b, h, local_a, local_b;
    double local_int, total_int;
    int source;
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    Read(&a, &b, &n, my_rank, comm_sz, comm);

    /**
     * Part 2: Replaced the below collective communication with MPI_Reduce
    */
    // h = (b - a) / n;       // h is the same for all processes ∗/
    // local_n = n / comm_sz; // So is the number of trapezoids ∗/

    // local_a = a + my_rank * local_n * h;
    // local_b = local_a + local_n * h;
    // local_int = Trap(local_a, local_b, local_n, h);

    // if (my_rank != 0)
    // {
    //     MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0,
    //              MPI_COMM_WORLD);
    // }
    // else
    // {
    //     total_int = local_int;
    //     for (source = 1; source < comm_sz; source++)
    //     {
    //         MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //         total_int += local_int;
    //     }
    // }
     MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        printf("With n = %d trapezoids, our estimate\n", n);
        printf("of the integral from %.2lf to %.2lf = %.15e\n", a, b, total_int);
    }

    MPI_Finalize();
    return 0;
}

/**
 * Part 1: Revise the program such that users can have their input used to calculate the defnite integral with the trapezoidal rule 
*/
void Read(double *a, double *b, int *n, int my_rank, int comm_sz, MPI_Comm comm)
{
    if (my_rank == 0)
    {    
        printf("Enter lower bound: \n");
        scanf("%lf", a);

        printf("Enter upper bound: \n");
        scanf("%lf", b);

        printf("Enter number of subintervals (trapezoids): \n");
        scanf("%d", n);
    }

    MPI_Bcast(a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

double Trap(
    double left_endpt,
    double right_endpt,
    int trap_count,
    double base_len)
{
    double estimate, x;
    int i;

    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    for (i = 1; i <= trap_count - 1; i++)
    {
        x = left_endpt + i * base_len;
        estimate += f(x);
    }

    estimate = estimate * base_len;

    return estimate;
}

double f(double x)
{
    return x * x;
}