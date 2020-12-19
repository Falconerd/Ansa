#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <inttypes.h>

enum ast_node_type {
	AST_ENTRY,
	AST_INFER,
	AST_OP_ADD,
	AST_INT,
	AST_VAR,
	AST_CONST,

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
	struct ast_node* left;
	struct ast_node* right;
};

enum ast_state {
	AST_STATE_DEFINING_MAIN,
	AST_STATE_IDLE
};

char* buf;
size_t char_count;
size_t line_count;

// @TODO: Support more than 500 tokens
size_t line_starts[500] = {0};
size_t line_lengths[500] = {0};

// State
char *c;
size_t t = 0;
size_t line = 0;

enum ast_state state;

struct ast_node* tree_head;
struct ast_node* tree_curr;
struct ast_node* list_head;
struct ast_node* list_tail;

void print_ast_node(struct ast_node* n) {
	switch (n->type) {
	case AST_INFER: printf("INFER"); break;
	case AST_VAR: printf("VAR (%s)", n->sval); break;
	case AST_INT: printf("CONSTANT (%u)", n->uval); break;
	case AST_ENTRY: printf("ENTRY"); break;
	case AST_CONST: printf("CONST"); break;
	case AST_INVALID: printf("INVALID"); break;
	case AST_SKIP: printf("SKIP"); break;
	}
}

enum ast_node_type get_type(const char* s, const char* v) {
	if (0 == strcmp(s, "IDENT")) {
		if (0 == strcmp(v, "main")) {
			state = AST_STATE_DEFINING_MAIN;
			return AST_ENTRY;
		}
		return AST_VAR;
	} else if (0 == strcmp(s, "CONST")) {
		if (state == AST_STATE_DEFINING_MAIN) {
			state = AST_STATE_IDLE;
			return AST_SKIP;
		} else {
			return AST_CONST;
		}
	} else if (0 == strcmp(s, "INFER")) {
		return AST_INFER;
	} else if (0 == strcmp(s, "INT")) {
		return AST_INT;
	} else {
		return AST_INVALID;
	}
}

struct ast_node* next() {
	struct ast_node* n = calloc(1, sizeof(struct ast_node));
	t = line_starts[line++];
	c = &buf[t];
	char* linecol = malloc(sizeof(char) * 100);
	char* type = malloc(sizeof(char) * 100);
	char* value = malloc(sizeof(char) * 100);

	const char s[2] = ",";
	char* tok;

	tok = strtok(c, s);
	strcpy(linecol, tok);

	tok = strtok(NULL, s);
	strcpy(type, tok);

	tok = strtok(NULL, "\n");
	strcpy(value, tok);

	n->type = get_type(type, value);
	switch (n->type) {
	case AST_VAR: {
		n->value_type = AST_VALUE_STRING;
		n->sval = value;
	} break;
	case AST_INT: {
		uint64_t num = strtoumax(value, NULL, 10);
		n->uval = num;
	} break;
	}

	return n;
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

	tree_curr = tree_head;

	struct ast_node* L = NULL;
	struct ast_node* R = NULL;
	struct ast_node* C = NULL;
	int subtree_done = 0;

	for (int i = 0; i < line_count; ++i) {
		struct ast_node* node = next();

		if (NULL == list_head) {
			list_head = node;
			list_tail = node;
		} else {
			list_tail->right = node;
			list_tail = list_tail->right;
		}

		switch (node->type) {
		case AST_VAR:
			if (NULL == L) {
				L = node;
			}
		case AST_INFER:
			if (NULL == R) {
				C = node;
			}
		case AST_INT:
			if (NULL == C) {
				R = node;
				subtree_done = 1;
			}
		}

		if (subtree_done) {
			C->left = L;
			C->right = R;
			tree_curr = C;
			L = NULL;
			R = NULL;
			C = NULL;
		}
	}

	struct ast_node* N = tree_head;
	int i = 0;

	while (N != NULL) {
		print_ast_node(N);
		N = N->left;
	}

	N = list_head;

	while (N != NULL) {
		if (N->type != AST_SKIP) {
			print_ast_node(N);
			if (NULL != N->right)
				printf(" -> ");
		}
		N = N->right;
	}
	printf("\n");

	return 0;
}
