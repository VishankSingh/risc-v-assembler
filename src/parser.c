/**
 * File Name: parser.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/utils.h"
#include "../include/globals.h"
#include "../include/parser.h"
#include "../include/instructions.h"

static unsigned int hash(char *label_name) {
    unsigned int hash = 0;
    for (unsigned int i = 0; i < strlen(label_name); i++) {
        hash += label_name[i];
    }
    return hash;
}

label_table *init_label_table(unsigned int size) {
    label_table *table = malloc(sizeof(label_table));
    table->size = size;
    table->table = malloc(size * sizeof(label_node));
    for (unsigned int i = 0; i < size; i++) {
        table->table[i] = NULL;
    }
    return table;
}

label_node *init_label_node(char *label_name, unsigned int instruction_number) {
    label_node *node = malloc(sizeof(label_node));
    node->label_name = label_name;
    node->instruction_number = instruction_number;
    node->next = NULL;
    return node;
}

void insert_label(label_table *table, char *label_name, unsigned int instruction_number) {
    unsigned int index = hash(label_name) % table->size;
    label_node *node = init_label_node(label_name, instruction_number);
    if (table->table[index] == NULL) {
        table->table[index] = node;
    } else {
        label_node *current = table->table[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
}

int search_label(label_table *table, char *label_name) {
    unsigned int index = hash(label_name) % table->size;
    label_node *current = table->table[index];
    while (current != NULL) {
        if (strcmp(current->label_name, label_name) == 0) {
            return current->instruction_number;
        }
        current = current->next;
    }
    return -1;
}

void print_label_table(label_table *table) {
    for (unsigned int i = 0; i < table->size; i++) {
        label_node *current = table->table[i];
        while (current != NULL) {
            printf("<%s:%d>\n", current->label_name, current->instruction_number);
            current = current->next;
        }
    }
}

void free_label_table(label_table *table) {
    for (unsigned int i = 0; i < table->size; i++) {
        label_node *current = table->table[i];
        while (current != NULL) {
            label_node *temp = current;
            current = current->next;
            free(temp->label_name);
            free(temp);
        }
    }
    free(table->table);
    free(table);
}

parser_struct *init_parser(token_list_struct *token_list, char *filename) {
    parser_struct *parser = malloc(sizeof(parser_struct));
    parser->token_list = token_list;
    parser->filename = filename;
    return parser;
}
// TODO: Implement the parser, refine the parser, add error checking
void parse(parser_struct *parser) {
    label_table *table = init_label_table(MAX_LABEL_TABLE_SIZE);

    // First pass to get all the labels
    parser->token_list->index = 0;
    while (parser->token_list->index < parser->token_list->size) {

        if (parser->token_list->list[parser->token_list->index].token_type == TOKEN_LABEL) {
            unsigned int labels_instruction = 0;
            unsigned int i = parser->token_list->index + 1;
            while (i < parser->token_list->size) {
                if (parser->token_list->list[i].token_type == TOKEN_INSTRUCTION) {
                    labels_instruction = parser->token_list->list[i].instruction_number;
                    break;
                }
                i++;
            }
            insert_label(table, parser->token_list->list[parser->token_list->index].value, labels_instruction);
        }
        parser->token_list->index++;
    }

    parser->token_list->index = 0;
    // print_label_table(table);

    // Second pass to parse the instructions tokens
    while (parser->token_list->index < parser->token_list->size) {
        unsigned int
            current_instruction_number = parser->token_list->list[parser->token_list->index].instruction_number;
        token_list_struct *instruction_tokens = init_token_list_struct(MAX_PARSER_TEMP_TOKEN_LINE);

        while (parser->token_list->list[parser->token_list->index].instruction_number == current_instruction_number) {
            instruction_tokens->list[instruction_tokens->index++] =
                parser->token_list->list[parser->token_list->index++];
        }

        // print_token_list(instruction_tokens);

        instruction_tokens->size = instruction_tokens->index;
        instruction_tokens->index = 0;

        while (instruction_tokens->list[instruction_tokens->index].token_type == TOKEN_LABEL
            || instruction_tokens->list[instruction_tokens->index].token_type == TOKEN_L_PAREN
            || instruction_tokens->list[instruction_tokens->index].token_type == TOKEN_R_PAREN) {
            instruction_tokens->index++;
        }

        if (instruction_tokens->list[instruction_tokens->index].token_type == TOKEN_INSTRUCTION) {
            char return_value = check_instruction_type(instruction_tokens->list[instruction_tokens->index].value);
            if (return_value == 'O') {
                printf(RED "[error]" RESET " %s:%d:%d: Unknown instruction type\n",
                       parser->filename,
                       instruction_tokens->list[instruction_tokens->index].line_number,
                       instruction_tokens->list[instruction_tokens->index].character_number);
                printf("%6d| %s\n", instruction_tokens->list[instruction_tokens->index].line_number,
                       instruction_tokens->list[instruction_tokens->index].value);
                printf("      |\n");
                error_code = 1;
                // exit(1);
            } else if (return_value == 'R') {
                if (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_REGISTER
                    && instruction_tokens->size == instruction_tokens->index + 4) {
                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10),
                                        strtol(instruction_tokens->list[instruction_tokens->index + 3].value, NULL, 10),
                                        0);
                } else {
                    printf(RED "[error]" RESET " %s:%d:%d: Syntax Error\n",
                           parser->filename,
                           instruction_tokens->list[instruction_tokens->index].line_number,
                           instruction_tokens->list[instruction_tokens->index].character_number);
                    printf("      | Expected: %s <reg> <reg> <reg>\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |\n");
                    error_code = 1;
                    // exit(1);
                }
            } else if (return_value == 'I') {
                if (((strcmp(instruction_tokens->list[instruction_tokens->index].value, "jalr") == 0)
                    || (strcmp(instruction_tokens->list[instruction_tokens->index].value, "lb") == 0)
                    || (strcmp(instruction_tokens->list[instruction_tokens->index].value, "lh") == 0)
                    || (strcmp(instruction_tokens->list[instruction_tokens->index].value, "lw") == 0)
                    || (strcmp(instruction_tokens->list[instruction_tokens->index].value, "ld") == 0)
                    || (strcmp(instruction_tokens->list[instruction_tokens->index].value, "lbu") == 0)
                    || (strcmp(instruction_tokens->list[instruction_tokens->index].value, "lhu") == 0)
                    || (strcmp(instruction_tokens->list[instruction_tokens->index].value, "lwu") == 0))
                    && instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_IMMEDIATE
                    && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_L_PAREN
                    && instruction_tokens->list[instruction_tokens->index + 4].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 5].token_type == TOKEN_R_PAREN
                    && instruction_tokens->size == instruction_tokens->index + 6) {
                    int immediate = strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10);
                    if (is_unsigned_instruction(instruction_tokens->list[instruction_tokens->index].value) && (immediate < 0 || immediate > 4095)) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 3].line_number,
                               instruction_tokens->list[instruction_tokens->index + 3].character_number);
                        printf("      | Immediate value should be 0 and 4095\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    if (-2048 > immediate || immediate > 2047) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 2].line_number,
                               instruction_tokens->list[instruction_tokens->index + 2].character_number);
                        printf("      | Immediate value should be between -2048 and 2047\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        strtol(instruction_tokens->list[instruction_tokens->index + 4].value, NULL, 10),
                                        0,
                                        immediate);

                } else if (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_IMMEDIATE
                    && instruction_tokens->size == instruction_tokens->index + 4) {
                    int immediate = strtol(instruction_tokens->list[instruction_tokens->index + 3].value, NULL, 10);
                    if (is_unsigned_instruction(instruction_tokens->list[instruction_tokens->index].value) && (immediate < 0 || immediate > 4095)) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 3].line_number,
                               instruction_tokens->list[instruction_tokens->index + 3].character_number);
                        printf("      | Immediate value should be between 0 and 4095\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    if (is_funct6_instruction(instruction_tokens->list[instruction_tokens->index].value) && (immediate < 0 || immediate > 63)) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 3].line_number,
                               instruction_tokens->list[instruction_tokens->index + 3].character_number);
                        printf("      | Immediate value should be between 0 and 63\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    if (-2048 > immediate || immediate > 2047) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 2].line_number,
                               instruction_tokens->list[instruction_tokens->index + 2].character_number);
                        printf("      | Immediate value should be between -2048 and 2047\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    if (strcmp(instruction_tokens->list[instruction_tokens->index].value, "srai") == 0) {
                        immediate += 1024;
                    }
                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10),
                                        0,
                                        immediate);
                } else if (strcmp(instruction_tokens->list[instruction_tokens->index].value, "jalr") == 0) {
                    printf(RED "[error]" RESET " %s:%d:%d: Syntax Error\n",
                           parser->filename,
                           instruction_tokens->list[instruction_tokens->index].line_number,
                           instruction_tokens->list[instruction_tokens->index].character_number);
                    printf("      | Expected: %s <reg> <imm>(<reg>)\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |       or: %s <reg> <reg> <imm>\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |\n");
                    error_code = 1;
                    // exit(1);
                } else {
                    printf(RED "[error]" RESET " %s:%d:%d: Syntax Error\n",
                           parser->filename,
                           instruction_tokens->list[instruction_tokens->index].line_number,
                           instruction_tokens->list[instruction_tokens->index].character_number);
                    printf("      | Expected: %s <reg> <reg> <imm>\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |\n");
                    error_code = 1;
                    // exit(1);
                }
            } else if (return_value == 'S') {
                if (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_IMMEDIATE
                    && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_L_PAREN
                    && instruction_tokens->list[instruction_tokens->index + 4].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 5].token_type == TOKEN_R_PAREN
                    && instruction_tokens->size == instruction_tokens->index + 6) {
                    int immediate = strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10);
                    if (-2048 > immediate || immediate > 2047) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 2].line_number,
                               instruction_tokens->list[instruction_tokens->index + 2].character_number);
                        printf("      | Immediate value should be between -2048 and 2047\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        0,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 4].value, NULL, 10),
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        immediate);
                } else {
                    printf(RED "[error]" RESET " %s:%d:%d: Syntax Error\n",
                           parser->filename,
                           instruction_tokens->list[instruction_tokens->index].line_number,
                           instruction_tokens->list[instruction_tokens->index].character_number);
                    printf("      | Expected: %s <reg> <imm>(<reg>)\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |\n");
                    error_code = 1;
                    // exit(1);
                }
            } else if (return_value == 'B') {
                if (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_IMMEDIATE
                    && instruction_tokens->size == instruction_tokens->index + 4) {
                    int immediate = strtol(instruction_tokens->list[instruction_tokens->index + 3].value, NULL, 10);

                    if (is_unsigned_instruction(instruction_tokens->list[instruction_tokens->index].value)
                        && (immediate < 0 || immediate > 4095)) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 3].line_number,
                               instruction_tokens->list[instruction_tokens->index + 3].character_number);
                        printf("      | Immediate value should be between 0 and 4095\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    if (-4096 > immediate || immediate > 4095) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 3].line_number,
                               instruction_tokens->list[instruction_tokens->index + 3].character_number);
                        printf("      | Immediate value should be between -2048 and 2047\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }

                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        0,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10),
                                        strtol(instruction_tokens->list[instruction_tokens->index + 3].value, NULL, 10));
                } else if (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_LABEL_REF
                    && instruction_tokens->size == instruction_tokens->index + 4) {
                    int label_instruction =
                        search_label(table, instruction_tokens->list[instruction_tokens->index + 3].value);
                    if (label_instruction == -1) {
                        printf(RED "[error]" RESET " %s:%d:%d: error: Undefined label\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 3].line_number,
                               instruction_tokens->list[instruction_tokens->index + 3].character_number);
                        printf("      | No label named " RED "%s" RESET "\n",
                               instruction_tokens->list[instruction_tokens->index + 3].value);
                    printf("      |\n");
                    error_code = 1;
                        // exit(1);
                    }
                    int immediate = 4
                        * (label_instruction - instruction_tokens->list[instruction_tokens->index].instruction_number);

                    if (is_unsigned_instruction(instruction_tokens->list[instruction_tokens->index].value)
                        && (immediate < 0 || immediate > 4095)) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 3].line_number,
                               instruction_tokens->list[instruction_tokens->index + 3].character_number);
                        printf("      | Immediate value should be between 0 and 4095\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    if (-2048 > immediate || immediate > 2047) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 3].line_number,
                               instruction_tokens->list[instruction_tokens->index + 3].character_number);
                        printf("      | Immediate value should be between -2048 and 2047\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        0,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10),
                                        immediate);
                } else {
                    printf(RED "[error]" RESET " %s:%d:%d: Syntax Error\n",
                           parser->filename,
                           instruction_tokens->list[instruction_tokens->index].line_number,
                           instruction_tokens->list[instruction_tokens->index].character_number);
                    printf("      | Expected: %s <reg> <reg> <label>\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |\n");
                    error_code = 1;
                    // exit(1);
                }
            } else if (return_value == 'U') {
                if (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_IMMEDIATE
                    && instruction_tokens->size == instruction_tokens->index + 3) {
                    // TODO: if negative value, check if it is working
                    int immediate = strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10);
                    if (0 > immediate || immediate > 1048575) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 2].line_number,
                               instruction_tokens->list[instruction_tokens->index + 2].character_number);
                        printf("      | Immediate value should be between 0 and 1048575\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        0,
                                        0,
                                        immediate);
                } else {
                    printf(RED "[error]" RESET " %s:%d:%d: Syntax Error\n",
                           parser->filename,
                           instruction_tokens->list[instruction_tokens->index].line_number,
                           instruction_tokens->list[instruction_tokens->index].character_number);
                    printf("      | Expected: %s <reg> <imm>\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |\n");
                    error_code = 1;
                    // exit(1);
                }
            } else if (return_value == 'J') {
                if (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_IMMEDIATE
                    && instruction_tokens->size == instruction_tokens->index + 3) {
                    int immediate = strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10);
                    if (-1048576 > immediate || immediate > 1048575 ) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 2].line_number,
                               instruction_tokens->list[instruction_tokens->index + 2].character_number);
                        printf("      | Immediate value should be between 0 and 1048575\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        0,
                                        0,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 2].value, NULL, 10));

                } else if (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                    && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_LABEL_REF
                    && instruction_tokens->size == instruction_tokens->index + 3) {
                    int label_instruction =
                        search_label(table, instruction_tokens->list[instruction_tokens->index + 2].value);
                    if (label_instruction == -1) {
                        printf(RED "[error]" RESET " %s:%d:%d: error: Undefined label\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 2].line_number,
                               instruction_tokens->list[instruction_tokens->index + 2].character_number);
                        printf("      | No label named " RED "%s" RESET "\n",
                               instruction_tokens->list[instruction_tokens->index + 2].value);
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    int immediate = 4
                        * (label_instruction - instruction_tokens->list[instruction_tokens->index].instruction_number);
                    if (-1048576 > immediate || immediate > 1048575) {
                        printf(RED "[error]" RESET " %s:%d:%d: Immediate value out of range\n",
                               parser->filename,
                               instruction_tokens->list[instruction_tokens->index + 2].line_number,
                               instruction_tokens->list[instruction_tokens->index + 2].character_number);
                        printf("      | Immediate value should be between -1048576 and 1048575\n");
                        printf("      |\n");
                        error_code = 1;
                        continue;
                        // exit(1);
                    }
                    generate_bit_string(instruction_tokens->list[instruction_tokens->index].value,
                                        strtol(instruction_tokens->list[instruction_tokens->index + 1].value, NULL, 10),
                                        0,
                                        0,
                                        immediate);
                } else {
                    printf(RED "[error]" RESET " %s:%d:%d: Syntax Error\n",
                           parser->filename,
                           instruction_tokens->list[instruction_tokens->index].line_number,
                           instruction_tokens->list[instruction_tokens->index].character_number);
                    printf("      | Expected: %s <reg> <label>\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |       or: %s <reg> <imm>\n", instruction_tokens->list[instruction_tokens->index].value);
                    printf("      |\n");
                    error_code = 1;
                    //exit(1);
                }
            }
        }
    }
    free_label_table(table);
}

void free_parser(parser_struct *parser) {
    free(parser);
}


