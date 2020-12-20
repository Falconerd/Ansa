#!/bin/sh

gcc -g3 compiler/lexer.c -o lex
gcc -g3 compiler/parser.c -o parse
#./lex | ./parse
./lex > out.l
./parse ./out.l
