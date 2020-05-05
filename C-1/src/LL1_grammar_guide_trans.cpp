#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef enum Token_type {
    ID,
    INTEGER, //integer
    REAL, //real

    COMMA, //,
    COLON, //:

    TERMINATE //$, end of token
}Token_type;

class Token {
public:
    Token_type type;
    string str;

    Token() {}

    Token(Token_type t, string s) {
        type = t;
        str = s;
    };
};

class Symbol {
public:
    string name;
    string type;
    Symbol(string name, string type) {
        this->name = name;
        this->type = type;
    }
};

class Parser {
public:
    vector<Token> m_token_stream;
    vector<Symbol> m_symbol_table;

    int m_cur_pos;
    Token m_cur_token;

    Parser(vector<Token> token_stream) {
        m_token_stream = token_stream;
        m_cur_pos = 0;
        m_cur_token = m_token_stream[m_cur_pos];
    }
public:
    void advance() {
        m_cur_pos += 1;
        m_cur_token = m_token_stream[m_cur_pos];
    }

    void error(string s) {
        cout << s << endl;
        exit(-1);
    }

    void parse() {
        fxD();
        if (m_cur_token.type != TERMINATE) {
            error("not finish");
        }
        else {
            cout << "AC" << endl;
        }
    }

    void fxD() {
        string id_name, L_type;
        if (m_cur_token.type != ID) {
            error("Need an id");
        }
        id_name = m_cur_token.str;
        advance();
        L_type = fxL();
        m_symbol_table.push_back(Symbol(id_name, L_type));
    }

    string fxL() {
        string id_name, L1_type, T_type;
        if (m_cur_token.type == COMMA) {
            advance();
            if (m_cur_token.type != ID) {
                error("Need an id");
            }
            id_name = m_cur_token.str;
            advance();
            L1_type = fxL();
            m_symbol_table.push_back(Symbol(id_name, L1_type));

            return L1_type;
        }
        else if (m_cur_token.type == COLON) {
            advance();
            T_type = fxT();

            return T_type;
        }
        else {
            error("Need , or :");
        }
    }

    string fxT() {
        if (m_cur_token.type == INTEGER) {
            advance();
            return "integer";
        }
        else if (m_cur_token.type == REAL) {
            advance();
            return "real";
        }
        else {
            error("need integer or real");
        }
    }
};

int main() {
    vector<Token> i_token_stream;
    //i, j, k: integer
    i_token_stream.push_back(Token(ID, "i"));
    i_token_stream.push_back(Token(COMMA, ","));
    i_token_stream.push_back(Token(ID, "j"));
    i_token_stream.push_back(Token(COMMA, ","));
    i_token_stream.push_back(Token(ID, "k"));
    i_token_stream.push_back(Token(COLON, ":"));
    i_token_stream.push_back(Token(INTEGER, "integer"));
    i_token_stream.push_back(Token(TERMINATE, "$"));

    Parser parser(i_token_stream);

    parser.parse();

    for (auto symbol : parser.m_symbol_table) {
        cout << symbol.name << " " << symbol.type << endl;
    }
    return 0;
}