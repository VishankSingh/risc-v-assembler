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

typedef struct Parser {
    token_list_struct *token_list;
    char *filename;
} parser_struct;

typedef struct LabelNode {
    char *label_name;
    unsigned int instruction_number;
    struct LabelNode *next;
} label_node;

typedef struct LabelTable {
    unsigned int size;
    label_node **table;
} label_table;

label_table *init_label_table(unsigned int size);

label_node *init_label_node(char *label_name, unsigned int instruction_number);

void insert_label(label_table *table, char *label_name, unsigned int instruction_number);

int search_label(label_table *table, char *label_name);

void print_label_table(label_table *table);

void free_label_table(label_table *table);

parser_struct *init_parser(token_list_struct *token_list, char *filename);

void parse(parser_struct *parser);

void free_parser(parser_struct *parser);

#endif // PARSER_H