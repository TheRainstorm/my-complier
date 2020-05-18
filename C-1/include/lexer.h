#ifndef LEXER_H
#define LEXER_H

#define DEBUG

#include "define.h"

using namespace std;

#ifdef DEBUG
#define BUF_SIZE 40
#else
#define BUF_SIZE 2048
#endif

#define MAX_LENGTH_ID 128
#define MAX_LENGTH_STR 1024
#define KEYWORD_HASH_SIZE 199 //素数
class Lexer {
private:
    char buf_left[BUF_SIZE], buf_right[BUF_SIZE]; //输入缓冲区
    char* s_ptr, * f_ptr; //识别记号的开始指针和前进指针
public:
    char m_file_name[100]; //当前文件路径
    FILE* m_fp; //当前文件指针
    int m_cur_line, m_cur_col; //当前的行和列号

    int m_nchar, m_nword; //源文件的字符数（EOF会多一个字符），单词数（标识符和关键字）

    char m_token_str[MAX_LENGTH_STR]; //存放识别出的记号的字符串，可用于转换整数等等。

    char m_keyword_hash_table[KEYWORD_HASH_SIZE][9]; //用于快速判断一个标识符是否为关键字。关键字最长为8字符
    char  m_keyword_id_hash_table[KEYWORD_HASH_SIZE]; //用于获得一个关键字的Token_type编号
public:
    Lexer();
    ~Lexer();
    void set_file(const char* file_path);
    Token* next_token();
    void forward(); //f_ptr向前移动一个字符
    bool get_token_str(char* buf, int size); //将s_ptr和f_ptr之间的字符串写入buf, size为buf的大小，当字符串过长时，会被截取成size-1的长度，以\0结尾
    int is_keyword(char* str);
};

#endif;