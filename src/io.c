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

char *read_line(char *filename, uint line_number) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("[-] Error: Unable to open file %s\n", filename);
        return NULL;
    }
    char *line = malloc(1024 * sizeof(char));
    uint count = 0;
    while (fgets(line, 1024, file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        line[strcspn(line, "#")] = 0;
        line[strcspn(line, ";")] = 0;
        ++count;
        if (count == line_number) break;
    }

    return line;
}
