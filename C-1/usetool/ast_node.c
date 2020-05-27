#include "ast_node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.tab.h"

ast_node *create_node(int lineno, char *name, ast_node *child, ast_node *sibling){
    ast_node *n = (ast_node *)malloc(sizeof(ast_node));

    n->is_leaf = 'N';
    n->lineno = lineno;
    n->child = child;
    n->sibling = sibling;

    strncpy(n->name, name, AST_NODE_NAME_MAX_LENGTH);
    return n;
}
ast_leaf *create_leaf(int lineno, int leaf_type, ast_node *child, ast_node *sibling){
    ast_leaf *n = (ast_leaf *)malloc(sizeof(ast_leaf));

    n->is_leaf='Y';
    n->lineno = lineno;
    n->child = child;
    n->sibling = sibling;
    
    n->leaf_type = leaf_type;
    return n;
}

void display(ast_node *root, int indent){
    ast_node *c, *s;
    c = root->child;
    s = root->sibling;

    printf("%*s", indent, "");
    if(root->is_leaf == 'N'){
        printf("%s(%d)\n", root->name, root->lineno);
    }else{
        // printf("%*shello world\n", indent, "");
        ast_leaf *r = (ast_leaf *)root;
        switch (r->leaf_type)
        {
        case ID:
            printf("%s: %s\n", "ID", r->type_id);
            break;
        case C_INT:
            printf("%s: %d\n", "Int", r->type_int);
            break;
        case C_FLOAT:
            printf("%s: %f\n", "Float", r->type_float);
            break;
        case C_CHAR:
            printf("%s: %c\n", "Char", r->type_char);
            break;
        case INT:
            printf("%s: %s\n", "Type", "int");
            break;
        case FLOAT:
            printf("%s: %s\n", "Type", "float");
            break;
        case CHAR:
            printf("%s: %s\n", "Type", "char");
            break;
        case BOOL:
            printf("%s: %s\n", "Type", "bool");
            break;
        case TRUE:
            printf("%s: %s\n", "Bool", "ture");
            break;
        case FALSE:
            printf("%s: %s\n", "Bool", "false");
            break;
        default:
            break;
        }
    }

    if(c!=NULL)
        display(c, indent+4);
    if(s!=NULL)
        display(s, indent);
}

void ast_free(ast_node *root){
    ast_node *c, *s;
    c = root->child;
    s = root->sibling;
    if(c!=NULL){
        ast_free(c);
    }
    if(s!=NULL)
        ast_free(s);

    free(root);
}