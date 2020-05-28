#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "ast_node.h"
#include "parse.tab.h" //含yyparse()声明
#include "symbol_table.h"
#include "define.h"

/**----------------EXTERN start---------------*/
//flex
extern FILE *yyin;
//bision
extern ast_node *root;

//semantic_analysis
void semantic_analysis(ast_node *root);
/**----------------EXTERN end----------------*/

/**----------------全局变量start---------------*/
char g_file_name[FILE_NAME_MAX_LENGTH];
//symbol table
SYMBOL_TABLE ST;
BLOCK_INDEX_TABLE BIT;
uint symbol_hash_table[SYMBOL_TABLE_MAX_SIZE];

/**----------------全局变量end-----------------*/



int main(int argc, char *argv[]){
    /*-----------------语法分析：构造AST树 start------------------*/
    if(argc==1){ //从stdin读取输入
        printf("----------------------\n");
        printf("Welcome to my Compiler\n");
        printf("----------------------\n");
        printf("> ");
        yyparse();
    }else{ //从文件读取输入
        strncpy(g_file_name, argv[1], FILE_NAME_MAX_LENGTH);
        FILE *fp = fopen(g_file_name, "r");
        if(fp==NULL){printf("Open %s faild!.", g_file_name); exit(-1); }
        yyin = fp;  //设置flex读取指定文件

        yyparse();
    }
    /*-----------------语法分析：构造AST树 end--------------------*/
    display(root, 0);

    /*----------------------语义分析 start------------------------*/
    ST.top = BIT.top = 0;   //初始化符号表，块索引表
    semantic_analysis(root);
    /*----------------------语义分析 end--------------------------*/


    ast_free(root);
    return 0;
}

void print_error(int lineno, char *s, ...){
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%s : %d \tError, ", g_file_name, lineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}