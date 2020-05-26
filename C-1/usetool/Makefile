#my_gcc.exe < sample.c >output 2>&1

my_gcc:
	bison -dv parse.y
	flex lex.l
	gcc -o $@ lex.yy.c parse.tab.c ast_node.c
lex_test:
	bison -dv parse.y
	flex lex.l
	gcc -o $@ lex.yy.c lex_test.c ast_node.c
# old:
# 	gcc lex.yy.c
# 	./a.exe ../test/1_ID.txt > 1_ID.txt.out
# 	./a.exe ../test/2_float.txt > 2_float.txt.out
# 	./a.exe ../test/3_operator.txt > 3_operator.txt.out
# 	./a.exe ../test/4_char.txt > 4_char.txt.out

clean:
	rm *.exe
	rm parse.output
	rm lex.yy.c parse.tab.c