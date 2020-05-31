#ifndef TAC_H_INCLUDE
#define TAC_H_INCLUDE
#include "define.h"

enum op_type{
    OP_LABEL,
    OP_FUNCTION,
    OP_ASSIGN_,
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV,
    OP_GOTO,
    OP_EQ,      //==
    OP_NEQ,     //!=
    OP_JLE,     //<=
    OP_JLT,     //< 
    OP_JGE,     //>=
    OP_JGT,     //> 
    OP_F_EQ,    
    OP_F_NEQ,   
    OP_F_JLE,   
    OP_F_JLT,   
    OP_F_JGE,   
    OP_F_JGT,   
    OP_RETURN,
    OP_ARG,
    OP_CALL,
    OP_PARAM,
    OP_READ,
    OP_WRITE
};

enum opn_type {
    OPN_VAR,
    OPN_FUNC,
    OPN_LABEL,
    OPN_C_INT,
    OPN_C_FLOAT,
    OPN_C_CHAR
};

typedef struct operand      //操作数
{
    int opn_type;       //操作数类型, -1表示空
    union 
    {
        int const_int;
        float const_float;
        char id[ID_MAX_LENGTH];     //变量名或函数名或标签名
    };
    int offset;         //对于变量类型记录一下偏移量，用于目标代码生成
}operand;

typedef struct code_node
{
    int op_type;
    operand opn1, opn2, result;
    struct code_node *next, *prev;  //采用双向循环链表将中间代码链接起来
}code_node;


code_node *genIR_p(int op_type, operand *p_opn1, operand *p_opn2, operand *p_result);
code_node *genIR(int op_type, operand opn1, operand opn2, operand result);
code_node *genAssign(int op_type, int opn_index1, int opn_index2, int res_index);
code_node *merge(code_node *to, code_node *from);

char *newTemp();
char *newLabel();
void printIR(struct code_node *head);

#endif