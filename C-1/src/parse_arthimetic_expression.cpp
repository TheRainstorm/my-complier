#include <iostream>
#include <string>
#include <stack>
#include <map>
#include <vector>

using namespace std;

typedef enum Token_type {
    TERMINATOR, //$

    PLUS,
    MINUS,
    MULT,
    DIV,
    OPEN_PARENTHESES,
    CLOSE_PARENTHESES,

    INTEGER,

    NO_TERMIN   //Vn, the stack need to store Vn, so define it as a special Token type
}Token_type;

int g_priority_relation[8][8] = { //both equal and empty is set to 0
    //$, +, -, *, /, (, ), i
    {0, -1, -1, -1, -1, -1, 0,  -1},
    {1, 1,  1,  -1, -1, -1, 1,  -1},
    {1, 1,  1,  -1, -1, -1, 1,  -1},
    {1, 1,  1,  1,  1,  -1, 1,  -1},
    {1, 1,  1,  1,  1,  -1, 1,  -1},
    {0, -1, -1, -1, -1, -1, 0,  -1},
    {1, 1,  1,  1,  1,  0,  1,  0},
    {1, 1,  1,  1,  1,  0,  1,  0}
};

int cmp_prior(Token_type a, Token_type b) {
    // static map<Token_type, int> Token_type2index={{TERMINATOR, 0}, {PLUS, 1}, {MINUS, 2}, {MULT, 3}, {DIV, 4}, {OPEN_PARENTHESES, 5}, {CLOSE_PARENTHESES, 6}, {INTEGER, 7}};

    return g_priority_relation[(int)a][(int)b];
}

class Token {
public:
    Token_type type;
    string str;

    Token(Token_type t, string s) {
        type = t;
        str = s;
    };
};


class Parser {
public:
    vector<Token> m_token_stream;
    vector<vector<Token> > m_reduce_sequence;

    Parser(vector<Token> token_stream) {
        m_token_stream = token_stream;
    };

    void parse(); //get reduce_sequence
    void construct_tree();//construct Óï·¨¼Ü×ÓÊ÷ from reduce sequence
    void draw_tree();
    int get_result(); //caculate result
};

void Parser::parse() {
    stack<Token> s, s_Vt; //s store Vt and Vn, s_Vt store only Vt

    //init
    s.push(Token(TERMINATOR, "$"));
    s_Vt.push(Token(TERMINATOR, "$"));
    auto cur_token_it = m_token_stream.begin();

    while (1) {
        int cmp_result = cmp_prior(s_Vt.top().type, cur_token_it->type);
        if (cmp_result <= 0) { //move forward
            if (cur_token_it->type == TERMINATOR) { //$
                s.pop();
                if (s.size() == 1 && s.top().type == TERMINATOR) {
                    cout << "parase success" << endl;
                }
                else {
                    cout << "parase error" << endl;
                    cout << "s content:---" << endl;
                    while (!s.empty()) {
                        cout << s.top().str << endl;
                        s.pop();
                    }
                    cout << "-------------" << endl;
                }
                break;
            }
            s.push(*cur_token_it);
            s_Vt.push(*cur_token_it);

            cur_token_it++;
        }
        else { //reduce
            Token t1 = s_Vt.top(); s_Vt.pop();
            Token t2 = s_Vt.top();
            int cnt = 1; //record the Vt num
            while (!(cmp_prior(t2.type, t1.type) < 0)) {
                t1 = t2; s_Vt.pop();
                t2 = s_Vt.top();
                cnt++;
            }
            vector<Token> reduce;
            
            for (int i = 0; i < cnt + 1;) {
                Token t = s.top();
                if (t.type != NO_TERMIN) {
                    i++;
                }
                if (i == cnt + 1) {
                    break;
                }
                s.pop();
                reduce.insert(reduce.begin(), t);
            }
            m_reduce_sequence.push_back(reduce);
            s.push(Token(NO_TERMIN, "Vn"));
        }
    }

    cout << "reduce sequence:" << endl;
    for (auto it : m_reduce_sequence) {
        for (auto it2 : it) {
            cout << it2.str;
        }
        cout << endl;
    }
}

void Parser::construct_tree() {
    ;
}

void Parser::draw_tree() {
    ;
}

int Parser::get_result() {
    stack<int> s;
    int a, b;
    for (auto reduce : m_reduce_sequence) {
        if (reduce.size() == 1) {
            s.push(atoi(reduce[0].str.c_str()));
        }
        else if (reduce[1].str == "+") {
            a = s.top(); s.pop();
            b = s.top(); s.pop();
            s.push(b + a);
        }
        else if (reduce[1].str == "-") {
            a = s.top(); s.pop();
            b = s.top(); s.pop();
            s.push(b - a);
        }
        else if (reduce[1].str == "*") {
            a = s.top(); s.pop();
            b = s.top(); s.pop();
            s.push(b * a);
        }
        else if (reduce[1].str == "/") {
            a = s.top(); s.pop();
            b = s.top(); s.pop();
            s.push(b / a);
        }
        else {
            continue;
        }
    }
    return s.top();
}

int main() {
    vector<Token> i_token_stream;
    //1+2*3
    /*i_token_stream.push_back(Token(INTEGER, "1"));
    i_token_stream.push_back(Token(PLUS, "+"));
    i_token_stream.push_back(Token(INTEGER, "2"));
    i_token_stream.push_back(Token(MULT, "*"));
    i_token_stream.push_back(Token(INTEGER, "3"));
    i_token_stream.push_back(Token(TERMINATOR, "$"));*/

    //(1+2)*3
    i_token_stream.push_back(Token(OPEN_PARENTHESES, "("));
    i_token_stream.push_back(Token(INTEGER, "1"));
    i_token_stream.push_back(Token(PLUS, "+"));
    i_token_stream.push_back(Token(INTEGER, "2"));
    i_token_stream.push_back(Token(CLOSE_PARENTHESES, ")"));
    i_token_stream.push_back(Token(MULT, "*"));
    i_token_stream.push_back(Token(INTEGER, "3"));
    i_token_stream.push_back(Token(TERMINATOR, "$"));

    //(1+2)*(3-4)
    /*i_token_stream.push_back(Token(OPEN_PARENTHESES, "("));
    i_token_stream.push_back(Token(INTEGER, "1"));
    i_token_stream.push_back(Token(PLUS, "+"));
    i_token_stream.push_back(Token(INTEGER, "2"));
    i_token_stream.push_back(Token(CLOSE_PARENTHESES, ")"));
    i_token_stream.push_back(Token(MULT, "*"));
    i_token_stream.push_back(Token(OPEN_PARENTHESES, "("));
    i_token_stream.push_back(Token(INTEGER, "3"));
    i_token_stream.push_back(Token(MINUS, "-"));
    i_token_stream.push_back(Token(INTEGER, "4"));
    i_token_stream.push_back(Token(CLOSE_PARENTHESES, ")"));
    i_token_stream.push_back(Token(TERMINATOR, "$"));*/

    Parser parser(i_token_stream);

    parser.parse();

    cout << "result: " <<parser.get_result()<< endl;

    return 0;
}