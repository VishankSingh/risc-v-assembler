/**
 * File Name: utils.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "../include/utils.h"
#include "../include/io.h"
#include "../include/globals.h"

// TODO: modify decimal_to_binary to return a bitstring of specified length, padded with zero or MSB

uint decimal_to_binary(int decimal, int bit_length, _Bool MSB_extend) {
    (void) bit_length;
    (void) MSB_extend;
    uint binary = 0;
    int remainder, i = 1;
    while (decimal != 0) {
        remainder = decimal % 2;
        decimal /= 2;
        binary += remainder * i;
        i *= 10;
    }
    return binary;
}

_Bool is_valid_decimal(const char *str) {
    uint i = 0;
    if (str[0] == '-' || str[0] == '+') {
        i = 1;
    }

    while (str[i] != '\0') {
        if (!isdigit(str[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}

_Bool is_valid_hexadecimal(const char *str) {
    uint i = 0;

    if (str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X')) {
        i += 2;
    } else {
        return 0;
    }

    while (str[i] != '\0') {
        if (!isxdigit(str[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}

_Bool is_valid_binary(const char *str) {
    uint i = 0;

    if (str[i] == '0' && (str[i + 1] == 'b' || str[i + 1] == 'B')) {
        i += 2;
    } else {
        return 0;
    }

    while (str[i] != '\0') {
        if (str[i] != '0' && str[i] != '1') {
            return 0;
        }
        i++;
    }
    return 1;
}

_Bool is_unsigned_instruction(const char *instruction) {
    if (strcmp(instruction, "lbu") == 0) {
        return 1;
    } else if (strcmp(instruction, "lhu") == 0) {
        return 1;
    } else if (strcmp(instruction, "lwu") == 0) {
        return 1;
    } else if (strcmp(instruction, "sltiu") == 0) {
        return 1;
    } else if (strcmp(instruction, "lui") == 0) {
        return 1;
    } else if (strcmp(instruction, "auipc") == 0) {
        return 1;
    }
    return 0;
}

_Bool is_funct6_instruction(const char *instruction) {
    if (strcmp(instruction, "slli") == 0) {
        return 1;
    } else if (strcmp(instruction, "srli") == 0) {
        return 1;
    } else if (strcmp(instruction, "srai") == 0) {
        return 1;
    }
    return 0;
}

_Bool is_I_Bracket_instruction(const char *instruction) {
    _Bool is = ((strcmp(instruction, "jalr") == 0)
             || (strcmp(instruction, "lb") == 0)
             || (strcmp(instruction, "lh") == 0)
             || (strcmp(instruction, "lw") == 0)
             || (strcmp(instruction, "ld") == 0)
             || (strcmp(instruction, "lbu") == 0)
             || (strcmp(instruction, "lhu") == 0)
             || (strcmp(instruction, "lwu") == 0));
    return is;
}

void print_error(const char *filename, uint line_number, uint character_number, const char *msg, const char *line_2) {
    printf(BOLD "%s:%d:%d: " RED "[error]" RESET " %s\n",
                           filename,
                           line_number,
                           character_number,
                           msg);
    printf("%5d | %s\n", line_number, read_line(input_file_, line_number));
    printf("      | %s\n",line_2);
    printf("      |\n");
}

void print_dash_n_times(int n) {
    for (int i = 0; i < n; i++) {
        putchar('-');
    }
}

void print_space_n_times(uint n) {
    for (uint i = 0; i < n; i++) {
        putchar(' ');
    }
}

void print_32_bit_instruction(uint num) {
    for (int i = 31; i >= 0; i--) {
        putchar((num & (1 << i)) ? '1' : '0');

        if (i == 7) {
            putchar(' ');
        } else if (i == 12) {
            putchar(' ');
        } else if (i == 15) {
            putchar(' ');
        } else if (i == 20) {
            putchar(' ');
        } else if (i == 25) {
            putchar(' ');
        }

    }
    putchar('\n'); // New line at the end
}