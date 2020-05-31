#include "mips.h"

int REGS[32];

void init_code(FILE* file) {
    fprintf(file,
".data\n\
_prompt: .asciiz \"Enter an integer:\"\n\
_space: .asciiz \"\\ \"\n\
.globl main\n\
.text\n");
}

void set_read_write_function(FILE* file) {
    fprintf(file,
        "read:\n\
\tli $v0, 4\n\
\tla $a0, _prompt\n\
\tsyscall\n\
\tli $v0, 5\n\
\tsyscall\n\
\tjr $ra\n\n\
write:\n\
\tli $v0, 1\n\
\tsyscall\n\
\tli $v0, 4\n\
\tla $a0, _space\n\
\tsyscall\n\
\tmove $v0, $0\n\
\tjr $ra\n\n");
}

void genMips(code_node* head, FILE* file) {
    code_node* n;
    int opn1_tmp_reg, opn2_tmp_reg, result_tmp_reg;
    n = head;

    init_code(file);

    do {
        switch (n->op_type)
        {
        case OP_FUNCTION:
            fprintf(file, "%s:\n", n->result.id);
            break;
        case OP_LABEL:
            fprintf(file, "%s:\n", n->result.id);
            break;
        case OP_GOTO:
            fprintf(file, "\tb %s\n", n->result.id);
            break;
        case OP_RETURN:
            if (n->result.opn_type == OPN_C_INT) {
                fprintf(file, "\tli $v0, %d\n", n->result.const_int);
            }
            else if (n->result.opn_type == OPN_VAR) {
                fprintf(file, "\tlw $v0, %d($sp)\n", n->result.offset);
            }
            fprintf(file, "\tjr $ra\n");
            break;
        case OP_CALL:
            fprintf(file, "\tjal %s\n", n->opn1.id);
            fprintf(file, "\tsw $v0, %d($sp)\n", n->result.offset);
            break;
        case OP_ARG:    //假定只有一个参数
            if (n->result.opn_type == OPN_C_INT) {
                fprintf(file, "\tli $t1, %d\n", n->result.const_int);
            }
            else {
                fprintf(file, "\tlw $t1, %d($sp)\n", n->result.offset);
            }
            fprintf(file, "\tmove $a0, $t1\n");
        case OP_F_EQ:
        case OP_F_NEQ:
        case OP_F_JLE:
        case OP_F_JLT:
        case OP_F_JGE:
        case OP_F_JGT:
            if (n->opn1.opn_type == OPN_C_INT) {
                fprintf(file, "\tli $t1, %d\n", n->opn1.const_int);
            }
            else if (n->opn1.opn_type == OPN_VAR) {
                fprintf(file, "\tlw $t1, %d($sp)\n", n->opn1.offset);
            }
            if (n->opn2.opn_type == OPN_C_INT) {
                fprintf(file, "\tli $t2, %d\n", n->opn2.const_int);
            }
            else if (n->opn2.opn_type == OPN_VAR) {
                fprintf(file, "\tlw $t2, %d($sp)\n", n->opn2.offset);
            }
            switch (n->op_type)
            {
            case OP_F_EQ:
                fprintf(file, "\tbne $t1, $t2, %s\n", n->result.id);
                break;
            case OP_F_NEQ:
                fprintf(file, "\tbeq $t1, $t2, %s\n", n->result.id);
                break;
            case OP_F_JLE:
                fprintf(file, "\tbgt $t1, $t2, %s\n", n->result.id);
                break;
            case OP_F_JLT:
                fprintf(file, "\tbge $t1, $t2, %s\n", n->result.id);
                break;
            case OP_F_JGE:
                fprintf(file, "\tblt $t1, $t2, %s\n", n->result.id);
                break;
            case OP_F_JGT:
                fprintf(file, "\tble $t1, $t2, %s\n", n->result.id);
                break;
            default:
                break;
            }
            break;
        case OP_ASSIGN_:
            if (n->opn1.opn_type == OPN_C_INT) {
                fprintf(file, "\tli $t1, %d\n", n->opn1.const_int);
            }
            else if (n->opn1.opn_type == OPN_VAR) {
                fprintf(file, "\tlw $t1, %d($sp)\n", n->opn1.offset);
            }
            fprintf(file, "\tsw $t1, %d($sp)\n", n->result.offset);
            break;
        case OP_PLUS:
        case OP_MINUS:
        case OP_MULT:
            if (n->opn1.opn_type == OPN_C_INT) {
                fprintf(file, "\tli $t1, %d\n", n->opn1.const_int);
            }
            else if (n->opn1.opn_type == OPN_VAR) {
                fprintf(file, "\tlw $t1, %d($sp)\n", n->opn1.offset);
            }

            if (n->opn2.opn_type == OPN_C_INT) {
                fprintf(file, "\tli $t2, %d\n", n->opn2.const_int);
            }
            else if (n->opn2.opn_type == OPN_VAR) {
                fprintf(file, "\tlw $t2, %d($sp)\n", n->opn2.offset);
            }

            fprintf(file, "\t%s $t3, $t1, $t2\n", n->op_type == OP_PLUS ? "add" :
                n->op_type == OP_MINUS ? "sub" : "mult");
            fprintf(file, "\tsw $t3, %d($sp)\n", n->result.offset);
            //
            break;

        default:
            break;
        }
        n = n->next;
    } while (n != head);

    set_read_write_function(file);
}
