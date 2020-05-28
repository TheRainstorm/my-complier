#include "ast_node.h"
#include "define.h"
#include "symbol_table.h"
#include "parse.tab.h"
#include <stdio.h>
#include <stdlib.h>

int analysis_func_call(ast_node *node){
    int type, index;
    ast_leaf *t;
    t = (ast_leaf *)(node->child);

    //查找符号表，寻找是否有该函数
    index = linear_search_symbol_table(t->type_id);
    if(index==-1){
        print_error(node->lineno, "函数(%s)未定义", t->type_id);
        return -1;   //该情况下可能会导致报类型不匹配错误
    }

    int param_cnt=0;
    //遍历所有实参
    ast_node *n;
    n = t->sibling;
    while(n){
        param_cnt++;
        n = n->sibling;
    }

    if(param_cnt!=ST.stack[index].dimen){
        print_error(node->lineno, "函数(%s)参数不匹配, 期望的是%d个, 而%d个被给予", t->type_id,\
                 ST.stack[index].dimen, param_cnt);
    }
    return ST.stack[index].type;
}

int analysis_exp(ast_node *exp){
    int type1, type2, index;
    if(exp->is_leaf==0){
        switch (exp->node_type)
        {
        case NT_PLUS :
        case NT_MINUS:
        case NT_MULT :
        case NT_MOD  :
        case NT_REL_OP1:
        case NT_REL_OP2:
        case NT_REL_OP3:
        case NT_REL_OP4:
        case NT_REL_OP5:
        case NT_REL_OP6:
        case NT_COMMA:
        //bool
        case NT_AND:
        case NT_OR:
            type1 = analysis_exp(exp->child);
            type2 = analysis_exp(exp->child->sibling);
            if(type1==0 && type2==1 || type1==1 && type2==0){
                print_error(exp->lineno, "算数表达式中，int和float不能隐式转换");
            }
            break;
        case NT_S_PLUS_L:
        case NT_S_PLUS_R:
        case NT_S_MINUS_L:
        case NT_S_MINUS_R:
        case NT_NEG:
        case NT_PARENTHESIS:
        case NT_NOT:
            return analysis_exp(exp->child);
        case NT_FUNC_CALL:
            return analysis_func_call(exp);
            break;
        case NT_DOT:
            //暂时不处理结构体
            return -1;
            break;
        default:
            break;
        }
    }else{
        ast_leaf *t;
        t = (ast_leaf *)exp;
        switch (t->leaf_type)
        {
        case ID:
            index = linear_search_symbol_table(t->type_id);
            if(index==-1){
                print_error(exp->lineno, "变量(%s)未定义", t->type_id);
            }
            return ST.stack[index].type;
        case C_INT:
            return 0;
        case C_FLOAT:
            return 1;
        case TRUE:
        case FALSE:
        case C_CHAR:
            return -1; //暂时不支持char
        default:
            break;
        }
    }
}

int analysis_specifier(ast_node *node){
    int type;
    ast_leaf *t;

    if(node->is_leaf){
        t = ((ast_leaf *)node);
        type =  t->leaf_type  ==INT? 0:\
                t->leaf_type == FLOAT? 1: -1;
        
        return type;
    }else{
        return -1;  //暂时不处理结构体
    }
}

void analysis_var_dec(ast_node *var_dec){
    int type;   //变量类型
    ast_node *c = var_dec->child;
    ast_node *n;
    ast_leaf *t;

    type = analysis_specifier(c);
    //遍历VarList
    n = c->sibling;
    while(n){
        if(n->is_leaf){ //Var -> ID
            linear_insert_symbol_table(((ast_leaf *)n)->type_id, type, 0);
        }else if(n->node_type==NT_VAR_ASSIGN){ //Var -> ID=Exp
            analysis_exp(n->child->sibling);
            linear_insert_symbol_table(((ast_leaf *)(n->child))->type_id, type, 0);
        }else{
            /*Var[]*/
        }

        n = n->sibling;
    }
}

void analysis_stmt(ast_node *stmt);

static inline void analysis_comp_st(ast_node *comp){
    analysis_stmt(comp->child);
}

void analysis_stmt(ast_node *stmt){
    int type1, type2, index;
    if(stmt){
        ast_leaf *t;
        switch(stmt->node_type){
            case NT_COMP_ST:
                analysis_comp_st(stmt);
                break;
            case NT_VAR_DEC:
                analysis_var_dec(stmt);
                break;
            case NT_STMT_EXP:
            case NT_STMT_RETURN:
                analysis_exp(stmt->child);
                break;
            case NT_STMT_ASSIGN:
            case NT_STMT_OP_ASSIGN1:
            case NT_STMT_OP_ASSIGN2:
            case NT_STMT_OP_ASSIGN3:
            case NT_STMT_OP_ASSIGN4:
                type1 = analysis_exp(stmt->child->sibling);
                t = ((ast_leaf *)(stmt->child));
                //查找符号表，确定该变量是否被定义
                index = linear_search_symbol_table(t->type_id);
                if(index==-1){
                    print_error(stmt->lineno, "变量(%s)未定义", t->type_id);
                    // exit(-1);
                }

                type2 = ST.stack[index].type;
                if(type1==0 && type2==1 || type1==1 && type2==0){
                    print_error(stmt->lineno, "赋值语句中，int和float不能隐式转换");
                }

                break;
            case NT_STMT_IF:
            case NT_STMT_WHILE:
                analysis_exp(stmt->child);
                analysis_stmt(stmt->child->sibling);
                break;
            case NT_STMT_IF_ELSE:
                analysis_exp(stmt->child);
                analysis_stmt(stmt->child->sibling);
                analysis_stmt(stmt->child->sibling->sibling);
                break;
            default:
                break;
        }
        analysis_stmt(stmt->sibling);
    }
}

int analysis_func_header(ast_node *func_header){

    ast_node *c = func_header->child;   //ID

    ast_node *n, *n2;
    int dimen = 0;
    int param_type;
    //遍历ParamList
    n = c->sibling;
    while(n){
        dimen++;
        n2 = n->child; //Param -> Specifier ID
        param_type = analysis_specifier(n2);
        linear_insert_symbol_table(((ast_leaf *)(n2->sibling))->type_id, param_type, 0);

        n = n->sibling;
    }

    return dimen;
}

void analysis_func_def(ast_node *func_def){
    ast_node *c = func_def->child;

    int type = analysis_specifier(c); //type确定函数返回类型
    int dimen;

    location(); //定位
    dimen = analysis_func_header(c->sibling);
    analysis_comp_st(c->sibling->sibling);

    print_symbol_table();
    print_block_index_table();
    relocation(); //重定位

    linear_insert_symbol_table(((ast_leaf *)(c->sibling->child))->type_id, type, dimen);
}

void semantic_analysis(ast_node *root){
    ast_node *c, *s;

    if(root){
        c = root->child;
        s = root->sibling;

        if(root->is_leaf==0){
            switch (root->node_type)
            {
            case NT_PROGRAM:
                location(); //定位
                print_symbol_table();
                print_block_index_table();
                semantic_analysis(root->child);
                relocation(); //重定位
                break;
            case NT_VAR_DEC:
                analysis_var_dec(root);
                semantic_analysis(root->sibling);
                break;
            case NT_FUNC_DEF:
                analysis_func_def(root);
                semantic_analysis(root->sibling);
                break;
            default:
                printf("-1\n");
                break;
            }
        }
    }
}