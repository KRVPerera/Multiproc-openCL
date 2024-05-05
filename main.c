//
// Created by ruksh on 21/02/2024.
//

#include <driver.h>
#include <opencl_flow_ex3.h>
#include <opencl_flow_ex5.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

#include "config.h"

#include <calc_pi.h>
#include <logger.h>
#include <omp.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void openmpTestCode(void)
{
    logger("Running multithreaded mode.");
    int maxthreads = omp_get_max_threads();

    printf("Max threads: %d\n", maxthreads);

//    #pragma omp parallel num_threads(5)
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Hello %d\n", id);
        sleep(1);
        printf("World %d\n", id);
    }

    int num_steps = 1000000000;
    struct timespec t0, t1;
    unsigned long sec, nsec;
    GET_TIME(t0)
    double pi = calc_pi(num_steps);
    GET_TIME(t1)
    logger("Pi: %f", pi);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("calc_pi(%d) time : %f micro seconds", num_steps, elapsed_time);

    GET_TIME(t0)
    pi = calc_pi_mt(num_steps);
    GET_TIME(t1)
    logger("MT Pi: %f", pi);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("calc_pi_mt(%d) time : %f micro seconds", num_steps, elapsed_time);
}

int main(int argc, char *argv[])
{
    logger("Starting Multiprocessor Programming project!");

    BENCHMARK_MODE benchmark = DO_NOT_BENCHMARK;
    int multithreadedMode = 0;
    if (argc < 2) {
        logger("Incorrect number of arguments. Expected 1, got %d. Running `mp - multithreaded` mode", argc - 1);
        multithreadedMode = 1;
    }

    if (argc > 2 && strcmp(argv[2], "-benchmark") == 0) {
        benchmark = BENCHMARK;
    }

    logger("Data folder %s", PROJECT_DATA_DIR);

    time_t t;
    srand((unsigned)time(&t));
    struct timespec t0, t1;
    unsigned long sec, nsec;
    // get starting time
    GET_TIME(t0)

    if (multithreadedMode || strcmp(argv[1], "mp") == 0) {
        fullFlow_MT();
    } else if (strcmp(argv[1], "opencl") == 0) {
        openclFlowEx5();
    } else if (strcmp(argv[1], "opencl_old") == 0) {
        openclFlowEx3();
    } else if (strcmp(argv[1], "single") == 0) {
        fullFlow(benchmark);
    } else if (strcmp(argv[1], "test") == 0) {
        openmpTestCode();
    } else {
        logger("Invalid argument. Expected 'opencl', 'mp', 'single' or 'test'. Got '%s'.", argv[1]);
        logger(" Running mp - multithreaded mode.");
    }

    // get program end time
    GET_TIME(t1)

    logger("Total time of the program : %f micro seconds", elapsed_time_microsec(&t0, &t1, &sec, &nsec));
    logger("Stopping Multiprocessor Programming project!");
    return 0;
}
