#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Lexer::Lexer() {

    /*初始化关键字hash表*/

    memset(m_keyword_hash_table, 0, KEYWORD_HASH_SIZE * 9);
    memset(m_keyword_id_hash_table, 0, KEYWORD_HASH_SIZE);

    char all_keyword_str[35][9] = { "auto","short","int","long","float","double","char","struct","union","enum","typedef","const","unsigned","signed","extern","register","static","volatile","void","if","else","switch","case","for","do","while","goto","continue","break","default","sizeof","return", "bool", "true", "false"};

    int i;
    for (i = 0; i < 35; i++) {
        uint h = my_hash(all_keyword_str[i])%KEYWORD_HASH_SIZE;

        //cout << "keyword hash" << h << endl;
        if (m_keyword_hash_table[h][0] != '\0') {
            printf("Error: There is a conflict in keyword hash table\n");
            exit(-1);
        }
        strcpy_s(m_keyword_hash_table[h], 9, all_keyword_str[i]);

        m_keyword_id_hash_table[h] = AUTO + i;
    }

    //
    ;
}

Lexer::~Lexer() {
    fclose(m_fp);
}

void Lexer::set_file(const char* file_path) {
    //设置文件名
    if (strlen(file_path) >= 100) {
        printf("Error: file path(%s) is too long!\n", file_path);
        exit(-1);
    }
    strcpy_s(m_file_name, 100, file_path);
    //设置文件指针
    if (0 != fopen_s(&m_fp, file_path, "rb")) {
        printf("Error: read file(%s) failed!\n", file_path);
        exit(-1);
    }
    f_ptr = s_ptr = buf_left;
    //方便用来判断f_ptr到达了缓冲区边界
    buf_left[BUF_SIZE - 1] = -2;
    buf_right[BUF_SIZE - 1] = -2;
    //首先读取到左半边缓冲区
    size_t size;
    size = fread(buf_left, 1, BUF_SIZE - 1, m_fp);
    if (size != BUF_SIZE - 1) { //读到文件结尾
        buf_left[size] = EOF;
    }
    //设置行号为1
    m_cur_line = 1;
    //初始化统计信息
    m_nchar = m_nword = 0;
}

void Lexer::forward() {
    f_ptr++;
    size_t size;
    if (*f_ptr == -2) {
        if (f_ptr == buf_left + BUF_SIZE - 1) {
            size = fread(buf_right, 1, BUF_SIZE - 1, m_fp);
            if (size != BUF_SIZE - 1) {
                buf_right[size] = EOF;
            }
            f_ptr = buf_right;
        }
        else {
            size = fread(buf_left, 1, BUF_SIZE - 1, m_fp);
            if (size != BUF_SIZE - 1) {
                buf_left[size] = EOF;
            }
            f_ptr = buf_left;
        }
    }
    m_nchar++;
}

int Lexer::is_keyword(char* str) {
    uint h = my_hash(str)%KEYWORD_HASH_SIZE;
    if (m_keyword_hash_table[h][0] == '\0') {
        return -1;
    }
    if (strcmp(str, m_keyword_hash_table[h]) != 0) {
        return -1;
    }

    return m_keyword_id_hash_table[h];
}

bool Lexer::get_token_str(char* buf, int size) {
    /*将识别出的记号字符串复制到buf中，以\0结尾
    */
    //标记s_ptr，f_ptr是否处于左半边
    bool is_s_left = buf_left <= s_ptr && s_ptr <= buf_left + BUF_SIZE - 1;
    bool is_f_left = buf_left <= f_ptr && f_ptr <= buf_left + BUF_SIZE - 1;

    //标记是否发生了截断
    bool is_cutted = false;

    if (is_f_left && !is_s_left) { // f_ptr | s_ptr
        my_strncpy_s(buf, size, s_ptr, buf_right + BUF_SIZE - 1 - s_ptr);
        my_strncat_s(buf, size, buf_left, f_ptr - buf_left);

        if (buf_right + BUF_SIZE - 1 - s_ptr + f_ptr - buf_left >= size)
            is_cutted = true;
    }
    else if (is_s_left && !is_f_left) { // s_ptr | f_ptr
        my_strncpy_s(buf, size, s_ptr, buf_left + BUF_SIZE - 1 - s_ptr);
        my_strncat_s(buf, size, buf_right, f_ptr - buf_right);

        if (buf_left + BUF_SIZE - 1 - s_ptr + f_ptr - buf_right >= size)
            is_cutted = true;
    }
    else { //same side
        my_strncpy_s(buf, size, s_ptr, f_ptr - s_ptr);

        if (f_ptr - s_ptr >= size)
            is_cutted = true;
    }

    return is_cutted;
}

Token* Lexer::next_token() {
    /*识别并返回一个记号指针
    */
    Token* tkn = nullptr;

    //---------d-loop until get a token----------
    while (tkn == nullptr) {
    
    //跳过空白字符
    while (*f_ptr == ' ' || *f_ptr == '\t' || *f_ptr == '\n' || *f_ptr == '\r') {
        if (*f_ptr == '\n') {
            m_cur_line++;
        }
        forward();
    }
    s_ptr = f_ptr;

    /*ID*/
    if (('a' <= *f_ptr && *f_ptr <= 'z') || ('A' <= *f_ptr && *f_ptr <= 'Z') || (*f_ptr == '_')) {
        forward();
        while (('a' <= *f_ptr && *f_ptr <= 'z') || ('A' <= *f_ptr && *f_ptr <= 'Z') ||
            ('0' <= *f_ptr && *f_ptr <= '9') || (*f_ptr == '_')) {
            forward();
        }

        m_nword++;
        get_token_str(m_token_str, MAX_LENGTH_STR);
        int keyword_id = is_keyword(m_token_str);

        if (keyword_id != -1) { //关键字
            tkn = new Token((Token_type)keyword_id);
        }
        else { //标识符
            if (strlen(m_token_str) >= MAX_LENGTH_ID) {
                printf("Warnning: ID(%s) is cutted to %d characters\n", m_token_str, MAX_LENGTH_ID);
            }

            ID_Token* id_tkn = new ID_Token(ID);
             my_strncpy_s(id_tkn->attr, MAX_LENGTH_ID, m_token_str, MAX_LENGTH_STR);

            tkn = (Token*)id_tkn;
        }
    }/*整常数, 浮点数，加号，减号，点*/
    else if('0' <= *f_ptr && *f_ptr <= '9' || *f_ptr=='.' || *f_ptr=='+' || *f_ptr=='-' ){
        int state = 0;
        bool quit_loop = false;
        do {
            switch (state) {
            case 0:
                if ('1' <= *f_ptr && *f_ptr <= '9') {
                    state = 2;
                }
                else if (*f_ptr == '+') {
                    state = 9;
                }
                else if (*f_ptr == '-') {
                    state = 109;
                }
                else if (*f_ptr == '0') {
                    state = 1;
                }
                else if(*f_ptr=='.'){
                    state = 7;
                }
                break;
            case 1:
                if (*f_ptr == 'x' || *f_ptr == 'X') {
                    state = 11;
                }
                else if ('0' <= *f_ptr && *f_ptr <= '7') {
                    state = 10;
                }
                else if (*f_ptr == '.') {
                    state = 3;
                }
                else if (*f_ptr == 'e' || *f_ptr == 'E') {
                    state = 4;
                }
                else {
                    //0
                    tkn = new Int_Token(CONST_INT, 0);
                    quit_loop = true;
                    break;
                }
                break;
            case 2:
                if ('0' <= *f_ptr && *f_ptr <= '9') {
                    state = 2;
                }
                else if (*f_ptr == '.') {
                    state = 3;
                }
                else if (*f_ptr == 'e' || *f_ptr == 'E') {
                    state = 4;
                }
                else {
                    //整数
                    get_token_str(m_token_str, MAX_LENGTH_STR);
                    int attr;
                    sscanf_s(m_token_str, "%d", &attr);
                    tkn = new Int_Token(CONST_INT, attr);
                    quit_loop = true;
                    break;
                }
                break;
            case 3:
                if ('0' <= *f_ptr && *f_ptr <= '9') {
                    state = 3;
                }
                else if (*f_ptr == 'e' || *f_ptr == 'E') {
                    state = 4;
                }
                else {
                    //浮点数
                    get_token_str(m_token_str, MAX_LENGTH_STR);
                    float attr;
                    sscanf_s(m_token_str, "%f", &attr);
                    tkn = new Float_Token(CONST_FLOAT, attr);
                    quit_loop = true;
                    break;
                }
                break;
            case 4:
                if ('1' <= *f_ptr && *f_ptr <= '9') {
                    state = 6;
                }
                else if (*f_ptr == '-' || *f_ptr == '+') {
                    state = 5;
                }
                else if (*f_ptr == '0') {
                    state = 13;
                }
                else {
                    //error
                    printf("Error: Line:%2d, incorrect float.\n", m_cur_line);
                    quit_loop = true;
                    break;
                }
                break;
            case 5:
                if ('1' <= *f_ptr && *f_ptr <= '9') {
                    state = 6;
                }
                else if (*f_ptr == '0') {
                    state = 13;
                }
                else {
                    //error
                    printf("Error: Line:%2d, incorrect float.\n", m_cur_line);
                    quit_loop = true;
                    break;
                }
                break;
            case 6:
                if ('0' <= *f_ptr && *f_ptr <= '9') {
                    state = 6;
                }
                else {
                    //浮点数
                    get_token_str(m_token_str, MAX_LENGTH_STR);
                    float attr;
                    sscanf_s(m_token_str, "%f", &attr);
                    tkn = new Float_Token(CONST_FLOAT, attr);
                    quit_loop = true;
                    break;
                }
                break;
            case 7:
                if ('0' <= *f_ptr && *f_ptr <= '9') {
                    state = 8;
                }
                else {
                    //.
                    tkn = new Token(DOT);
                    quit_loop = true;
                    break;
                }
                break;
            case 8:
                if ('0' <= *f_ptr && *f_ptr <= '9') {
                    state = 8;
                }
                else if (*f_ptr == 'e' || *f_ptr == 'E') {
                    state = 4;
                }
                else {
                    //浮点数
                    get_token_str(m_token_str, MAX_LENGTH_STR);
                    float attr;
                    sscanf_s(m_token_str, "%f", &attr);
                    tkn = new Float_Token(CONST_FLOAT, attr);
                    quit_loop = true;
                    break;
                }
                break;
            case 9:
                if ('1' <= *f_ptr && *f_ptr <= '9') {
                    state = 2;
                }
                else if (*f_ptr == '0') {
                    state = 1;
                }
                else if(*f_ptr == '+'){
                    forward(); //保证f_ptr多走一步
                    tkn = new Token(SELF_PLUS);
                    quit_loop = true;
                    break;
                }
                else if (*f_ptr == '=') {
                    forward(); //保证f_ptr多走一步
                    OpAssign_Token * tmp_tkn = new OpAssign_Token(OP_ASSIGN);
                    get_token_str(tmp_tkn->attr, 4);
                    tkn = (Token*)tmp_tkn;
                    quit_loop = true;
                    break;
                }
                else {
                    tkn = new Token(PLUS);
                    quit_loop = true;
                    break;
                }
                break;
            case 109:
                if ('1' <= *f_ptr && *f_ptr <= '9') {
                    state = 2;
                }
                else if (*f_ptr == '0') {
                    state = 1;
                }
                else if (*f_ptr == '-') {
                    forward(); //保证f_ptr多走一步
                    tkn = new Token(SELF_MINUS);
                    quit_loop = true;
                    break;
                }
                else if (*f_ptr == '=') {
                    forward(); //保证f_ptr多走一步
                    OpAssign_Token* tmp_tkn = new OpAssign_Token(OP_ASSIGN);
                    get_token_str(tmp_tkn->attr, 4);
                    tkn = (Token*)tmp_tkn;
                    quit_loop = true;
                    break;
                }
                else if (*f_ptr == '>') {
                    forward(); //保证f_ptr多走一步
                    tkn = new Token(RIGHT_ARROW);
                    quit_loop = true;
                    break;
                }
                else {
                    tkn = new Token(MINUS);
                    quit_loop = true;
                    break;
                }
                break;
            case 10:
                if ('0' <= *f_ptr && *f_ptr <= '7') {
                    state = 10;
                }
                else {
                    //整常数（8进制）
                    get_token_str(m_token_str, MAX_LENGTH_STR);
                    int attr;
                    sscanf_s(m_token_str, "%o", &attr);
                    tkn = new Int_Token(CONST_INT, attr);
                    quit_loop = true;
                    break;
                }
                break;
            case 11:
                if ('0' <= *f_ptr && *f_ptr <= '9' || 'a' <= *f_ptr && *f_ptr <= 'f' || 'A' <= *f_ptr && *f_ptr <= 'F') {
                    state = 12;
                }
                else {
                    //error
                    printf("Error: line:%2d, hexadecimal incorrect.\n", m_cur_line);
                    quit_loop = true;
                    break;
                }
                break;
            case 12:
                if ('0' <= *f_ptr && *f_ptr <= '9' || 'a' <= *f_ptr && *f_ptr <= 'f' || 'A' <= *f_ptr && *f_ptr <= 'F') {
                    state = 12;
                }
                else {
                    //整常数（16进制）
                    get_token_str(m_token_str, MAX_LENGTH_STR);
                    int attr;
                    sscanf_s(m_token_str, "%x", &attr);
                    tkn = new Int_Token(CONST_INT, attr);
                    quit_loop = true;
                    break;
                }
                break;
            case 13:
                //浮点数
                get_token_str(m_token_str, MAX_LENGTH_STR);
                float attr;
                sscanf_s(m_token_str, "%f", &attr);
                tkn = new Float_Token(CONST_FLOAT, attr);
                quit_loop = true;
                break;
            }
            if (quit_loop) {
                break;
            }
            forward();
        } while (1);
    }
    else if (*f_ptr == '\'') {
        int state = 3;
        int category = 0; //0: 'a'  1: '\n';     2: '\000    3: '\xff'
        do {
            forward();
            switch (state) {
            case 3:
                if (*f_ptr == '\\') {
                    state = 5;
                }
                else if(*f_ptr == '\''){
                    //error
                    printf("Error: line:%2d, empty char const\n", m_cur_line);
                    state = 8;
                }
                else{
                    category = 0;
                    state = 4;
                }
                break;
            case 1:
                if ('0' <= *f_ptr && *f_ptr <= '7') {
                    state = 2;
                }
                else {
                    //error
                    printf("Error: line:%2d, octal char format: \\xxx\n", m_cur_line);
                    state = 8;
                }
                break;
            case 2:
                if ('0' <= *f_ptr && *f_ptr <= '7') {
                    category = 2;
                    state = 4;
                }
                else {
                    //error
                    printf("Error: line:%2d, octal char format: \\xxx\n", m_cur_line);
                    state = 8;
                }
                break;
            case 4:
                if (*f_ptr == '\'') {
                    //CHAR
                    forward();
                    char ch;
                    get_token_str(m_token_str, MAX_LENGTH_STR);
                    if (category == 0) {    //'a'
                        ch = m_token_str[1];
                    }
                    else if (category == 1) {   //'\n'
                        switch (m_token_str[2]) {
                        case '\'':
                            ch = '\'';
                            break;
                        case '\\':
                            ch = '\\';
                            break;
                        case 'a':
                            ch = '\a';
                            break;
                        case 'b':
                            ch = '\b';
                            break;
                        case 'f':
                            ch = '\f';
                            break;
                        case 'n':
                            ch = '\n';
                            break;
                        case 'r':
                            ch = '\r';
                            break;
                        case 't':
                            ch = '\t';
                            break;
                        case 'v':
                            ch = '\v';
                            break;
                        default:
                            ch = 0; //can't get there
                        }
                    }
                    else if (category == 2) {   //'\000'
                        uint tmp;
                        sscanf_s(m_token_str + 2, "%o", &tmp);
                        ch = tmp;
                    }
                    else{                       //'\xff'
                        uint tmp;
                        m_token_str[1] = '0'; //凑成0xff
                        sscanf_s(m_token_str + 1, "%x", &tmp);
                        ch = tmp;
                    }
                    tkn = new Char_Token(CONST_CHAR, ch);
                    state = 8;
                }
                else {
                    //error
                    printf("Error: line:%2d, char miss \'\n", m_cur_line);
                }
                break;
            case 5:
                if (*f_ptr == '\'' || *f_ptr == '\\' || *f_ptr == 'a' || *f_ptr == 'b' ||
                    *f_ptr == 'f' || *f_ptr == 'n' || *f_ptr == 'r' || *f_ptr == 't' || *f_ptr == 'v') {
                    category = 1;
                    state = 4;
                }
                else if ('0' <= *f_ptr && *f_ptr <= '7') {
                    state = 1;
                }
                else if (*f_ptr == 'x' || *f_ptr == 'X') {
                    state = 6;
                }
                else {
                    //error    
                    printf("Error: line:%2d, Invaild char\n", m_cur_line);
                    state = 8;
                }
                break;
            case 6:
                if ('0' <= *f_ptr && *f_ptr <= '9' || 'a' <= *f_ptr && *f_ptr <= 'f' ||
                    'A' <= *f_ptr && *f_ptr <= 'F') {
                    state = 7;
                }
                else{
                    //error    
                    printf("Error: line:%2d, Invaild char\n", m_cur_line);
                    state = 8;
                }
                break;
            case 7:
                if ('0' <= *f_ptr && *f_ptr <= '9' || 'a' <= *f_ptr && *f_ptr <= 'f' ||
                    'A' <= *f_ptr && *f_ptr <= 'F') {
                    category = 3;
                    state = 4;
                }
                else {
                    //error    
                    printf("Error: line:%2d, Invaild char\n", m_cur_line);
                    state = 8;
                }
                break;
            }
        } while (state != 8);
    }
    else if (*f_ptr == '\"') {
        int state = 1;
        int cnt = 0;
        char ch;
        char octal[4];  //存放字符串中的八进制数
        char hexa[5];
        do {
            forward();
            switch (state) {
            case 1:
                if (*f_ptr == '\"') {
                    //String
                    forward();

                    m_token_str[cnt] = '\0';
                    String_Token* tmp_tkn = new String_Token(CONST_STR);
                    strcpy_s(tmp_tkn->attr, MAX_LENGTH_STR, m_token_str);
                    tkn = (Token*)tmp_tkn;
                    state = 7;
                }
                else if (*f_ptr == '\\') {
                    state = 2;
                }
                else {
                    cnt++;
                    m_token_str[cnt - 1] = *f_ptr;
                    state = 1;
                }
                break;
            case 2:
                if (*f_ptr == '\'' || *f_ptr == '\"'  || *f_ptr == '\\' || *f_ptr == 'a' || *f_ptr == 'b' ||
                    *f_ptr == 'f' || *f_ptr == 'n' || *f_ptr == 'r' || *f_ptr == 't' || *f_ptr == 'v') {
                    switch (*f_ptr) {
                    case '\'':
                        ch = '\'';
                        break;
                    case '\\':
                        ch = '\\';
                        break;
                    case 'a':
                        ch = '\a';
                        break;
                    case 'b':
                        ch = '\b';
                        break;
                    case 'f':
                        ch = '\f';
                        break;
                    case 'n':
                        ch = '\n';
                        break;
                    case 'r':
                        ch = '\r';
                        break;
                    case 't':
                        ch = '\t';
                        break;
                    case 'v':
                        ch = '\v';
                        break;
                    default:
                        ch = 0; //can't get there
                    }
                    cnt++;
                    m_token_str[cnt - 1] = ch;
                    state = 1;
                }
                else if ('0' <= *f_ptr && *f_ptr <= '7') {
                    octal[0] = *f_ptr;
                    state = 3;
                }
                else if (*f_ptr == 'x' || *f_ptr == 'X') {
                    state = 5;
                }
                else {
                    printf("Error: line:%2d, invaild escape character.\n", m_cur_line);
                    state = 7;
                }
                break;
            case 3:
                if ('0' <= *f_ptr && *f_ptr <= '7') {
                    octal[1] = *f_ptr;
                    state = 4;
                }
                else {
                    printf("Error: line:%2d, invaild octal format.\n", m_cur_line);
                    state = 7;
                }
                break;
            case 4:
                if ('0' <= *f_ptr && *f_ptr <= '7') {
                    octal[2] = *f_ptr;
                    octal[3] = '\0';

                    cnt++;
                    uint tmp;
                    sscanf_s(octal, "%o", &tmp);
                    m_token_str[cnt - 1] = (char)tmp;
                    state = 1;
                }
                else {
                    printf("Error: line:%2d, invaild octal format.\n", m_cur_line);
                    state = 7;
                }
                break;
            case 5:
                if ('0' <= *f_ptr && *f_ptr <= '9' || 'a' <= *f_ptr && *f_ptr <= 'f' || 'A' <= *f_ptr && *f_ptr <= 'F') {
                    hexa[0] = '0';
                    hexa[1] = 'x';
                    hexa[2] = *f_ptr;

                    state = 6;
                }
                else {
                    printf("Error: line:%2d, invaild hexadecimal format.\n", m_cur_line);
                    state = 7;
                }
                break;
            case 6:
                if ('0' <= *f_ptr && *f_ptr <= '9' || 'a' <= *f_ptr && *f_ptr <= 'f' || 'A' <= *f_ptr && *f_ptr <= 'F') {
                    hexa[3] = *f_ptr;
                    hexa[4] = '\0';

                    cnt++;
                    uint tmp;
                    sscanf_s(hexa, "%x", &tmp);
                    m_token_str[cnt - 1] = (char)tmp;
                    state = 1;
                }
                else {
                    printf("Error: line:%2d, invaild hexadecimal format.\n", m_cur_line);
                    state = 7;
                }
                break;
            }
        } while (state != 7);
    }
    else if (*f_ptr == '*') {
        forward();
        if (*f_ptr == '=') {
            forward();
            OpAssign_Token* tmp_tkn = new OpAssign_Token(OP_ASSIGN);
            strcpy_s(tmp_tkn->attr, 4, "*=");
            tkn = (Token*)tmp_tkn;
        }
        else {
            tkn = new Token(MULT);
        }
    }
    else if (*f_ptr == '/') {
        forward();
        if (*f_ptr == '=') {
            forward();
            OpAssign_Token* tmp_tkn = new OpAssign_Token(OP_ASSIGN);
            strcpy_s(tmp_tkn->attr, 4, "/=");
            tkn = (Token*)tmp_tkn;
        }
        else if (*f_ptr == '/') {
            do {
                forward();
            } while (*f_ptr != '\n');
        }
        else if (*f_ptr=='*') {
            int state = 0;
            bool quit_loop = false;
            do {
                forward();
                if (*f_ptr == '\n') {
                    m_cur_line++;
                }
                switch (state) {
                case 0:
                    if (*f_ptr == '*') {
                        state = 1;
                    }
                    break;
                case 1:
                    if (*f_ptr == '/') {
                        forward();
                        quit_loop = true;
                    }
                    else if (*f_ptr != '*') {
                        state = 0;
                    }
                    break;
                }
            } while (!quit_loop);
            ;
        }
        else {
            tkn = new Token(DIV);
        }
    }
    else if (*f_ptr == '%') {
        forward();
        if (*f_ptr == '=') {
            forward();
            OpAssign_Token* tmp_tkn = new OpAssign_Token(OP_ASSIGN);
            strcpy_s(tmp_tkn->attr, 4, "%=");
            tkn = (Token*)tmp_tkn;
        }
        else {
            tkn = new Token(MOD);
        }
    }
    else if (*f_ptr == '!') {
        forward();
        if (*f_ptr == '=') {
            forward();
            RelOp_Token* tmp_tkn = new RelOp_Token(REL_OP);
            strcpy_s(tmp_tkn->attr, 4, "!=");
            tkn = (Token*)tmp_tkn;
        }
        else {
            tkn = new Token(NOT);
        }
    }
    else if (*f_ptr == '=') {
        forward();
        if (*f_ptr == '=') {
            forward();
            RelOp_Token * tmp_tkn = new RelOp_Token(REL_OP);
            strcpy_s(tmp_tkn->attr, 4, "==");
            tkn = (Token*)tmp_tkn;
        }
        else {
            tkn = new Token(ASSIGN);
        }
    }
    else if (*f_ptr == '<') {
        forward();
        if (*f_ptr == '=') {
            forward();
            RelOp_Token* tmp_tkn = new RelOp_Token(REL_OP);
            strcpy_s(tmp_tkn->attr, 4, "<=");
            tkn = (Token*)tmp_tkn;
        }
        else {
            RelOp_Token* tmp_tkn = new RelOp_Token(REL_OP);
            strcpy_s(tmp_tkn->attr, 4, "<");
            tkn = (Token*)tmp_tkn;
        }
    }
    else if (*f_ptr == '>') {
        forward();
        if (*f_ptr == '=') {
            forward();
            RelOp_Token* tmp_tkn = new RelOp_Token(REL_OP);
            strcpy_s(tmp_tkn->attr, 4, ">=");
            tkn = (Token*)tmp_tkn;
        }
        else {
            RelOp_Token* tmp_tkn = new RelOp_Token(REL_OP);
            strcpy_s(tmp_tkn->attr, 4, ">");
            tkn = (Token*)tmp_tkn;
        }
    }
    else if (*f_ptr == '&') {
        forward();
        if (*f_ptr == '&') {
            forward();
            tkn = new Token(AND);
        }
        else {
            tkn = new Token(BIT_AND);
        }
    }
    else if (*f_ptr == '|') {
        forward();
        if (*f_ptr == '|') {
            forward();
            tkn = new Token(OR);
        }
        else {
            tkn = new Token(BIT_OR);
        }
    }
    else if (*f_ptr == '^') {
        forward();
        tkn = new Token(BIT_XOR);
    }
    else if (*f_ptr == '?') {
        forward();
        tkn = new Token(QUESTION);
    }
    else if (*f_ptr == ':') {
        forward();
        tkn = new Token(COLON);
    }
    else if (*f_ptr == ',') {
        forward();
        tkn = new Token(COMMA);
    }/*delimiter*/
    else if (*f_ptr == ';') {
        forward();
        tkn = new Token(SEMICOLON);
    }
    else if (*f_ptr == '(') {
        forward();
        tkn = new Token(OPEN_PARENTHESES);
    }
    else if (*f_ptr == ')') {
        forward();
        tkn = new Token(CLOSE_PARENTHESES);
    }
    else if (*f_ptr == '{') {
        forward();
        tkn = new Token(OPEN_BRACE);
    }
    else if (*f_ptr == '}') {
        forward();
        tkn = new Token(CLOSE_BRACE);
    }
    else if (*f_ptr == '[') {
        forward();
        tkn = new Token(OPEN_BRACKET);
    }
    else if (*f_ptr == ']') {
        forward();
        tkn = new Token(CLOSE_BRACKET);
    }
    //EOF
    else if (*f_ptr == EOF) {
        tkn = new Token(END_OF_FILE);
    }
    else {
        printf("Error: invaild character in line:%2d\n", m_cur_line);
        forward();
    }
    s_ptr = f_ptr;

    //----------loop until get a token----------
    }

    return tkn;
}