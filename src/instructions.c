/**
 * File Name: main.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include "../include/instructions.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../include/globals.h"
#include "../include/io.h"

R_Type add_instruction = {0b0000000, 0b000, 0b0110011};
R_Type sub_instruction = {0b0100000, 0b000, 0b0110011};
R_Type xor_instruction = {0b0000000, 0b100, 0b0110011};
R_Type or_instruction = {0b0000000, 0b110, 0b0110011};
R_Type and_instruction = {0b0000000, 0b111, 0b0110011};
R_Type sll_instruction = {0b0000000, 0b001, 0b0110011};
R_Type srl_instruction = {0b0000000, 0b101, 0b0110011};
R_Type sra_instruction = {0b0100000, 0b101, 0b0110011};

I_Type addi_instruction = {0b000, 0b0010011};
I_Type xori_instruction = {0b100, 0b0010011};
I_Type ori_instruction = {0b110, 0b0010011};
I_Type andi_instruction = {0b111, 0b0010011};
I_Type slli_instruction = {0b001, 0b0010011};
I_Type srli_instruction = {0b101, 0b0010011};
I_Type srai_instruction = {0b101, 0b0010011};

I_Type lb_instruction = {0b000, 0b0000011};
I_Type lh_instruction = {0b001, 0b0000011};
I_Type lw_instruction = {0b010, 0b0000011};
I_Type ld_instruction = {0b011, 0b0000011};
I_Type lbu_instruction = {0b100, 0b0000011};
I_Type lhu_instruction = {0b101, 0b0000011};
I_Type lwu_instruction = {0b110, 0b0000011};

I_Type jalr_instruction = {0b000, 0b1100111};

S_Type sb_instruction = {0b000, 0b0100011};
S_Type sh_instruction = {0b001, 0b0100011};
S_Type sw_instruction = {0b010, 0b0100011};
S_Type sd_instruction = {0b011, 0b0100011};

B_Type beq_instruction = {0b000, 0b1100011};
B_Type bne_instruction = {0b001, 0b1100011};
B_Type blt_instruction = {0b100, 0b1100011};
B_Type bge_instruction = {0b101, 0b1100011};
B_Type bltu_instruction = {0b110, 0b1100011};
B_Type bgeu_instruction = {0b111, 0b1100011};

U_Type lui_instruction = {0b0110111};
U_Type auipc_instruction = {0b0010111};

J_Type jal_instruction = {0b1101111};

int r_format[] = {0, 1, 1, 1};
int i_format[] = {0, 1, 1, 2};
int s_format[] = {0, 1, 2, 5, 1, 6};
int b_format[] = {0, 1, 1, 4};
int u_format[] = {0, 1, 2};
int j_format[] = {0, 1, 4};

// char *output_file = "output.hex";

char check_instruction_type(char *instruction) {
    if (strcmp(instruction, "add") == 0) {
        return 'R';
    } else if (strcmp(instruction, "sub") == 0) {
        return 'R';
    } else if (strcmp(instruction, "xor") == 0) {
        return 'R';
    } else if (strcmp(instruction, "or") == 0) {
        return 'R';
    } else if (strcmp(instruction, "and") == 0) {
        return 'R';
    } else if (strcmp(instruction, "sll") == 0) {
        return 'R';
    } else if (strcmp(instruction, "srl") == 0) {
        return 'R';
    } else if (strcmp(instruction, "sra") == 0) {
        return 'R';
    } else if (strcmp(instruction, "addi") == 0) {
        return 'I';
    } else if (strcmp(instruction, "xori") == 0) {
        return 'I';
    } else if (strcmp(instruction, "ori") == 0) {
        return 'I';
    } else if (strcmp(instruction, "andi") == 0) {
        return 'I';
    } else if (strcmp(instruction, "slli") == 0) {
        return 'I';
    } else if (strcmp(instruction, "srli") == 0) {
        return 'I';
    } else if (strcmp(instruction, "srai") == 0) {
        return 'I';
    } else if (strcmp(instruction, "jalr") == 0) {
        return 'I';
    } else if (strcmp(instruction, "lb") == 0) {
        return 'I';
    } else if (strcmp(instruction, "lh") == 0) {
        return 'I';
    } else if (strcmp(instruction, "lw") == 0) {
        return 'I';
    } else if (strcmp(instruction, "ld") == 0) {
        return 'I';
    } else if (strcmp(instruction, "lbu") == 0) {
        return 'I';
    } else if (strcmp(instruction, "lhu") == 0) {
        return 'I';
    } else if (strcmp(instruction, "lwu") == 0) {
        return 'I';
    } else if (strcmp(instruction, "sb") == 0) {
        return 'S';
    } else if (strcmp(instruction, "sh") == 0) {
        return 'S';
    } else if (strcmp(instruction, "sw") == 0) {
        return 'S';
    } else if (strcmp(instruction, "sd") == 0) {
        return 'S';
    } else if (strcmp(instruction, "beq") == 0) {
        return 'B';
    } else if (strcmp(instruction, "bne") == 0) {
        return 'B';
    } else if (strcmp(instruction, "blt") == 0) {
        return 'B';
    } else if (strcmp(instruction, "bge") == 0) {
        return 'B';
    } else if (strcmp(instruction, "bltu") == 0) {
        return 'B';
    } else if (strcmp(instruction, "bgeu") == 0) {
        return 'B';
    } else if (strcmp(instruction, "lui") == 0) {
        return 'U';
    } else if (strcmp(instruction, "auipc") == 0) {
        return 'U';
    } else if (strcmp(instruction, "jal") == 0) {
        return 'J';
    } else {
        return 'O';
    }
}

uint32_t R_format_binary_code(R_Type instruction, unsigned int rd, unsigned int rs1, unsigned int rs2) {
    unsigned int opcode = instruction.opcode;
    unsigned int funct3 = instruction.funct3;
    unsigned int funct7 = instruction.funct7;

    uint32_t machine_code = 0;
    machine_code |= (funct7 << 25); // funct7 occupies bits 31-25
    machine_code |= (rs2 << 20);    // rs2 occupies bits 24-20
    machine_code |= (rs1 << 15);    // rs1 occupies bits 19-15
    machine_code |= (funct3 << 12); // funct3 occupies bits 14-12
    machine_code |= (rd << 7);      // rd occupies bits 11-7
    machine_code |= opcode;         // opcode occupies bits 6-0

    return machine_code;
}

uint32_t I_format_binary_code(I_Type instruction, unsigned int rd, unsigned int rs1, int imm) {
    unsigned int opcode = instruction.opcode;
    unsigned int funct3 = instruction.funct3;

    uint32_t machine_code = 0;
    machine_code |= (imm << 20);    // imm occupies bits 31-20
    machine_code |= (rs1 << 15);    // rs1 occupies bits 19-15
    machine_code |= (funct3 << 12); // funct3 occupies bits 14-12
    machine_code |= (rd << 7);      // rd occupies bits 11-7
    machine_code |= opcode;         // opcode occupies bits 6-0

    return machine_code;
}
uint32_t S_format_binary_code(S_Type instruction, unsigned int rs1, unsigned int rs2, unsigned int imm) {
    unsigned int opcode = instruction.opcode;
    unsigned int funct3 = instruction.funct3;

    unsigned int imm_11_5 = (imm >> 5) & 0b1111111;
    unsigned int imm_4_0 = imm & 0b11111;

    uint32_t machine_code = 0;
    machine_code |= (imm_11_5 << 25); // imm[11:5] occupies bits 31-25
    machine_code |= (rs2 << 20);      // rs2 occupies bits 24-20
    machine_code |= (rs1 << 15);      // rs1 occupies bits 19-15
    machine_code |= (funct3 << 12);   // funct3 occupies bits 14-12
    machine_code |= (imm_4_0 << 7);   // imm[4:0] occupies bits 11-7
    machine_code |= opcode;           // opcode occupies bits 6-0

    return machine_code;
}

uint32_t B_format_binary_code(B_Type instruction, unsigned int rs1, unsigned int rs2, int imm) {
    unsigned int opcode = instruction.opcode;
    unsigned int funct3 = instruction.funct3;

    unsigned int imm_12 = (imm >> 12) & 0b1;
    unsigned int imm_10_5 = (imm >> 5) & 0b111111;
    unsigned int imm_4_1 = (imm >> 1) & 0b1111;
    unsigned int imm_11 = (imm >> 11) & 0b1;

    uint32_t machine_code = 0;
    machine_code |= (imm_12 << 31); // imm[12] occupies bit 31
    machine_code |= (imm_10_5 << 25); // imm[10:5] occupies bits 30-25
    machine_code |= (rs2 << 20);      // rs2 occupies bits 24-20
    machine_code |= (rs1 << 15);      // rs1 occupies bits 19-15
    machine_code |= (funct3 << 12);   // funct3 occupies bits 14-12
    machine_code |= (imm_4_1 << 8);   // imm[4:1] occupies bits 11-8
    machine_code |= (imm_11 << 7);    // imm[11] occupies bit 7
    machine_code |= opcode;           // opcode occupies bits 6-0

    return machine_code;
}

uint32_t U_format_binary_code(U_Type instruction, unsigned int rd, int imm) {
    unsigned int opcode = instruction.opcode;

    uint32_t machine_code = 0;
    machine_code |= (imm << 12); // imm occupies bits 31-12
    machine_code |= (rd << 7);   // rd occupies bits 11-7
    machine_code |= opcode;      // opcode occupies bits 6-0

    return machine_code;
}

uint32_t J_format_binary_code(J_Type instruction, unsigned int rd, int imm) {
    unsigned int opcode = instruction.opcode;

    unsigned int imm_20 = (imm >> 20) & 0b1;
    unsigned int imm_10_1 = (imm >> 1) & 0b1111111111;
    unsigned int imm_11 = (imm >> 11) & 0b1;
    unsigned int imm_19_12 = (imm >> 12) & 0b11111111;

    uint32_t machine_code = 0;
    machine_code |= (imm_20 << 31); // imm[20] occupies bit 31
    machine_code |= (imm_10_1 << 21); // imm[10:1] occupies bits 30-21
    machine_code |= (rd << 7);        // rd occupies bits 11-7
    machine_code |= (imm_11 << 20);   // imm[11] occupies bit 20
    machine_code |= (imm_19_12 << 12); // imm[19:12] occupies bits 19-12
    machine_code |= opcode;           // opcode occupies bits 6-0

    return machine_code;
}

void generate_bit_string(char *code, unsigned int rd, unsigned int rs1, unsigned int rs2, int imm) {
    /*
    printf("code: %s\n", code);
    printf("rd: %d\n", rd);
    printf("rs1: %d\n", rs1);
    printf("rs2: %d\n", rs2);
    printf("imm: %d\n", imm);
    //*/
    if (strcmp(code, "add") == 0) {
        uint32_t machine_code = R_format_binary_code(add_instruction, rd, rs1, rs2);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "sub") == 0) {
        uint32_t machine_code = R_format_binary_code(sub_instruction, rd, rs1, rs2);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "xor") == 0) {
        uint32_t machine_code = R_format_binary_code(xor_instruction, rd, rs1, rs2);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "or") == 0) {
        uint32_t machine_code = R_format_binary_code(or_instruction, rd, rs1, rs2);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "and") == 0) {
        uint32_t machine_code = R_format_binary_code(and_instruction, rd, rs1, rs2);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "sll") == 0) {
        uint32_t machine_code = R_format_binary_code(sll_instruction, rd, rs1, rs2);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "srl") == 0) {
        uint32_t machine_code = R_format_binary_code(srl_instruction, rd, rs1, rs2);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "sra") == 0) {
        uint32_t machine_code = R_format_binary_code(sra_instruction, rd, rs1, rs2);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "addi") == 0) {
        uint32_t machine_code = I_format_binary_code(addi_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "xori") == 0) {
        uint32_t machine_code = I_format_binary_code(xori_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "ori") == 0) {
        uint32_t machine_code = I_format_binary_code(ori_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "andi") == 0) {
        uint32_t machine_code = I_format_binary_code(andi_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "slli") == 0) {
        uint32_t machine_code = I_format_binary_code(slli_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "srli") == 0) {
        uint32_t machine_code = I_format_binary_code(srli_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "srai") == 0) {
        uint32_t machine_code = I_format_binary_code(srai_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "lb") == 0) {
        uint32_t machine_code = I_format_binary_code(lb_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "lh") == 0) {
        uint32_t machine_code = I_format_binary_code(lh_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "lw") == 0) {
        uint32_t machine_code = I_format_binary_code(lw_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "ld") == 0) {
        uint32_t machine_code = I_format_binary_code(ld_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "lbu") == 0) {
        uint32_t machine_code = I_format_binary_code(lbu_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "lhu") == 0) {
        uint32_t machine_code = I_format_binary_code(lhu_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "lwu") == 0) {
        uint32_t machine_code = I_format_binary_code(lwu_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "jalr") == 0) {
        uint32_t machine_code = I_format_binary_code(jalr_instruction, rd, rs1, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "sb") == 0) {
        uint32_t machine_code = S_format_binary_code(sb_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "sh") == 0) {
        uint32_t machine_code = S_format_binary_code(sh_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "sw") == 0) {
        uint32_t machine_code = S_format_binary_code(sw_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "sd") == 0) {
        uint32_t machine_code = S_format_binary_code(sd_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "beq") == 0) {
        uint32_t machine_code = B_format_binary_code(beq_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "bne") == 0) {
        uint32_t machine_code = B_format_binary_code(bne_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "blt") == 0) {
        uint32_t machine_code = B_format_binary_code(blt_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "bge") == 0) {
        uint32_t machine_code = B_format_binary_code(bge_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "bltu") == 0) {
        uint32_t machine_code = B_format_binary_code(bltu_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "bgeu") == 0) {
        uint32_t machine_code = B_format_binary_code(bgeu_instruction, rs1, rs2, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "jal") == 0) {
        uint32_t machine_code = J_format_binary_code(jal_instruction, rd, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "lui") == 0) {
        uint32_t machine_code = U_format_binary_code(lui_instruction, rd, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else if (strcmp(code, "auipc") == 0) {
        uint32_t machine_code = U_format_binary_code(auipc_instruction, rd, imm);
        // printf("%08x\n", machine_code);
        write_newline_to_file(output_file, machine_code);
    } else {
        printf("Invalid instruction\n");
        exit(1);
    }

}
