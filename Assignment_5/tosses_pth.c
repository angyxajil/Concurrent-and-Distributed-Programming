#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int num_threads, total_tosses;
int num_in_circle = 0;
pthread_mutex_t mutex;

void Usage(char *prog_name)
{
    fprintf(stderr, "usage: %s <t> <total_tosses>\n", prog_name);
    fprintf(stderr, "   <t> is the number of threads\n");
    fprintf(stderr, "   <total_tosses> is the total number of tosses, which should be divisible by t\n");
    exit(0);
}

void Get_args(int argc, char *argv[])
{
    if (argc != 3)
        Usage(argv[0]);
    num_threads = strtoll(argv[1], NULL, 10);
    total_tosses = strtoll(argv[2], NULL, 10);

    if (num_threads <= 0 || total_tosses <= 0)
        Usage(argv[0]);
}

void *Thread_func(void *rank)
{
    int my_rank = *(int *)rank;
    int local_num_in_circle = 0;
    unsigned int seed = (unsigned)time(NULL);
    srand(seed + my_rank);

    int num_tosses = total_tosses / num_threads;
    for (int t = 0; t < num_tosses; ++t)
    {
        // -1 to 1.
        double x = -1.0 + (rand() / (RAND_MAX / (1.0 - (-1.0))));
        double y = -1.0 + (rand() / (RAND_MAX / (1.0 - (-1.0))));
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1)
            local_num_in_circle++;
    }

    pthread_mutex_lock(&mutex);
    num_in_circle += local_num_in_circle;
    pthread_mutex_unlock(&mutex);

    return 0;
}

void Calculate_and_output_pi()
{
    double pi_estimate = 4 * num_in_circle / (double)total_tosses;
    printf("The estimated value of Pi is: %lf\n", pi_estimate);
}

int main(int argc, char *argv[])
{
    pthread_t *thread_handles;

    Get_args(argc, argv);
    thread_handles = malloc(num_threads * sizeof(pthread_t));

    pthread_mutex_init(&mutex, NULL);

    int temp_is[num_threads];
    for (int i = 0; i < num_threads; ++i)
    {
        temp_is[i] = i;
        pthread_create(&(thread_handles[i]), NULL, Thread_func, &temp_is[i]);
    }
    for (int j = 0; j < num_threads; ++j)
    {
        pthread_join(thread_handles[j], NULL);
    }

    pthread_mutex_destroy(&mutex);

    Calculate_and_output_pi();

    free(thread_handles);
    return 0;
} /* main */
