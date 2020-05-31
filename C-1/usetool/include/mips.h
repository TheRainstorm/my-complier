#ifndef MIPS_H_INCLUDE
#define MIPS_H_INCLUDE
#include "tac.h"
#include <stdio.h>

extern int REGS[32];
/*
$0      zero
$1      at
$2-$3   v0-v1
$4-$7   a0-a3
$8-$15  t0-t7
$16-$23 s0-s7
$24-$25 t8-t9
$28     gp
$29     sp
$30     fp
$31     ra
*/

////寄存器描述符
//struct reg_des
//{
//    int is_used;
//    char id[20];
//};
//
////地址描述符
//struct var_des
//{
//    int offset; //变量相对于sp的偏移量 sp + offset
//    int regn;   //寄存器标号
//};
//
//extern int sp_offset;


void init_code(FILE* file);
void genMips(code_node *head, FILE *file);

//int get_temp_reg(char* name); 
//int find_temp_reg(char *name);

#endif