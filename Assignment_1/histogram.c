#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

// double Trap(double left_endpt, double right_endpt, int trap_count, double base_len);
// double f(double x);
void Read(double *a, double *b, int *n, int *data_count, int my_rank, int comm_sz, MPI_Comm comm);

int main(void)
{
    // Removed the value of n
    int my_rank, comm_sz, n;
    // Removed the value of a and b
    double a, b;
    // Adding variable data_count to represent the number of measurements
    int data_count, local_data;
    // Array to store all generated numbers
    double *local_measurements = NULL;
    /**
     * Unused variables
     */
    // double bin_range, *local_bin_count, total_bin_count, h, local_a, local_b;
    // double local_int, total_int;
    // int local_n, source

    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    Read(&a, &b, &n, &data_count, my_rank, comm_sz, comm);
    // Print_numbers(local_measurements, local_n, n, "\nGenerated numbers: ", my_rank, comm);

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
    // MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Allocating memory for array of randonly generated numbers

    if (my_rank == 0)
    {

        local_measurements = (double *)malloc(data_count * sizeof(double));

        // srand(my_rank);
        for (int i = 0; i < data_count; i++)
        {
            local_measurements[i] = ((double)rand() / RAND_MAX) * (a - b) + b;
        }

        // for (int i = 0; i < local_data; i++)
        // {
        //     int bin_index = sort(local_measurements[i], bin_range, a, n);
        //     local_bin_count[bin_index]++; // ----------------------------------------
        // }
        // Printing the generated numbers
        printf("\nGenerated numbers:\n");
        for (int i = 0; i < data_count; i++)
        {
            printf("%.2f ", local_measurements[i]);
        }
        printf("\n");
    }

    local_data = data_count / comm_sz;
    double *p = (double *)malloc(local_data * sizeof(double));

    // Scatter the generated numbers to all processes
    MPI_Scatter(local_measurements, local_data, MPI_DOUBLE, p, local_data, MPI_DOUBLE, 0, comm);

    double bin_range = (double)(a - b) / n;
    int *bin_counts = (int *)calloc(n, sizeof(int));

    double *bin_ranges = (double *)malloc((n + 1) * sizeof(double));

    // local_bin_count = (int *)calloc(n, sizeof(int));
    //-----------------------------------------------------------

    for (int i = 0; i <= n; i++)
    {
        bin_ranges[i] = b + i * bin_range;
    }

    // for (int i = 0; i < local_data; i++)
    // {
    //     total_bin_counts = malloc(n * sizeof(int));
    // }
    // Assign measurements to bins and count them
    for (int i = 0; i < local_data; i++)
    {
        int bin_index = (int)((p[i] - b) / bin_range);

        if (bin_index < 0)
        {
            bin_index = 0;
        }
        else if (bin_index >= n)
        {
            bin_index = n - 1;
        }
        bin_counts[bin_index]++;
    }
    // MPI_Gather(local_bin_count, n, MPI_INT, total_bin_counts, n, MPI_INT, 0, MPI_COMM_WORLD);
    int *total_bin_counts = NULL;

    if (my_rank == 0)
    {
        total_bin_counts = (int *)malloc(n * comm_sz * sizeof(int));
    }
    // double bin_max, bin_min;
    MPI_Reduce(bin_counts, total_bin_counts, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Printing the bins and their bounds
    if (my_rank == 0)
    {

        printf("\nBin Ranges:\n");
        for (int i = 0; i < n; i++)
        {
            printf("Bin %d: %.2f to %.2f\n", i, bin_ranges[i], bin_ranges[i + 1]);
        }
        // Print the gathered bin counts
        printf("\n");
        for (int i = 0; i < n; i++)
        {
            int total_count = 0;
            for (int j = 0; j < comm_sz; j++)
            {
                total_count += total_bin_counts[j * n + i];
            }
            printf("In Bin %d there are: %d elements\n", i, total_count);
        }
        printf("\n");

        // Free all_count memory
        free(total_bin_counts);
    }
    // for (int i = 0; i < n; i++)
    // {
    //     if (i == 0)
    //     {
    //         bin_max = a + bin_range;
    //         printf("Bin: %d | %.2lf - %.2lf | \n", i, a, bin_max);
    //     }
    //     else
    //     {
    //         bin_min = bin_max;
    //         bin_max = bin_min + bin_range;
    //         printf("Bin: %d | %.2lf - %.2lf | \n", i, bin_min, bin_max);
    //     }
    // }

    free(local_measurements);
    free(p);
    free(bin_counts);
    MPI_Finalize();
    return 0;
}

// int sort(double num, double bin_range, double a, int n)
// {
//     for (int i = 0; i < n; i++)
//     {
//         double bin_min = a + i * bin_range;
//         double bin_max = bin_min + bin_range;
//         if (num >= bin_min && num < bin_max)
//         {
//             return i;
//         }
//     }
//     // If the number is outside the specified range, return -1 (an invalid bin index)
//     return -1;
// }

/**
 * Part 1: Revise the program such that users can have their input used to calculate the defnite integral with the trapezoidal rule
 */
/**
 * Part 3: Changed message prompts to include number of bins and number of measurements
 */
void Read(double *a, double *b, int *n, int *data_count, int my_rank, int comm_sz, MPI_Comm comm)
{
    if (my_rank == 0)
    {
        printf("Enter lower bound of the measurements: \n");
        scanf("%lf", b);

        printf("Enter upper bound of the measurements: \n");
        scanf("%lf", a);

        printf("Enter number of number of bins: \n");
        scanf("%d", n);

        printf("Enter number of measurements: \n");
        scanf("%d", data_count);

        //
    }

    // Print_numbers(local_measurements, local_n, n, "\nGenerated Numbers: ", my_rank, comm);
    MPI_Bcast(a, 1, MPI_DOUBLE, 0, comm);
    MPI_Bcast(b, 1, MPI_DOUBLE, 0, comm);
    MPI_Bcast(n, 1, MPI_INT, 0, comm);
    MPI_Bcast(data_count, 1, MPI_INT, 0, comm);
}

/**
 * ununecessary functions
*/
// double Trap(
//     double left_endpt,
//     double right_endpt,
//     int trap_count,
//     double base_len)
// {
//     double estimate, x;
//     int i;

//     estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
//     for (i = 1; i <= trap_count - 1; i++)
//     {
//         x = left_endpt + i * base_len;
//         estimate += f(x);
//     }

//     estimate = estimate * base_len;

//     return estimate;
// }

// double f(double x)
// {
//     return x * x;
// }