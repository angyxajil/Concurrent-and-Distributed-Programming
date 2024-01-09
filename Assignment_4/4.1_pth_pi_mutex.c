#include "timer.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

const int MAX_THREADS = 1024;

long thread_count;
long long n;
double sum = 0;
pthread_mutex_t mutex;

void *Thread_sum(void *rank);

/* Only executed by main thread */
void Get_args(int argc, char *argv[]);
void Usage(char *prog_name);
double Serial_pi(long long n);

int main(int argc, char *argv[])
{
   pthread_t *thread_handles;
   double start, finish, elapsed_serial, elapsed_parallel;
   double output_serial;

   Get_args(argc, argv);
   thread_handles = malloc(thread_count * sizeof(pthread_t));

   GET_TIME(start);
   output_serial = Serial_pi(n);
   GET_TIME(finish);
   elapsed_serial = finish - start;
   printf("Estimated value of pi serial value: %.15f\n", output_serial);
   printf("Elapsed time: %e seconds\n", elapsed_serial);

   sum = 0.0;
   pthread_mutex_init(&mutex, NULL);
   long temp_is[thread_count];
   GET_TIME(start);

   // Loop through all threads
   for (long i = 0; i < thread_count; ++i)
   {
      // Assining numbers
      temp_is[i] = i;
      // Creating threads
      pthread_create(&(thread_handles[i]), NULL, Thread_sum, &temp_is[i]);
   }

   // Wait for each thread to finish executing
   for (long j = 0; j < thread_count; ++j)
   {
      pthread_join(thread_handles[j], NULL);
   }

   double parallel_pi = 4.0 * sum;
   GET_TIME(finish);

   pthread_mutex_destroy(&mutex);

   printf("-------------------------------------------------------");

   elapsed_parallel = finish - start;
   printf("\nEstimated value of pi parallel value: %.15f\n", parallel_pi);
   printf("Elapsed time: %e seconds\n", elapsed_parallel);

   free(thread_handles);
   return 0;
}

/*------------------------------------------------------------------
 * Function: Thread_sum
 * Purpose: used to calculate partial sums of the series used to estimate the value of pi concurrently across multiple threads.
 * In arg: rank
 * Return val: NULL (void pointer) - The function doesn't return a meaningful value.
 */
void *Thread_sum(void *rank)
{
   long my_rank = *(long *)rank;
   double factor;
   long long i;
   long long my_n = n / thread_count;
   long long my_first_i = my_n * my_rank;
   long long my_last_i = my_first_i + my_n;

   if (my_first_i % 2 == 0)
   {

      factor = 1.0;
   }
   else
   {
      factor = -1.0;
   }

   for (i = my_first_i; i < my_last_i; ++i, factor = -factor)
   {

      pthread_mutex_lock(&mutex);
      sum += factor / (2 * i + 1);
      pthread_mutex_unlock(&mutex);
   }

   return NULL;
} /* Thread_sum */

/*------------------------------------------------------------------
 * Function:   Serial_pi
 * Purpose:    Estimate pi using 1 thread
 * In arg:     n
 * Return val: Estimate of pi using n terms of Maclaurin series
 */
double Serial_pi(long long n)
{
   double sum = 0.0;
   long long i;
   double factor = 1.0;

   for (i = 0; i < n; i++, factor = -factor)
   {
      sum += factor / (2 * i + 1);
   }
   return 4.0 * sum;
} /* Serial_pi */

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char *argv[])
{
   if (argc != 3)
      Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   if (thread_count <= 0 || thread_count > MAX_THREADS)
      Usage(argv[0]);
   n = strtoll(argv[2], NULL, 10);
   if (n <= 0)
      Usage(argv[0]);
} /* Get_args */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char *prog_name)
{
   fprintf(stderr, "usage: %s <number of threads> <n>\n", prog_name);
   fprintf(stderr, "   n is the number of terms and should be >= 1\n");
   fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
   exit(0);
} /* Usage */