#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "ast_node.h"
#include "parse.tab.h"

#define FILE_NAME_MAX_LENGTH 20
extern int yylex (void);
extern FILE *yyin;
extern int yylineno;
extern char *yytext;
extern char g_file_name[FILE_NAME_MAX_LENGTH];


int main(int argc, char **argv){
    FILE *fp;

    fp = fopen(argv[1], "r");
    if(fp==NULL){
        printf("can't open file: %s", argv[1]);
    }
    strncpy(g_file_name, argv[1], FILE_NAME_MAX_LENGTH);

    yyin = fp;
    yylineno = 1;

    int tkn;
    while(tkn = yylex()){
        printf("%2d, %3d, %s\n", yylineno, tkn, yytext);
    }
}