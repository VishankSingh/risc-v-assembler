/**
 * File Name: io.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/io.h"

void create_file(char *filename) {
    FILE *file = fopen(filename, "w");
    fclose(file);
}

void write_newline_to_file(char *filename, uint32_t machine_code) {
    FILE *file = fopen(filename, "a");
    fprintf(file, "%08x\n", machine_code);
    fclose(file);
}