/**
 * File Name: tokens.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef TOKENS_H
#define TOKENS_H

typedef struct Token {
    enum {
        TOKEN_INSTRUCTION,
        TOKEN_REGISTER,
        TOKEN_IMMEDIATE,
        TOKEN_LABEL,
        TOKEN_LABEL_REF,
        TOKEN_L_PAREN,
        TOKEN_R_PAREN
    } token_type;
    unsigned int line_number;
    unsigned int character_number;
    unsigned int instruction_number;
    char *value;
} token_struct;

typedef struct TokenList {
    unsigned int index;
    unsigned int size;
    token_struct *list;
} token_list_struct;

token_struct *init_token(int token_type, unsigned int line_number, unsigned int character_number, unsigned int instruction_number, char *value);

void print_token(token_struct *token);

token_list_struct *init_token_list_struct(unsigned int size);

void print_token_list(token_list_struct *token_list);

void free_token_list(token_list_struct *token_list);

char *get_register_value(char *register_name);

#endif // TOKENS_H