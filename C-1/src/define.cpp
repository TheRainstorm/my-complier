#include "define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

std::string Debug_token_type_to_str(Token_type type) {
    switch (type)
    {
    case ID:
        return "ID";
    case CONST_INT:
        return "CONST_INT";
    case CONST_FLOAT:
        return "CONST_FLOAT";
    case CONST_CHAR:
        return "CONST_CHAR";
    case CONST_STR:
        return "CONST_STR";
    case DOT:
        return ".";
    case RIGHT_ARROW:
        return "->";
    case SELF_PLUS:
        return "++";
    case SELF_MINUS:
        return "--";
    case PLUS:
        return "+";
    case MINUS:
        return "-";
    case MULT:
        return "*";
    case DIV:
        return "/";
    case MOD:
        return "%";
    case AND:
        return "&&";
    case OR:
        return "||";
    case NOT:
        return "!";
    case BIT_AND:
        return "&";
    case BIT_OR:
        return "|";
    case BIT_XOR:
        return "^";
    case QUESTION:
        return "?";
    case COLON:
        return ":";
    case COMMA:
        return ",";
    case ASSIGN:
        return "=";
    case SEMICOLON:
        return ";";
    case OPEN_PARENTHESES:
        return "(";
    case CLOSE_PARENTHESES:
        return ")";
    case OPEN_BRACE:
        return "{";
    case CLOSE_BRACE:
        return "}";
    case OPEN_BRACKET:
        return "[";
    case CLOSE_BRACKET:
        return "]";
    case END_OF_FILE:
        return "EOF";
    default:
        return "DEFAULT"; //can't get there
    }
}

uint my_hash(char* str) {
    uint hash = 0;
    char ch;
    while (ch = *str++) {
        hash = (hash * 9) ^ ch;
    }
    return hash;
}


char* my_strncpy_s(char* to, int buf_size, char* from, int max_cpy) {
    /*
    ��from�и������max_cpy���ַ���to�У��ұ�֤to�������������buf_sizeΪto�������Ĵ�С������to�ַ�����\0��β��
    */
    char* tmp = to;

    buf_size = buf_size - 1; //��ȥ\0ռ�õ�һ���ַ�
    while (buf_size && max_cpy && (*to++ = *from++)) {
        buf_size--;
        max_cpy--;
    }

    *to = '\0';

    return tmp;
}

char* my_strncat_s(char* to, int buf_size, char* from, int max_cat) {
    /*
    ��from�������max_cat���ַ���to�ַ����Ľ�β���ұ�֤to�������������buf_sizeΪto�������Ĵ�С������to�ַ�����\0��β��

    to�ַ���������\0��β������õ���Ϊto����
    */
    char* tmp = to;

    while (buf_size && *to) {
        to++;
        buf_size--;
    }
    //ѭ������ʱ��buf_sizeΪto��ʣ��Ļ�������С������\0��

    if (buf_size - 1 > 0)
        my_strncpy_s(to, buf_size, from, max_cat);
    return tmp;
}