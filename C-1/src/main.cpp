#include <iostream>

#include "lexer.h"

using namespace std;


int main() {
    Token* token;

    Lexer lex;
    lex.set_file("D:\\Projects\\VS_projects\\C-1\\C-1\\test\\lex\\1_ID_INTEGER.txt");

    int cnt = 1;
    token = lex.next_token();
    while (1) {
        printf("%3d\t", lex.m_cur_line);
        if (token->type == END_OF_FILE) {
            cout << "EOF" << endl;
            break;
        }
        else if (token->type == ID) {
            cout << "ID: " << ((ID_Token*)token)->attr << endl;
        }
        else if (token->type == CONST_INT) {
            cout << "CONST_INT: " << ((Int_Token*)token)->attr << endl;
        }
        else if (token->type == CONST_FLOAT) {
            cout << "CONST_FLOAT: " << ((Float_Token*)token)->attr << endl;
        }
        else if (token->type == CONST_CHAR) {
            cout << "CONST_CHAR: " << ((Char_Token*)token)->attr << endl;
        }
        else if (token->type == CONST_STR) {
            cout << "CONST_STR: " << ((String_Token*)token)->attr << endl;
        }
        else if (token->type == OP_ASSIGN) {
            cout << "OP_ASSIGN: " << ((OpAssign_Token*)token)->attr << endl;
        }
        else if (token->type == REL_OP) {
            cout << "REL_OP: " << ((RelOp_Token*)token)->attr << endl;
        }
        else if (AUTO <= token->type && token->type < END_OF_FILE) {
            cout << "Keyword: " << token->type << endl;
        }
        else {
            cout << Debug_token_type_to_str(token->type) << endl;
        }
        token = lex.next_token();

        cnt++;
    }
    cout << "统计信息：" << "\n字符数：" << lex.m_nchar <<
            "\n单词数：" << lex.m_nword <<
            "\n行数：" << lex.m_cur_line << endl;

    return 0;
}