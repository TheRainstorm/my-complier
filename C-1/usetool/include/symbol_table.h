#ifndef SYMBOL_H_INCLUDE
#define SYMBOL_H_INCLUDE
#include "define.h"

#define SYMBOL_TABLE_MAX_SIZE 1000
#define HASH_TABLE_MAX_SIZE 1000
#define BLOCK_INDEX_TABLE_MAX_SIZE 20

typedef struct symbol //符号表的一行
{
    char name[ID_MAX_LENGTH];   //符号名

    int type;                   //变量类型或函数返回类型 int: 0, float: 1
    int dimen;                  //数组维数或函数参数个数
    // int level;                  //层数，全局变量和函数名为0，每进入一个块加1，函数形参为1
    
    char flag;                  //'V':变量 'F':函数 'T':临时变量
    int offset;                 //偏移
    int width;                  //字节数
    uint chain_next;            //符号散列表冲突时，用于指向之前的符号表项。
    //...
}symbol;

typedef struct  //符号表
{
    symbol stack[SYMBOL_TABLE_MAX_SIZE];
    int top;
}SYMBOL_TABLE;

typedef struct  //块索引表
{
    int stack[BLOCK_INDEX_TABLE_MAX_SIZE];
    int top;
}BLOCK_INDEX_TABLE;

extern SYMBOL_TABLE ST;
extern BLOCK_INDEX_TABLE BIT;
//哈希表
extern uint symbol_hash_table[SYMBOL_TABLE_MAX_SIZE];

#define HASH BPHash
uint BPHash(char* str);

//
int search_symbol_table(char *name);
int insert_symbol_table(char *name, int type, int dimen);

//
int linear_search_symbol_table(char *name);
int linear_insert_symbol_table(char *name, int type, int dimen);

void print_symbol_table();

void location();
void relocation();

void print_block_index_table();

#endif