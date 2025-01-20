#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    struct timeval start, end;
    long seconds, useconds;
    double elapsed;
    
    if (argc != 2) {
        printf("Usage: %s <num_steps>\n", argv[0]);
        return 1;
    }

    long num_steps = strtol(argv[1], NULL, 10);
    double step = 1.0 / (double)num_steps;
    double pi = 0.0;

    gettimeofday(&start, NULL);

    #pragma omp parallel for reduction(+:pi)
    for (long i = 0; i < num_steps; i++) {
        double x = (i + 0.5) * step;
        pi += 4.0 / (1.0 + x * x);
    }

    pi *= step;

    gettimeofday(&end, NULL);

    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    elapsed = seconds + useconds / 1000000.0;

    printf("Time elapsed: %f\n", elapsed);
    printf("Estimated value of Pi: %f\n", pi);

    return 0;
}