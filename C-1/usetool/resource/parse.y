%{
//parse.tab.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast_node.h"
#include "define.h"

extern int yylineno;
int yylex(void);
void yyerror(char *s, ...);

ast_node *root;

char *strcat_tmp(char *, char *);
%}
%locations
%union {
    ast_node *type_p_ast_node;
    ast_leaf *type_p_ast_leaf;
    int type;     //用于确定rel_op和op_assign
}

%token '.' S_PLUS S_MINUS '+' '-' '*' '/' '%' AND OR '!' ',' '='
%token ';' '(' ')' '[' ']' '{' '}'
%token  IF ELSE WHILE RETURN STRUCT 
%token <type> REL_OP
%token <type> OP_ASSIGN
%token <type_p_ast_leaf> ID C_INT C_FLOAT C_CHAR INT FLOAT CHAR VOID BOOL TRUE FALSE

%type <type_p_ast_node> Program ExtDefList ExtDef VarDec FuncDef Specificer VarList Var FuncHeader ParamList CompSt Param RParamList RParam TagOpt VarDecList StmtList Stmt Exp

%nonassoc IF
%nonassoc ELSE

%left ','
%right '=' OP_ASSIGN
%left OR
%left AND
%left REL_OP
%left '+' '-'
%left '*' '/' '%'
%right S_PLUS S_MINUS '!'

%%
Program:
    ExtDefList  {
                    if($1!=NULL){
                        $$=create_node2(yylloc.first_line, NT_PROGRAM, "Program", $1, NULL);
                        root=$$;
                    }else{
                        printf("Not accept.\n");
                        root = NULL;
                    }
                }
;

ExtDefList:                 {$$=NULL; }
    | ExtDef ExtDefList     {$1->sibling = $2; $$=$1; }
;

ExtDef:
    VarDec                   
    | FuncDef                
;

//变量声明
VarDec:
    Specificer VarList ';'      {$1->sibling=$2; $$=create_node2(yylloc.first_line, NT_VAR_DEC, "VarDec", $1, NULL); }
    | Specificer ';'            {$$=create_node2(yylloc.first_line, NT_VAR_DEC, "VarDec", $1, NULL); }   //结构体定义
;

VarList:                        
    Var ',' VarList             {$1->sibling = $3; $$=$1; }
    | Var                       
;

Var: 
    ID                          {$$=(ast_node *)$1; }
    | ID '=' Exp                {$1->sibling=$3; $$=create_node2(yylloc.first_line, NT_VAR_ASSIGN, "Var=", (ast_node *)$1, NULL); }
    | Var '[' C_INT ']'         {$1->sibling=(ast_node *)$3; $$=create_node2(yylloc.first_line, NT_VAR_ARRAY, "Var[]", $1, NULL); }
;

//函数定义
FuncDef: 
    Specificer FuncHeader CompSt
                                {
                                    $1->sibling = $2;
                                    $2->sibling = $3;
                                    $$=create_node2(yylloc.first_line, NT_FUNC_DEF, "FuncDef", $1, NULL);
                                }
;

FuncHeader:
    ID '(' ParamList ')'        {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_FUNC_HEADER, "Header", (ast_node *)$1, NULL);}
;

ParamList:                      {$$=NULL; }
    | Param ',' ParamList       {$1->sibling = $3; $$=$1; }
    | Param                     
;

Param: 
    Specificer ID               {$1->sibling=(ast_node *)$2; $$=create_node2(yylloc.first_line, NT_PARAM, "Param", $1, NULL); }
;

Specificer:
    INT                         {$$=(ast_node *)$1; }
	| FLOAT                     {$$=(ast_node *)$1; }
	| CHAR                      {$$=(ast_node *)$1; }
	| BOOL                      {$$=(ast_node *)$1; }
    | STRUCT TagOpt '{' VarDecList '}'  {$2->sibling=$4; $$=create_node2(yylloc.first_line, NT_STRUCT, "Struct", $2, NULL); }
    | STRUCT ID                 {$$=(ast_node *)$2; }
;

TagOpt:                         {$$=create_node2(yylloc.first_line, NT_TAG_OPT, "TagOpt", NULL, NULL); }
    | ID                        {$$=create_node2(yylloc.first_line, NT_TAG_OPT, "TagOpt", (ast_node *)$1, NULL); }
;

VarDecList:                     
    VarDec VarDecList           {$1->sibling=$2; $$=$1; }
    | VarDec                    
;

//复合语句 | 语句块
CompSt:
    '{' StmtList '}'                {$$=create_node2(yylloc.first_line, NT_COMP_ST, "CompSt", $2, NULL);  }
;

StmtList:                           {$$=NULL; }
    | Stmt StmtList                 {$1->sibling = $2; $$=$1; }
;

Stmt:
      Exp ';'                       {$$=create_node2(yylloc.first_line, NT_STMT_EXP, "StmtExp", $1, NULL); }
    | ID '=' Exp ';'                {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_STMT_ASSIGN, "Stmt=", (ast_node *)$1, NULL); }
    | ID OP_ASSIGN Exp ';'          {$1->sibling = $3; $$=create_node2(yylloc.first_line, $2, "op_asgn", (ast_node *)$1, NULL); }
    | IF '(' Exp ')' Stmt           %prec IF {$3->sibling = $5; $$=create_node2(yylloc.first_line, NT_STMT_IF, "if", $3, NULL); }
    | IF '(' Exp ')' Stmt ELSE Stmt %prec ELSE {
                                        $3->sibling = $5;
                                        $5->sibling = $7;
                                        $$=create_node2(yylloc.first_line, NT_STMT_IF_ELSE, "if-else", $3, NULL); 
                                    }
    | WHILE '(' Exp ')' Stmt        {$3->sibling = $5; $$=create_node2(yylloc.first_line, NT_STMT_WHILE, "while", $3, NULL); }
    | RETURN Exp ';'                {$$=create_node2(yylloc.first_line, NT_STMT_RETURN, "return", $2, NULL); }
    | CompSt 
    | VarDec                         
;

Exp:
      Exp '+' Exp   {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_PLUS , "exp+", $1, NULL); }
    | Exp '-' Exp   {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_MINUS, "exp-", $1, NULL); }
    | Exp '*' Exp   {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_MULT , "exp*", $1, NULL); }
    | Exp '/' Exp   {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_DIV  , "exp/", $1, NULL); }
    | Exp '%' Exp   {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_MOD  , "exp%", $1, NULL); }
    | Exp REL_OP Exp{$1->sibling = $3; $$=create_node2(yylloc.first_line, $2, "relop", $1, NULL); }
    | S_PLUS Exp    {$$=create_node2(yylloc.first_line, NT_S_PLUS_L, "expl++", $2, NULL); }
    | S_MINUS Exp   {$$=create_node2(yylloc.first_line, NT_S_MINUS_L, "expl--", $2, NULL); }
    | Exp S_PLUS    {$$=create_node2(yylloc.first_line, NT_S_PLUS_R, "expr++", $1, NULL); }
    | Exp S_MINUS   {$$=create_node2(yylloc.first_line, NT_S_MINUS_R, "expr--", $1, NULL); }
    | Exp AND Exp   {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_AND, "exp-and", $1, NULL); }
    | Exp OR Exp    {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_OR, "exp-or", $1, NULL); }
    | '!' Exp       {$$=create_node2(yylloc.first_line, NT_NOT, "exp-not", $2, NULL); }
    | '-' Exp       %prec '!' {$$=create_node2(yylloc.first_line, NT_NEG, "exp-neg", $2, NULL); }
    | '(' Exp ')'   {$$=create_node2(yylloc.first_line, NT_PARENTHESIS, "exp()", $2, NULL); }
    | Exp ',' Exp   {$1->sibling = $3; $$=create_node2(yylloc.first_line, NT_COMMA, "exp,", $1, NULL); }
    | ID            {$$=(ast_node *)$1; }
    | C_INT         {$$=(ast_node *)$1; }
    | C_FLOAT       {$$=(ast_node *)$1; }
    | C_CHAR        {$$=(ast_node *)$1; }
    | TRUE          {$$=(ast_node *)$1; }
    | FALSE         {$$=(ast_node *)$1; }
    | ID '(' RParamList ')' {$1->sibling=$3; $$=create_node2(yylloc.first_line, NT_FUNC_CALL, "expCall", (ast_node*)$1, NULL); }
    | ID '.' ID     {$1->sibling=(ast_node *)$3; $$=create_node2(yylloc.first_line, NT_DOT, "expDot", (ast_node*)$1, NULL);}
    /*常见错误*/
    // | Exp Exp       {$$=NULL; printf("缺少运算符"); }
    // | '(' Exp       {$$=NULL; printf("缺失右括号)"); }
    // | Exp '+'       {$$=NULL; printf("缺失操作数"); }
;

//实参列表
RParamList:                     {$$=NULL; }
    | RParam ',' RParamList     {$1->sibling=$3; $$=$1; } 
    | RParam                    
;

RParam:                         
    Exp                         %prec ','{$$=create_node2(yylloc.first_line, NT_RPARAM, "RParam", $1, NULL); } //prec 用于解决冲突
;
%%
char *strcat_tmp(char *t, char *f){
    static char buf[20];
    strcpy(buf, t);
    return strcat(buf, f);
}


