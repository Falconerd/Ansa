ansa: compiler/main.c list.o lexer.o parser.o
	gcc -g3 $^ -o ansa

list.o:
	gcc -g3 -c compiler/list.c

lexer.o: list.o
	gcc -g3 -c compiler/lexer.c
	
parser.o: lexer.o list.o
	gcc -g3 -c compiler/parser.c

clean:
	rm ./*.o
	rm ansa
