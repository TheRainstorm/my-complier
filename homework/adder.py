import re
from collections import deque

class Token():
    def __init__(self, t='', v=''):
        self.type = t #'integer', 'operator', 'EOF'
        self.value = v # '[1-9]', '[+-*/()]', 'EOF'
    
    def __str__(self):
        return 'Token(\'{}\', \'{}\')'.format(
            self.type,
            self.value
        )

    def __repr__(self):
        return self.__str__()

class Lexer:
    def __init__(self, text):
        self.text = text
    
    def error(self):
        raise Exception('Lex error! Invaild character.')
    
    def lex(self):
        '''convert input to token_stream'''
        token_stream = []
        flg_new_tkn = True
        tkn = Token()
        cur_type = ''

        re_space = re.compile(r'\s')
        re_operator = re.compile(r'[-+*/()]')
        re_digit = re.compile(r'\d')
        for ch in self.text:
            if re_space.match(ch): # match
                continue # ignore the space character
            elif re_operator.match(ch) or re_digit.match(ch): # not space
                type = 'integer' if re_digit.match(ch) else 'operator'
                flg_new_tkn = not (cur_type=='integer' and type=='integer') # not a number
                if flg_new_tkn:
                    token_stream.append(tkn)
                    cur_type = type
                    tkn = Token(type, ch)
                else:
                    tkn.value += ch
            else:
                self.error()
        token_stream.append(tkn) #add the last tkn
        token_stream.append(Token('EOF','EOF'))
        return token_stream[1:] #the first token is empty

class Parser:
    def __init__(self, token_stream):
        self.token_stream = token_stream
        self.cur_pos = 0
        self.cur_token = token_stream[self.cur_pos]
        self.suffix_expr = deque()
    
    def error(self):
        raise Exception('Parse error! Invalid syntax.')
    
    def advance(self):
        self.cur_pos+=1
        self.cur_token = self.token_stream[self.cur_pos]
    
    def push(self, token):
        self.suffix_expr.append(token)
    
    def Expr(self):
        '''
        E -> T + T | T - T | T
        T -> F * F | F / F | F
        F -> (E) | i
        '''
        self.Term()
        while self.cur_token.value in ('+', '-'):
            token = self.cur_token
            self.advance()
            self.Term()
            self.push(token)
    
    def Term(self):
        self.Factor()
        while self.cur_token.value in ('*', '/'):
            token = self.cur_token
            self.advance()
            self.Factor()
            self.push(token)

    def Factor(self):
        if self.cur_token.type == 'integer':
            self.push(self.cur_token)
            self.advance()
        elif self.cur_token.value == '(':
            self.advance()
            self.Expr()
            if self.cur_token.value != ')':
                self.error()
            self.advance()
        else:
            self.error()

    def parse(self):
        self.Expr()
        return self.suffix_expr

class Interpreter:
    def __init__(self, text):
        # lexical analysis: get the token stream
        self.lexer = Lexer(text)
        
        self.parser = Parser(self.lexer.lex())
        self.suffix_deque = self.parser.parse()

    def execute(self, show_instr=True):
        Stack = deque() # stack computer use the stack, not registers
        while len(self.suffix_deque)!=0:
            tkn = self.suffix_deque.popleft()
            if tkn.type=='integer':
                if show_instr:
                    print('PUSH', int(tkn.value))
                Stack.append(int(tkn.value))
            else:
                b = Stack.pop()
                a = Stack.pop()
                op = tkn.value
                c = a + b if op=='+' else\
                    a - b if op=='-' else\
                    a * b if op=='*' else\
                    a // b
                if show_instr:
                    print(op, '->', c)
                Stack.append(c)
        return Stack.pop()

def welcome():
    welcome = r'''
     ____                 _     
    /\  _`\             /' \            C-1 (compiler-1) is a
    \ \ \/\_\          /\_, \           experimental compiler
     \ \ \/_/_  _______\/_/\ \          for class 《编译原理》  
      \ \ \L\ \/\______\  \ \ \                              
       \ \____/\/______/   \ \_\                             
        \/___/              \/_/                                
                                                            
    '''
    print(welcome)
    print("")
    print("It's simple now, it can:")
    print("1) caculate 10*(1+4)-(8/2+(3-2))*10")
    print("...\n")

def main():
    welcome()
    counter = 0
    while True:
        try:
            text = input("In[{}]: ".format(counter)).strip()
            interpreter = Interpreter(text)

            ans = interpreter.execute()
            print("Out[{}]: {}".format(counter, ans))
            counter += 1
        except EOFError:
            print('EOF, exit')
            break
        except Exception as e:
            print(e)
            continue
if __name__ == "__main__":
    main()