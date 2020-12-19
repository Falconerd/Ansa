#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

enum token {
	TOKEN_EOF,
	TOKEN_ILLEGAL,
	TOKEN_IDENT,
	TOKEN_INT,
	TOKEN_STRING,

	TOKEN_SEMI,     // ;

	TOKEN_ADD,      // +
	TOKEN_SUB,      // -
	TOKEN_MUL,      // *
	TOKEN_DIV,      // /

	TOKEN_CONST,    // ::
	TOKEN_ASSIGN,   // =
	TOKEN_INFER,    // :=

	TOKEN_COLON,    // :

	TOKEN_OBRACE,   // {
	TOKEN_CBRACE,   // }
	TOKEN_OBRACKET, // [
	TOKEN_CBRACKET, // ]
	TOKEN_OPAREN,   // (
	TOKEN_CPAREN,   // )

	TOKEN_COUNT
};

char* tokens[TOKEN_COUNT][20];

struct value {
	size_t line;
	size_t column;
	int token;
	char string[80];
};

char* buf;
size_t char_count;

// Lexer state
size_t line;
size_t column;
size_t abs_pos;

const char* token_name(enum token token) {
	switch (token) {
	case TOKEN_EOF: return "EOF";
	case TOKEN_ILLEGAL: return "ILLEGAL";
	case TOKEN_IDENT: return "IDENT";
	case TOKEN_INT: return "INT";
	case TOKEN_STRING: return "STRING";
	case TOKEN_SEMI: return "SEMI";
	case TOKEN_ADD: return "ADD";
	case TOKEN_SUB: return "SUB";
	case TOKEN_MUL: return "MUL";
	case TOKEN_DIV: return "DIV";
	case TOKEN_CONST: return "CONST";
	case TOKEN_ASSIGN: return "ASSIGN";
	case TOKEN_INFER: return "INFER";
	case TOKEN_COLON: return "COLON";
	case TOKEN_OBRACE: return "OBRACE";
	case TOKEN_CBRACE: return "CBRACE";
	case TOKEN_OBRACKET: return "OBRACKET";
	case TOKEN_CBRACKET: return "CBRACKET";
	case TOKEN_OPAREN: return "OPAREN";
	case TOKEN_CPAREN: return "CPAREN";
	default: return "-";
	}
}

int is_space(char c) {
	return (c == ' ' || c == '\t');
}

int is_digit(char c) {
	return (c >= '0' && c <= '9');
}

int is_alpha(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int is_alpha_(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

void reset_position() {
	++line;
	column = 0;
}

struct value* next() {
	struct value* value = malloc(sizeof(struct value));
	char c;
	for (int t = abs_pos; t < char_count; ++t) {
		c = buf[t];
		++column;
		++abs_pos;
		value->column = column;
		value->line = line;

		switch (c) {
		case '\n':
			value->token = TOKEN_SEMI;
			strcpy(value->string, ";");
			reset_position();
			return value;
		case ';':
			value->token = TOKEN_SEMI;
			strcpy(value->string, ";");
			return value;
		case ':': {
			if (buf[t+1] == ':') {
				value->token = TOKEN_CONST;
				strcpy(value->string, "::");
				++abs_pos;
				return value;
			} else if (buf[t+1] == '=') {
				value->token = TOKEN_INFER;
				strcpy(value->string, ":=");
				++abs_pos;
				return value;
			} else {
				value->token = TOKEN_COLON;
				strcpy(value->string, ":");
				return value;
			}
		}
		case '+':
			value->token = TOKEN_ADD;
			strcpy(value->string, "+");
			return value;
		case '-':
			value->token = TOKEN_SUB;
			strcpy(value->string, "-");
			return value;
		case '*':
			value->token = TOKEN_MUL;
			strcpy(value->string, "*");
			return value;
		case '/':
			value->token = TOKEN_DIV;
			strcpy(value->string, "/");
			return value;
		case '=':
			value->token = TOKEN_ASSIGN;
			strcpy(value->string, "=");
			return value;
		case '{':
			value->token = TOKEN_OBRACE;
			strcpy(value->string, "{");
			return value;
		case '}':
			value->token = TOKEN_CBRACE;
			strcpy(value->string, "}");
			return value;
		case '[':
			value->token = TOKEN_OBRACKET;
			strcpy(value->string, "[");
			return value;
		case ']':
			value->token = TOKEN_CBRACKET;
			strcpy(value->string, "]");
			return value;
		case '(':
			value->token = TOKEN_OPAREN;
			strcpy(value->string, "(");
			return value;
		case ')':
			value->token = TOKEN_CPAREN;
			strcpy(value->string, ")");
			return value;
		case '"': {
			char d[500] = {0};
			int i = 0;
			c = buf[++t];
			while (c != '"') {
				d[i++] = c;
				c = buf[++t];
			}
			value->token = TOKEN_STRING;
			strcpy(value->string, d);
			abs_pos += strlen(d) + 1;
			return value;
		}
		default:
			if (is_space(c)) {
				continue;
			} else if (is_digit(c)) {
				char d[80] = {0};
				int i = 0;
				while (is_digit(c)) {
					d[i++] = c;
					c = buf[++t];
				}
				value->token = TOKEN_INT;
				strcpy(value->string, d);
				if (strlen(d) > 1)
					abs_pos += strlen(d);
				return value;
			} else if (is_alpha(c)) {
				char d[80] = {0};
				int i = 0;
				while (is_alpha_(c)) {
					d[i++] = c;
					c = buf[++t];
				}
				int len = strlen(d);
				value->token = TOKEN_IDENT;
				strcpy(value->string, d);
				if (strlen(d) > 1)
					abs_pos += strlen(d)-1;
				return value;
			}
		}
	}
	return NULL;
}

int main() {
	FILE *f = fopen("test.ans", "rb");
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	buf = malloc(fsize + 1);
	fread(buf, 1, fsize, f);
	fclose(f);
	buf[fsize] = 0;
	char_count = fsize * sizeof(char);

	strcpy((char*)(&tokens[TOKEN_EOF]), "EOF");
	strcpy((char*)(&tokens[TOKEN_ILLEGAL]), "ILLEGAL");
	strcpy((char*)(&tokens[TOKEN_IDENT]), "IDENT");
	strcpy((char*)(&tokens[TOKEN_INT]), "INT");
	strcpy((char*)(&tokens[TOKEN_STRING]), "STRING");
	strcpy((char*)(&tokens[TOKEN_SEMI]), ";");
	strcpy((char*)(&tokens[TOKEN_ADD]), "+");
	strcpy((char*)(&tokens[TOKEN_SUB]), "-");
	strcpy((char*)(&tokens[TOKEN_MUL]), "*");
	strcpy((char*)(&tokens[TOKEN_DIV]), "/");
	strcpy((char*)(&tokens[TOKEN_CONST]), "::");
	strcpy((char*)(&tokens[TOKEN_ASSIGN]), "=");
	strcpy((char*)(&tokens[TOKEN_INFER]), ":=");
	strcpy((char*)(&tokens[TOKEN_COLON]), ":");
	strcpy((char*)(&tokens[TOKEN_OBRACE]), "{");
	strcpy((char*)(&tokens[TOKEN_CBRACE]), "}");
	strcpy((char*)(&tokens[TOKEN_OBRACKET]), "[");
	strcpy((char*)(&tokens[TOKEN_CBRACKET]), "]");
	strcpy((char*)(&tokens[TOKEN_OPAREN]), "(");
	strcpy((char*)(&tokens[TOKEN_CPAREN]), ")");

	for (;;) {
		struct value* v = next();
		if (NULL != v) {
			printf("%d:%d,%s,%s\n", v->line, v->column, token_name(v->token), v->string);
		}
		if (abs_pos == char_count)
			break;
	}

	return 0;
}

