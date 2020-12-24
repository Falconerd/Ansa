/* This is totally ripped off of Zig. I wrote a simple lexer which was alright,
 * but when I saw Zig's source code I knew I had to steal it.
 * 
 * It's actually from one of the earliest commits, back in 2015 (it's almost
 * 2021 now), and I'm not sure how much of this code is still around.
 * 
 * https://github.com/ziglang/zig
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>
#include "./list.h"

#include "./lexer.h"

#define WHITESPACE \
	' ': \
	case '\t': \
	case '\n': \
	case '\f': \
	case '\r': \
	case 0xb

#define DIGIT \
	'0': \
	case '1': \
	case '2': \
	case '3': \
	case '4': \
	case '5': \
	case '6': \
	case '7': \
	case '8': \
	case '9'

#define ALPHA \
	'a': \
	case 'b': \
	case 'c': \
	case 'd': \
	case 'e': \
	case 'f': \
	case 'g': \
	case 'h': \
	case 'i': \
	case 'j': \
	case 'k': \
	case 'l': \
	case 'm': \
	case 'n': \
	case 'o': \
	case 'p': \
	case 'q': \
	case 'r': \
	case 's': \
	case 't': \
	case 'u': \
	case 'v': \
	case 'w': \
	case 'x': \
	case 'y': \
	case 'z': \
	case 'A': \
	case 'B': \
	case 'C': \
	case 'D': \
	case 'E': \
	case 'F': \
	case 'G': \
	case 'H': \
	case 'I': \
	case 'J': \
	case 'K': \
	case 'L': \
	case 'M': \
	case 'N': \
	case 'O': \
	case 'P': \
	case 'Q': \
	case 'R': \
	case 'S': \
	case 'T': \
	case 'U': \
	case 'V': \
	case 'W': \
	case 'X': \
	case 'Y': \
	case 'Z'

#define SYMBOL_CHAR \
	ALPHA: \
	case DIGIT: \
	case '_'

typedef enum state {
	state_start,
	state_directive,
	state_symbol,
	state_string,
	state_number
} State;

typedef struct context {
	uint64_t pos;
	State state;
	uint64_t line;
	uint64_t column;
	List* token_list;
	Token* current;
} Context;

static void begin_token(Context* x, TokenType type) {
	assert(!x->current);
	Token token = {0};
	token.type = type;
	token.start = x->pos;
	token.line = x->line;
	token.column = x->column;
	x->current = (Token*)list_add(x->token_list, &token);
}

static void end_token(Context* x) {
	assert(x->current);
	x->current->end = x->pos + 1;
	x->current = NULL;
}

static void reverse(Context* x, int count) {
	x->pos -= count;
}

static void set_state(Context* x, State new_state) {
	// printf("Switching state from %d to %d\n", x->state, new_state);
	x->state = new_state;
}

static const char* token_name(Token* token) {
	switch (token->type) {
		case token_type_symbol: return "Symbol";
		case token_type_directive: return "Directive";
		case token_type_lparen: return "LParen";
		case token_type_rparen: return "RParen";
		case token_type_lbrace: return "LBrace";
		case token_type_rbrace: return "RBrace";
		case token_type_lchevron: return "LChevron";
		case token_type_rchevron: return "RChevron";
		case token_type_string_literal: return "String";
		case token_type_number_literal: return "Number";
		case token_type_comma: return "Comma";
		case token_type_colon: return "Colon";
		case token_type_semicolon: return "Semicolon";
		case token_type_equals: return "Equals";
		case token_type_plus: return "Plus";
		case token_type_asterisk: return "Asterisk";
	}
	return "Invalid";
}

static void print_tokens(const uint8_t* buf, List* token_list) {
	for (int i = 0; i < token_list->length; ++i) {
		Token* token = list_at(token_list, i);
		printf("%s ", token_name(token));
		fwrite(&buf[0]+token->start, 1, token->end - token->start, stdout);
		printf("\n");
	}
}

List* lex(const uint8_t* buf, size_t bufsize) {
	clock_t start_time, end_time;
	start_time = clock();
	Context x = {0};
	List l = {0};
	l.item_size = sizeof(Token);
	x.token_list = malloc(sizeof(List));
	memcpy(x.token_list, &l, sizeof(List));
	for (x.pos = 0; x.pos < bufsize; ++x.pos) {
		uint8_t c = buf[x.pos];
		switch (x.state) {
		case state_start:
			switch (c) {
			case WHITESPACE:
				break;
			case ALPHA:
				set_state(&x, state_symbol);
				begin_token(&x, token_type_symbol);
				break;
			case DIGIT:
				set_state(&x, state_number);
				begin_token(&x, token_type_number_literal);
				break;
			case '#':
				set_state(&x, state_directive);
				begin_token(&x, token_type_directive);
				break;
			case '(':
				begin_token(&x, token_type_lparen);
				end_token(&x);
				break;
			case ')':
				begin_token(&x, token_type_rparen);
				end_token(&x);
				break;
			case ',':
				begin_token(&x, token_type_comma);
				end_token(&x);
				break;
			case '*':
				begin_token(&x, token_type_asterisk);
				end_token(&x);
				break;
			case '{':
				begin_token(&x, token_type_lbrace);
				end_token(&x);
				break;
			case '}':
				begin_token(&x, token_type_rbrace);
				end_token(&x);
				break;
			case '[':
				begin_token(&x, token_type_lbracket);
				end_token(&x);
				break;
			case ']':
				begin_token(&x, token_type_rbracket);
				end_token(&x);
				break;
			case '<':
				begin_token(&x, token_type_lchevron);
				end_token(&x);
				break;
			case '>':
				begin_token(&x, token_type_rchevron);
				end_token(&x);
				break;
			case ';':
				begin_token(&x, token_type_semicolon);
				end_token(&x);
				break;
			case '+':
				begin_token(&x, token_type_plus);
				end_token(&x);
				break;
			case '-':
				begin_token(&x, token_type_minus);
				end_token(&x);
				break;
			case '/':
				begin_token(&x, token_type_slash);
				end_token(&x);
				break;
			case '\\':
				begin_token(&x, token_type_bslash);
				end_token(&x);
				break;
			case '.':
				begin_token(&x, token_type_period);
				end_token(&x);
				break;
			case '\'':
				begin_token(&x, token_type_apostrophe);
				end_token(&x);
				break;
			case '@':
				begin_token(&x, token_type_at);
				end_token(&x);
				break;
			case ':':
				begin_token(&x, token_type_colon);
				end_token(&x);
				break;
			case '=':
				begin_token(&x, token_type_equals);
				end_token(&x);
				break;
			case '"':
				begin_token(&x, token_type_string_literal);
				set_state(&x, state_string);
				break;
				/*
			case '&':
				begin_token(&x, token_type_ampersand);
				end_token(&x);
				break;
			case '!':
				begin_token(&x, token_type_exclamation_mark);
				end_token(&x);
				break;
			case '?':
				begin_token(&x, token_type_question_mark);
				end_token(&x);
				break;
			case '|':
				begin_token(&x, token_type_pipe);
				end_token(&x);
				break;
			case '^':
				begin_token(&x, token_type_circumflex);
				end_token(&x);
				break;
			case '$':
				begin_token(&x, token_type_dollar_sign);
				end_token(&x);
				break;
			case '%':
				begin_token(&x, token_type_percent_sign);
				end_token(&x);
				break;
			case '~':
				begin_token(&x, token_type_tilde);
				end_token(&x);
				break;
			case '`':
				begin_token(&x, token_type_grave);
				end_token(&x);
				break;
			case '_':
				begin_token(&x, token_type_underscore);
				end_token(&x);
				break;
				*/
			default:
				printf("invalid character: '%c'\n", c);
				break;
			}
			break;
		case state_directive:
			if (c == '\n') {
				assert(x.current);
				x.current->end = x.pos;
				x.current = NULL;
				set_state(&x, state_start);
			}
			break;
		case state_symbol:
			switch (c) {
			case SYMBOL_CHAR:
				break;
			default:
				reverse(&x, 1);
				end_token(&x);
				set_state(&x, state_start);
				break;
			}
			break;
		case state_string:
			switch (c) {
			case '"':
				end_token(&x);
				set_state(&x, state_start);
				break;
			}
			break;
		case state_number:
			switch (c) {
			case DIGIT:
				break;
			default:
				reverse(&x, 1);
				end_token(&x);
				set_state(&x, state_start);
				break;
			}
			break;
		}
		if (c == '\n') {
			++x.line;
			x.column = 0;
		} else {
			++x.column;
		}
	}
	end_time = clock();

	puts("Done lexing, here is what we got:");
	printf("Lines: %lu, Tokens: %lu, Time taken: %f\n", x.line, x.token_list->length, ((double) (end_time - start_time)) / CLOCKS_PER_SEC);
	print_tokens(buf, x.token_list);

	return x.token_list;
}
