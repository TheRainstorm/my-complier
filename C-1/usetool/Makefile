
WORKDIR = .
RESDIR = $(WORKDIR)/resource

SrcFiles=$(wildcard $(WORKDIR)/src/*.c)
ObjFiles=$(patsubst %.c, %.o, $(SrcFiles))

CC = gcc
INCLUDES = -I./include
CFLAGS = $(INCLUDES)

my_gcc: $(WORKDIR)/src/parse.tab.o $(WORKDIR)/src/lex.yy.o $(ObjFiles)
	$(CC) $(CFLAGS) -o $@ $^

all:
	make bison
	make flex
	$(CC) $(CFLAGS) -o my_gcc $(SrcFiles)

$(WORKDIR)/src/parse.tab.o: $(RESDIR)/parse.y
	bison -dv $(RESDIR)/parse.y
	mv $(WORKDIR)/parse.tab.h $(WORKDIR)/include
	mv $(WORKDIR)/parse.tab.c $(WORKDIR)/src
	mv $(WORKDIR)/parse.output $(WORKDIR)/tmp
	
	$(CC) $(CFLAGS) -c -o $@ $(WORKDIR)/src/parse.tab.c

$(WORKDIR)/src/lex.yy.o: $(RESDIR)/lex.l $(WORKDIR)/include/parse.tab.h
	flex $(RESDIR)/lex.l
	mv $(WORKDIR)/lex.yy.c $(WORKDIR)/src
	$(CC) $(CFLAGS) -c -o $@ $(WORKDIR)/src/lex.yy.c

flex: $(RESDIR)/lex.l
	flex $(RESDIR)/lex.l
	mv $(WORKDIR)/lex.yy.c $(WORKDIR)/src

bison: $(RESDIR)/parse.y
	bison -dv $^
	mv $(WORKDIR)/parse.tab.h $(WORKDIR)/include
	mv $(WORKDIR)/parse.tab.c $(WORKDIR)/src
	mv $(WORKDIR)/parse.output $(WORKDIR)/tmp

lex_test:
	make flex
	$(CC) $(CFLAGS) -o $@ $(WORKDIR)/src/lex.yy.c $(WORKDIR)/src/ast_node.c $(WORKDIR)/tools/lex_test.c 

clean:
	rm *.exe
	rm $(ObjFiles)

move:
	cp -r /d/Projects/VS_projects/my_gcc/my_gcc/include .
	cp -r /d/Projects/VS_projects/my_gcc/my_gcc/src .
run:
	./my_gcc.exe sample4_fibonacci.c >./tmp/output