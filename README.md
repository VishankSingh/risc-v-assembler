
# RISC-V Assembler

## Description

RISC-V assembler as part of CS2323-Computer Architecture Fall 2024 Lab project.
This has limited capabilities, able to handle R,I,S,U,B,J instructions.  

## Usage

`./riscv_asm`  

By default takes `input.s` as input and output in `output.hex`  
Custom input and output can be given using `./riscv_asm custom_input.s -o=custom_output.hex`

## Code base information
- main.c: caller for assembler 
- assembler.h assembler.c: assembler file creating the pipeline
- globals.h globals.c: file storing global macros and variable
- instructions.h instructions.c: storing the instruction bit data and utility functions
- io.h io.c: contains input output utility functions 
- lexer.h lexer.c: lexer which tokenizes the input code
- parser.h parser.c: takes tokenlist and check for errors and uses code generation code to generate hex
- tokens.h tokens.c: token and token list struct and their utility functions
- utils.h utils.c: contains utility functions needed by all files

