#ifndef DEFINE_H
#define DEFINE_H
#include <cstring>
#include <string>

/**Symbol
0 序号 -
1 名字 -
2 类型
3 存储地址
4 维数/参数个数
5 声明行
6 引用行
7 指针
*/
typedef struct Symbol
{
    int id;
    char str[20];
}Symbol;

typedef unsigned int uint;

uint my_hash(char* str);

/**Token
* 记号的编码表
*/
enum Token_type{
    //Misc
    ID =10,
        //const
    CONST_INT,    //int
    CONST_FLOAT,      //float
    CONST_CHAR,
    CONST_STR,

    //Operator
    DOT = 20,
    RIGHT_ARROW,

    SELF_PLUS,
    SELF_MINUS,

    PLUS,
    MINUS,
    MULT,
    DIV,
    MOD,

    AND,
    OR,
    NOT,

    BIT_AND, //get address
    BIT_OR,
    BIT_XOR,

    QUESTION, //?
    COLON, //:

    ASSIGN,
    OP_ASSIGN, //+=, -=, *=, /=

    COMMA,
        //relation operator
    REL_OP, //==, !=, >, <, >=, <=

    //Delimiter
    SEMICOLON = 70,
    OPEN_PARENTHESES,
    CLOSE_PARENTHESES,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    
    //Keyword
    AUTO=80,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    CHAR,
    STRUCT,
    UNION,
    ENUM,
    TYPEDEF,
    CONST,
    UNSIGNED,
    SIGNED,
    EXTERN,
    REGISTER,
    STATIC,
    VOLATILE,
    VOID,
    IF,
    ELSE,
    SWITCH,
    CASE,
    FOR,
    DO,
    WHILE,
    GOTO,
    CONTINUE,
    BREAK,
    DEFAULT,
    SIZEOF,
    RETURN,
        //extend boolean
    BOOL,
    TRUE,
    FALSE,


    //ELSE
    END_OF_FILE = 120, //EOF
};

class Token {
public:
    Token_type type;
    Token(Token_type t){
        type = t;
    };
};

class ID_Token: public Token {
public:
    char attr[20];
    ID_Token(Token_type t):Token(t){
        memset(attr, 0, 20);
    };
};

class Char_Token: public Token {
public:
    char attr;
    Char_Token(Token_type t, char a):Token(t){
        attr = a;
    };
};

class String_Token: public Token {
public:
    char attr[1024];
    String_Token(Token_type t):Token(t){
        memset(attr, 0, 1024);
    };
};

class Int_Token: public Token {
public:
    int attr;
    Int_Token(Token_type t, int a):Token(t){
        attr = a;
    };
};

class Float_Token: public Token {
public:
    float attr;
    Float_Token(Token_type t, float a):Token(t){
        attr = a;
    };
};

class OpAssign_Token : public Token {
public:
    char attr[4];
    OpAssign_Token(Token_type t) :Token(t) {
        memset(attr, 0, 4);
    };
};

class RelOp_Token : public Token {
public:
    char attr[4];
    RelOp_Token(Token_type t) :Token(t) {
        memset(attr, 0, 4);
    };
};

std::string Debug_token_type_to_str(Token_type type);

char* my_strncpy_s(char* to, int buf_size, char* from, int max_cpy);
char* my_strncat_s(char* to, int buf_size, char* from, int max_cat);

#endif