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

// TODO: modify decimal_to_binary to return a bitstring of specified length, padded with zero or MSB

unsigned int decimal_to_binary(int decimal, int bit_length, _Bool MSB_extend) {
    (void) bit_length;
    (void) MSB_extend;
    unsigned int binary = 0;
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
    unsigned int i = 0;
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
    unsigned int i = 0;

    if (str[0] == '-' || str[0] == '+') {
        i = 1;
    }

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
    unsigned int i = 0;

    if (str[0] == '-' || str[0] == '+') {
        i = 1;
    }

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
    } else if (strcmp(instruction, "bltu") == 0) {
        return 1;
    } else if (strcmp(instruction, "bgeu") == 0) {
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

void print_dash_n_times(int n) {
    for (int i = 0; i < n; i++) {
        putchar('-');
    }
}

void print_space_n_times(unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        putchar(' ');
    }
}

void print_32_bit_instruction(unsigned int num) {
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