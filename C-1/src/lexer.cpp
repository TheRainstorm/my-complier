#define DEBUG

#include "define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

#ifdef DEBUG
#define BUF_SIZE 10
#else
#define BUF_SIZE 2048
#endif
#define ID_MAX_LENGTH 20

class Lexer {
private:
    char m_buf[2*BUF_SIZE],* buf_left,* buf_right; //输入缓冲区
    char *s_ptr, *f_ptr; //识别记号的开始指针和前进指针

    char m_file_name[100]; //当前文件路径
    FILE *m_fp; //当前文件指针
    int m_cur_line, m_cur_col; //当前的行和列号

    char m_token_str[1024]; //存放识别的记号的字符串。字符串类型记号最长为1024
public:
    Lexer();
    ~Lexer();
    void set_file(const char *file_path);
    Token *next_token();
    void forward();
    void backward();
    void get_token_str(char *buf, int size);
};

Lexer::Lexer(){
    buf_left = m_buf;
    buf_right = m_buf + BUF_SIZE;
}

Lexer::~Lexer(){
    fclose(m_fp);
}

void Lexer::set_file(const char *file_path){
    //设置文件名
    if(strlen(file_path)>=100){
        printf("Error: file path(%s) is too long!\n",file_path);
        exit(-1);
    }
    strcpy_s(m_file_name, 100, file_path);
    //设置文件指针
    
    if(0!=fopen_s(&m_fp, file_path, "rb")){
        printf("Error: read %s failed!\n", file_path);
        exit(-1);
    }
    f_ptr = s_ptr = buf_left;
    //方便用来判断f_ptr到达了缓冲区边界
    buf_left[BUF_SIZE-1] = -2;
    buf_right[BUF_SIZE-1] = -2;
    //首先读取到左半边缓冲区
    size_t size;
    size = fread(buf_left, 1, BUF_SIZE - 1, m_fp);
    if (size != BUF_SIZE - 1) {
        buf_left[size] = EOF;
    }
    //设置行号为0
    m_cur_line =0;
}

void Lexer::forward(){
    f_ptr++;
    size_t size;
    if(*f_ptr==-2){
        if(f_ptr==buf_left+BUF_SIZE-1){
            size = fread(buf_right, 1, BUF_SIZE - 1, m_fp);
            if (size != BUF_SIZE - 1) {
                buf_right[size] = EOF;
            }
            f_ptr = buf_right;
        }else{
            size = fread(buf_left, 1, BUF_SIZE - 1, m_fp);
            if (size != BUF_SIZE - 1) {
                buf_left[size] = EOF;
            }
            f_ptr = buf_left;
        }
    }
}

void Lexer::backward(){
    if(f_ptr==buf_right){
        f_ptr = buf_left+BUF_SIZE-2;
    }else if(f_ptr==buf_left){
        f_ptr = buf_right+BUF_SIZE-2;
    }else{
        f_ptr--;
    }
}

void Lexer::get_token_str(char *buf, int size){
    /*将识别出的记号字符串复制到buf中
    */
    if(s_ptr>f_ptr){
        strncpy_s(buf, size, s_ptr, buf_right + BUF_SIZE - 1 - s_ptr);
        strncat_s(buf, size, buf_left, f_ptr-buf_left);
    }else if(f_ptr<buf_right || buf_left<=s_ptr){
        strncpy_s(buf, size, s_ptr, f_ptr-s_ptr);
    }else{
        strncpy_s(buf, size, s_ptr, buf_left + BUF_SIZE - 1 - s_ptr);
        strncat_s(buf, size, buf_right, f_ptr-buf_right);
    }
}

Token *Lexer::next_token(){
    /*识别并返回一个记号指针
    */
    Token *tkn;
    //跳过空白字符
    while(*f_ptr==' '||*f_ptr=='\t'||*f_ptr=='\n'||*f_ptr == '\r'){
        if(*f_ptr=='\n'){
            m_cur_line++;
        }
        forward();
    }
    s_ptr = f_ptr;

    /*ID*/
    if(('a'<=*f_ptr && *f_ptr<='z')||('A'<=*f_ptr && *f_ptr<='Z')||(*f_ptr=='_')){
        forward();
        while(('a'<=*f_ptr && *f_ptr<='z')||('A'<=*f_ptr && *f_ptr<='Z')||
              ('0'<=*f_ptr &&*f_ptr<='9')||(*f_ptr=='_')){
            forward();
        }
        get_token_str(m_token_str, 1024);
        if(strnlen_s(m_token_str, 1024)>20){
            printf("Warnning: ID(%s) is cut to 20 characters", m_token_str);
            m_token_str[20]='\0'; //标识符截取20个字符
        }
        tkn = new ID_Token(ID, NULL);
    /*整数*/
    //0
    }else if(*f_ptr=='0'){
        //mylabel: 准备下一次识别
        forward();
        tkn = new Int_Token(INTEGER, 0);
    //无符号整数
    }else if('1'<=*f_ptr && *f_ptr<='9'){
        forward();
        while('0'<=*f_ptr && *f_ptr<='9'){
            forward();
        }
        get_token_str(m_token_str, 1024);
        int attr;
        sscanf_s(m_token_str, "%d", &attr);
        tkn = new Int_Token(INTEGER, attr);
    //符号整数
    }else if(*f_ptr=='+' || *f_ptr=='-'){
        int t_plus_flag = 0;
        if (*f_ptr == '+') {
            t_plus_flag = 1;
        }
        forward();
        if(*f_ptr=='0'){
            forward();

            tkn = new Int_Token(INTEGER, 0);
        }else if('1'<=*f_ptr && *f_ptr<='9'){
            forward();
            while('0'<=*f_ptr && *f_ptr<='9'){
                forward();
            }
            get_token_str(m_token_str, 1024);
            int attr;
            sscanf_s(m_token_str, "%d", &attr);
            tkn = new Int_Token(INTEGER, attr);
        //加号、减号
        }else{
            //mylabel: 准备下一次识别，上面已经forward故不必再次forward
            if(t_plus_flag){
                tkn = new Token(PLUS);
            }else{
                tkn = new Token(MINUS);
            }
        }
    /**/
    }else if(*f_ptr=='*'){
        forward();
        tkn = new Token(MULT);
    }else if (*f_ptr == '/') {
        forward();
        tkn = new Token(DIV);
    }else if(*f_ptr==';'){
        forward();
        tkn = new Token(SEMICOLON);
    }else if (*f_ptr == '=') {
        forward();
        if (*f_ptr == '=') {
            forward();
            tkn = new Token(EQUAL);
        }
        else {
            tkn = new Token(ASSIGN);
        }
    }
    //delimiter
    else if (*f_ptr == '(') {
        forward();
        tkn = new Token(OPEN_PARENTHESES);
    }
    else if (*f_ptr == ')') {
        forward();
        tkn = new Token(CLOSE_PARENTHESES);
    }
    else if (*f_ptr == '{') {
        forward();
        tkn = new Token(OPEN_BRACE);
    }
    else if (*f_ptr == '}') {
        forward();
        tkn = new Token(CLOSE_BRACE);
    }
    else if (*f_ptr == '[') {
        forward();
        tkn = new Token(OPEN_BRACKET);
    }
    else if (*f_ptr == ']') {
        forward();
        tkn = new Token(CLOSE_BRACKET);
    }
    //EOF
    else if (*f_ptr == EOF) {
        tkn = new Token(END_OF_FILE);
    }
    else {
        tkn = new Token(OTHER);
    }
    s_ptr = f_ptr;
    return tkn;
}

int main(){
    Token* token;

    Lexer lex;
    lex.set_file("D:\\Projects\\VS_projects\\C-1\\C-1\\test\\lex\\1_ID_INTEGER.txt");

    int cnt = 1;
    token = lex.next_token();
    while (1) {
        if (token->type == END_OF_FILE) {
            cout << "EOF" <<endl;
            break;
        }
        else if (token->type == ID) {
            cout << "ID" << endl;
        }
        else if (token->type == INTEGER) {
            cout << "INTEGER: " << ((Int_Token *)token)->attr<< endl;
        }
        else {
            cout << debug_token_type_label(token->type) << endl;
        }
        token = lex.next_token();

        cnt++;
    }
    
    return 0;
}