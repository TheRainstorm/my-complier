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

%token <type_p_ast_leaf> ID C_INT C_FLOAT C_CHAR
%token <type_relop> REL_OP
%token <type_opassign> OP_ASSIGN
%token S_PLUS S_MINUS '+' '-' '*' '/' '%' AND OR '!' ',' '='
%token ';' '(' ')' '[' ']' '{' '}'
%token INT FLOAT CHAR VOID IF ELSE WHILE RETURN BOOL TRUE FALSE END_OF_FILE

// %type <type_p_ast_node> Program ExtDefList ExtDef VarDec FuncDef Specificer VarList Var ParamList CompSt Param
// %type <type_p_ast_node> StmtList Stmt Exp RParamList RParam

%type <type_p_ast_node> Program CompSt StmtList Stmt Exp

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
    Stmt{
            if($1!=NULL){
                $$=create_node(yylineno, "Program", $1, NULL);
                root=$$;
            }else{
                printf("Not accept.\n");
                root = NULL;
            }
        }
;

//复合语句 | 语句块
CompSt:
    '{' StmtList '}'                {$$=create_node(yylineno, "CompSt", $2, NULL);  }
;

StmtList:                           {$$=NULL; }
    | Stmt StmtList                     {$1->sibling = $2; $$=$1; }
;

Stmt:
      Exp ';'                       {$$=create_node(yylineno, "Stmt-Exp", $1, NULL); }
    | ID '=' Exp ';'                {$1->sibling = $3; $$=create_node(yylineno, "Stmt-Assign", (ast_node *)$1, NULL); }
    | ID OP_ASSIGN Exp ';'          {$1->sibling = $3; char tmp[10]="Stmt-"; $$=create_node(yylineno, strcat(tmp, $2), (ast_node *)$1, NULL); }
    | IF '(' Exp ')' Stmt           %prec IF {$3->sibling = $5; $$=create_node(yylineno, "Stmt-if", $3, $5); }
    | IF '(' Exp ')' Stmt ELSE Stmt %prec ELSE {
                                        $3->sibling = $5;
                                        $5->sibling = $7;
                                        $$=create_node(yylineno, "Stmt-if-else", $3, NULL); 
                                    }
    | WHILE '(' Exp ')' Stmt        {$3->sibling = $5; $$=create_node(yylineno, "Stmt-while", $3, NULL); }
    | RETURN Exp ';'                {$$=create_node(yylineno, "Stmt-Return", $2, NULL); }
    | CompSt                        
;

Exp:
      Exp '+' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp+", $1, NULL); }
    | Exp '-' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp-", $1, NULL); }
    | Exp '*' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp*", $1, NULL); }
    | Exp '/' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp/", $1, NULL); }
    | Exp '%' Exp   {$1->sibling = $3; $$=create_node(yylineno, "exp%", $1, NULL); }
    | Exp REL_OP Exp{$1->sibling = $3; char tmp[10]="exp-"; $$=create_node(yylineno, strcat(tmp, $2), $1, NULL); }
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
    | TRUE          {$$=(ast_node *)create_leaf(yylineno, TRUE, NULL, NULL); }
    | FALSE         {$$=(ast_node *)create_leaf(yylineno, FALSE, NULL, NULL); }
    // | ID '(' RParamList ')'
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