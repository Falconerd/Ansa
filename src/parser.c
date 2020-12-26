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
	ast_state_function_definition,
	ast_state_function_body
} AstState;

typedef struct context {
	uint64_t pos;
	AstState state;
	AstNode* current;
	List* node_list;
	Stack* scope_stack;
} Context;

static TokenType peek_type(Context* x, List* token_list, uint64_t offset) {
	return ((Token*) list_at(token_list, x->pos + offset))->type;
}

static Buffer* get_value(Context* x, uint64_t offset, List* token_list, const char* src) {
	Buffer* b = malloc(sizeof(*b));
	Token* token = ((Token*) list_at(token_list, x->pos));
	size_t size;
	switch (token->type) {
	case token_type_number_literal: {
		// Determine the type and size of number
		// @TODO: ^
		// For now just use u32
		size = sizeof(uint32_t);
		uint32_t number = strtoumax(token->value, NULL, 10);
		b->data = malloc(size);
		memcpy(b->data, &number, size);
	} break;
	case token_type_symbol: {
		uint64_t length = strlen(token->value);
		printf(">>>> |%s| %d\n", token->value, length);
		size = length * sizeof(char);
		b->data = malloc(size);
		memcpy(b->data, token->value, size);
	} break;
	}
	b->size = size;
	return b;
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

static void set_node_value(Context* x, Buffer* buffer) {
	// memcpy(x->current->value, buffer->data, buffer->size);
	x->current->value.size = buffer->size;
	x->current->value.data = buffer->data;
}

void print_node(AstNode* node) {
	switch (node->type) {
	case ast_node_type_identifier: printf("Identifier: %s\n", (char*)node->value.data); break;
	case ast_node_type_infer: printf("Infer\n"); break;
	case ast_node_type_number: printf("Number: %u\n", *(uint32_t*)node->value.data); break;
	}
}

void parse(List* token_list, const char* src) {
	printf("Hello! I will now parse your token list. :)\n");
	Stack s = {0};
	AstNode* root_node = malloc(sizeof(AstNode));
	Context x = {0};
	List l = {0};

	l.item_size = sizeof(AstNode);
	x.node_list = malloc(sizeof(List));
	memcpy(x.node_list, &l, sizeof(List));

	s.item_size = sizeof(AstNode);
	x.scope_stack = malloc(sizeof(Stack));
	memcpy(x.scope_stack, &s, sizeof(Stack));
	stack_push(x.scope_stack, root_node);

	// On the way down, we want to build up the scope stack and figure out ast types
	// On the way up, we want to evaluate the values into their final forms and compress the tree as much as possible
	for (x.pos = 0; x.pos < token_list->length; ++x.pos) {
		Token* curr = ((Token*) list_at(token_list, x.pos));
		switch (curr->type) {
		case token_type_symbol: {
			switch (x.state) {
			// We aren't doing anything atm
			case ast_state_start: {
				begin_node(&x, ast_node_type_identifier);
				set_node_value(&x, get_value(&x, x.pos, token_list, src));
				if (peek_type(&x, token_list, 1) == token_type_colon) {
					// const 
					if (peek_type(&x, token_list, 2) == token_type_colon) {
						// function decl
						if (peek_type(&x, token_list, 3) == token_type_lparen) {
							stack_push(x.scope_stack, &x.current);
							end_node(&x);
							x.pos += 3;
							continue;
						}
					} else
					// infer
					if (peek_type(&x, token_list, 2) == token_type_equals) {
					} // else 
					// @TODO: type decl?
				}
				// assignment
			} break;
			}
		} break;
		}
		/*
		TokenType t = ((Token*) list_at(token_list, x.pos))->type;
		switch (t) {
		case token_type_symbol:
			switch (x.state) {
			case ast_state_start:
				if (peek_type(&x, token_list, 1) == token_type_colon) {
					begin_node(&x, ast_node_type_identifier);
					set_node_value(&x, get_value(&x, x.pos, token_list, src));
					end_node(&x);
					if (peek_type(&x, token_list, 2) == token_type_equals) {
						set_state(&x, ast_state_statement);
					} else if (peek_type(&x, token_list, 3) == token_type_lparen) {
						set_state(&x, ast_state_function_definition);
					}
				}
				break;
			case ast_state_function_definition: {
				// @TODO: get fn params
			} break;
			default:
				break;
			}
		case token_type_lbrace: {
			switch (x.state) {
			case ast_state_function_definition: {
				set_state(&x, ast_state_start);
			} break;
			}
		} break;
		case token_type_colon: {
			if (peek_type(&x, token_list, 1) == token_type_equals) {
				declare_node(&x, ast_node_type_infer);
				//skip(&x, 1);
				//continue;
			}
		} break;
		case token_type_number_literal: {
			begin_node(&x, ast_node_type_number);
			set_node_value(&x, get_value(&x, x.pos, token_list, src));
			end_node(&x);
		} break;
		default:
			break;
		}
		*/
	}

	/*
	for (int i = x.node_list->length - 1; i >= 0; --i) {
		AstNode* node = (AstNode*) list_at(x.node_list, i);
		AstNode* prev = NULL;
		if (0 != i)
			prev = (AstNode*) list_at(x.node_list, i-1);
		// Evaluate expression
		// Set parent...
		switch (node->type) {
		case ast_node_type_infer: {
		} break;
		}
	}
	*/

	for (int i = 0; i < x.node_list->length; ++i) {
		AstNode* node = (AstNode*) list_at(x.node_list, i);
		print_node(node);
	}
}
