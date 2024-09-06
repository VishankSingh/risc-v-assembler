/**
 * File Name: assembler.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/assembler.h"

#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/globals.h"
#include "../include/io.h"

void assemble(char *output_file) {
    create_file(output_file);
    token_list_s *token_list = init_token_list_s(MAX_MAIN_TOKEN_LIST_SIZE);
    lexer_s *lexer = init_lexer(token_list);
    if (lexer == NULL) {
        return;
    }
    lex(lexer);
    free_lexer(lexer);
    // print_token_list(token_list);
    parser_s *parser = init_parser(token_list);
    if (parser == NULL) {
        return;
    }
    parse(parser);
    free_parser(parser);
    free_token_list(token_list);
    if (error_code_) {
        remove(output_file);
        exit(1);
    }
}