//
// Created by ruksh on 21/02/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <util.h>
#include <driver.h>
#include <opencl_flow_ex3.h>

#include "config.h"

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <logger.h>
#include <omp.h>
#include <unistd.h>

void openmpTestCode(void) {
    logger("Running multithreaded mode.");
    int maxthreads = omp_get_max_threads();

    printf("Max threads: %d\n", maxthreads);

//    #pragma omp parallel num_threads(5)
    #pragma omp parallel num_threads(5)
    {
        int id = omp_get_thread_num();
        printf("Hello %d\n", id);
        sleep(1);
        printf("World %d\n", id);
    }
}

int main(int argc, char *argv[]) {
    logger("[MultiProc] : Starting Multiprocessor Programming project!");

    int multithreadedMode = 0;
    if (argc != 2) {
        logger("Incorrect number of arguments. Expected 1, got %d.", argc - 1);
        multithreadedMode = 1;
    }

    logger("Data folder %s", PROJECT_DATA_DIR);

    time_t t;
    srand((unsigned) time(&t));
    struct timespec t0, t1;
    unsigned long sec, nsec;
    GET_TIME(t0);
    if (!multithreadedMode && strcmp(argv[1], "opencl") == 0) {
        openclFlowEx3();
    } else if (multithreadedMode || strcmp(argv[1], "multithreaded") == 0) {
        openmpTestCode();
    } else if (!multithreadedMode && strcmp(argv[1], "singlethreaded") == 0) {
        fullFlow();
    } else {
        logger("Invalid argument. Expected 'opencl', 'multithreaded', or 'singlethreaded'. Got '%s'.", argv[1]);
        logger(" Running multithreaded mode.");
    }
    GET_TIME(t1);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Total time of the program : %f micro seconds", elapsed_time);
    logger("Stopping Multiprocessor Programming project!");
    return 0;
}


