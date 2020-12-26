ansa: src/main.c list.o stack.o lexer.o parser.o
	gcc -g3 $^ -o ansa

test: src/stack.test.c stack.o
	gcc -g3 $^ -o test

stack.o:
	gcc -g3 -c src/stack.c
	
list.o:
	gcc -g3 -c src/list.c

lexer.o: list.o
	gcc -g3 -c src/lexer.c
	
parser.o: lexer.o list.o
	gcc -g3 -c src/parser.c

clean:
	rm ./*.o
	rm ansa
