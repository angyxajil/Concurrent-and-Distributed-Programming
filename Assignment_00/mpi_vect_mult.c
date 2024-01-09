#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);
void Check_for_error(int local_ok, char fname[], char message[],
                     MPI_Comm comm);
void Read_data(double local_vec1[], double local_vec2[], double *scalar_p,
               int local_n, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_vec[], int local_n, int n, char title[],
                  int my_rank, MPI_Comm comm);
double Par_dot_product(double local_vec1[], double local_vec2[],
                       int local_n, MPI_Comm comm);
void Par_vector_scalar_mult(double local_vec[], double scalar,
                            double local_result[], int local_n);

int main(void)
{
      int n, local_n;
      double *local_vec1, *local_vec2;
      double scalar;
      double *local_scalar_mult1, *local_scalar_mult2;
      double dot_product;
      int comm_sz, my_rank;
      MPI_Comm comm;
      // Defines a communicator with all processes created at the beginning of the program
      MPI_Init(NULL, NULL);
      comm = MPI_COMM_WORLD;
      MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
      MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

      // Reading the value of n, vector size
      Read_n(&n, &local_n, my_rank, comm_sz, comm);

      // Allocating memory for arrays representing  vector and scalar multiples
      local_vec1 = (double *)malloc(local_n * sizeof(double));
      local_vec2 = (double *)malloc(local_n * sizeof(double));
      local_scalar_mult1 = (double *)malloc(local_n * sizeof(double));
      local_scalar_mult2 = (double *)malloc(local_n * sizeof(double));

      // Reading the entered values of the vectors
      Read_data(local_vec1, local_vec2, &scalar, local_n, my_rank, comm_sz, comm);

      /* Print input data */
      // Checking if rank is 0 to print
      if (my_rank == 0)
      {
            printf("\nEntered Scalar: %d\n", (int)scalar);
      }

      // Calling print_vector function to display entered vectors
      Print_vector(local_vec1, local_n, n, "\nVector 1:", my_rank, comm);
      Print_vector(local_vec2, local_n, n, "Vector 2:", my_rank, comm);

      /* Print results */
      /* Compute and print dot product */
      dot_product = Par_dot_product(local_vec1, local_vec2, local_n, comm);
      if (my_rank == 0)
      {
            printf("Dot Product: %d\n", (int)dot_product);
      }

      /* Compute scalar multiplication and print out result */
      Par_vector_scalar_mult(local_vec1, scalar, local_scalar_mult1, local_n);
      Print_vector(local_scalar_mult1, local_n, n, "\nScalar Mult Vector 1 Result:", my_rank, comm);
      Par_vector_scalar_mult(local_vec2, scalar, local_scalar_mult2, local_n);
      Print_vector(local_scalar_mult2, local_n, n, "Scalar Mult Vector 2 Result:", my_rank, comm);

      free(local_scalar_mult1);
      free(local_scalar_mult2);
      free(local_vec1);
      free(local_vec2);

      MPI_Finalize();
      return 0;
}

/*-------------------------------------------------------------------*/
void Check_for_error(
    int local_ok /* in */,
    char fname[] /* in */,
    char message[] /* in */,
    MPI_Comm comm /* in */)
{
      int ok;

      MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
      if (ok == 0)
      {
            int my_rank;
            MPI_Comm_rank(comm, &my_rank);
            if (my_rank == 0)
            {
                  fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname,
                          message);
                  fflush(stderr);
            }
            MPI_Finalize();
            exit(-1);
      }
} /* Check_for_error */

/* Get the input of n: size of the vectors, and then calculate local_n according to comm_sz and n */
/* where local_n is the number of elements each process obtains */
/*-------------------------------------------------------------------*/
void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm)
{
      // Checking if rank is 0 to print and read input
      if (my_rank == 0)
      {
            printf("Enter vector size (n):\n");
            scanf("%d", n_p);
      }

      // Sending value of n to all processes in the communicator
      MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

      // Splitting the work equally among all processes
      *local_n_p = *n_p / comm_sz;

} /* Read_n */

/* local_vec1 and local_vec2 are the two local vectors of size local_n which the process pertains */
/* process 0 will take the input of the scalar, the two vectors a and b */
/* process 0 will scatter the two vectors a and b across all processes */
/*-------------------------------------------------------------------*/
void Read_data(double local_vec1[], double local_vec2[], double *scalar_p,
               int local_n, int my_rank, int comm_sz, MPI_Comm comm)
{
      double *a = NULL;
      int i;
      if (my_rank == 0)
      {
            printf("What is the scalar?\n");
            scanf("%lf", scalar_p);
      }

      MPI_Bcast(scalar_p, 1, MPI_DOUBLE, 0, comm);

      if (my_rank == 0)
      {
            a = malloc(local_n * comm_sz * sizeof(double));
            printf("Enter the first vector\n");

            for (i = 0; i < local_n * comm_sz; i++)
                  scanf("%lf", &a[i]);

            MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
            free(a);
      }
      else
      {
            MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
      }

      // Master process will do the following
      if (my_rank == 0)
      {
            // Allocating memory for array a, local_n is the num of processes. comm_sz determines the size of the array
            a = malloc(local_n * comm_sz * sizeof(double));
            printf("Enter the second vector\n");

            // loop through all elements in the array
            for (i = 0; i < local_n * comm_sz; i++)
                  scanf("%lf", &a[i]);

            // Distributing values throughout processors
            // Array contaning vector values, number of elements each process will receive of type double
            // Each process will store their elements in local_vec2 in double type
            // Scatter is done by the master process using comm to communicate with all processors
            MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm);
            free(a);
      }
      else
      {
            // All other processors will skip the above code and only receive and send data back
            MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm);
      }

} /* Read_data */

/* The print_vector gathers the local vectors from all processes and print the gathered vector */
/*-------------------------------------------------------------------*/
void Print_vector(double local_vec[], int local_n, int n, char title[],
                  int my_rank, MPI_Comm comm)
{
      double *a = NULL;
      int i;

      if (my_rank == 0)
      {
            // Allocating memory for array containing the vector
            a = malloc(n * sizeof(double));
            // Gathering data from all processors, master process collects
            MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
            // Printing vector
            printf("%s\n", title);
            for (i = 0; i < n; i++)
                  printf("%d ", (int)a[i]);
            printf("\n\n");
            free(a);
      }
      else
      {
            MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
      }

} /* Print_vector */

/* This function computes and returns the partial dot product of local_vec1 and local_vec2 */
/*-------------------------------------------------------------------*/
double Par_dot_product(double local_vec1[], double local_vec2[],
                       int local_n, MPI_Comm comm)

{ // Variable to hold dot procuct of one process
      double partial_dot_product = 0.0;
      // Variable to collect dot product of all processors
      double total_dot_product;

      // loop through arrays
      for (int i = 0; i < local_n; i++)
      {
            // Multiplying i-th value in vector 1 to i-th value in vector 2 and adding it to partial_dot_product
            partial_dot_product += local_vec1[i] * local_vec2[i];
      }

      // Combining products of all processors
      // Reducing all local variable into one variable available in the master process
      // 1 element (local value) of type double is reduced by addition operation and store in process 0 (master)
      // Processors communicate through comm
      MPI_Reduce(&partial_dot_product, &total_dot_product, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
      return total_dot_product;
} /* Par_dot_product */

/* This function gets the vector which is the scalar times local_vec, and put the vector into local_result */
/*-------------------------------------------------------------------*/
void Par_vector_scalar_mult(double local_vec[], double scalar,
                            double local_result[], int local_n)
{
      // Iterate through elements in the array
      for (int i = 0; i < local_n; i++)
      {
            // multiplying each element by the scalar value
            local_result[i] = local_vec[i] * scalar;
      }
} /* Par_vector_scalar_mult */