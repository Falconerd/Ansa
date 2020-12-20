#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <inttypes.h>

enum ast_node_type {
	AST_ENTRY,
	AST_FUNCDEF,
	AST_FUNCCALL,
	AST_INFER,
	AST_DECLARE,
	AST_ASSIGN,
	AST_NUMBER,
	AST_STRING,
	AST_VAR,
	AST_CONST,
	AST_IF,
	AST_THEN,
	AST_ELSE,

	AST_OP_ADD,
	AST_OP_LT,
	AST_OP_GT,

	AST_NODE_TYPE_COUNT,
	AST_INVALID = 500,
	AST_SKIP = 600
};

enum ast_value_type {
	AST_VALUE_DOUBLE,
	AST_VALUE_FLOAT,
	AST_VALUE_INT,
	AST_VALUE_UINT,
	AST_VALUE_STRING
};

struct ast_node {
	enum ast_node_type type;
	enum ast_value_type value_type;
	union {
		double dval;
		float fval;
		int64_t ival;
		uint64_t uval;
		char* sval;
	};
	// if part of the same subtree, has direct access to neighbour
	//struct ast_node* neighbour;
	int child_count;
	struct ast_node* children;
	int arg_count;
	struct ast_node* args;
};

enum ast_state {
	AST_STATE_IDLE,
	AST_STATE_DEFINING_MAIN,
	AST_STATE_DEFINING_FUNCTION_CALL_ARGUMENTS
};

char* buf;
size_t char_count;
size_t line_count;

// @TODO: Support more than 500 tokens
size_t line_starts[500] = {0};
size_t line_lengths[500] = {0};

// State
char* c;
size_t t = 0;
size_t line = 0;
size_t oparen_count = 0;

enum ast_state state;

struct ast_node* tree_head;
struct ast_node* tree_curr;
struct ast_node* list_head;
struct ast_node* list_tail;

void print_ast_node(struct ast_node* n) {
	switch (n->type) {
	case AST_INFER: printf("INFER"); break;
	case AST_ASSIGN: printf("ASSIGN"); break;
	case AST_DECLARE: printf("DECLARE"); break;
	case AST_VAR: printf("VAR (%s)", n->sval); break;
	case AST_NUMBER: printf("NUMBER (%u)", n->uval); break;
	case AST_ENTRY: printf("ENTRY"); break;
	case AST_CONST: printf("CONST"); break;
	case AST_IF: printf("IF"); break;
	case AST_THEN: printf("THEN"); break;
	case AST_ELSE: printf("ELSE"); break;
	case AST_OP_ADD: printf("+"); break;
	case AST_OP_LT: printf("<"); break;
	case AST_OP_GT: printf(">"); break;
	case AST_INVALID: printf("INVALID"); break;
	case AST_STRING: printf("STRING (%s)", n->sval); break;
	case AST_FUNCCALL: {
		printf("FUNCCALL (%s) (", n->sval);
		for (int i = 0; i < n->arg_count; ++i) {
			switch (n->args->value_type) {
			case AST_VALUE_STRING:
				printf("%s", n->args->sval);
				break;
			}
			if (i + 1 != n->arg_count) printf(", ");
		}
		printf(")");
	} break;
	case AST_SKIP: printf("SKIP"); break;
	}
}

void split_line(char* linecol, char* type, char* value, char* str) {
	const char s[2] = ",";
	char* tok;

	tok = strtok(c, s);
	strcpy(linecol, tok);

	tok = strtok(NULL, s);
	strcpy(type, tok);

	tok = strtok(NULL, "\n");
	strcpy(value, tok);
}

int next_type_matches(const char* pattern) {
	int line_start = line_starts[line];
	char* start = &buf[line_start];
	char l[200] = {0};
	int i = 0;
	while ('\n' != *start) {
		l[i] = *start++;
	}

	return NULL != strstr(l, pattern);
}

enum ast_node_type get_type(const char* s, const char* v) {
	if (*v == '<') {
		return AST_OP_LT;
	} else if (*v == '>') {
		return AST_OP_GT;
	} else if (*v == ')') {
		--oparen_count;
		if (0 == oparen_count && state == AST_STATE_DEFINING_FUNCTION_CALL_ARGUMENTS) {
			state == AST_STATE_IDLE;
		}
	} else if (0 == strcmp(s, "IDENT")) {;
		if (0 == strcmp(v, "main")) {
			state = AST_STATE_DEFINING_MAIN;
			return AST_ENTRY;
		} else if (0 == strcmp(v, "if")) {
			return AST_IF;
		} else if (next_type_matches("(")) {
			state =  AST_STATE_DEFINING_FUNCTION_CALL_ARGUMENTS;
			++oparen_count;
			return AST_FUNCCALL;
		}
		return AST_VAR;
	} else if (0 == strcmp(s, "CONST")) {
		if (state == AST_STATE_DEFINING_MAIN) {
			state = AST_STATE_IDLE;
			return AST_SKIP;
		} else {
			return AST_CONST;
		}
	} else if (0 == strcmp(s, "COLON")) {
		return AST_DECLARE;
	} else if (0 == strcmp(s, "ASSIGN")) {
		return AST_ASSIGN;
	} else if (0 == strcmp(s, "INFER")) {
		return AST_INFER;
	} else if (0 == strcmp(s, "INT")) {
		return AST_NUMBER;
	} else if (0 == strcmp(s, "STRING")) {
		return AST_STRING;
	} else {
		return AST_INVALID;
	}
}

struct ast_node* next() {
	struct ast_node* node = calloc(1, sizeof(struct ast_node));
	t = line_starts[line++];
	c = &buf[t];
	char* linecol = malloc(sizeof(char) * 100);
	char* type = malloc(sizeof(char) * 100);
	char* value = malloc(sizeof(char) * 100);

	split_line(linecol, type, value, c);

	node->type = get_type(type, value);
	switch (node->type) {
	case AST_STRING: {
		node->value_type = AST_VALUE_STRING;
		node->sval = value;
	} break;
	case AST_NUMBER: {
		if (strchr(value, '.')) {
			// float of some kind?
			// @TODO Get locale and reset after
			// @TODO Double finding
			float num = atof(value);
			node->value_type = AST_VALUE_FLOAT;
			node->fval = num;
		} else if (value[0] == '-') {
			int64_t num = strtoimax(value, NULL, 10);
			node->value_type = AST_VALUE_INT;
			node->ival = num;
		} else {
			uint64_t num = strtoumax(value, NULL, 10);
			node->value_type = AST_VALUE_UINT;
			node->uval = num;
		}
	} break;
	case AST_VAR: {
		node->value_type = AST_VALUE_STRING;
		node->sval = value;
	} break;
	case AST_FUNCCALL: {
		node->value_type = AST_FUNCCALL;
		node->sval = value;
		// get args
		// that means they are what is inside the parens...
		//printf("%s %s %s\n", linecol, type, value);
		struct ast_node* next_node = next(); // Always "("
		next_node = next(); // First argument or INVALID
		if (next_node->type == AST_INVALID) break;

		// @TODO loop over args

		switch (next_node->type) {
		case AST_STRING: {
			node->arg_count++;
			node->args = realloc(node->args, sizeof(struct ast_node) * node->arg_count); 
			if (!node->args) {
				printf("Failed to realloc for node args");
				exit(EXIT_FAILURE);
			} else {
				struct ast_node* new_node = &node->args[node->arg_count-1];
				new_node->value_type = AST_VALUE_STRING;
				new_node->sval = malloc(strlen(next_node->sval));
				strcpy(new_node->sval, next_node->sval);
			}
		} break;
		}
	} break;
	}

	return node;
}

int main(int argc, char* argv[]) {
	// FILE *f = fopen(argv[1], "rb");
	FILE *f = fopen("./out.l", "rb");
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	buf = malloc(fsize + 1);
	fread(buf, 1, fsize, f);
	fclose(f);
	buf[fsize] = 0;
	char_count = fsize * sizeof(char);
	int lc = 0;

	for (int i = 0; i < char_count; ++i) {
		lc++;
		if (buf[i] == '\n') {
			line_lengths[line_count] = lc;
			line_starts[++line_count] = i+1;
			lc = 0;
		}
		printf("%c", buf[i]);
	}

	// build tree
	//for (int i = 0; i < line_count; ++i) {
	while (line < line_count) {
		struct ast_node* node = next();
		if (node->type == AST_ENTRY) {
			tree_head = node;
			tree_curr = tree_head;
		} else {
			if (node->type == AST_INVALID || node->type == AST_SKIP) continue;
			print_ast_node(node);
			printf(" -> ");
		}
	}

	printf("\n");

	return 0;
}
