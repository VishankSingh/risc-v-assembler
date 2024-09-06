/**
 * File Name: parser.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "instructions.h"
#include "tokens.h"

#define uint unsigned int

typedef struct Parser {
    token_list_s *token_list;
} parser_s;

typedef struct LabelNode {
    char *label_name;
    uint instruction_number;
    struct LabelNode *next;
} label_node;

typedef struct LabelTable {
    uint size;
    label_node **table;
} label_table;

label_table *init_label_table(uint size);

label_node *init_label_node(char *label_name, uint instruction_number);

void insert_label(label_table *table, char *label_name, uint instruction_number);

int search_label(label_table *table, char *label_name);

void print_label_table(label_table *table);

void free_label_table(label_table *table);

parser_s *init_parser(token_list_s *token_list);

void parse(parser_s *parser);

void free_parser(parser_s *parser);

#endif // PARSER_H