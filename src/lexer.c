/**
 * File Name: lexer.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/globals.h"
#include "../include/instructions.h"
#include "../include/lexer.h"
#include "../include/utils.h"

lexer_struct *init_lexer(char *filename, token_list_struct *token_list) {
    lexer_struct *lexer = malloc(sizeof(lexer_struct));
    lexer->token_list = token_list;
    lexer->filename = filename;
    lexer->file = fopen(lexer->filename, "r");
    if (lexer->file == NULL) {
        printf("[-] Error: Unable to open file %s\n", lexer->filename);
        return NULL;
    }
    lexer->line = NULL;
    lexer->line_number = 0;
    lexer->character_number = 0;
    lexer->instruction_number = 0;
    return lexer;
}

_Bool tokenize_line(lexer_struct *lexer) {
    size_t i = 0;
    _Bool error = 0;
    unsigned int initial_size = lexer->token_list->size;
    while (i < strlen(lexer->line)) {
        lexer->character_number = i + 1;
        if ((lexer->line[i] == ' ') || (lexer->line[i] == '\t') || (lexer->line[i] == ',')) {
            i++;
            continue;
        } else if (lexer->line[i] == '#') {
            break;
        } else if (lexer->line[i] == '(') {
            token_struct *token =
                init_token(TOKEN_L_PAREN, lexer->line_number, lexer->character_number, lexer->instruction_number, "(");
            lexer->token_list->list[lexer->token_list->index++] = *token;
            lexer->token_list->size++;
            free(token);
            i++;
            continue;
        } else if (lexer->line[i] == ')') {
            token_struct *token =
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
            if ((token_name[j - 1] == ':') && (j > 1)) {
                token_name[j - 1] = '\0';
                token_struct
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
                token_struct
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
            } else if (is_valid_binary(token_name) || is_valid_decimal(token_name)
                || is_valid_hexadecimal(token_name)) {
                // TODO: handle minus and plus signs
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
                token_struct
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
                token_struct
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
            } else if ((lexer->token_list->list[lexer->token_list->index - 1].token_type == TOKEN_INSTRUCTION)
                || (lexer->token_list->list[lexer->token_list->index - 1].token_type == TOKEN_REGISTER)) {
                token_struct
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
        printf(RED "[error]" RESET " %s:%d:%d: Unknown token type\n",
               lexer->filename,
               lexer->line_number,
               lexer->character_number);
        printf("%6d| %s\n", lexer->line_number, lexer->line);
        printf("      |");
        print_space_n_times(lexer->character_number);
        printf(RED "^\n" RESET);

        error = 1;
        error_code = 1;
        break;
    }

    _Bool only_labels_or_parentheses = 1;
    // check if all tokens are labels or parentheses
    for (unsigned int j = initial_size; j < lexer->token_list->index; j++) {
        if (lexer->token_list->list[j].token_type != TOKEN_LABEL
            && lexer->token_list->list[j].token_type != TOKEN_L_PAREN
            && lexer->token_list->list[j].token_type != TOKEN_R_PAREN) {
            only_labels_or_parentheses = 0;
            break;
        }
    }
    if (only_labels_or_parentheses) {
        // subtract 1 from instruction number as only labels or parentheses are not "instructions"
        lexer->instruction_number--;
        return error;
    }

    return error;
}

void lex(lexer_struct *lexer) {
    lexer->line = malloc(LEXER_LINE_BUFFER * sizeof(char));
    if (!lexer->line) {
        printf(RED "[error]" RESET "%s:1: failed to allocate memory for line buffer\n", lexer->filename);
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

        if (lexer->line[i] == '#') {
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

void free_lexer(lexer_struct *lexer) {
    fclose(lexer->file);
    free(lexer);
}
