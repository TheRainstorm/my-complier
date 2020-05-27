%option noyywrap nodefault yylineno
%x COMMENT

%{
//lex.yy.c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "ast_node.h"
#include "parse.tab.h"

void yyerror(char *s, ...);
YYSTYPE yylval;

int str_octal_to_int(char *);
int str_hexa_to_int(char *);
char str_to_char(char *);

#define FILE_NAME_MAX_LENGTH 20
char g_file_name[FILE_NAME_MAX_LENGTH];
%}

R_ID        ([_a-zA-Z][_0-9a-zA-Z]*)
R_C_INT     ((0|[1-9][0-9]*))
R_C_OCTAL_INT   (0[0-7]{1,3})
R_C_HEXA_INT    (0[xX][0-9a-fA-F]{1,2})
EXP         ([eE]{R_C_INT})
R_C_FLOAT   ({R_C_INT}\.[0-9]*{EXP}?|{R_C_INT}{EXP}|\.[0-9]+{EXP}?)
R_C_CHAR    (\'([^'\\]|\\['"\\abfnrtv]|\\[0-7]{1,3}|\\[xX][0-9a-fA-F]{1,2})\')

%%
 /*注释（两种）*/
"//".*\n            
 /**/
"/*"                {BEGIN COMMENT;}
<COMMENT>"*/"       {BEGIN INITIAL;}
<COMMENT>[^*]+|.|\n /* Match 1 char every time. when meet * / use the rule above. [^*]+ is for speed up. */
<COMMENT><<EOF>>    {yyerror("Comment are not close!");}
"*/"                {yyerror("没匹配到开始注释!");}
 /*运算符*/
 /*"."                 {return yytext[0]; }
"->"                {return R_ARROW; }*/
 /**/
"++"                {return S_PLUS; }
"--"                {return S_MINUS; }
 /**/
"=="                {strncpy(yylval.type_relop, yytext, 3); return REL_OP; }
"!="                {strncpy(yylval.type_relop, yytext, 3); return REL_OP; }
"<="                {strncpy(yylval.type_relop, yytext, 3); return REL_OP; }
"<"                 {strncpy(yylval.type_relop, yytext, 3); return REL_OP; }
">="                {strncpy(yylval.type_relop, yytext, 3); return REL_OP; }
">"                 {strncpy(yylval.type_relop, yytext, 3); return REL_OP; }
 /**/
"&&"                {return AND; }
"||"                {return OR; }
"!"                 {return yytext[0]; }
 /**/
"="                 {return yytext[0]; }
"+="                {strncpy(yylval.type_opassign, yytext, 3); return OP_ASSIGN; }
"-="                {strncpy(yylval.type_opassign, yytext, 3); return OP_ASSIGN; }
"*="                {strncpy(yylval.type_opassign, yytext, 3); return OP_ASSIGN; }
"/="                {strncpy(yylval.type_opassign, yytext, 3); return OP_ASSIGN; }
 /**/
","                 {return yytext[0]; }
 /**/
"+"                 {return yytext[0]; }
"-"                 {return yytext[0]; }
"*"                 {return yytext[0]; }
"/"                 {return yytext[0]; }
"%"                 {return yytext[0]; }
 /*界限符*/
";"                 {return yytext[0]; }
"("                 {return yytext[0]; }
")"                 {return yytext[0]; }
"{"                 {return yytext[0]; }
"}"                 {return yytext[0]; }
 /*关键字*/
"int"               {yylval.type_p_ast_leaf = create_leaf(yylineno, INT, NULL, NULL); return INT; }
"float"             {yylval.type_p_ast_leaf = create_leaf(yylineno, FLOAT, NULL, NULL); return FLOAT; }
"char"              {yylval.type_p_ast_leaf = create_leaf(yylineno, CHAR, NULL, NULL); return CHAR; }
"void"              {yylval.type_p_ast_leaf = create_leaf(yylineno, VOID, NULL, NULL); return VOID; }
"if"                {return IF; }
"else"              {return ELSE; }
"while"             {return WHILE; }
"return"            {return RETURN; }
"bool"              {yylval.type_p_ast_leaf = create_leaf(yylineno, BOOL, NULL, NULL); return BOOL; }
"true"              {yylval.type_p_ast_leaf = create_leaf(yylineno, TRUE, NULL, NULL); return TRUE; }
"false"             {yylval.type_p_ast_leaf = create_leaf(yylineno, FALSE, NULL, NULL); return FALSE; }
 /*杂项*/
{R_ID}              {
                        yylval.type_p_ast_leaf = create_leaf(yylineno, ID, NULL, NULL);
                        strncpy(yylval.type_p_ast_leaf->type_id, yytext, ID_MAX_LENGTH);
                        return ID;
                    }
{R_C_INT}           {
                        yylval.type_p_ast_leaf = create_leaf(yylineno, C_INT, NULL, NULL);
                        yylval.type_p_ast_leaf->type_int = atoi(yytext);
                        return C_INT;
                    }
{R_C_OCTAL_INT}     {
                        yylval.type_p_ast_leaf = create_leaf(yylineno, C_INT, NULL, NULL);
                        yylval.type_p_ast_leaf->type_int = str_octal_to_int(yytext);
                        return C_INT;
                    }
{R_C_HEXA_INT}      {
                        yylval.type_p_ast_leaf = create_leaf(yylineno, C_INT, NULL, NULL);
                        yylval.type_p_ast_leaf->type_int = str_hexa_to_int(yytext);
                        return C_INT;
                    }
{R_C_FLOAT}         {
                        yylval.type_p_ast_leaf = create_leaf(yylineno, C_FLOAT, NULL, NULL);
                        yylval.type_p_ast_leaf->type_float = atof(yytext);
                        return C_FLOAT;
                    }
{R_C_CHAR}          {
                        yylval.type_p_ast_leaf = create_leaf(yylineno, C_CHAR, NULL, NULL);
                        yylval.type_p_ast_leaf->type_char = str_to_char(yytext);
                        return C_CHAR;
                    }
 /*常见错误*/
"''"                {yyerror("Empty character"); }
\'([^'\\]|\\['"\\abfnrtv]|\\[0-7]{1,3}|\\[xX][0-9a-fA-F]{1,2})+\'                {yyerror("More than one character"); }
[ \r\n\t]
                    /*忽略空白字符*/
.                   {yyerror("Invaild character: %c", yytext[0]); }

%%
void yyerror(char *s, ...){
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%s:%-4d error, ", g_file_name, yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

int str_octal_to_int(char* text) {
    int neg_flag = 0;
    char ch;
    if (text[0] == '-') {
        neg_flag = 1;
        text++;
    }
    else if (text[0] == '+') {
        text++;
    }
    text++; //跳过0
    int result = 0;
    while (ch = *(text++)) {
        result = (result << 3) + (ch - '0');    //快速计算多项式
    }
    return (neg_flag) ? -1 * result : result;
}

int str_hexa_to_int(char* text) {
    int neg_flag = 0;
    char ch;
    if (text[0] == '-') {
        neg_flag = 1;
        text++;
    }
    else if (text[0] == '+') {
        text++;
    }
    text += 2;  //跳过0x
    int result = 0;
    while (ch = *(text++)) {
        result = (result << 4) + (ch - '0');
    }
    return (neg_flag) ? -1*result : result;
}

char str_to_char(char* text) {
    if (text[1] != '\\') {
        return text[1];
    }
    else {
        if ('0' <= text[2] && text[2] <= '7') {
            char* p = text + 2;
            int result = 0;
            while (*p != '\'') {
                result = (result << 3) + *p - '0';
                p++;
            }
            return result;
        }
        else if ('x' == text[2] || 'X' == text[2]) {
            char* p = text + 3;
            int result = 0;
            while (*p != '\'') {
                result = (result << 4) + *p - '0';
                p++;
            }
            return result;
        }
        else {
            switch (text[2]) {
            case '\'':
                return '\'';
            case '\\':
                return '\\';
            case 'a':
                return '\a';
            case 'b':
                return '\b';
            case 'f':
                return '\f';
            case 'n':
                return '\n';
            case 'r':
                return '\r';
            case 't':
                return '\t';
            case 'v':
                return '\v';
            default:
                return -1; //can't get there
            }
        }
    }
}

// int main(int argc, char **argv){
//     FILE *fp;

//     fp = fopen(argv[1], "r");
//     if(fp==NULL){
//         printf("can't open file: %s", argv[1]);
//     }
//     strncpy(g_file_name, argv[1], FILE_NAME_MAX_LENGTH);

//     yyin = fp;
//     yylineno = 1;

//     int tkn;
//     while((tkn = yylex())){
//         printf("%2d, %s\n", yylineno, yytext);
//     }
// }