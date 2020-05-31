#include "tac.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

code_node *TAC; //三地址码

char *strcat0(char *s1,char *s2) {
    static char result[10];
    strcpy(result,s1);
    strcat(result,s2);
    return result;
}

char *newLabel() {
    static int no=1;
    char s[10];
    itoa(no++,s,10);
    return strcat0("label",s);
}

char *newTemp(){
    static int no=1;
    char s[10];
    itoa(no++,s,10);
    return strcat0("t",s);
}

code_node *merge(code_node *to, code_node *from){
    //to为第一个双向循环链表的头指针
    //from为第二个的头指针
    //现在将from连接到to的尾部，形成一个新的循环链表
    code_node *tail1, *tail2;

    if(!to) return from; 
    if(!from) return to;

    tail1 = to->prev;
    tail2 = from->prev;

    tail1->next = from;
    from->prev = tail1;

    tail2->next = to;
    to->prev = tail2;

    return to;
}

//输出中间代码
void printIR(struct code_node *head){
    char opnstr1[32],opnstr2[32],resultstr[32];
    struct code_node *h=head;
    if(!head){
        exit(0);
    }
    do {
        if (h->opn1.opn_type==OPN_C_INT)
             sprintf(opnstr1,"#%d",h->opn1.const_int);
        if (h->opn1.opn_type==OPN_C_FLOAT)
             sprintf(opnstr1,"#%f",h->opn1.const_float);
        if (h->opn1.opn_type==OPN_VAR || h->opn1.opn_type==OPN_FUNC)
             sprintf(opnstr1,"%s",h->opn1.id);
        if (h->opn2.opn_type==OPN_C_INT)
             sprintf(opnstr2,"#%d",h->opn2.const_int);
        if (h->opn2.opn_type==OPN_C_FLOAT)
             sprintf(opnstr2,"#%f",h->opn2.const_float);
        if (h->opn2.opn_type==OPN_VAR || h->opn2.opn_type==OPN_FUNC)
             sprintf(opnstr2,"%s",h->opn2.id);
        sprintf(resultstr,"%s",h->result.id);
        switch (h->op_type) {
            case OP_ASSIGN_:  printf("  %s := %s\n",resultstr,opnstr1);
                            break;
            case OP_PLUS:
            case OP_MINUS:
            case OP_MULT:
            case OP_DIV: printf("  %s := %s %c %s\n",resultstr,opnstr1, \
                      h->op_type==OP_PLUS?'+':h->op_type==OP_MINUS?'-':h->op_type==OP_MULT?'*':'\\',opnstr2);
                      break;
            case OP_FUNCTION: printf("\nFUNCTION %s :\n",h->result.id);
                           break;
            case OP_PARAM:    printf("  PARAM %s\n",h->result.id);
                           break;
            case OP_LABEL:    printf("LABEL %s :\n",h->result.id);
                           break;
            case OP_GOTO:     printf("  GOTO %s\n",h->result.id);
                           break;
            case OP_JLE:      printf("  IF %s <= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_JLT:      printf("  IF %s < %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_JGE:      printf("  IF %s >= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_JGT:      printf("  IF %s > %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_EQ:       printf("  IF %s == %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_NEQ:      printf("  IF %s != %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_F_NEQ:      printf("  IF FALSE %s != %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_F_JLE:      printf("  IF FALSE %s <= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_F_JLT:      printf("  IF FALSE %s < %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_F_JGE:      printf("  IF FALSE %s >= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_F_JGT:      printf("  IF FALSE %s > %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_F_EQ:       printf("  IF FALSE %s == %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case OP_ARG:      printf("  ARG %s\n",h->result.id);
                           break;
            case OP_CALL:     printf("  %s := CALL %s\n",resultstr, opnstr1);
                           break;
            case OP_RETURN:   printf("  RETURN %s\n",resultstr);
                           break;
        }
    h=h->next;
    } while (h!=head);
}

code_node *genIR(int op_type, operand opn1, operand opn2, operand result){
    code_node *n=(code_node *)malloc(sizeof(code_node));
    n->op_type=op_type;
    n->opn1=opn1;
    n->opn2=opn2;
    n->result=result;
    n->next=n->prev=n;
    return n;
}

code_node *genIR_p(int op_type, operand *p_opn1, operand *p_opn2, operand *p_result){
    code_node *n=(code_node *)malloc(sizeof(code_node));
    n->op_type=op_type;
    if(p_opn1){
        n->opn1=*p_opn1;
    }else{
        n->opn1.opn_type = -1;
    }
    if(p_opn2){
        n->opn2=*p_opn2;
    }else{
        n->opn2.opn_type = -1;
    }
    if(p_result){
        n->result=*p_result;
    }else{
        n->result.opn_type = -1;
    }
    n->next=n->prev=n;
    return n;
}

code_node *genAssign(int op_type, int opn_index1, int opn_index2, int res_index){
    operand opn1, opn2, result;
    opn1.opn_type = OPN_VAR;
    strcpy(opn1.id, ST.stack[opn_index1].name);

}
