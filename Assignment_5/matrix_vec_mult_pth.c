#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int order;
double *matrix;
double *vector;
double *res_vector;
pthread_mutex_t mutex;

void Usage(char *prog_name)
{
  fprintf(stderr, "usage: %s <n>\n", prog_name);
  fprintf(stderr, "   n is the order of the multiplication vector where n^2 is the size of matrix\n");
  fprintf(stderr, "   n also defines the number of threads\n");
  exit(0);
}

void Get_args(int argc, char *argv[])
{
  if (argc != 2)
    Usage(argv[0]);
  order = strtoll(argv[1], NULL, 10);
  if (order <= 0)
    Usage(argv[0]);
}

double *Get_matrix()
{
  int matrix_order = order * order;
  double *matrix = malloc(matrix_order * sizeof(double));
  printf("Enter %d elements for the matrix:\n", matrix_order);
  for (int i = 0; i < matrix_order; ++i)
  {
    scanf("%lf", &matrix[i]);
  }
  printf("\nMatrix:\n");
  for (int i = 0; i < order; ++i)
  {
    for (int j = 0; j < order; j++)
    {
      printf(" %.0lf", matrix[i * order + j]);
    }
    printf("\n");
  }
  printf("\n");
  return matrix;

  /**
   * void Print_matrix(double *matrix)
{
  printf("\nMatrix:\n");
  for (int i = 0; i < order; ++i)
  {
    for (int j = 0; j < order; ++j)
    {
      printf("%.0lf\t", matrix[i * order + j]);
    }
    printf("\n");
  }
}
  */
}

double *Get_vector()
{
  double *vector = malloc(order * sizeof(double));
  printf("\nEnter %d elements for the vector:\n", order);
  for (int i = 0; i < order; ++i)
  {
    scanf("%lf", &vector[i]);
  }
  // Output entered vector.
  printf("\nEntered vector: ");
  for (int i = 0; i < order; ++i)
  {
    printf(" %.0lf ", vector[i]);
  }
  printf("\n");
  return vector;
}

void *Thread_dot_product(void *rank)
{
  int my_rank = *(int *)rank;
  int my_first_i = my_rank * order;
  double my_res = 0.0;
  for (int j = 0, my_i = my_first_i; j < order; ++j, ++my_i)
  {
    my_res += matrix[my_i] * vector[j];
  }

  pthread_mutex_lock(&mutex);
  res_vector[my_rank] = my_res;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

void Output_result()
{
  printf("Product: ");
  for (int i = 0; i < order; ++i)
  {
    printf(" %.0lf ", res_vector[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  pthread_t *thread_handles;

  Get_args(argc, argv);
  thread_handles = malloc(order * sizeof(pthread_t));
  matrix = Get_matrix();
  vector = Get_vector();
  res_vector = malloc(order * sizeof(double));

  pthread_mutex_init(&mutex, NULL);

  int temp_is[order];
  for (int i = 0; i < order; ++i)
  {
    temp_is[i] = i;
    pthread_create(&(thread_handles[i]), NULL, Thread_dot_product, &temp_is[i]);
  }
  for (int j = 0; j < order; ++j)
  {
    pthread_join(thread_handles[j], NULL);
  }

  pthread_mutex_destroy(&mutex);

  Output_result();

  free(thread_handles);
  free(matrix);
  free(vector);
  free(res_vector);
  return 0;
} /* main */
