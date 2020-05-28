#ifndef AST_NODE_H_INCLUDE
#define AST_NODE_H_INCLUDE
#define AST_NODE_NAME_MAX_LENGTH 8
#include "define.h"

enum node_type {
    //Exp
    NT_PLUS,
    NT_MINUS,
    NT_MULT,
    NT_DIV,
    NT_MOD,
    NT_REL_OP1, //==
    NT_REL_OP2, //!=
    NT_REL_OP3, //<=
    NT_REL_OP4, //< 
    NT_REL_OP5, //>=
    NT_REL_OP6, //> 
    NT_S_PLUS_L,
    NT_S_PLUS_R,
    NT_S_MINUS_L,
    NT_S_MINUS_R,
    NT_AND,
    NT_OR,
    NT_NOT,
    NT_NEG,
    NT_PARENTHESIS,
    NT_COMMA,
    NT_FUNC_CALL,
    NT_DOT,
    //stmt
    NT_STMT_EXP,
    NT_STMT_ASSIGN,
    NT_STMT_OP_ASSIGN1, //+=
    NT_STMT_OP_ASSIGN2, //-=
    NT_STMT_OP_ASSIGN3, //*=
    NT_STMT_OP_ASSIGN4, // /=
    NT_STMT_IF,
    NT_STMT_IF_ELSE,
    NT_STMT_WHILE,
    NT_STMT_RETURN,
    NT_COMP_ST,
    //top level
    NT_PROGRAM,
    NT_VAR_DEC,
    NT_VAR_ASSIGN,
    NT_VAR_ARRAY,

    NT_FUNC_DEF,
    NT_FUNC_HEADER,

    NT_PARAM,
    NT_RPARAM,
    NT_STRUCT,
    NT_TAG_OPT
};

typedef struct ast_node
{
    char is_leaf;                           //0
    int lineno;                             //节点所在行号
    struct ast_node *child, *sibling;
    int node_type;
    char name[AST_NODE_NAME_MAX_LENGTH];    //节点名称，如Exp+Exp可产生一个名为"+"的内部节点
}ast_node;

typedef struct ast_leaf
{
    char is_leaf;                           //1
    int lineno;                             //节点所在行号
    struct ast_node *child, *sibling;
    int leaf_type;                          //用于标识采用下面联合体的哪个属性
                                            //取值：INT, FLOAT, CHAR , VOID, BOOL, TRUE, FALSE, ID, C_INT, C_FLOAT, C_CHAR
    union
    {
        char type_char;
        int type_int;
        float type_float;
        char type_id[ID_MAX_LENGTH];
    };
}ast_leaf;

ast_node *create_node2(int lineno, int node_type, char *name, ast_node *child, ast_node *sibling);
ast_node *create_node(int lineno, char *name, ast_node *child, ast_node *sibling);
ast_leaf *create_leaf(int lineno, int leaf_type, ast_node *child, ast_node *sibling);

void display(ast_node *root, int indent);

void ast_free(ast_node *root);
#endif