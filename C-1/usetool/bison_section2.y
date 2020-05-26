Program: ExtDefList //全局定义
;

ExtDefList: ExtDef ExtDefList
;

ExtDef: VarDec
| FuncDef
;


VarDec: Specificer VarList ';' //变量声明
;

VarList: Var ',' VarList 
| Var
;

Var: ID 
| ID '=' Exp
;


FuncDef: Specificer ID '(' ParamList ')' CompSt //函数定义
;

ParamList: 
| Param ',' ParamList 
| Param 
;

Param: Specificer ID
;


CompSt: '{' StmtList '}' //复合语句 | 语句块
;

StmtList:
| Stmt StmtList
;

Stmt: 
      Exp ';'                       {$$=create_node(yylineno, "Stmt-Exp", $1, NULL); }
    // | CompSt                        
    // | VarDec                        
    | ID '=' Exp;                   {$1->sibling = $3; $$=create_node(yylineno, "Stmt-Assign", $1, NULL); }
    | ID OP_ASSIGN Exp;             {$1->sibling = $3; $$=create_node(yylineno, $2, $1, NULL); }
    | IF '(' Exp ')' Stmt           {$3->sibling = $5; $$=create_node(yylineno, "if", $3, $5); }
    | IF '(' Exp ')' Stmt ELSE Stmt {
                                        $3->sibling = $5;
                                        $5->sibling = $7;
                                        $$=create_node(yylineno, "if else", $3, NULL); 
                                    }
    | WHILE '(' Exp ')' Stmt        {$3->sibling = $5; $$=create_node(yylineno, "while", $3, NULL); }
;

Exp: ID
| C_INT
| C_FLOAT
| C_CHAR
| TRUE
| FALSE
| '(' Exp ')'
| Exp '+' Exp
| Exp '-' Exp
| Exp '*' Exp
| Exp '/' Exp
| Exp '%' Exp
| Exp REL_OP Exp
| S_PLUS Exp
| S_MINUS Exp
| Exp S_PLUS
| Exp S_MINUS
| Exp AND Exp
| Exp OR Exp
| '!' Exp
| ID '(' RParamList ')'
;


RParamList: //实参列表
| RParam ',' RParamList 
| RParam 
;

RParam: Exp
;


Specificer: INT
| FLOAT
| CHAR
| BOOL
;