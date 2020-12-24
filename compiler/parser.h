#ifndef parser_h_INCLUDED
#define parser_h_INCLUDED
#include "./list.h"
#include "./lexer.h"
#include "./buffer.h"

typedef struct ast_node AstNode;

typedef enum ast_node_type {
	ast_node_type_identifier,
	ast_node_type_infer,
	ast_node_type_number,

	ast_node_type_count
} AstNodeType;

typedef enum ast_node_expression_type {
	ast_node_expression_type_number,
	ast_node_expression_type_string
} AstNodeExpressionType;

typedef struct ast_node_expression {
	AstNodeExpressionType type;
	union {
		Buffer number;
		Buffer string;
	} data;
} AstNodeExpression;

struct ast_node {
	AstNode* parent;
	AstNodeType type;
	AstTokenType data;
};

void parse(List* token_list);
#endif

