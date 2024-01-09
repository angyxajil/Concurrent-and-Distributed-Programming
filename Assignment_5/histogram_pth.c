#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int num_threads, data_count, n;
double a, b;
double *data;
double *bin_maxes;
int *bin_counts;
pthread_mutex_t mutex;

void Usage(char *prog_name)
{
    fprintf(stderr, "usage: %s <t> <data_count> <a> <b> <n>\n", prog_name);
    fprintf(stderr, "   <t> is the number of threads\n");
    fprintf(stderr, "   <data_count> is the number of measurements, which should be divisible by t\n");
    fprintf(stderr, "   <a> is the lower bound of the measurements\n");
    fprintf(stderr, "   <b> is the upper bound of the measurements\n");
    fprintf(stderr, "   <n> is the number of bins\n");
    exit(0);
}

void Get_args(int argc, char *argv[])
{
    if (argc != 6)
        Usage(argv[0]);
    num_threads = strtoll(argv[1], NULL, 10);
    data_count = strtoll(argv[2], NULL, 10);
    a = strtoll(argv[3], NULL, 10);
    b = strtoll(argv[4], NULL, 10);
    n = strtoll(argv[5], NULL, 10);

    if (num_threads <= 0 || data_count <= 0 || a <= 0 || b <= 0 || n <= 0)
        Usage(argv[0]);
}

double *Populate_data()
{
    data = malloc(data_count * sizeof(double));
    for (int i = 0; i < data_count; ++i)
    {
        // Generate a random double in range a to b.
        double f = (double)rand() / RAND_MAX;
        data[i] = a + f * (b - a);
    }
    printf("Generated data: \n");
    for (int i = 0; i < data_count; ++i)
    {
        printf("%.2lf ", data[i]);
    }
    printf("\n");
    return data;
}

double *Populate_bin_maxes()
{
    bin_maxes = malloc(n * sizeof(double));
    double bin_width = (b - a) / n;

    for (int bin_i = 0; bin_i < n; ++bin_i)
    {
        bin_maxes[bin_i] = a + bin_width * (bin_i + 1);
    }
    // printf("bin_maxes: ");
    // for (int i = 0; i < n; ++i)
    // {
    //     printf(" %lf ", bin_maxes[i]);
    // }
    // printf("]\n");
    return bin_maxes;
}

int Find_bin(double d)
{
    if (a <= d && d < bin_maxes[0])
    {
        return 0;
    }
    for (int i = 1; i < n; ++i)
    {
        if (bin_maxes[i - 1] <= d && d < bin_maxes[i])
        {
            return i;
        }
    }
    return -1;
}

void *Thread_func(void *rank)
{
    int my_rank = *(int *)rank;
    int *local_counts = malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i)
    {
        local_counts[i] = 0;
    }

       int start_i = my_rank * (data_count / num_threads);
    int end_i = start_i + (data_count / num_threads);

    for (int i = start_i; i < end_i; ++i)
    {
        int bin = Find_bin(data[i]);
        local_counts[bin]++;
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < n; ++i)
    {
        bin_counts[i] += local_counts[i];
    }
    pthread_mutex_unlock(&mutex);

    return 0;
    free(local_counts);
}

void Output_result()
{

    printf("\nBin Ranges:\n");
    double prev_min = a;
    for (int i = 0; i < n; ++i)
    {
        printf("Bin %d: %.3f to %.3f\n", i, prev_min,
               bin_maxes[i]);
        prev_min = bin_maxes[i];
    }

    printf("\n");
    for (int i = 0; i < n; ++i)
    {
        printf("In bin %d there are %d elements\n", i, bin_counts[i]);
        prev_min = bin_maxes[i];
    }
}

int main(int argc, char *argv[])
{
    pthread_t *thread_handles;

    Get_args(argc, argv);
    thread_handles = malloc(num_threads * sizeof(pthread_t));

    data = Populate_data();
    bin_maxes = Populate_bin_maxes();
    bin_counts = malloc(n * sizeof(int));

    for (int i = 0; i < n; ++i)
    {
        bin_counts[i] = 0.0;
    }

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

    Output_result();

    free(data);
    free(bin_maxes);
    free(bin_counts);
    free(thread_handles);
    return 0;
} /* main */