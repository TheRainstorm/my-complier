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
#define KEYWORD_HASH_SIZE 199 //����
class Lexer {
private:
    char buf_left[BUF_SIZE], buf_right[BUF_SIZE]; //���뻺����
    char* s_ptr, * f_ptr; //ʶ��ǺŵĿ�ʼָ���ǰ��ָ��
public:
    char m_file_name[100]; //��ǰ�ļ�·��
    FILE* m_fp; //��ǰ�ļ�ָ��
    int m_cur_line, m_cur_col; //��ǰ���к��к�

    int m_nchar, m_nword; //Դ�ļ����ַ�����EOF���һ���ַ���������������ʶ���͹ؼ��֣�

    char m_token_str[MAX_LENGTH_STR]; //���ʶ����ļǺŵ��ַ�����������ת�������ȵȡ�

    char m_keyword_hash_table[KEYWORD_HASH_SIZE][9]; //���ڿ����ж�һ����ʶ���Ƿ�Ϊ�ؼ��֡��ؼ����Ϊ8�ַ�
    char  m_keyword_id_hash_table[KEYWORD_HASH_SIZE]; //���ڻ��һ���ؼ��ֵ�Token_type���
public:
    Lexer();
    ~Lexer();
    void set_file(const char* file_path);
    Token* next_token();
    void forward(); //f_ptr��ǰ�ƶ�һ���ַ�
    bool get_token_str(char* buf, int size); //��s_ptr��f_ptr֮����ַ���д��buf, sizeΪbuf�Ĵ�С�����ַ�������ʱ���ᱻ��ȡ��size-1�ĳ��ȣ���\0��β
    int is_keyword(char* str);
};

#endif;