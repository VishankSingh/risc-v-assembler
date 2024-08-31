/**
 * File Name: lexer.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "tokens.h"

typedef struct Lexer {
    token_list_struct *token_list;
    char *filename;
    FILE *file;
    char *line;
    unsigned int line_number;
    unsigned int instruction_number;
    unsigned int character_number;
} lexer_struct;

lexer_struct *init_lexer(char *filename, token_list_struct *token_list);

void lex(lexer_struct *lexer);

_Bool tokenize_line(lexer_struct *lexer);

void free_lexer(lexer_struct *lexer);

#endif // LEXER_H