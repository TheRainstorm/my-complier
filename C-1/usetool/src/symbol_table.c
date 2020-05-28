#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int BPHash(char* str) 
{ 
  unsigned int hash = 0; 
  while(*str){
      hash = hash << 7 ^ (*str);
      str++;
  }
  return hash;
}

static inline void set_symbol_value(symbol *sym, char *name, int type, int dimen){
    strncpy(sym->name, name, ID_MAX_LENGTH);
    sym->type = type;
    sym->dimen = dimen;
}

int search_symbol_table(char *name){    //引用时调用
    symbol *sym;
    uint h = HASH(name)%HASH_TABLE_MAX_SIZE;
    uint index = symbol_hash_table[h];

    if(index>=ST.top){
        return -1;    //没找到
    }
    //访问整个冲突链，检查是否有其定义
    while(1){
        sym = &(ST.stack[index]);
        if(strcmp(name, sym->name)==0){ //定义过
            return index;
        }
        if(sym->chain_next!=-1){
            index = sym->chain_next;
        }else{
            break;
        }
    }

    return -1;
}

int insert_symbol_table(char *name, int type, int dimen){  //声明时调用
    symbol *sym;

    uint h = HASH(name)%HASH_TABLE_MAX_SIZE;
    uint index = symbol_hash_table[h];
    uint index_save;

    if(index>=ST.top){  //0 没定义过，且没hash冲突
        symbol_hash_table[h] = ST.top;  //更新hash表
        sym = &ST.stack[ST.top++];
    }else{  //1 定义过：1.1 局部变量 1.2 非局部变量  
            //2 没定义过，hash冲突
        //访问冲突链，检查是否有重定义
        index_save = index;
        while(index >= BIT.stack[BIT.top]){  //while条件成立时表示，即使定义过也是非局部变量
            sym = &(ST.stack[index]);
            if(strcmp(name, sym->name)==0){ //定义过
                printf("Error, 符号重定义\n");
                return -1;
            }
            if(sym->chain_next!=-1){
                index = sym->chain_next;
            }else{
                break;
            }
        }
        //可以定义该符号
        symbol_hash_table[h] = ST.top;
        sym = &ST.stack[ST.top++];
        sym->chain_next = index_save;
    }

    //设置sym的属性值
    set_symbol_value(sym, name, type, dimen);

    return symbol_hash_table[h];
}

int linear_search_symbol_table(char *name){
    int i;
    for(i=ST.top-1; i>=0; i--){
        if(strcmp(ST.stack[i].name, name)==0) return i;
    }
    return -1;
}

int linear_insert_symbol_table(char *name, int type, int dimen){
    int i;
    for(i=ST.top-1; i>=0; i--){
        if(i<BIT.stack[BIT.top-1]) break;
        if(strcmp(ST.stack[i].name, name)==0){
            printf("Error, 符号(%s)重定义\n", name);
            return -1;
        }
    }
    symbol *sym;
    sym = &(ST.stack[ST.top++]);
    //设置sym的属性值
    set_symbol_value(sym, name, type, dimen);

    return ST.top-1;
}

void print_symbol_table(){
    int i=0;
    // wprintf(L"%6ls %6ls %6ls %6ls\n",L"编 号", L"变量名", L"类  型", L"链 接");
    printf("-------------------------------------------------\n");
    printf("%6s %6s %6s %6s %6s\n", "Num", "Name", "Type", "Dimen", "Chain");
    printf("-------------------------------------------------\n");
    for(i=ST.top-1;i>=0;i--)
        printf("%6d %6s %6s %6d\n", i,\
            ST.stack[i].name,\
            ST.stack[i].type==0? "int": ST.stack[i].type==1 ? "float" : "-1",\
            ST.stack[i].dimen,\
            ST.stack[i].chain_next);
    printf("-------------------------------------------------\n");
}

void location(){
    BIT.stack[BIT.top++] = ST.top;  //push(ST.top)
}

void relocation(){
    ST.top = BIT.stack[--BIT.top]; //ST.top = pop()
}

void print_block_index_table(){
    printf("--------------------------\n");
    printf("%6s %6s\n", "Num", "Index");
    printf("--------------------------\n");
    int i=0;
    for(i=BIT.top-1;i>=0;i--)
        printf("%6d %6d\n", i,\
            BIT.stack[i]);

    printf("--------------------------\n");
}
