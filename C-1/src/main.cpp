#include <iostream>

#include "lexer.h"

using namespace std;


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: C-1.exe filepath" << endl;
        exit(-1);
    }

    Token* token;
    Lexer lex;
    lex.set_file(argv[1]);

    int cnt = 1;
    token = lex.next_token();
    while (1) {
        printf("%3d\t", lex.m_cur_line);
        if (token->type == END_OF_FILE) {
            cout << "EOF" << endl;
            delete token;
            break;
        }
        else if (token->type == ID) {
            cout << "ID: " << ((ID_Token*)token)->attr << endl;
            delete ((ID_Token*)token);
        }
        else if (token->type == CONST_INT) {
            cout << "CONST_INT: " << ((Int_Token*)token)->attr << endl;
            delete ((Int_Token*)token);
        }
        else if (token->type == CONST_FLOAT) {
            cout << "CONST_FLOAT: " << ((Float_Token*)token)->attr << endl;
            delete ((Float_Token*)token);
        }
        else if (token->type == CONST_CHAR) {
            cout << "CONST_CHAR: " << ((Char_Token*)token)->attr << endl;
            delete ((Char_Token*)token);
        }
        else if (token->type == CONST_STR) {
            cout << "CONST_STR: " << ((String_Token*)token)->attr << endl;
            delete ((String_Token*)token);
        }
        else if (token->type == OP_ASSIGN) {
            cout << "OP_ASSIGN: " << ((OpAssign_Token*)token)->attr << endl;
            delete ((OpAssign_Token*)token);
        }
        else if (token->type == REL_OP) {
            cout << "REL_OP: " << ((RelOp_Token*)token)->attr << endl;
            delete ((RelOp_Token*)token);
        }
        else if (AUTO <= token->type && token->type < END_OF_FILE) {
            cout << "Keyword: " << token->type << endl;
            delete token;
        }
        else {
            cout << Debug_token_type_to_str(token->type) << endl;
            delete token;
        }
        token = lex.next_token();

        cnt++;
    }
    cout << "统计信息：" << "\n字符数：" << lex.m_nchar <<
        "\n单词数：" << lex.m_nword <<
        "\n行数：" << lex.m_cur_line << endl;

    return 0;
}