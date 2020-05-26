#ifndef AST_NODE_H_INCLUDE
#define AST_NODE_H_INCLUDE

#define ID_MAX_LENGTH 33
#define AST_NODE_NAME_MAX_LENGTH 20
typedef struct ast_node
{
    char is_leaf;                           //'N'
    int lineno;                             //节点所在行号
    struct ast_node *child, *sibling;
    char name[AST_NODE_NAME_MAX_LENGTH];    //标识节点名称，如Exp+Exp可产生一个名为"+"的内部节点
}ast_node;

typedef struct ast_leaf
{
    char is_leaf;                           //'Y'
    int lineno;                             //节点所在行号
    struct ast_node *child, *sibling;
    int leaf_type;                          //用于标识采用下面联合体的哪个属性，true, false节点特殊，leaf_type直接可以表示其值。
    union
    {
        char type_char;
        int type_int;
        float type_float;
        char type_id[ID_MAX_LENGTH];
    };
}ast_leaf;

ast_node *create_node(int lineno, char *name, ast_node *child, ast_node *sibling);
ast_leaf *create_leaf(int lineno, int leaf_type, ast_node *child, ast_node *sibling);

void display(ast_node *root, int indent);

void ast_free(ast_node *root);
#endif