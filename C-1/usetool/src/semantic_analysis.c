#include "parse.tab.h"
#include "ast_node.h"
#include "symbol_table.h"
#include "define.h"
#include "tac.h"
#include <stdio.h>
#include <stdlib.h>

extern code_node *TAC;

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

int analysis_exp(ast_node *exp, operand *p_result){
    /*作用：分析exp节点，并将exp作为中间代码的操作数所需的信息通过指针传递给调用者，
    并返回exp数据类型（如果是常数则返回常数类型，如果是变量则返回变量的数据类型）
    暂时只认为有0：int和1：float两种
    */
    int index, type1, type2;
    operand opn1,opn2;
    code_node *code;

    if(exp->is_leaf==0){
        switch (exp->node_type)
        {
        case NT_PLUS :
        case NT_MINUS:
        case NT_MULT :
        case NT_DIV :
        case NT_MOD  :
            type1 = analysis_exp(exp->child, &opn1);
            type2 = analysis_exp(exp->child->sibling, &opn2);
            if(type1 != type2 ){
                print_error(exp->lineno, "算数表达式中，int和float不能隐式转换");
            }

            //在符号表中新建临时变量存储结果
            index = linear_insert_symbol_table(newTemp(), type1, 0); //这里认为经过了类型检验后的type1==type2
            p_result->opn_type = OPN_VAR; strcpy(p_result->id, ST.stack[index].name);
            code = genIR(exp->node_type==NT_PLUS? OP_PLUS:\
                exp->node_type==NT_MINUS? OP_MINUS:\
                exp->node_type==NT_MULT? OP_MULT:\
                exp->node_type==NT_DIV? OP_DIV: -1, opn1, opn2, *p_result);
            
            exp->code = merge(merge(exp->child->code, exp->child->sibling->code), code);
            break;
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
            exp->code = NULL;
            break;
        case NT_S_PLUS_L:
        case NT_S_PLUS_R:
        case NT_S_MINUS_L:
        case NT_S_MINUS_R:
        case NT_NEG:
        case NT_PARENTHESIS:
        case NT_NOT:
            analysis_exp(exp->child, p_result);
            break;
        case NT_FUNC_CALL:
            analysis_func_call(exp, p_result);
            break;
        case NT_DOT:
            //暂时不处理结构体
            break;
        default:
            break;
        }
    }else{
        ast_leaf *t;
        t = (ast_leaf *)exp;

        t->code = NULL;
        switch (t->leaf_type)
        {
        case ID:
            index = linear_search_symbol_table(t->type_id);
            if(index==-1){
                print_error(exp->lineno, "变量(%s)未定义", t->type_id);
            }
            
            p_result->opn_type = OPN_VAR; strcpy(p_result->id, ST.stack[index].name);
            return ST.stack[index].type;
        case C_INT:
            p_result->opn_type = OPN_C_INT; p_result->const_int = t->type_int;
            return 0;
        case C_FLOAT:
            p_result->opn_type = OPN_C_FLOAT; p_result->const_float = t->type_float;
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
    operand opn;

    type = analysis_specifier(c);
    //遍历VarList
    n = c->sibling;
    while(n){
        if(n->is_leaf){ //Var -> ID
            linear_insert_symbol_table(((ast_leaf *)n)->type_id, type, 0);
        }else if(n->node_type==NT_VAR_ASSIGN){ //Var -> ID=Exp
            analysis_exp(n->child->sibling, &opn);
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
    ast_leaf *t;
    int type1, type2, index;
    operand opn1, opn2, result;
    if(stmt){
        switch(stmt->node_type){
            case NT_COMP_ST:
                location();
                analysis_comp_st(stmt);
                print_symbol_table();
                print_block_index_table();
                relocation();
                break;
            case NT_VAR_DEC:
                analysis_var_dec(stmt);
                break;
            case NT_STMT_EXP:
            case NT_STMT_RETURN:
                analysis_exp(stmt->child, &opn1);
                break;
            case NT_STMT_ASSIGN:
            case NT_STMT_OP_ASSIGN1:
            case NT_STMT_OP_ASSIGN2:
            case NT_STMT_OP_ASSIGN3:
            case NT_STMT_OP_ASSIGN4:
                type1 = analysis_exp(stmt->child->sibling, &opn1);
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
                
                opn2.opn_type = -1; //单个赋值，没有操作数2
                result.opn_type = OPN_VAR; strcpy(result.id, ST.stack[index].name);

                code_node *code =genIR(OP_ASSIGN, opn1, opn2, result);
                stmt->code = merge(stmt->child->sibling->code, code);   //将生成的代码连接到Exp的代码

                break;
            case NT_STMT_IF:
            case NT_STMT_WHILE:
                analysis_exp(stmt->child, &opn1);
                analysis_stmt(stmt->child->sibling);
                break;
            case NT_STMT_IF_ELSE:
                analysis_exp(stmt->child, &opn1);
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

    func_def->code = c->sibling->sibling->code;
}

void semantic_analysis(ast_node *root){
    // if(root){
    //     switch (root->node_type)
    //         {
    //         case NT_PROGRAM:
    //             semantic_analysis(root->child);
    //         case NT_VAR_DEC:
    //             analysis_var_dec(root);
    //             semantic_analysis(root->sibling);
    //             break;
    //         case NT_FUNC_DEF:
    //             analysis_func_def(root);
    //             root->code = merge(root->code, root->child->code);
    //             semantic_analysis(root->sibling);
    //             break;
    //         default:
    //             printf("-1\n");
    //             break;
    //         }
    // }
    ast_node *n;
    if(root){
        n = root->child;
        while(n){
            switch (n->node_type)
            {
            case NT_VAR_DEC:
                analysis_var_dec(n);
                break;
            case NT_FUNC_DEF:
                analysis_func_def(n);
                root->code = merge(root->code, n->code);
            default:
                break;
            }

            n = n->sibling;
        }
    }
}