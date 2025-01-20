#include <stdio.h>
#include <omp.h>
#include <sys/time.h>
#include <stdlib.h>

double step;

int main(int argc, char *argv[])
{
    struct timeval start_time, end_time;
    long seconds, useconds;
    double elapsed;
    unsigned long long num_steps;
    double pi = 0.0;

    if (argc != 2) {
        printf("Usage: %s <num_steps>\n", argv[0]);
        return 1;
    }

    num_steps = strtoull(argv[1], NULL, 10);
    step = 1.0 / (double)num_steps;

    int max_threads = omp_get_max_threads();
    double *sum = (double *)malloc(max_threads * sizeof(double));

    gettimeofday(&start_time, NULL);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int nthrds = omp_get_num_threads();
        double x, func_val;
        sum[id] = 0.0;

        for (unsigned long long i = id; i < num_steps; i += max_threads) {
            x = (i + 0.5) * step;
            func_val = 4.0 / (1.0 + x * x);
            sum[id] += func_val;
        }
    }

    gettimeofday(&end_time, NULL);

    for (int i = 0; i < max_threads; i++) {
        pi += sum[i];
    }

    pi *= step;

    seconds = end_time.tv_sec - start_time.tv_sec;
    useconds = end_time.tv_usec - start_time.tv_usec;
    elapsed = seconds + useconds / 1000000.0;

    printf("Time elapsed: %f\n", elapsed);
    printf("Estimated value of Pi: %f\n", pi);

    free(sum);  // Free allocated memory
    return 0;
}