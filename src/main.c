/**
 * File Name: main.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <string.h>
#include <stdio.h>

#include "../include/globals.h"
#include "../include/assembler.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("[-] Usage: %s <filename>\n", argv[0]);
        return 1;
    } else {
        for (int i = 1; i < argc; i++) {
            if (strncmp(argv[i], "-o=", 9) == 0) {
                output_file = argv[i] + 9;
            }
        }
        assemble(argv[1], output_file);
    }
    // ISSUE: if two labels are together without space, takes them as 1 label, label1:label2: is one label valued "label1:label2"
    // ISSUE: if empty brackets it takes them as one instruction [SOLVED]
    // Labels are brackets are essentially ignored in second pass unless in between the instruction, which is syntax error so no problem as of now
    return 0;
}