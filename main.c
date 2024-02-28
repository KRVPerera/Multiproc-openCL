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

    char* image0Name = "im0.png";
    char* image0_BW_Name = "im_0_bw.png";

    char imagePath0[256];
    char imagePath0_out[256];
    snprintf(imagePath0, sizeof(imagePath0), "%s/sample/%s", PROJECT_DATA_DIR, image0Name);
    snprintf(imagePath0_out, sizeof(imagePath0_out), "%s/out/%s", PROJECT_DATA_DIR, image0_BW_Name);

    time_t t;
    srand((unsigned) time(&t));

    zncc_flow_driver(imagePath0, imagePath0_out);

    printf("Stopping Multiprocessor Programming project!\n");
    return 0;
}


