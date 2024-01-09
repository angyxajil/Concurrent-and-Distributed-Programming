#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int thread_count;
double total_sum;
int n;
double a, b, h;
pthread_mutex_t mutex;

void *Thread_sum(void *rank);
double f(double x);
double Trap(double left_endpt, double right_endpt, int trap_count, double base_len);

int main(int argc, char *argv[])
{
    long thread;
    pthread_t *thread_handles;
    thread_count = strtol(argv[1], NULL, 10);
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    printf("What is the Lower Bound? \n");
    scanf("%lf", &a);
    printf("What is the Upper Bound? \n");
    scanf("%lf", &b);
    printf("How many subintervals? \n");
    scanf("%d", &n);
    h = (b - a) / n;
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, Thread_sum, (void *)thread);
    }
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }
    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.15e\n", a, b, total_sum);
    pthread_mutex_destroy(&mutex);
    return 0;
}
void *Thread_sum(void *rank)
{
    long my_rank = (long)rank;
    double local_n = n / thread_count;
    double local_a = a + my_rank * local_n * h;
    double local_b = local_a + local_n * h;
    double local_sum = Trap(local_a, local_b, local_n, h);
    pthread_mutex_lock(&mutex);
    total_sum += local_sum;
    pthread_mutex_unlock(&mutex);
    return NULL;
}
double Trap(double left_endpt, double right_endpt, int trap_count, double base_len)
{
    double estimate, x;
    double f(double x);
    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    for (int i = 1; i <= trap_count - 1; i++)
    {
        x = left_endpt + i * base_len;
        estimate += f(x);
    }
    estimate = estimate * base_len;
    return estimate;
}
double f(double x)
{
    return (x * x);
}