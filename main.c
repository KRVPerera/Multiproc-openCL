#include <driver.h>
#include <opencl_flow_ex3.h>
#include <opencl_flow_ex5.h>
#include <opencl_flow_ex6.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

#include "config.h"

#include <logger.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
    printf("-----------------------------------printHeader---------------------------------------------------\n");
    printHeader();

    BENCHMARK_MODE benchmark = DO_NOT_BENCHMARK;
    bool multithreadedMode = false;
    if (argc < 2)
    {
        logger("Incorrect number of arguments. Expected 1, got %d. Running `mp - multithreaded` mode", argc - 1);
        multithreadedMode = true;
    }

    if (argc > 2 && strcmp(argv[2], "-benchmark") == 0)
    {
        benchmark = BENCHMARK;
    }

    logger("Data folder %s", PROJECT_DATA_DIR);

    time_t t;
    srand((unsigned)time(&t));
    struct timespec t0, t1;
    unsigned long sec, nsec;
    // get starting time
    GET_TIME(t0)

    if (multithreadedMode || (strcmp(argv[1], "mp") == 0))
    {
        multithreadedMode = true;
        fullFlow(benchmark, multithreadedMode);

    } else if ((strcmp(argv[1], "help") == 0) || (strcmp(argv[1], "help") == 0) || (strcmp(argv[1], "-h") == 0))
    {
        printHelp();
    }
    else if (strcmp(argv[1], "opencl") == 0)
    {
        openclFlowEx5();
    } else if (strcmp(argv[1], "opencl_opt") == 0)
    {
        openclFlowEx6();
    } else if (strcmp(argv[1], "opencl_old") == 0)
    {
        openclFlowEx3();
    } else if (strcmp(argv[1], "single") == 0)
    {
        fullFlow(benchmark, multithreadedMode);
    } else if (strcmp(argv[1], "test") == 0)
    {
        openmpTestCode();
    } else if (strcmp(argv[1], "opencl_platform") == 0)
    {
        printDeviceInformation();
    } else
    {
        printHelp();
    }

    // get program end time
    GET_TIME(t1)

    logger("Total time of the main : %.3f ms", elapsed_time_microsec(&t0, &t1, &sec, &nsec));
    logger("Multiprocessor project : Ran successfully!");
    printf("--------------------------------------------------------------------------------------\n");
    return 0;
}
