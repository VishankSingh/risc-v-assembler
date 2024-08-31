/**
 * File Name: utils.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef UTILS_H
#define UTILS_H

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

unsigned int decimal_to_binary(int decimal, int bit_length, _Bool MSB_extend);

_Bool is_valid_decimal(const char *str);

_Bool is_valid_hexadecimal(const char *str);

_Bool is_valid_binary(const char *str);

_Bool is_unsigned_instruction(const char *str);

_Bool is_funct6_instruction(const char *instruction);

void print_dash_n_times(int n);

void print_space_n_times(unsigned int n);

void print_32_bit_instruction(unsigned int num);

#endif // UTILS_H