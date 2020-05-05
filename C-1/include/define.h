#ifndef DEFINE_H
#define DEFINE_H
#include <string>

/**Symbol table
*/

typedef struct Symbol
{
    int id;
    char str[20];
}Symbol;

typedef unsigned int uint;

/**Token
*/
enum Token_type{
    //misc
    ID =10,
        //const
    INTEGER,
    FLOAT,
    CONST_CHAR,
    CONST_STR,

    //operator
    PLUS =20,
    MINUS,
    MULT,
    DIV,
    AND,
    OR,
    NOT,
    ASSIGN,
    COMMA,
        //relation operator
    EQUAL = 30,
    NOT_EQUAL,
    LT,
    LE,
    GT,
    GE,

    //delimiter
    SEMICOLON = 70,
    OPEN_PARENTHESES,
    CLOSE_PARENTHESES,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    
    //keyword
    IF = 80,
    ELES,
    WHILE,
    FOR,
    RETURN,
    INT,
    CHAR,

    END_OF_FILE,
    OTHER
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
    Symbol *attr;
    ID_Token(Token_type t, Symbol *a):Token(t){
        attr = a;
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
    char *attr;
    String_Token(Token_type t, char *a):Token(t){
        attr = a;
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

std::string debug_token_type_label(Token_type type);

#endif