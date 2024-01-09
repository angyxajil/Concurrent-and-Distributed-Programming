#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <string.h>

/**
 * suppose we too darts randomly at a square dartboard, who bullseye is at the origin, and whose sides are 2 feet in length.
 * suppose also there’s a circle  inscribied in the square dartboard. the radius of the circle is 1 foot, and its area is π square feet
 * if the points that are hit by the darts are uniformly distributed (and we always hit the square), then the number of darts that hit
 * inside the circle should approximately satisfy the equation. number in circle / total number of tosses = π/4. since the ratio of the
 * area of the circle to the are of the square is π/4 we can use this formula to estimate the value of π with a random number generator
 * -------------------------------------------------------------------------
 * number_in_circle = 0;
 * for (toss = 0; toss < number_of_tosses; toss++) {
 * x = random double between -1 and 1;
 * y + random double between -1 and 1;
 * distance_squared <= 1) number_in_circle++;
 * }
 * pi_estimate = 4*number_in_circle / ((double) number_of_tosses);
 * --------------------------------------------------------------------------
 * Use Open MPI in c to write a parallel program for this method. the processes evenly distribute the tosses, while process 0 sums up the total number of darts which hit inside the circle from processes, calculates and outputs the estimate value of π
 * INPUT: n the number of total tosses
 * OUTPUT: the estimate of the value of π
 */

void Read_n(long *total_tosses, int my_rank);
long tosses(long toss_per_process, int my_rank);
double Rand_range(double min, double max);

int main()
{
    int my_rank, comm_sz;
    long total_tosses, toss_per_process, number_in_circle_process, total_circle;
    double pi_estimate;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &my_rank);
    MPI_Comm_size(comm, &comm_sz);

    Read_n(&total_tosses, my_rank);

    toss_per_process = total_tosses / comm_sz;
    number_in_circle_process = tosses(toss_per_process, my_rank);

    MPI_Reduce(&number_in_circle_process, &total_circle, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        pi_estimate = (4 * total_circle) / (double)total_tosses;
        printf("Pi is approximately :  %f", pi_estimate);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}

void Read_n(long *total_tosses, int my_rank)
{
    if (my_rank == 0)
    {
        printf("Enter the total number of tosses \n");
        fflush(stdout);
        scanf("%ld", total_tosses);
    }
    MPI_Bcast(total_tosses, 1, MPI_LONG, 0, MPI_COMM_WORLD);
}

long tosses(long toss_per_process, int my_rank)
{
    srand(time(NULL));
    long toss, number_in_circle = 0;
    double x, y;
    for (toss = 0; toss < toss_per_process; toss++)
    {
        x = Rand_range(-1.0, 1.0);
        y = Rand_range(-1.0, 1.0);
        if ((x * x + y * y) <= 1.0)
        {
            number_in_circle++;
        }
    }
    return number_in_circle;
}

double Rand_range(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}