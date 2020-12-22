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
#include "./list.h"

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

typedef enum token_type {
	token_type_symbol,
	token_type_directive,
	token_type_lparen,
	token_type_rparen,
	token_type_lbrace,
	token_type_rbrace,
	token_type_lchevron,
	token_type_rchevron,
	token_type_string_literal,
	token_type_number_literal,

	token_type_comma,
	token_type_colon,
	token_type_semicolon,

	token_type_equals,
	token_type_plus,
	token_type_asterisk,

	token_type_count
} TokenType;

typedef struct token {
	TokenType type;
	int start;
	int end;
} Token;

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
	List token_list;
	Token *current;
} Context;

static void begin_token(Context* x, TokenType type) {
	assert(!x->current);
	Token token = {0};
	token.type = type;
	token.start = x->pos;
	x->current = (Token*)list_add(&x->token_list, &token);
}

static void end_token(Context* x) {
	assert(x->current);
	x->current->end = x->pos + 1;
	x->current = NULL;
}

static void put_back(Context* x, int count) {
	x->pos -= count;
}

static void set_state(Context* x, State new_state) {
	// printf("Switching state from %d to %d\n", x->state, new_state);
	x->state = new_state;
}

void lex(const uint8_t* buf, size_t bufsize) {
	Context x = {0};
	x.token_list.item_size = sizeof(Token);
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
			case ALPHA:
			case DIGIT:
			case '_':
				break;
			default:
				put_back(&x, 1);
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
				put_back(&x, 1);
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
}
