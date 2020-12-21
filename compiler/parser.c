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
	int is_lval;
	int is_rval;
	struct ast_node* parent;
	int child_count;
	struct ast_node* children;
	int arg_count;
	struct ast_node* args;
};

char AST_STATE_DEFINING_MAIN = 0;
char AST_STATE_DEFINING_MAIN_BODY = 0;
char AST_STATE_DEFINING_FUNCTION_CALL_ARGUMENTS = 0;
char AST_STATE_DEFINING_FUNCTION_BODY = 0;

char* buf;
size_t char_count;
size_t line_count = 1;

// @TODO: Support more than 500 tokens
size_t line_starts[500] = {0};
size_t line_lengths[500] = {0};

// State
size_t line = 0;
size_t oparen_count = 0;
size_t obrace_count = 0;

struct ast_node* tree_head;
struct ast_node* tree_curr;
struct ast_node* list_head;
struct ast_node* list_tail;

void set_line(int value) {
	//printf("\n -- line : %d -> %d -- \n", line, value);
	line = value;
}

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
	default: printf("%d", n->type); break;
	}
}

void split_line(char* linecol, char* type, char* value, size_t start) {
	const char s[2] = ",";
	char* tok;
	// @TODO malloc
	char copy[500];
	char* pos = &buf[start];
	char* cursor = &copy[0];
	while (*pos != '\n')
		*cursor++ = *pos++;

	*cursor = '\n';

	tok = strtok(copy, s);
	strcpy(linecol, tok);

	tok = strtok(NULL, s);
	strcpy(type, tok);

	tok = strtok(NULL, "\n");
	strcpy(value, tok);

	// printf("linecol: %s, type: %s, value: %s\n", linecol, type, value);
}

int next_type_matches(const char* pattern) {
	char* linecol = malloc(sizeof(char) * 100);
	char* type = malloc(sizeof(char) * 100);
	char* value = malloc(sizeof(char) * 100);

	split_line(linecol, type, value, line_starts[line+1]);

	return NULL != strstr(value, pattern);
}

enum ast_node_type get_type(const char* s, const char* v) {
	if (*v == '<') {
		return AST_OP_LT;
	} else if (*v == '>') {
		return AST_OP_GT;
	} else if (*v == ')') {
		// @TODO nested func calls
		--oparen_count;
		if (0 == oparen_count && AST_STATE_DEFINING_FUNCTION_CALL_ARGUMENTS) {
			AST_STATE_DEFINING_FUNCTION_CALL_ARGUMENTS = 0;
		}
		return AST_SKIP;
	} else if (0 == strcmp(s, "IDENT")) {;
		if (0 == strcmp(v, "main")) {
			AST_STATE_DEFINING_MAIN = 1;
			return AST_ENTRY;
		} else if (0 == strcmp(v, "if")) {
			return AST_IF;
		} else if (next_type_matches("(")) {
			AST_STATE_DEFINING_FUNCTION_CALL_ARGUMENTS = 1;
			++oparen_count;
			return AST_FUNCCALL;
		}
		return AST_VAR;
	} else if (0 == strcmp(s, "CONST")) {
		if (AST_STATE_DEFINING_MAIN) {
			AST_STATE_DEFINING_MAIN = 0;
			AST_STATE_DEFINING_FUNCTION_BODY = 1;
			AST_STATE_DEFINING_MAIN_BODY = 1;
			obrace_count++;
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
	set_line(line+1);
	char* linecol = malloc(sizeof(char) * 100);
	char* type = malloc(sizeof(char) * 100);
	char* value = malloc(sizeof(char) * 100);

	split_line(linecol, type, value, line_starts[line]);

	node->type = get_type(type, value);
	switch (node->type) {
	case AST_STRING: {
		node->value_type = AST_VALUE_STRING;
		node->sval = value;
		node->is_lval = 1;
	} break;
	case AST_NUMBER: {
		node->is_lval = 1;
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
		node->is_lval = 1;
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

struct ast_node* peek() {
	struct ast_node* node = next();
	//printf("|> %d\n", node->type);
	set_line(line-1);
	return node;
}

enum ast_node_type last_node_type = AST_INVALID;

char* lines;

struct ast_node* current_function_node = NULL;

void print_ast() {
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
	while (line < line_count - 1) {
		int was_defining_function_body = AST_STATE_DEFINING_FUNCTION_BODY;
		struct ast_node* node = next();

		if (!was_defining_function_body && AST_STATE_DEFINING_FUNCTION_BODY) {
			current_function_node = node;
		}

		if (node->type == AST_ENTRY) {
			tree_head = node;
			tree_curr = tree_head;
		} else {
			if (node->type == AST_INVALID || node->type == AST_SKIP) continue;


			struct ast_node* next_node = peek();
			// this is not lval or rval
			if (!next_node->is_lval && !next_node->is_rval) {
			}
			// if this node is a LVAL
			// and next node is an op
			// (and/and not) we are inside a function
			if (current_function_node) {
				//current_function_node->child_count
			}
		}
		print_ast_node(node);
		printf(" -> ");
	}

	printf("\n");

	return 0;
}
