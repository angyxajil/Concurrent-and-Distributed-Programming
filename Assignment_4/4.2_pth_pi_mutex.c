#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "timer.h"

const int MAX_THREADS = 1024;

long thread_count;
long long n;
double sum;

sem_t *sem;

void *Thread_sum(void *rank);

/* Only executed by the main thread */
void Get_args(int argc, char *argv[]);
void Usage(char *prog_name);
double Serial_pi(long long n);

int main(int argc, char *argv[])
{
  long thread; /* Use long in case of a 64-bit system */
  pthread_t *thread_handles;
  double start, finish, elapsed_parallel, elapsed_serial;

  /* Get number of threads from the command line */
  Get_args(argc, argv);
  thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));

  GET_TIME(start);
  double serial_pi = Serial_pi(n);
  GET_TIME(finish);
  elapsed_serial = finish - start;

  printf("Serial pi: %.15f\n", serial_pi);
  printf("Serial elapsed time: %e seconds\n", elapsed_serial);
  printf("-------------------------------------------------------");

  sem = sem_open("my_semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1); // Initialize semaphore
  
  GET_TIME(start);
  for (thread = 0; thread < thread_count; thread++)
  {
    pthread_create(&thread_handles[thread], NULL, Thread_sum, (void *)thread);
  }

  for (thread = 0; thread < thread_count; thread++)
  {
    pthread_join(thread_handles[thread], NULL);
  }
  GET_TIME(finish)

  elapsed_parallel = finish - start;

  printf("\nParallel pi: %.15f\n", 4.0 * sum);
  printf("Parallel elapsed time: %e seconds\n", elapsed_parallel);

  sem_close(sem);             // Close the semaphore
  sem_unlink("my_semaphore"); // Remove the semaphore

  free(thread_handles);

  return 0;
}

/*------------------------------------------------------------------*/
void *Thread_sum(void *rank)
{
  long my_rank = (long)rank;
  long long i;
  long long my_n = n / thread_count;
  long long my_first_i = my_n * my_rank;
  long long my_last_i = my_first_i + my_n;
  double my_sum = 0.0;
  double factor;

  if (my_first_i % 2 == 0)
  {
    factor = 1.0;
  }
  else
  {
    factor = -1.0;
  }

  for (i = my_first_i; i < my_last_i; i++, factor = -factor)
  {
    my_sum += factor / (2 * i + 1);
  }

  sem_wait(sem); // Wait on the semaphore
  sum += my_sum;
  sem_post(sem); // Release the semaphore

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
