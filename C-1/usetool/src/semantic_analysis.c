#include "ast_node.h"
#include "parse.tab.h"
#include "symbol_table.h"
#include "define.h"
#include "tac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern code_node *TAC;

int analysis_specifier(ast_node *node);
int analysis_func_call(ast_node *node, operand *p_result);
int analysis_exp(ast_node *exp, operand *p_result);
void analysis_var_dec(ast_node *var_dec);
void analysis_stmt(ast_node *stmt);
int analysis_func_header(ast_node *func_header);
void analysis_func_def(ast_node *func_def);

int analysis_func_call(ast_node *node, operand *p_result){
    int type, index;
    ast_leaf *t;
    t = (ast_leaf *)(node->child);

    code_node *code;
    operand opn1, opn2, result;

    //查找符号表，寻找是否有该函数
    index = search_symbol_table(t->type_id);
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

        analysis_exp(n, &result);
        code = genIR_p(OP_ARG, NULL, NULL, &result);    //生成ARG代码
        node->code = merge(node->code, merge(n->code, code));
        n = n->sibling;
    }

    if(param_cnt!=ST.stack[index].dimen){
        print_error(node->lineno, "函数(%s)参数不匹配, 期望的是%d个, 而%d个被给予", t->type_id,\
                 ST.stack[index].dimen, param_cnt);
    }
    type = ST.stack[index].type;

    //中间代码部分
    //操作数1为函数
    opn1.opn_type = OPN_FUNC; strcpy(opn1.id, ST.stack[index].name);
    opn2.opn_type = -1; 
    //在符号表中新建临时变量存储结果
    index = insert_symbol_table(newTemp(), type, 0, 'T');         //变量类型为函数返回类型
    p_result->opn_type = OPN_VAR; strcpy(p_result->id, ST.stack[index].name); p_result->offset = ST.stack[index].offset;
    code = genIR(OP_CALL, opn1, opn2, *p_result);

    node->code = merge(node->code, code);

    return type;
}

int analysis_exp(ast_node *exp, operand *p_result){
    /*作用：分析exp节点，并将exp作为中间代码的操作数所需的信息通过指针传递给调用者，
    并返回exp数据类型（如果是常数则返回常数类型，如果是变量则返回变量的数据类型）
    暂时只认为有0：int和1：float两种

    由于需要中间代码生成，目前只处理以下几种结构：
    1. +, -, *, /
    2. ID
    3. C_INT, C_FLOAT
    */
    int index, type1, type2;
    operand opn1,opn2;
    code_node *code;

    if(exp){
        exp->code =NULL;

        if(exp->is_leaf==0){
            switch (exp->node_type)
            {
            case NT_PLUS :
            case NT_MINUS:
            case NT_MULT :
            case NT_DIV :
                type1 = analysis_exp(exp->child, &opn1);
                type2 = analysis_exp(exp->child->sibling, &opn2);
                if(type1 != type2 ){
                    print_error(exp->lineno, "算数表达式中，int和float不能隐式转换");
                }

                //在符号表中新建临时变量存储结果
                index = insert_symbol_table(newTemp(), type1, 0, 'T'); //这里认为经过了类型检验后的type1==type2
                p_result->opn_type = OPN_VAR; strcpy(p_result->id, ST.stack[index].name); p_result->offset = ST.stack[index].offset;
                code = genIR(exp->node_type==NT_PLUS? OP_PLUS:\
                    exp->node_type==NT_MINUS? OP_MINUS:\
                    exp->node_type==NT_MULT? OP_MULT:\
                    exp->node_type==NT_DIV? OP_DIV: -1, opn1, opn2, *p_result);
                
                exp->code = merge(merge(exp->child->code, exp->child->sibling->code), code);
                return type1;
            case NT_COMMA:
                analysis_exp(exp->child, &opn1);
                type2 = analysis_exp(exp->child->sibling, p_result);    //右侧的作为结果
                exp->code = merge(exp->child->code, exp->child->sibling->code);
                return type2;
            case NT_PARENTHESIS:
                type1 = analysis_exp(exp->child, p_result); //p_result由子exp确定
                exp->code = exp->child->code;       //code直接传递
                return type1;
            case NT_RPARAM:
                type1 = analysis_exp(exp->child, p_result);
                exp->code = exp->child->code;
                return type1;
            case NT_FUNC_CALL:
                type1 = analysis_func_call(exp, p_result);
                return type1;
            //以下的皆未处理
            case NT_MOD  :
            case NT_REL_OP1:
            case NT_REL_OP2:
            case NT_REL_OP3:
            case NT_REL_OP4:
            case NT_REL_OP5:
            case NT_REL_OP6:
            //bool
            case NT_AND:
            case NT_OR:
            case NT_NOT:
            //
            case NT_S_PLUS_L:
            case NT_S_PLUS_R:
            case NT_S_MINUS_L:
            case NT_S_MINUS_R:
            case NT_NEG:
            //
            case NT_DOT: //暂时不处理结构体
                return -1;
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
                index = search_symbol_table(t->type_id);
                if(index==-1){
                    print_error(exp->lineno, "变量(%s)未定义", t->type_id);
                }
                
                p_result->opn_type = OPN_VAR; strcpy(p_result->id, ST.stack[index].name);  p_result->offset = ST.stack[index].offset;
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
                return -1;
            default:
                break;
            }
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
    operand opn, result;
    int index;

    var_dec->code=NULL;

    type = analysis_specifier(c);
    //遍历VarList
    n = c->sibling;
    while(n){
        if(n->is_leaf){ //Var -> ID
            insert_symbol_table(((ast_leaf *)n)->type_id, type, 0, 'V');
        }else if(n->node_type==NT_VAR_ASSIGN){ //Var -> ID=Exp

            analysis_exp(n->child->sibling, &opn);
            index = insert_symbol_table(((ast_leaf *)(n->child))->type_id, type, 0, 'V');
            

            result.opn_type=OPN_VAR; strcpy(result.id, ST.stack[index].name); result.offset = ST.stack[index].offset;
            var_dec->code = merge(n->child->sibling->code, genIR_p(OP_ASSIGN_, &opn, NULL, &result));
        }else{
            /*Var[]*/
        }

        n = n->sibling;
    }
}

void analysis_comp_st(ast_node *comp){
    ast_node *stmt;
    if(comp){
        comp->code = NULL;

        stmt = comp->child;
        while(stmt){
            analysis_stmt(stmt);
            comp->code = merge(comp->code, stmt->code);

            stmt = stmt->sibling;
        }
    }
}

void analysis_stmt(ast_node *stmt){
    ast_leaf *t;
    int type1, type2, index;
    operand opn1, opn2, result, result2;
    code_node *code;
    char * label;

    if(stmt){
        stmt->code =NULL;
        switch(stmt->node_type){
            case NT_COMP_ST:
                // location();
                analysis_comp_st(stmt);

                //重定位前打印符号表
                 print_symbol_table();
                 print_block_index_table();

                // relocation();
                break;
            case NT_VAR_DEC:
                analysis_var_dec(stmt);
                break;
            case NT_STMT_EXP:
                analysis_exp(stmt->child, &result);
                stmt->code = stmt->child->code;
                break;
            case NT_STMT_RETURN:
                type1 = analysis_exp(stmt->child, &opn1);

                ////return 语句增加一次赋值
                //index = insert_symbol_table(newTemp(), type1, 0);
                //result.opn_type = OPN_VAR; strcpy(result.id, ST.stack[index].name); result.offset = ST.stack[index].offset;
                //code = genIR_p(OP_ASSIGN_, &opn1, NULL, &result);

                //code = merge(stmt->child->code, code);

                stmt->code = merge(stmt->child->code, genIR_p(OP_RETURN, NULL, NULL, &opn1));
                break;
            case NT_STMT_ASSIGN:
            case NT_STMT_OP_ASSIGN1:
            case NT_STMT_OP_ASSIGN2:
            case NT_STMT_OP_ASSIGN3:
            case NT_STMT_OP_ASSIGN4:
                type1 = analysis_exp(stmt->child->sibling, &opn1);
                t = ((ast_leaf *)(stmt->child));
                //查找符号表，确定该变量是否被定义
                index = search_symbol_table(t->type_id);
                if(index==-1){
                    print_error(stmt->lineno, "变量(%s)未定义", t->type_id);
                    // exit(-1);
                }
                type2 = ST.stack[index].type;
                if(type1==0 && type2==1 || type1==1 && type2==0){
                    print_error(stmt->lineno, "赋值语句中，int和float不能隐式转换");
                }
                
                opn2.opn_type = -1; //单个赋值，没有操作数2
                result.opn_type = OPN_VAR; strcpy(result.id, ST.stack[index].name);result.offset = ST.stack[index].offset;

                code =genIR(OP_ASSIGN_, opn1, opn2, result);
                stmt->code = merge(stmt->child->sibling->code, code);   //将生成的代码连接到Exp的代码
                break;
            case NT_STMT_IF_ELSE:
            case NT_STMT_IF:
                // if(Exp) Stmt
                //Exp must be opn1 REL_OP opn2
                type1 = stmt->child->node_type; //暂时用一下type1
                if(NT_REL_OP1 <= type1 && type1 <= NT_REL_OP6){
                    
                    label = newLabel();
                    result.opn_type = OPN_LABEL; strcpy(result.id, label);

                    analysis_exp(stmt->child->child, &opn1);
                    analysis_exp(stmt->child->child->sibling, &opn2);
                    //if op1 relop op2 goto label1
                    code = genIR_p(type1==NT_REL_OP1 ? OP_F_EQ : \
                            type1==NT_REL_OP2 ? OP_F_NEQ : \
                            type1==NT_REL_OP3 ? OP_F_JLE : \
                            type1==NT_REL_OP4 ? OP_F_JLT : \
                            type1==NT_REL_OP5 ? OP_F_JGE : \
                            type1==NT_REL_OP6 ? OP_F_JGT : -1\
                            , &opn1, &opn2, &result);
                    
                    // stmt1
                    analysis_stmt(stmt->child->sibling);
                    stmt->code = merge(code, stmt->child->sibling->code);

                    //----if-else------
                    //goto label2
                    if(stmt->node_type==NT_STMT_IF_ELSE){
                        label = newLabel();
                        opn1.opn_type=OPN_LABEL; strcpy(opn1.id, label);    //不覆盖result，故使用opn1
                        code = genIR_p(OP_GOTO, NULL, NULL, &opn1);

                        stmt->code = merge(stmt->code, code);
                    }
                    //-----if-else-----

                    //Label 1
                    stmt->code = merge(stmt->code, genIR_p(OP_LABEL, NULL, NULL, &result));

                    //----if-else------
                    //stmt2
                    //Label2
                    if(stmt->node_type==NT_STMT_IF_ELSE){
                        analysis_stmt(stmt->child->sibling->sibling);
                        stmt->code = merge(stmt->code, stmt->child->sibling->sibling->code);
                        stmt->code = merge(stmt->code, genIR_p(OP_LABEL, NULL, NULL, &opn1));
                    }
                    //-----if-else-----
                }
                break;
            case NT_STMT_WHILE:
                //while(Exp) stmt
                type1 = stmt->child->node_type; //暂时用一下type1
                if(NT_REL_OP1 <= type1 && type1 <= NT_REL_OP6){
                    //generate label1, label2
                    label = newLabel();
                    result.opn_type = OPN_LABEL; strcpy(result.id, label);
                    label = newLabel();
                    result2.opn_type = OPN_LABEL; strcpy(result2.id, label);

                    //label1
                    stmt->code = genIR_p(OP_LABEL, NULL, NULL, &result);

                    //if op1 relop op2 goto label2
                    analysis_exp(stmt->child->child, &opn1);
                    analysis_exp(stmt->child->child->sibling, &opn2);
                    code = genIR_p(type1==NT_REL_OP1 ? OP_F_EQ : \
                            type1==NT_REL_OP2 ? OP_F_NEQ : \
                            type1==NT_REL_OP3 ? OP_F_JLE : \
                            type1==NT_REL_OP4 ? OP_F_JLT : \
                            type1==NT_REL_OP5 ? OP_F_JGE : \
                            type1==NT_REL_OP6 ? OP_F_JGT : -1\
                            , &opn1, &opn2, &result2);
                    stmt->code = merge(stmt->code, code);

                    //stmt
                    analysis_stmt(stmt->child->sibling);
                    stmt->code = merge(stmt->code, stmt->child->sibling->code);

                    //goto label1
                    stmt->code = merge(stmt->code, genIR_p(OP_GOTO, NULL, NULL, &result));

                    //label2
                    stmt->code = merge(stmt->code, genIR_p(OP_LABEL, NULL, NULL, &result2));
                }
                break;
            default:
                break;
        }
    }
}

int analysis_func_header(ast_node *func_header){
    ast_node *c = func_header->child;   //ID

    operand result;
    int index;
    //FUNCTION F
    result.opn_type=OPN_FUNC; strcpy(result.id, ((ast_leaf *)c)->type_id);
    func_header->code = genIR_p(OP_FUNCTION, NULL, NULL, &result);

    ast_node *n, *n2;
    int dimen = 0;
    int param_type;
    //遍历ParamList
    n = c->sibling;
    while(n){
        dimen++;
        n2 = n->child; //Param -> Specifier ID
        param_type = analysis_specifier(n2);
        index = insert_symbol_table(((ast_leaf *)(n2->sibling))->type_id, param_type, 0, 'V');

        //PARAM
        result.opn_type=OPN_VAR; strcpy(result.id, ST.stack[index].name); result.offset = ST.stack[index].offset;
        func_header->code = merge(func_header->code, genIR_p(OP_PARAM, NULL, NULL, &result));

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

    insert_symbol_table(((ast_leaf *)(c->sibling->child))->type_id, type, dimen, 'F');    //header->child -> ID

    func_def->code = merge(c->sibling->code, c->sibling->sibling->code);
}

void semantic_analysis(ast_node *root){
    ast_node *n;
    root->code = NULL;
    if(root){
        n = root->child;
        while(n){
            switch (n->node_type)
            {
            case NT_VAR_DEC:
                analysis_var_dec(n);
                root->code = merge(root->code, n->code);
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