/**
 * File Name: lexer.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/globals.h"
#include "../include/instructions.h"
#include "../include/lexer.h"
#include "../include/utils.h"

lexer_s *init_lexer(token_list_s *token_list) {
    lexer_s *lexer = malloc(sizeof(lexer_s));
    lexer->token_list = token_list;
    lexer->file = fopen(input_file_, "r");
    if (lexer->file == NULL) {
        printf("[-] Error: Unable to open file %s\n", input_file_);
        return NULL;
    }
    lexer->line = NULL;
    lexer->line_number = 0;
    lexer->character_number = 0;
    lexer->instruction_number = 0;
    return lexer;
}

_Bool tokenize_line(lexer_s *lexer) {
    size_t i = 0;
    _Bool error = 0;
    unsigned int initial_size = lexer->token_list->size;
    while (i < strlen(lexer->line)) {
        lexer->character_number = i + 1;
        if ((lexer->line[i] == ' ') || (lexer->line[i] == '\t') || (lexer->line[i] == ',')) {
            i++;
            continue;
        } else if (lexer->line[i] == '#' || lexer->line[i] == ';') {
            break;
        } else if (lexer->line[i] == '(') {
            token_s *token =
                init_token(TOKEN_L_PAREN, lexer->line_number, lexer->character_number, lexer->instruction_number, "(");
            lexer->token_list->list[lexer->token_list->index++] = *token;
            lexer->token_list->size++;
            free(token);
            i++;
            continue;
        } else if (lexer->line[i] == ')') {
            token_s *token =
                init_token(TOKEN_R_PAREN, lexer->line_number, lexer->character_number, lexer->instruction_number, ")");
            lexer->token_list->list[lexer->token_list->index++] = *token;
            lexer->token_list->size++;
            free(token);
            i++;
            continue;
        } else {
            char *token_name = malloc(MAX_TOKEN_NAME_SIZE * sizeof(char));
            int j = 0;
            while (lexer->line[i] != ' ' && lexer->line[i] != '\t' && lexer->line[i] != ',' && lexer->line[i] != '('
                && lexer->line[i] != ')' && lexer->line[i] != '\n' && lexer->line[i] != '\0' && lexer->line[i] != '#') {
                token_name[j] = lexer->line[i];
                i++;
                j++;
            }
            token_name[j] = '\0';
            if ((j > 1) && (token_name[j - 1] == ':') && (!isdigit(token_name[0]))) {
                token_name[j - 1] = '\0';
                token_s
                    *token =
                    init_token(TOKEN_LABEL,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (get_register_value(token_name) != NULL) {
                token_s
                    *token =
                    init_token(TOKEN_REGISTER,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               get_register_value(token_name));
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (is_valid_binary(token_name) || is_valid_decimal(token_name) || is_valid_hexadecimal(token_name)) {
                long decimal_value;
                if (is_valid_binary(token_name)) {
                    token_name += 2;
                    decimal_value = strtol(token_name, NULL, 2);
                    sprintf(token_name, "%ld", decimal_value);
                } else if (is_valid_hexadecimal(token_name)) {
                    token_name += 2;
                    decimal_value = strtol(token_name, NULL, 16);
                    sprintf(token_name, "%ld", decimal_value);
                }
                token_s
                    *token =
                    init_token(TOKEN_IMMEDIATE,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (check_instruction_type(token_name) != 'O') {
                token_s
                    *token =
                    init_token(TOKEN_INSTRUCTION,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (lexer->token_list->index > 0 && isalpha(token_name[0])
                ) {
                token_s
                    *token =
                    init_token(TOKEN_LABEL_REF,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            }
            free(token_name);
        }
        printf(BOLD "%s:%d:%d: " RED "[error]" RESET " Unexpected token\n",
               input_file_,
               lexer->line_number,
               lexer->character_number);
        printf("%5d | %s\n", lexer->line_number, lexer->line);
        printf("      |");
        print_space_n_times(lexer->character_number);
        printf(RED "^\n" RESET);

        error = 1;
        error_code_ = 1;
        break;
    }

    _Bool if_instruction = 0;

    for (unsigned int j = initial_size; j < lexer->token_list->index; j++) {
        if (lexer->token_list->list[j].token_type == TOKEN_INSTRUCTION) {
            if_instruction = 1;
            break;
        }
    }

    if (!if_instruction) {
        lexer->instruction_number--;
        return error;
    }

    

    return error;
}

void lex(lexer_s *lexer) {
    lexer->line = malloc(LEXER_LINE_BUFFER * sizeof(char));
    if (!lexer->line) {
        printf(RED "[error]" RESET "%s:1: failed to allocate memory for line buffer\n", input_file_);
        fclose(lexer->file);
        free(lexer);
        return;
    }

    while (fgets(lexer->line, LEXER_LINE_BUFFER, lexer->file) != NULL) {
        lexer->line_number++;

        size_t length = strlen(lexer->line);
        if (length > 0 && lexer->line[length - 1] == '\n') {
            lexer->line[length - 1] = '\0';
            length--;
        }

        size_t i = 0;
        while (i < length && (lexer->line[i] == ' ' || lexer->line[i] == '\t')) {
            i++;
        }
        if (i == length) {
            continue;
        }

        if (lexer->line[i] == '#' || lexer->line[i] == ';') {
            continue;
        }

        lexer->instruction_number++;
//        if (tokenize_line(lexer)) {
//            exit(1);
//        }
        tokenize_line(lexer);
    }


    free(lexer->line);
}

void free_lexer(lexer_s *lexer) {
    fclose(lexer->file);
    free(lexer);
}
