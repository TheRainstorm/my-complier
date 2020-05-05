%option noyywrap
%{
/*程序作用：将输入添加行号输出

1. 可以读取多个输入
*/
#include <stdio.h>
%}

%%
^.          {fprintf(yyout, "%-4d%s", yylineno, yytext);}
^\n         {fprintf(yyout, "%-4d%s", yylineno++, yytext); }
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
        FILE *fp;
        fp = fopen(argv[i], "r");
        if(!fp) return -1;

        printf("File: %s\n", argv[i]);
        yyrestart(fp);
        yylex();
    }
    return 0;
}