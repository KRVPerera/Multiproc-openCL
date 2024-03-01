//
// Created by ruksh on 21/02/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <util.h>
#include "config.h"
#include "driver.h"

int main() {
    printf("Starting Multiprocessor Programming project!\n");
    printf("Data folder %s\n", PROJECT_DATA_DIR);

    time_t t;
    srand((unsigned) time(&t));

    fullFlow();

    printf("Stopping Multiprocessor Programming project!\n");
    return 0;
}


