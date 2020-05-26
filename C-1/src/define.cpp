#include "define.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
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
    从from中复制最多max_cpy个字符到to中，且保证to不会溢出（其中buf_size为to缓冲区的大小），且to字符串以\0结尾。
    */
    char* tmp = to;

    buf_size = buf_size - 1; //除去\0占用的一个字符
    while (buf_size && max_cpy && (*to++ = *from++)) {
        buf_size--;
        max_cpy--;
    }

    *to = '\0';

    return tmp;
}

char* my_strncat_s(char* to, int buf_size, char* from, int max_cat) {
    /*
    从from复制最多max_cat个字符到to字符串的结尾，且保证to不会溢出（其中buf_size为to缓冲区的大小），且to字符串以\0结尾。

    to字符串必须以\0结尾，否则得到的为to本身
    */
    char* tmp = to;

    while (buf_size && *to) {
        to++;
        buf_size--;
    }
    //循环结束时，buf_size为to中剩余的缓冲区大小（包含\0）

    if (buf_size - 1 > 0)
        my_strncpy_s(to, buf_size, from, max_cat);
    return tmp;
}