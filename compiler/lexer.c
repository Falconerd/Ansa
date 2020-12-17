#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

enum {
	TOKEN_IDENTIFIER = 256,
	TOKEN_NUMBER,
	TOKEN_ELSE,
	TOKEN_ENUM,
	TOKEN_IF,
	TOKEN_INTEGER,
	TOKEN_RETURN,
	TOKEN_SIZE,
	TOKEN_WHILE,
	TOKEN_ASSIGN,
	TOKEN_CONDITION,
	TOKEN_LOGICAL_OR,
	TOKEN_LOGICAL_AND,
	TOKEN_OR,
	TOKEN_XOR,
	TOKEN_AND,
	TOKEN_EQ,
	TOKEN_NE,
	TOKEN_TL,
	TOKEN_GT,
	TOKEN_LTE,
	TOKEN_GTE,
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_MOD,
	TOKEN_INC,
	TOKEN_DEC,
	TOKEN_BREAK,
	TOKEN_STRING,

	TOKEN_INFER = 300,
	TOKEN_FUNCTION_OR
};

enum {
	ERROR_UNSUPPORTED_CHARACTER_IN_IDENTIFIER,
};

struct tokdef {
	int token;
	char* name;
};

struct tokdef* tokens = NULL;
int token_count = 0;

int line = 1;
int col = -1;
int token = 0;
char* buf = NULL;
char* c;

char* get_line(int which) {
	char *c = &buf[0];
	int i = 0;
	while (i < line-1) {
		++c;
		if (*c == '\n')
			i++;
	}
	char* ln = malloc(5000 * sizeof(char));
	i = 0;
	if (*c == '\n')
		++c;
	while (*c != '\n') {
		ln[i] = *c;
		++c;
		++i;
	}
	ln[i+1] = '\0';

	return ln;
}

const char* ERROR_REPLACE_ME = "Error. Replace me with better error message.";
const char* ERROR_UNSUPPORTED_CHARACTER_IN_IDENTIFIER_MSG = "There seems to be an unsupported character (%c) in this idenifier. Identifiers can be any combination alphanumric character or '_'.";

const char* error_msg(int type) {
	switch (type) {
		case ERROR_UNSUPPORTED_CHARACTER_IN_IDENTIFIER:
			return ERROR_UNSUPPORTED_CHARACTER_IN_IDENTIFIER_MSG;
		default:
			return ERROR_REPLACE_ME;
	}
}

int line_number_length(int line) {
	if (line >= 0 && line <= 9)
		return 1;
	else if (line >= 10 && line <= 99)
		return 2;
	else if (line >= 100 && line <= 999)
		return 3;
	else if (line >= 1000 && line <= 9999)
		return 4;
}

void error(int error_type, ...) {

	const char *fmt = error_msg(error_type);
	char arrow[256];
	int len = col + line_number_length(line) + 1;
	memset(arrow, '-', len*sizeof(char));
	arrow[len] = '^';
	arrow[len+1] = '\0';

	va_list args;
	va_start(args, error_type);
	vfprintf(stdout, fmt, args);
	va_end(args);

	printf("\n");
	char* content = get_line(line);
	printf("%d| %s\n", line, content);

	printf("%s\n", arrow);

	printf("\n");
}

int is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

int is_number(char c) {
	return c >= '0' && c <= '9';
}

void next() {
	while (token = *c) {
		++c;
		++col;

		if (token == '\n') {
			++line;
			col = -1;
			while (token == ' ' || token == '\t') {
				col++;
				++c;
			}
		} else if (is_alpha(token)) {
			char name[256];
			int i = 0;
			if (is_alpha(token) || is_number(token)) {
				name[0] = token;
				i++;
			}
			while (is_alpha(*c) || is_number(token)) {
				name[i++] = *c;
				++c;
				++col;
			}
			if (*c != ':' && *c != '=' && *c != '(' && *c != ' ') {
				error(ERROR_UNSUPPORTED_CHARACTER_IN_IDENTIFIER, *c);
			}
			tokens[token_count].name = calloc(i, sizeof(char));
			strcpy(tokens[token_count].name, name);
			tokens[token_count].name[i] = '\0';
			tokens[token_count++].token = TOKEN_IDENTIFIER;
			return;
		} else if (is_number(token)) {
			char *num = calloc(100, sizeof(char));
			int i = 0;
			while (is_number(*c)) {
				num[i++] = *c;
				++c;
			}
			if (!i) {
				i = 1;
				num[0] = token;
			}

			tokens[token_count].name = calloc(i, sizeof(char));
			strcpy(tokens[token_count].name, num);
			tokens[token_count++].token = TOKEN_NUMBER;
			return;
		} else if (token == '"') {
			char* str = calloc(10000, sizeof(char));
			int i = 0;
			while (*c != 0 && *c != token) {
				if ('"' == *c)
					break;
				str[i++] = *c;
				++c;
			}
			str[i] = '\0';
			++c;
			tokens[token_count].name = calloc(i, sizeof(char));
			strcpy(tokens[token_count].name, str);
			tokens[token_count++].token = TOKEN_STRING;
			return;
		} else if (token == ':') {
			if (*c == '=') {
				tokens[token_count++].token = TOKEN_INFER;
				return;
			}
		} else if (token == '%') {
			tokens[token_count++].token = TOKEN_MOD;
			return;
		} else if (token == '(' || token == ')' || token == '{' || token == '}' || token == ';' || token == '~' || token == '[' || token == ']' || token == ',') {
			tokens[token_count++].token = token;
			return;
		}
	}
}

void process() {
	next();
}

void lexer_start(const char* filename) {
	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = malloc(fsize + 1);
	fread(buf, 1, fsize, f);
	fclose(f);

	buf[fsize] = 0;

	c = &buf[0];

	tokens = calloc(fsize, sizeof(struct tokdef));

	// printf("file contents(%d):\n%s\n", fsize, buf);

	token = 1;

	while (token > 0) {
		process();
	}

	for (int i = 0; i < token_count; ++i) {
		switch (tokens[i].token) {
			case TOKEN_IDENTIFIER:
				printf("[%s] ", tokens[i].name);
				break;
			case TOKEN_STRING:
				printf("\"%s\" ", tokens[i].name);
				break;
			case TOKEN_INFER:
				printf(":= ");
				break;
			case TOKEN_NUMBER:
				printf("%s ", tokens[i].name);
				break;
			default:
				printf("%c ", tokens[i].token);
				break;
		}
		/*
		if (tokens[i].token != TOKEN_STRING && tokens[i].token != TOKEN_IDENTIFIER)
			printf("'%c' ", tokens[i].token);
		if (tokens[i].name)
			printf("'%s' ", tokens[i].name);
		*/
		if (i + 1 != token_count)
			printf("-> ");
	}
	printf("\n");
}

int main(int argc, char* argv[]) {
	if (2 == argc) {
		lexer_start(argv[1]);
	} else {
		printf("usage: lexer filename\n");
	}
	return 0;
}
