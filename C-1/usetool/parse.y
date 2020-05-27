%{
//parse.tab.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast_node.h"
extern int yylineno;
int yylex(void);
void yyerror(char *s, ...);

ast_node *root;
%}
%union {
    ast_node *type_p_ast_node;
    ast_leaf *type_p_ast_leaf;
    char type_relop[3];
    char type_opassign[3];
}

%token S_PLUS S_MINUS '+' '-' '*' '/' '%' AND OR '!' ',' '='
%token ';' '(' ')' '[' ']' '{' '}'
%token  IF ELSE WHILE RETURN END_OF_FILE
%token <type_relop> REL_OP
%token <type_opassign> OP_ASSIGN
%token <type_p_ast_leaf> ID C_INT C_FLOAT C_CHAR INT FLOAT CHAR VOID BOOL TRUE FALSE

%type <type_p_ast_node> Program ExtDefList ExtDef VarDec FuncDef Specificer VarList Var FuncHeader ParamList CompSt Param RParamList RParam
%type <type_p_ast_node> StmtList Stmt Exp

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
                        $$=create_node(yylineno, "Program", $1, NULL);
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
    Specificer VarList ';'      {$1->sibling=$2; $$=create_node(yylineno, "VarDec", $1, NULL); }
;

VarList:                        
    Var ',' VarList             {$1->sibling = $3; $$=$1; }
    | Var                       
;

Var: 
    ID                          {$$=create_node(yylineno, "Var", (ast_node *)$1, NULL); }
    | ID '=' Exp                {$1->sibling=$3; $$=create_node(yylineno, "Var=", (ast_node *)$1, NULL); }
;

//函数定义
FuncDef: 
    Specificer FuncHeader CompSt
                                {
                                    $1->sibling = $2;
                                    $2->sibling = $3;
                                    $$=create_node(yylineno, "FuncDef", $1, NULL);
                                }
;

FuncHeader:
    ID '(' ParamList ')'        {$1->sibling = $3; $$=create_node(yylineno, "FuncHeader", (ast_node *)$1, NULL);}
;

ParamList:                      {$$=NULL; }
    | Param ',' ParamList       {$1->sibling = $3; $$=$1; }
    | Param                     
;

Param: 
    Specificer ID               {$1->sibling=(ast_node *)$2; $$=create_node(yylineno, "Param", $1, NULL); }
;

Specificer:
    INT                         {$$=(ast_node *)$1; }
	| FLOAT                     {$$=(ast_node *)$1; }
	| CHAR                      {$$=(ast_node *)$1; }
	| BOOL                      {$$=(ast_node *)$1; }
;

//复合语句 | 语句块
CompSt:
    '{' StmtList '}'                {$$=create_node(yylineno, "CompSt", $2, NULL);  }
;

StmtList:                           {$$=NULL; }
    | Stmt StmtList                 {$1->sibling = $2; $$=$1; }
;

Stmt:
      Exp ';'                       {$$=create_node(yylineno, "Stmt-Exp", $1, NULL); }
    | ID '=' Exp ';'                {$1->sibling = $3; $$=create_node(yylineno, "Stmt-Assign", (ast_node *)$1, NULL); }
    | ID OP_ASSIGN Exp ';'          {$1->sibling = $3; char tmp[20]="Stmt-OpAssign"; $$=create_node(yylineno, strcat(tmp, $2), (ast_node *)$1, NULL); }
    | IF '(' Exp ')' Stmt           %prec IF {$3->sibling = $5; $$=create_node(yylineno, "Stmt-if", $3, NULL); }
    | IF '(' Exp ')' Stmt ELSE Stmt %prec ELSE {
                                        $3->sibling = $5;
                                        $5->sibling = $7;
                                        $$=create_node(yylineno, "Stmt-if-else", $3, NULL); 
                                    }
    | WHILE '(' Exp ')' Stmt        {$3->sibling = $5; $$=create_node(yylineno, "Stmt-while", $3, NULL); }
    | RETURN Exp ';'                {$$=create_node(yylineno, "Stmt-return", $2, NULL); }
    | CompSt 
    | VarDec                         
;

Exp:
      Exp '+' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp+", $1, NULL); }
    | Exp '-' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp-", $1, NULL); }
    | Exp '*' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp*", $1, NULL); }
    | Exp '/' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp/", $1, NULL); }
    | Exp '%' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp%", $1, NULL); }
    | Exp REL_OP Exp{$1->sibling = $3; char tmp[10]="exp"; $$=create_node(yylineno, strcat(tmp, $2), $1, NULL); }
    | S_PLUS Exp    {$$=create_node(yylineno, "exp-left++", $2, NULL); }
    | S_MINUS Exp   {$$=create_node(yylineno, "exp-left--", $2, NULL); }
    | Exp S_PLUS    {$$=create_node(yylineno, "exp-right++", $1, NULL); }
    | Exp S_MINUS   {$$=create_node(yylineno, "exp-right--", $1, NULL); }
    | Exp AND Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp-and", $1, NULL); }
    | Exp OR Exp    {$1->sibling = $3; $$=create_node(yylineno, "exp-or", $1, NULL); }
    | '!' Exp       {$$=create_node(yylineno, "exp-not", $2, NULL); }
    | '-' Exp       %prec '!' {$$=create_node(yylineno, "exp-neg", $2, NULL); }
    | '(' Exp ')'   {$$=create_node(yylineno, "exp-()", $2, NULL); }
    | Exp ',' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp-comma", $1, NULL); }
    | ID            {$$=(ast_node *)$1; }
    | C_INT         {$$=(ast_node *)$1; }
    | C_FLOAT       {$$=(ast_node *)$1; }
    | C_CHAR        {$$=(ast_node *)$1; }
    | TRUE          {$$=(ast_node *)$1; }
    | FALSE         {$$=(ast_node *)$1; }
    | ID '(' RParamList ')' {$1->sibling=$3; $$=create_node(yylineno, "exp-func-call", (ast_node*)$1, NULL); }
;

//实参列表
RParamList:                     {$$=NULL; }
    | RParam ',' RParamList     {$1->sibling=$3; $$=$1; } 
    | RParam                    
;

RParam:                         
    Exp                         %prec ','{$$=create_node(yylineno, "RParam", $1, NULL); } //prec 用于解决冲突
;
%%

int main(){
    printf("----------------------\n");
    printf("Welcome to my Compiler\n");
    printf("----------------------\n");
    printf("> ");
    yyparse();

    display(root, 0);
    return 0;
}