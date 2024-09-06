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
#include "../include/io.h"
#include "../include/globals.h"
#include "../include/parser.h"
#include "../include/instructions.h"

static uint hash(char *label_name) {
    uint hash = 0;
    for (uint i = 0; i < strlen(label_name); i++) {
        hash += label_name[i];
    }
    return hash;
}

label_table *init_label_table(uint size) {
    label_table *table = malloc(sizeof(label_table));
    table->size = size;
    table->table = malloc(size * sizeof(label_node));
    for (uint i = 0; i < size; i++) {
        table->table[i] = NULL;
    }
    return table;
}

label_node *init_label_node(char *label_name, uint instruction_number) {
    label_node *node = malloc(sizeof(label_node));
    node->label_name = label_name;
    node->instruction_number = instruction_number;
    node->next = NULL;
    return node;
}

void insert_label(label_table *table, char *label_name, uint instruction_number) {
    uint index = hash(label_name) % table->size;
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
    uint index = hash(label_name) % table->size;
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
    for (uint i = 0; i < table->size; i++) {
        label_node *current = table->table[i];
        while (current != NULL) {
            printf("<%s:%d>\n", current->label_name, current->instruction_number);
            current = current->next;
        }
    }
}

void free_label_table(label_table *table) {
    for (uint i = 0; i < table->size; i++) {
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

parser_s *init_parser(token_list_s *token_list) {
    parser_s *parser = malloc(sizeof(parser_s));
    parser->token_list = token_list;
    return parser;
}

void parse(parser_s *parser) {
    label_table *table = init_label_table(MAX_LABEL_TABLE_SIZE);

    // First pass to get all the labels
    parser->token_list->index = 0;
    while (parser->token_list->index < parser->token_list->size - 1) {
        if (parser->token_list->list[parser->token_list->index].token_type == TOKEN_LABEL) {
            uint labels_instruction = 0;
            uint i = parser->token_list->index + 1;
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
    while (parser->token_list->index < parser->token_list->size - 1) {
        uint current_instruction_number = parser->token_list->list[parser->token_list->index].instruction_number;
        token_list_s *instr_tokens = init_token_list_s(MAX_PARSER_TEMP_TOKEN_LINE);

        while (parser->token_list->list[parser->token_list->index].instruction_number == current_instruction_number) {
            instr_tokens->list[instr_tokens->index++] =
                    parser->token_list->list[parser->token_list->index++];
        }

        // print_token_list(instr_tokens);

        instr_tokens->size = instr_tokens->index;
        instr_tokens->index = 0;

        // Skip all the labels and paranthesis
        while ((instr_tokens->list[instr_tokens->index].token_type == TOKEN_LABEL
                || instr_tokens->list[instr_tokens->index].token_type == TOKEN_L_PAREN
                || instr_tokens->list[instr_tokens->index].token_type == TOKEN_R_PAREN)
               && instr_tokens->index < instr_tokens->size) {
            instr_tokens->index++;
        }

        if (instr_tokens->list[instr_tokens->index].token_type == TOKEN_INSTRUCTION) {
            char return_value = check_instruction_type(instr_tokens->list[instr_tokens->index].value);

            switch (return_value) {
                case 'O':
                    printf(BOLD "%s:%d:%d: " RED "[error]" RESET " Unknown instruction type\n",
                           input_file_,
                           instr_tokens->list[instr_tokens->index].line_number,
                           instr_tokens->list[instr_tokens->index].character_number);
                    printf("%5d | %s\n", instr_tokens->list[instr_tokens->index].line_number,
                           instr_tokens->list[instr_tokens->index].value);
                    printf("      |\n");
                    error_code_ = 1;
                    // exit(1);
                    break;
                case 'R':
                    if (valid_R_format(instr_tokens)) {
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10),
                                            strtol(instr_tokens->list[instr_tokens->index + 3].value, NULL, 10),
                                            0);
                    } else {
                        char buffer[100];
                        sprintf(buffer, "Expected: %s <reg> <reg> <reg>",
                                instr_tokens->list[instr_tokens->index].value);
                        print_error(input_file_,
                                    instr_tokens->list[instr_tokens->index].line_number,
                                    instr_tokens->list[instr_tokens->index].character_number,
                                    "Syntax Error",
                                    buffer);
                        error_code_ = 1;
                        // exit(1);
                    }
                    break;
                case 'I':
                    if (is_I_Bracket_instruction(instr_tokens->list[instr_tokens->index].value)
                        && valid_I_bracket_format(instr_tokens)) {
                        int immediate = strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10);
                        if (-2048 > immediate || immediate > 2047) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between -2048 and 2047");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            strtol(instr_tokens->list[instr_tokens->index + 4].value, NULL, 10),
                                            0,
                                            immediate);

                    } else if (!is_I_Bracket_instruction(instr_tokens->list[instr_tokens->index].value)
                               && valid_I_format(instr_tokens)) {

                        if (strcmp(instr_tokens->list[instr_tokens->index].value, "jalr") == 0) {

                        }

                        int immediate = strtol(instr_tokens->list[instr_tokens->index + 3].value, NULL, 10);
                        if (is_funct6_instruction(instr_tokens->list[instr_tokens->index].value) &&
                            (immediate < 0 || immediate > 63)) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between 0 and 63");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        if (-2048 > immediate || immediate > 2047) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between -2048 and 2047");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        if (strcmp(instr_tokens->list[instr_tokens->index].value, "srai") == 0) {
                            immediate += 1024;
                        }
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10),
                                            0,
                                            immediate);
                    } else if (strcmp(instr_tokens->list[instr_tokens->index].value, "ecall") == 0
                               && instr_tokens->size == instr_tokens->index + 1) {
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value, 0, 0, 0, 0);
                    } else if (strcmp(instr_tokens->list[instr_tokens->index].value, "ecall") == 0) {
                        char buffer[100];
                        sprintf(buffer, "Expected: %s , but got arguements",
                                instr_tokens->list[instr_tokens->index].value);

                        print_error(input_file_,
                                    instr_tokens->list[instr_tokens->index].line_number,
                                    instr_tokens->list[instr_tokens->index].character_number,
                                    "Syntax Error",
                                    buffer);
                        error_code_ = 1;
                    } else if (is_I_Bracket_instruction(instr_tokens->list[instr_tokens->index].value)) {
                        char buffer[100];
                        sprintf(buffer, "Expected: %s <reg> <imm>(<reg>)",
                                instr_tokens->list[instr_tokens->index].value);
                        print_error(input_file_,
                                    instr_tokens->list[instr_tokens->index].line_number,
                                    instr_tokens->list[instr_tokens->index].character_number,
                                    "Syntax Error",
                                    buffer);
                        error_code_ = 1;
                        // exit(1);
                    } else {
                        char buffer[100];
                        sprintf(buffer, "Expected: %s <reg> <reg> <imm>",
                                instr_tokens->list[instr_tokens->index].value);
                        print_error(input_file_,
                                    instr_tokens->list[instr_tokens->index].line_number,
                                    instr_tokens->list[instr_tokens->index].character_number,
                                    "Syntax Error",
                                    buffer);
                        error_code_ = 1;
                        // exit(1);
                    }
                    break;
                case 'S':
                    if (valid_S_format(instr_tokens)) {
                        int immediate = strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10);
                        if (-2048 > immediate || immediate > 2047) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between -2048 and 2047");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            0,
                                            strtol(instr_tokens->list[instr_tokens->index + 4].value, NULL, 10),
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            immediate);
                    } else {
                        char buffer[100];
                        sprintf(buffer, "Expected: %s <reg> <imm>(<reg>)",
                                instr_tokens->list[instr_tokens->index].value);
                        print_error(input_file_,
                                    instr_tokens->list[instr_tokens->index].line_number,
                                    instr_tokens->list[instr_tokens->index].character_number,
                                    "Syntax Error",
                                    buffer);
                        error_code_ = 1;
                        // exit(1);
                    }
                    break;
                case 'B':
                    if (valid_B_imm_format(instr_tokens)) {
                        int immediate = strtol(instr_tokens->list[instr_tokens->index + 3].value, NULL, 10);

                        if (-4096 > immediate || immediate > 4095) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between -4096 and 4095");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }

                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            0,
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10),
                                            strtol(instr_tokens->list[instr_tokens->index + 3].value, NULL, 10));
                    } else if (valid_B_label_format(instr_tokens)) {
                        int label_instruction =
                                search_label(table, instr_tokens->list[instr_tokens->index + 3].value);
                        if (label_instruction == -1) {
                            char buffer[100];
                            sprintf(buffer, "No label named " RED "%s" RESET,
                                    instr_tokens->list[instr_tokens->index + 3].value);
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 3].line_number,
                                        instr_tokens->list[instr_tokens->index + 3].character_number,
                                        "Undefined label",
                                        buffer);
                            error_code_ = 1;
                            // exit(1);
                        }
                        int immediate = 4
                                        * (label_instruction -
                                           instr_tokens->list[instr_tokens->index].instruction_number);

                        if (-4096 > immediate || immediate > 4095) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between -4096 and 4095");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            0,
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10),
                                            immediate);
                    } else {
                        char buffer[100];
                        sprintf(buffer, "Expected: %s <reg> <reg> <label>",
                                instr_tokens->list[instr_tokens->index].value);
                        print_error(input_file_,
                                    instr_tokens->list[instr_tokens->index].line_number,
                                    instr_tokens->list[instr_tokens->index].character_number,
                                    "Syntax Error",
                                    buffer);
                        error_code_ = 1;
                        // exit(1);
                    }
                    break;
                case 'U':
                    if (valid_U_format(instr_tokens)) {
                        int immediate = strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10);
                        if (0 > immediate || immediate > 1048575) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between 0 and 1048575");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            0,
                                            0,
                                            immediate);
                    } else {
                        char buffer[100];
                        sprintf(buffer, "Expected: %s <reg> <imm>",
                                instr_tokens->list[instr_tokens->index].value);
                        print_error(input_file_,
                                    instr_tokens->list[instr_tokens->index].line_number,
                                    instr_tokens->list[instr_tokens->index].character_number,
                                    "Syntax Error",
                                    buffer);
                        error_code_ = 1;
                        // exit(1);
                    }
                    break;
                case 'J':
                    if (valid_J_imm_format(instr_tokens)) {
                        int immediate = strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10);
                        if (-1048576 > immediate || immediate > 1048575) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between -1048576 and 1048575");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            0,
                                            0,
                                            strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10));

                    } else if (valid_J_label_format(instr_tokens)) {
                        int label_instruction =
                                search_label(table, instr_tokens->list[instr_tokens->index + 2].value);
                        if (label_instruction == -1) {
                            char buffer[100];
                            sprintf(buffer, "No label named " RED "%s" RESET,
                                    instr_tokens->list[instr_tokens->index + 2].value);
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Undefined label",
                                        buffer);
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        int immediate = 4
                                        * (label_instruction -
                                           instr_tokens->list[instr_tokens->index].instruction_number);
                        if (-1048576 > immediate || immediate > 1048575) {
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index + 2].line_number,
                                        instr_tokens->list[instr_tokens->index + 2].character_number,
                                        "Immediate value out of range",
                                        "Immediate value should be between -1048576 and 1048575");
                            error_code_ = 1;
                            continue;
                            // exit(1);
                        }
                        generate_bit_string(instr_tokens->list[instr_tokens->index].value,
                                            strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                            0,
                                            0,
                                            immediate);
                    } else {
                        char buffer[100];
                        sprintf(buffer, "Expected: %s <reg> <label>\n      |       or: %s <reg> <imm>",
                                instr_tokens->list[instr_tokens->index].value,
                                instr_tokens->list[instr_tokens->index].value);
                        print_error(input_file_,
                                    instr_tokens->list[instr_tokens->index].line_number,
                                    instr_tokens->list[instr_tokens->index].character_number,
                                    "Syntax Error",
                                    buffer);
                        error_code_ = 1;
                        //exit(1);
                    }
                    break;

                case 'P':
                    if (strcmp(instr_tokens->list[instr_tokens->index].value, "nop") == 0) {
                        if (instr_tokens->size == instr_tokens->index + 1) {
                            generate_bit_string("addi", 0, 0, 0, 0);
                        } else {
                            char buffer[100];
                            sprintf(buffer, "Expected: %s", instr_tokens->list[instr_tokens->index].value);
                            print_error(input_file_,
                                        instr_tokens->list[instr_tokens->index].line_number,
                                        instr_tokens->list[instr_tokens->index].character_number,
                                        "Syntax Error",
                                        buffer);
                            error_code_ = 1;
                            // exit(1);
                        }
                    }

                    if (strcmp(instr_tokens->list[instr_tokens->index].value, "mv") == 0) {
                        if (instr_tokens->list[instr_tokens->index + 1].token_type == TOKEN_REGISTER
                            && instr_tokens->list[instr_tokens->index + 2].token_type == TOKEN_REGISTER
                            && instr_tokens->size == instr_tokens->index + 3) {
                            generate_bit_string("addi",
                                                strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                                strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10),
                                                0,
                                                0);
                        } else {
                            printf(BOLD "%s:%d:%d: "RED "[error]" RESET " Syntax Error\n",
                                   input_file_,
                                   instr_tokens->list[instr_tokens->index].line_number,
                                   instr_tokens->list[instr_tokens->index].character_number);
                            printf("      | Expected: %s <reg> <reg>\n",
                                   instr_tokens->list[instr_tokens->index].value);
                            printf("      |\n");
                            error_code_ = 1;
                            // exit(1);
                        }
                    }

                    if (strcmp(instr_tokens->list[instr_tokens->index].value, "not") == 0) {
                        if (instr_tokens->list[instr_tokens->index + 1].token_type == TOKEN_REGISTER
                            && instr_tokens->list[instr_tokens->index + 2].token_type == TOKEN_REGISTER
                            && instr_tokens->size == instr_tokens->index + 3) {
                            generate_bit_string("xori",
                                                strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                                strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10),
                                                0,
                                                -1);
                        } else {
                            printf(BOLD "%s:%d:%d: "RED "[error]" RESET " Syntax Error\n",
                                   input_file_,
                                   instr_tokens->list[instr_tokens->index].line_number,
                                   instr_tokens->list[instr_tokens->index].character_number);
                            printf("      | Expected: %s <reg> <reg>\n",
                                   instr_tokens->list[instr_tokens->index].value);
                            printf("      |\n");
                            error_code_ = 1;
                            // exit(1);
                        }
                    }

                    if (strcmp(instr_tokens->list[instr_tokens->index].value, "neg") == 0) {
                        if (instr_tokens->list[instr_tokens->index + 1].token_type == TOKEN_REGISTER
                            && instr_tokens->list[instr_tokens->index + 2].token_type == TOKEN_REGISTER
                            && instr_tokens->size == instr_tokens->index + 3) {
                            generate_bit_string("sub",
                                                strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10),
                                                0,
                                                strtol(instr_tokens->list[instr_tokens->index + 2].value, NULL, 10),
                                                0);
                        } else {
                            printf(BOLD "%s:%d:%d: "RED "[error]" RESET " Syntax Error\n",
                                   input_file_,
                                   instr_tokens->list[instr_tokens->index].line_number,
                                   instr_tokens->list[instr_tokens->index].character_number);
                            printf("      | Expected: %s <reg> <reg>\n",
                                   instr_tokens->list[instr_tokens->index].value);
                            printf("      |\n");
                            error_code_ = 1;
                            // exit(1);
                        }
                    }

                    if (strcmp(instr_tokens->list[instr_tokens->index].value, "j") == 0) {
                        if (instr_tokens->list[instr_tokens->index + 1].token_type == TOKEN_IMMEDIATE
                            && instr_tokens->size == instr_tokens->index + 2) {
                            int immediate = strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL,
                                                   10);
                            if (-1048576 > immediate || immediate > 1048575) {
                                print_error(input_file_,
                                            instr_tokens->list[instr_tokens->index + 2].line_number,
                                            instr_tokens->list[instr_tokens->index + 2].character_number,
                                            "Immediate value out of range",
                                            "Immediate value should be between -1048576 and 1048575");
                                error_code_ = 1;
                                continue;
                            }
                            generate_bit_string("jal", 0, 0, 0,
                                                strtol(instr_tokens->list[instr_tokens->index + 1].value, NULL, 10));
                        } else if (instr_tokens->list[instr_tokens->index + 1].token_type == TOKEN_LABEL_REF
                                   && instr_tokens->size == instr_tokens->index + 2) {
                            int label_instruction =
                                    search_label(table, instr_tokens->list[instr_tokens->index + 1].value);
                            if (label_instruction == -1) {
                                char buffer[100];
                                sprintf(buffer, "No label named " RED "%s" RESET,
                                        instr_tokens->list[instr_tokens->index + 1].value);
                                print_error(input_file_,
                                            instr_tokens->list[instr_tokens->index + 2].line_number,
                                            instr_tokens->list[instr_tokens->index + 2].character_number,
                                            "Undefined label",
                                            buffer);
                                error_code_ = 1;
                                continue;
                            }
                            int immediate = 4
                                            * (label_instruction -
                                               instr_tokens->list[instr_tokens->index].instruction_number);
                            if (-1048576 > immediate || immediate > 1048575) {
                                print_error(input_file_,
                                            instr_tokens->list[instr_tokens->index + 2].line_number,
                                            instr_tokens->list[instr_tokens->index + 2].character_number,
                                            "Immediate value out of range",
                                            "Immediate value should be between -1048576 and 1048575");
                                error_code_ = 1;
                                continue;
                            }
                            generate_bit_string("jal",
                                                0,
                                                0,
                                                0,
                                                immediate);
                        } else {
                            printf(BOLD "%s:%d:%d: "RED "[error]" RESET " Syntax Error\n",
                                   input_file_,
                                   instr_tokens->list[instr_tokens->index].line_number,
                                   instr_tokens->list[instr_tokens->index].character_number);
                            printf("%5d | %s\n",
                                   instr_tokens->list[instr_tokens->index].line_number,
                                   read_line(input_file_,
                                             instr_tokens->list[instr_tokens->index].line_number));
                            printf("      | Expected: %s <label>\n",
                                   instr_tokens->list[instr_tokens->index].value);
                            printf("      |       or: %s <imm>\n",
                                   instr_tokens->list[instr_tokens->index].value);
                            printf("      |\n");
                            error_code_ = 1;
                        }
                    }
                    break;
                default:
                    break;
            }
        } else {
            printf(BOLD "%s:%d:%d: "RED "[error]" RESET " Unexpected token\n",
                   input_file_,
                   instr_tokens->list[instr_tokens->index].line_number,
                   instr_tokens->list[instr_tokens->index].character_number);
            printf("%5d | %s\n",
                   instr_tokens->list[instr_tokens->index].line_number,
                   read_line(input_file_, instr_tokens->list[instr_tokens->index].line_number));
            printf("      |");
            print_space_n_times(instr_tokens->list[instr_tokens->index].character_number);
            printf(RED "^\n" RESET);
            error_code_ = 1;
        }
    }
    free_label_table(table);
}

void free_parser(parser_s *parser) {
    free(parser);
}


