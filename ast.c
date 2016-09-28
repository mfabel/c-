#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "token.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

extern const char* token_name(int token_class);

void _ast_print(ast_t* node, int level, int sibling_num, int child_num);
void _ast_print_data(ast_t* node);

ast_t* ast_create_node() {
	int i;
	ast_t* node;

	node = (ast_t*) malloc(sizeof(ast_t));
	assert(node != NULL);

	node->lineno = 0;

	node->type = TYPE_NONE;
	node->data.name = NULL;
	node->data.token_class = -1;
	node->data.int_val = 0;
	node->data.char_val = '\0';
	node->data.str_val = NULL;

	node->num_children = 0;
	for (i = 0; i < AST_MAX_CHILDREN; i++) {
		node->child[i] = NULL;
	}
	node->sibling = NULL;

	return node;
}

ast_t* ast_from_token(token_t* tok) {
	ast_t* node;

	node = ast_create_node();
	node->lineno = tok->lineno;

	switch (tok->value_mode) {
		case MODE_CHAR:
			node->data.name = strdup(token_name(tok->type));
			node->type = TYPE_TOKEN_CHAR;
			node->data.char_val = tok->value.char_val;
			break;
		case MODE_INT:
			node->data.name = strdup(token_name(tok->type));
			node->type = TYPE_TOKEN_INT;
			node->data.int_val = tok->value.int_val;
			break;
		case MODE_STR:
			node->data.name = strdup(token_name(tok->type));
			node->type = TYPE_TOKEN_STR;
			node->data.str_val = strdup(tok->value.str_val);
			break;
		case MODE_NONE:
			node->data.name = strdup(token_name(tok->type));
			node->type = TYPE_TOKEN_NONE;
			break;
		default:
			fprintf(stderr, "ast_from_token: unknown token: %s\n", tok->input);
			exit(1);
	}

	node->data.token_class = tok->type;

	return node;
}

void ast_print(ast_t* tree) {
	_ast_print(tree, 0, -1, -1);

	return;
}

void ast_add_sibling(ast_t* root, ast_t* sibling) {
	while (root->sibling != NULL) {
		root = root->sibling;
	}

	root->sibling = sibling;
	sibling->sibling = NULL;

	return;
}

void ast_add_child(ast_t* root, int index, ast_t* child) {
	assert(index >= 0);
	assert(index < AST_MAX_CHILDREN);

	root->num_children = MAX(root->num_children, index + 1);
	root->child[index] = child;

	return;
}

void _ast_print(ast_t* node, int level, int sibling_num, int child_num) {
	int i;

	if (node == NULL) return;

	for (i = 0; i < level; i++) {
		fprintf(stdout, "!   ");
	}

	if (sibling_num > -1) {
		fprintf(stdout, "Sibling: %i ", sibling_num);
	}

	if (child_num > -1) {
		fprintf(stdout, "Child: %i ", child_num);
	}

	_ast_print_data(node);

	fprintf(stdout, " [line: %i]", node->lineno);
	fprintf(stdout, "\n");

	_ast_print(node->sibling, level, sibling_num + 1, -1);

	for (i = 0; i < node->num_children; i++) {
		_ast_print(node->child[i], level + 1, -1, i);
	}

	return;
}

void _ast_print_data(ast_t* node) {
	switch (node->type) {
		case TYPE_VAR_BOOL:
			fprintf(stdout, "Var %s of type bool", node->data.name);
			break;
		case TYPE_VAR_CHAR:
			fprintf(stdout, "Var %s of type char", node->data.name);
			break;
		case TYPE_VAR_INT:
			fprintf(stdout, "Var %s of type int", node->data.name);
			break;
		case TYPE_TOKEN_INT:
			fprintf(stdout, "Token %s of value %i",
				node->data.name, node->data.int_val);
			break;
		case TYPE_TOKEN_CHAR:
			fprintf(stdout, "Token %s of value %c",
				node->data.name, node->data.char_val);
			break;
		case TYPE_TOKEN_STR:
			fprintf(stdout, "Token %s of value %s",
				node->data.name, node->data.str_val);
			break;
		case TYPE_TOKEN_NONE:
			fprintf(stdout, "Token %s", node->data.name);
			break;
		case TYPE_NONE:
			break;
	}

	return;
}
