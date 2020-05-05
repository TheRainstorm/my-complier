%option noyywrap nodefault yylineno
%x COMMENT
%x INCLUDE


%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NHASH 10000
enum token_type{
    //keyword
    IF = 10,
    ELES,
    WHILE,
    FOR,
    RETURN,
    INT,
    CHAR,

    //const
    INTEGER =50,
    FLOAT,
    CONST_CHAR,
    CONST_STR,

    //operator
    PLUS =60,
    MINS,
    MULT,
    DIV,
    AND,
    OR,
    NOT,
    ASSIGN,
    COMMA,
    //relation operator
    EQUAL = 70,
    NOT_EQUAL,
    LT,
    LE,
    GT,
    GE,

    //delimiter
    SEMICOLON = 100,
    OPEN_PARENTHESES,
    CLOSE_PARENTHESES,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    
    //else
    ID =120
};

typedef unsigned int uint;
typedef struct symbol{
    uint id;
    char *str;
}symbol;
uint g_id_cnt=0; //记录符号的编号id，从1开始递增

/*用hash表存储符号，便于查找*/
uint hash(char *);
symbol g_symtab[NHASH];
symbol * lookup(char *);

/* #include */
typedef struct bufstack{
    struct bufstack *next_buf;
    YY_BUFFER_STATE bs;
    char *filename;
    int lineno;
    FILE *fp;
}bufstack;
bufstack *cur_buf;
int push_file(char *);
int pop_file();
%}


NUM                 ([0-9]|[1-9][0-9]*)
EXP                 (E[-+]?([0-9]|[1-9][0-9]*))
I_TYPE              (([uU]?(l|L|ll|LL)?)|((l|L|ll|LL)?[uU]?))
F_TYPE              (f|F|l|L)


%%
 /*关键字*/
if                  {return IF;     }
else                {return ELES;   }
while               {return WHILE;  }
for                 {return FOR;    }
return              {return RETURN; }
int                 {return INT;    }
char                {return CHAR;   }
 /*运算符*/
'.'                 {return CONST_CHAR; }
"+"                 {return PLUS;   }
"-"                 {return MINS;   }
"*"                 {return MULT;   }
"/"                 {return DIV;    }
"&"                 {return AND;    }
"|"                 {return OR;     }
"!"                 {return NOT;    }
"="                 {return ASSIGN;   }
","                 {return COMMA;   }
"=="                {return EQUAL;   }
"!="                {return NOT_EQUAL;   }
"<"                 {return LT;   }
"<="                {return LE;   }
">"                 {return GT;   }
">="                {return GE;   }
 /*界限符*/
";"                 {return SEMICOLON;  }
"("                 {return OPEN_PARENTHESES;   }
")"                 {return CLOSE_PARENTHESES;  }
"{"                 {return OPEN_BRACE;     }
"}"                 {return CLOSE_BRACE;    }
"["                 {return OPEN_BRACKET;   }
"]"                 {return CLOSE_BRACKET;  }
 /*整数*/
[+-]?({NUM}|0[0-7]+|0[xX][0-9a-fA-F]+){I_TYPE}?     {return INTEGER;   }
[0-9]+                                              {printf("error num on line: %d", yylineno);}
 /*浮点数*/
[+-]?{NUM}\.[0-9]+{EXP}?{F_TYPE}? |
[+-]?{NUM}{EXP}{F_TYPE}? |
[+-]?{NUM}\.{EXP}?{F_TYPE}? |
[+-]?\.[0-9]+{EXP}?{F_TYPE}?                        {return FLOAT;}
 /*字符常量*/
\'([^\\'"]|\\['"\\abfnrtv]|\\0[0-7]{0,3}|\\0[xX][0-9a-fA-F]{1,2})\'     {return CONST_CHAR;}
 /*字符串*/
L?\"([^\\"]|\\['"\\abfnrtv]|\\0[0-7]{0,3}|\\0[xX][0-9a-fA-F]{1,2})*\"   {return CONST_STR;}
 /*标识符*/
[a-zA-Z_][a-zA-Z0-9_]*                              {return ID;  }
 /*else*/
    /*空白符*/
[ \t\n]+
    /*续行符*/
\\$                 /*暂时忽略其功能*/
    /*注释1*/
"//".*$
    /*注释2*/
"/*"                {BEGIN COMMENT;}
<COMMENT>"*/"       {BEGIN INITIAL;}
<COMMENT>[^*]+|.|\n /* Match 1 char every time. when meet * / use the rule above.
                        [^*]+ is for speed up. */
<COMMENT><<EOF>>    {fprintf(yyout, "Comment are not close!");}
"*/"                {fprintf(yyout, "Comment are not close!");}
 /*#include */
^#[ \t]*include[ \t]*[<"]  {BEGIN INCLUDE; }
<INCLUDE>[^ \t\n">]+    {
                            {
                                char ch;
                                while((ch=input())&&ch!='\n');
                            }
                            fprintf(yyout, "Include %s\n", yytext);
                            push_file(yytext);
                            BEGIN INITIAL;
                        }
<<EOF>>             {
                        fprintf(yyout, "Pop %s\n", cur_buf->filename);
                        if(!pop_file()) yyterminate();
                    }      
 /*无效字符*/
.                   {fprintf(yyout, "Invaild character on %s:%4d\n", cur_buf->filename, yylineno);}
%%


void print_result();

int main(int argc, char **argv){
    memset(g_symtab, '\0', sizeof(symbol)*NHASH);
    cur_buf = NULL;

    if(argc==1){ //stdin
        yylineno = 1;
        print_result();

        return 0;       
    }

    int i;
    for(i=1;i<argc;i++){ //read every file
        push_file(argv[i]);
        print_result();
    }
    return 0;
}

uint hash(char *name){
    uint hash=0;
    char ch;
    while(ch=*name++){
        hash = (hash*9)^ch;
    }
    return hash;
}

symbol *lookup(char *name){
    symbol *sym_ptr = &g_symtab[hash(name)%NHASH];
    int i;
    for(i=0; i<NHASH; i++){
        if(sym_ptr->id==0){ //empty, need to add create new item
            sym_ptr->id = ++g_id_cnt;
            sym_ptr->str = strdup(name);
            return sym_ptr;
        }else{
            if(strcmp(sym_ptr->str, name)==0) return sym_ptr;

            //find the next
            sym_ptr++;
            if(sym_ptr==g_symtab+NHASH) sym_ptr = g_symtab;
        }
    }
    printf("error, hash table overflow\n");
}

void print_result(){
    printf("Format:\n(identifier)\ttoken_code\tidentifier_code\tidentifier\tor\n");
    printf("(number)\ttoken_code\torigin_num\tnum_value\n");
    printf("(other)\ttoken_code\n");
    enum token_type tkn;
    while(1){
        tkn=yylex();
        if(tkn==ID){
            symbol *sym_ptr = lookup(yytext); //add new item in table when not find;
            printf("%4d\t%d\t%s\n", tkn, sym_ptr->id, sym_ptr->str);
        }else if(tkn==INTEGER){
            printf("%4d\t%s\t%ld\n", tkn, yytext, atol(yytext));
        }else if(tkn==FLOAT){
            printf("%4d\t%s\t%lf\n", tkn, yytext, atof(yytext));
        }else if(tkn==CONST_CHAR){
            printf("%4d\t%s\n", tkn, yytext);
        }else if(tkn==CONST_STR){
            printf("%4d\t%s\n", tkn, yytext);
        }else{
            printf("%4d\n", tkn);
        }
    }
}

int push_file(char *filename){
    //save lineno
    if(cur_buf!=NULL){
        cur_buf->lineno = yylineno + 1;
    }
    //new buf
    FILE *fp = fopen(filename, "r");
    if(!fp){fprintf(yyout, "open file %s failed.\n", filename); exit(-1);}

    bufstack *buf = malloc(sizeof(bufstack));
    buf->next_buf = cur_buf;
    cur_buf = buf;
    buf->bs = yy_create_buffer(fp, YY_BUF_SIZE);
    buf->filename = filename;
    buf->fp = fp;

    yy_switch_to_buffer(cur_buf->bs);
    yylineno = 1;
    return 0;
}

int pop_file(){
    //delete
    bufstack *buf = cur_buf;
    cur_buf = buf->next_buf;
    fclose(buf->fp);
    yy_delete_buffer(buf->bs);

    //resume lineno
    if(cur_buf==NULL){
        return 0; //empty stack
    }
    yylineno = cur_buf->lineno;
    yy_switch_to_buffer(cur_buf->bs);
    return 1;
}