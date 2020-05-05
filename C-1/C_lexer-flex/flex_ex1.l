%option noyywrap
%{
/**function: wc
*/
#include <stdio.h>
int chars=0;
int words=0;
int lines=0;
%}

%%
[a-zA-Z]+   {words++; chars+=strlen(yytext);}
\n          {lines++; chars++;};
.           {chars++;};
%%

int main(int argc, char **argv){
    if(argc==1){ //读取标准输入
        yylex();
        printf("chars: %8d\n words: %8d\n lines: %8d\n", chars, words, lines);
        return 0;
    }
    int i;
    int total_words, total_chars, total_lines;
    total_chars = total_words = total_lines = 0;
    for(i=1;i<argc;i++){ //依次读取每个文件
        FILE *f = fopen(argv[i], "r");
        if(!f){printf("Open file %s faild", argv[i]); return -1;}

        chars = words = lines = 0;
        yyrestart(f);
        yylex();
        printf("File: %s\n chars: %8d\n words: %8d\n lines: %8d\n", argv[i], chars, words, lines);
        total_chars+=chars;
        total_words+=words;
        total_lines+=lines;
    }

    printf("Totally:\n chars: %8d\n words: %8d\n lines: %8d\n", total_chars, total_words, total_lines);

    return 0;
}