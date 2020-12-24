#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#include "./lexer.h"
#include "./parser.h"
#include "./list.h"

typedef enum ast_state {
	ast_state_start,
	ast_state_statement,
	ast_state_function_definition
} AstState;

typedef struct context {
	uint64_t pos;
	AstState state;
	AstNode* current;
	List* node_list;
} Context;

static TokenType peek_type(Context* x, List* token_list, uint64_t distance) {
	return ((Token*) list_at(token_list, x->pos + distance))->type;
}

static void begin_node(Context* x, AstNodeType type) {
	assert(!x->current);
	AstNode node = {0};
	node.type = type;
	x->current = (AstNode*) list_add(x->node_list, &node);
}

static void end_node(Context* x) {
	assert(x->current);
	x->current = NULL;
}

static void declare_node(Context* x, AstNodeType type) {
	begin_node(x, type);
	end_node(x);
}

static void set_state(Context* x, AstState new_state) {
	x->state = new_state;
}

static void skip(Context* x, uint64_t amount) {
	x->pos += amount;
}

void print_node(AstNode* node) {
	switch (node->type) {
	case ast_node_type_identifier: printf("Identifier\n");
	case ast_node_type_infer: printf("Infer\n");
	case ast_node_type_number: printf("Number\n");
	}
}

void parse(List* token_list) {
	printf("Hello! I will now parse your token list. :)\n");
	Context x = {0};
	List l = {0};
	l.item_size = sizeof(AstNode);
	x.node_list = malloc(sizeof(List));
	memcpy(x.node_list, &l, sizeof(List));
	for (x.pos = 0; x.pos < token_list->length; ++x.pos) {
		TokenType t = ((Token*) list_at(token_list, x.pos))->type;
		//printf("Type is %d\n", t);
		switch (t) {
		case token_type_symbol:
			switch (x.state) {
			case ast_state_start:
				if (peek_type(&x, token_list, 1) == token_type_colon) {
					declare_node(&x, ast_node_type_identifier);
					if (peek_type(&x, token_list, 2) == token_type_equals) {
						set_state(&x, ast_state_statement);
					} else if (peek_type(&x, token_list, 3) == token_type_lparen) {
						set_state(&x, ast_state_function_definition);
					}
				}
				break;
			default:
				break;
			}
		case token_type_colon: {
			if (peek_type(&x, token_list, 1) == token_type_equals) {
				declare_node(&x, ast_node_type_infer);
				skip(&x, 1);
			}
		} break;
		default:
			break;
		}
	}

	for (int i = 0; i < x.node_list->length; ++i) {
		AstNode* node = (AstNode*) list_at(x.node_list, i);
		print_node(node);
	}
}
