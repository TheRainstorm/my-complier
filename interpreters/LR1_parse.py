class Symbol:
    '''
    符号：终结符或非终结符
    '''
    def __init__(self, is_Vn, value):
        self.is_Vn = is_Vn  #true or false
        self.value = value #string
    def __repr__(self):
        return self.value
    def __str__(self):
        return self.value
    
    def __eq__(self, value):
        return self.value == value.value
    
class Production:
    '''
    产生式，左部为一个Symbol对象，右部为Symbol列表
    '''
    def __init__(self, s):
        self.left = Symbol(True, s.split('->')[0].strip())
        s_right = s.split('->')[1].strip()
        self.right = []
        for x in s_right:
            if x==' ':
                continue
            elif ord(x)<=ord('Z') and ord(x) >= ord('A'):
                self.right.append(Symbol(True, x))
            else:
                self.right.append(Symbol(False, x))

    def __repr__(self):
        s = self.left.value
        s += ' -> '
        for x in self.right:
            s += x.value
        
        return s
    
    def __eq__(self, value):
        return self.left==value.left and self.right==value.right
    
    def __str__(self):
        return self.__repr__()
    
class LR1_Item:
    '''
    LR1项目：产生式、点所处位置、展望信息
    '''
    def __init__(self, prod, index, symbol):
        self.prod = prod
        self.index = index
        self.symbol = symbol
    
    def __repr__(self):
        s = str(self.prod)
        s = '['+ s[:self.index+5]+'|'+s[self.index+5:]+', '+self.symbol+']'
        return s
    
    def __str__(self):
        return self.__repr__
    
    def __eq__(self, value):
        return self.prod==value.prod\
                and self.index==value.index\
                and self.symbol == value.symbol
    
def read_production(file_path):
    '''从文件中读取产生式，返回一个产生式列表
    '''
    with open(file_path) as fp:
        s_production_list = fp.readlines()
    production_list = []

    #拓广文法
    start_symbol = s_production_list[0].split('->')[0].strip()
    s_production_list.insert(0, '#->'+start_symbol)
    for s_prod in s_production_list:
        production_list.append(Production(s_prod))
    
    return production_list

def get_all_symbol():
    '''从所有产生式中提取终结符和非终结符
    '''
    vt_list = []
    vn_list = []
    for prod in g_production_list:
        if prod.left not in vn_list:
            vn_list.append(prod.left)
        for X in prod.right:
            if not X.is_Vn and X not in vt_list:
                vt_list.append(X)
    
    return (vt_list, vn_list)

def get_first_dic():
    '''计算一个非终结符的First终结符。
    '''
    first_dic = {}
    def recur_first(vn):
        if len(first_dic[vn.value])!=0: #已经生成过该非终结符的first符号列表
            return first_dic[vn.value]
        for prod in g_production_list:
            if prod.left==vn:
                if not prod.right[0].is_Vn:
                    if prod.right[0].value not in first_dic[vn.value]:
                        first_dic[vn.value].append(prod.right[0].value)
                else:
                    if prod.right[0]!=vn:
                        symbol_list = recur_first(prod.right[0])
                        for symbol in symbol_list:
                            if symbol not in first_dic[vn.value]:
                                first_dic[vn.value].append(symbol)
        return first_dic[vn.value]
    for vn in vn_list:
        first_dic[vn.value]=[]
    
    for vn in vn_list:
        recur_first(vn)
    
    return first_dic

def get_item_set_family():
    '''
    生成文法的项目集规范族
    '''
    global vt_list, vn_list
    def closure(item_set):
        # 存放项目集的闭包（item的list）
        result = []
        # 原有的项目仍处于闭包中
        for item in item_set:
            result.append(item)
        # 标记已经处理过的非终结符
        labeled = []
        # 复制一份，用于之后对比，若对比相同则退出循环
        result_cpy = result[:]
        while True:
            for item in result:
                if item.index==len(item.prod.right):
                    continue
                # 点后的第一个符号
                X = item.prod.right[item.index]
                if X.is_Vn and X not in labeled: #是非终结符，尝试添加对应项
                    labeled.append(X)
                    for prod in g_production_list: #遍历所有产生式，找到对应产生式
                        if prod.left.value == X.value:
                            if item.index==len(item.prod.right)-1:
                                result_cpy.append(LR1_Item(prod, 0, item.symbol))
                            else:
                                Y = item.prod.right[item.index+1] # 点后的第二个符号
                                if Y.is_Vn:
                                    result_cpy.append(LR1_Item(prod, 0, g_first_dic[Y.value]))
                                else:
                                    result_cpy.append(LR1_Item(prod, 0, Y.value))

            if result_cpy==result:
                break
            else:
                result = result_cpy[:]
        return result

    item_set_family = []
    i0 = closure([LR1_Item(g_production_list[0], 0, '$')])
    item_set_family.append(i0)

    def go(item_set, X):
        n_item_set = []
        for item in item_set:
            if item.index==len(item.prod.right): 
                continue
            if item.prod.right[item.index].value==X.value:
                n_item_set.append(LR1_Item(item.prod, item.index+1, item.symbol))
        return closure(n_item_set)
    
    go_vt_dic = {}
    go_vn_dic = {}
    for item_set in item_set_family: #item_set_family will change when append
        for vn in vn_list:
            n_item_set = go(item_set, vn)
            if len(n_item_set)!=0:
                if n_item_set not in item_set_family:
                    item_set_family.append(n_item_set)
                    go_vn_dic[(item_set_family.index(item_set), vn.value)] = len(item_set_family)-1
                else:
                    go_vn_dic[(item_set_family.index(item_set), vn.value)] = item_set_family.index(n_item_set)
        for vt in vt_list:
            n_item_set = go(item_set, vt)
            if len(n_item_set)!=0:
                if n_item_set not in item_set_family:
                    item_set_family.append(n_item_set)
                    go_vt_dic[(item_set_family.index(item_set), vt.value)] = len(item_set_family)-1
                else:
                    go_vt_dic[(item_set_family.index(item_set), vt.value)] = item_set_family.index(n_item_set)

    return item_set_family, go_vt_dic, go_vn_dic

def get_LR1_table():
    pass

if __name__ == "__main__":
    g_production_list = read_production('productions.txt')
    print('production list')
    print(g_production_list)

    print('all symbol')
    vt_list, vn_list = get_all_symbol()
    print(vt_list, vn_list)

    g_first_dic = get_first_dic()
    print('First table(Vn)')
    print(g_first_dic)

    print('Item set family')
    item_set_family, go_vt_dic, go_vn_dic = get_item_set_family()
    for i, item_set in enumerate(item_set_family):
        print(i, item_set)
    print(go_vn_dic)
    print(go_vt_dic)