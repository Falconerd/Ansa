#ifndef lexer_h_INCLUDED
#define lexer_h_INCLUDED
#include <stddef.h>
#include "./list.h"

typedef enum token_type {
	token_type_symbol,
	token_type_directive,
	token_type_lparen,
	token_type_rparen,
	token_type_lbrace,
	token_type_rbrace,
	token_type_lbracket,
	token_type_rbracket,
	token_type_lchevron,
	token_type_rchevron,
	token_type_string_literal,
	token_type_number_literal,

	token_type_comma,
	token_type_colon,
	token_type_semicolon,

	token_type_equals,
	token_type_plus,
	token_type_minus,
	token_type_asterisk,
	token_type_slash,
	token_type_bslash,
	token_type_period,
	token_type_apostrophe,
	token_type_at,
	/*
	token_type_ampersand,
	token_type_exclamation_mark,
	token_type_question_mark,
	token_type_pipe,
	token_type_circumflex,
	token_type_dollar_sign,
	token_type_percent_sign,
	token_type_tilde,
	token_type_grave,
	token_type_underscore,
	*/

	token_type_count
} TokenType;

typedef struct token {
	TokenType type;
	uint64_t start;
	uint64_t end;
	uint64_t line;
	uint64_t column;
	char* value;
} Token;

List* lex(const char* buf, size_t bufsize);
#endif

