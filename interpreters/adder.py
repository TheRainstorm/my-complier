import re
from collections import deque

class Token():
    def __init__(self, t='', v=''):
        self.typ = t #'integer', 'operator', 'EOF', 'float'
        self.val = v # '[1-9]', '[+-*/()]', 'EOF'
    
    def __str__(self):
        return 'Token(\'{}\', \'{}\')'.format(
            self.typ,
            self.val
        )

    def __repr__(self):
        return self.__str__()

def lex(text):
    '''convert input to token_stream'''
    token_stream = []
    flg_new_tkn = True
    tkn = Token()
    cur_typ = ''

    re_space = re.compile(r'\s')
    re_operator = re.compile(r'[-+*/()]')
    re_digit = re.compile(r'\d')
    for ch in text:
        if re_space.match(ch): # match
            continue # ignore the space character
        else: # not space
            typ = 'integer' if re_digit.match(ch) else 'operator'
            flg_new_tkn = not (cur_typ=='integer' and typ=='integer') # not a number
            if flg_new_tkn:
                token_stream.append(tkn)
                cur_typ = typ
                tkn = Token(typ, ch)
            else:
                tkn.val += ch
    token_stream.append(tkn) #add the last tkn
    token_stream.append(Token('EOF','EOF'))
    return token_stream[1:] #the first token is empty

def parse(token_stream):
    '''Convert infix to suffix (prefix, infix, suffix)
    return suffix deque
    '''
    priority = {'+':1, '-':1, '*': 2, '/':2, '(':0}
    S1 = deque()
    S2 = deque()
    for tkn in token_stream[:-1]: #exclude the EOP token
        if tkn.typ == 'integer':
            S1.append(tkn)
        else:
            if len(S2)==0 or tkn.val=='(':
                S2.append(tkn)
            elif tkn.val == ')':
                while S2[-1].val!='(':
                    S1.append(S2.pop())
                S2.pop()
            else:
                while len(S2)!=0 and (priority[tkn.val] < priority[S2[-1].val]):
                    S1.append(S2.pop())
                S2.append(tkn)
    while len(S2)!=0:
        S1.append(S2.pop())
    return S1

def execute(suffix_deque, show_instr=True):
    Stack = deque() # stack computer use the stack, not registers
    while len(suffix_deque)!=0:
        tkn = suffix_deque.popleft()
        if tkn.typ=='integer':
            if show_instr:
                print('PUSH', int(tkn.val))
            Stack.append(int(tkn.val))
        else:
            b = Stack.pop()
            a = Stack.pop()
            op = tkn.val
            c = a + b if op=='+' else\
                a - b if op=='-' else\
                a * b if op=='*' else\
                a / b
            if show_instr:
                print(op, '->', c)
            Stack.append(c)
    return Stack.pop()

def Interpre_top(text):
    # lexical analysis: get the token stream
    token_stream = lex(text)
    # print(token_stream)
    # parse
    suffix_deque = parse(token_stream)
    # print(suffix_deque)
    # semantic: a suffix expression
    # generate object code: use the suffix expression as obj-code
    return execute(suffix_deque, show_instr=True)

def welcome():
    welcm = r'''
     ____                 _     
    /\  _`\             /' \            C-1 (compiler-1) is a
    \ \ \/\_\          /\_, \           experimental compiler
     \ \ \/_/_  _______\/_/\ \          for class 《编译原理》  
      \ \ \L\ \/\______\  \ \ \                              
       \ \____/\/______/   \ \_\                             
        \/___/              \/_/                                
                                                            
    '''
    print(welcm)
    print("")
    print("It's simple now, it can:")
    print("1) caculate 10*(1+4)-(8/2+(3-2))*10")
    print("...\n")

def main():
    welcome()
    counter = 0
    while True:
        try:
            text = input("In[{}]: ".format(counter))
            if re.match(r'^[-+*/()\d\s]+$', text):
                ans = Interpre_top(text)
                print("Out[{}]: {}".format(counter, ans))
                counter += 1
            else:
                print('bad input')
                continue
        except EOFError:
            print('EOF, exit')
            break
if __name__ == "__main__":
    main()