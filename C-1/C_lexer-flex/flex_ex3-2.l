%option noyywrap
%x IFILE
%{
/*作用：将输入添加行号输出

1. 读取多个输入
2. #include <file> 嵌套处理
*/
#include <stdio.h>
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

%%
^#[ \t]*include[ \t]*[<"]  {BEGIN IFILE; }
<IFILE>[^ \t\n">]+ {
                {
                    char ch;
                    while((ch=input())&&ch!='\n');
                }
                fprintf(yyout, "INCLUDE %s\n", yytext);
                push_file(yytext);
                BEGIN INITIAL;
            }
<IFILE>\n   {fprintf(yyout, "bad input in %d of \"%\"", yylineno, cur_buf->filename);
             yyterminate();}
<<EOF>>     {fprintf(yyout, "\n"); if(!pop_file()) yyterminate();}

^\n         {fprintf(yyout, "%-4d%s", yylineno++, yytext); }
^.          {fprintf(yyout, "%-4d%s", yylineno, yytext);}
\n          {ECHO; yylineno++; }
.           {ECHO; }
%%

int main(int argc, char **argv){
    if(argc==1){
        yylex();
        return 0;
    }
    int i;
    for(i=1; i<argc; i++){
        push_file(argv[i]);
        yylex();
    }
    return 0;
}

int push_file(char *filename){
    //save lineno
    if(cur_buf){
        cur_buf->lineno = yylineno + 1;
    }
    //new buf
    FILE *fp = fopen(filename, "r");
    if(!fp){fprintf(yyout, "open file %s failed.", filename); exit(-1);}

    bufstack *buf = malloc(sizeof(bufstack));
    buf->next_buf = cur_buf;
    cur_buf = buf;
    buf->bs = yy_create_buffer(fp, YY_BUF_SIZE);
    buf->filename = filename;
    buf->fp;

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
    if(!cur_buf){
        return 0; //empty stack
    }
    yylineno = cur_buf->lineno;
    yy_switch_to_buffer(cur_buf->bs);
    return 1;
}

