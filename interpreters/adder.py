import re

class Token():
    def __init__(self, t='', v=''):
        self.typ = t #'integer', 'operator', 'EOF'
        self.val = v # '1', '2' ... '9', '+', 'EOF'
    
    def __str__(self):
        return 'Token({}, {})'.format(
            self.typ,
            self.val
        )

    def __repr__(self):
        return self.__str__()

def lex(text):
    token_stream = []
    flg_new_tkn = True
    tkn = Token()
    cur_typ = ''

    re_space = re.compile(r'\s')
    re_operator = re.compile(r'[-+*/]')
    re_digit = re.compile(r'\d')
    for ch in text:
        if re_space.match(ch): # match
            continue # ignore the space character
        else: # not space
            typ = 'integer' if re_digit.match(ch) else 'operator'
            flg_new_tkn = cur_typ != typ
            if flg_new_tkn:
                token_stream.append(tkn)
                cur_typ = typ
                tkn = Token(typ, ch)
            else:
                tkn.val += ch
    token_stream.append(tkn) #add the last tkn
    token_stream.append(Token('EOF','EOF'))
    return token_stream[1:] #the first token is empty

def Interpre_top(text):
    # lexical analysis: get the token stream
    token_stream = lex(text)
    # parse
    # semantic: a <op> b
    a, b = int(token_stream[0].val), int(token_stream[2].val)
    op = token_stream[1].val
    return  a + b if op=='+' else\
            a - b if op=='-' else\
            a * b if op=='*' else\
            a / b

def main():
    counter = 0
    while True:
        try:
            text = input("In[{}]: ".format(counter))
            if text:
                ans = Interpre_top(text)
                print("Out[{}]: {}".format(counter, ans))
                counter += 1
            else:
                continue
        except EOFError:
            print('EOF, exit')
            break
if __name__ == "__main__":
    main()