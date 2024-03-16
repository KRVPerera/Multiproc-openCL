//
// Created by ruksh on 16/03/2024.
//
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

void logger(const char *format, ...) {
    time_t now;
    time(&now);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';  // remove newline character

    printf("[%s] [MultiProc] : ", time_str);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}
