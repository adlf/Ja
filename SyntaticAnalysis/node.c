#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

struct node *make_type_node(char *type)
{
	struct node *new_node = malloc(sizeof(struct node));
	new_node->c1 = NULL;
	new_node->c2 = NULL;
	new_node->c3 = NULL;
	new_node->next = NULL;
	new_node->terminal = NULL;
	new_node->type = type;
	new_node->flag = 0;

	return new_node;
}

struct node *make_program_node(char *token, struct node *declarations)
{
	struct node *new_node = make_type_node("Program");
	new_node->c1 = make_terminal_node("Id", token);	/* e.g. Id(abc) */
	if (declarations) {
		new_node->c2 = declarations;
	}

	return new_node;
}

struct node *make_terminal_node(char *terminal, char *token)
{
	struct node *new_node = make_type_node(NULL);
	new_node->terminal = terminal;
	new_node->type = token;	/* the token read from yacc */

	if (!strcmp(terminal, "Int") || !strcmp(terminal, "Void") ||
	    !strcmp(terminal, "Double") || !strcmp(terminal, "Bool")) {
		new_node->flag = 1;
	}

	return new_node;
}

struct node *make_length_node(char *token)
{
	struct node *new_node = make_type_node("Length");
	new_node->c1 = make_terminal_node("Id", token);

	return new_node;
}

struct node *make_operator_node(struct node *expr_left, char *operation, struct node *expr_right)
{
	struct node *new_node = make_type_node(operation);	/* the token read from yacc */

	if (expr_left) {
		new_node->c1 = expr_left;
	}

	new_node->c2 = expr_right;

	return new_node;
}

struct node *merge_declarations(struct node *first_dec, struct node *last_dec)
{
	struct node *temp = first_dec;
	while (temp->next) {
		temp = temp->next;
	}
	temp->next = last_dec;
	return first_dec;
}

struct node *merge_declarations_xxx(struct node *first_dec, struct node *last_dec)
{
	struct node *temp;
	temp = first_dec;

	if (!first_dec) {
		return first_dec;
	}

	while (temp->next) {
		temp = temp->next;
	}

	temp->next = last_dec;
	return first_dec;
}

struct node *merge_stat_declarations(struct node *first_stat, struct node *last_stat)
{
	return (!first_stat) ? last_stat : merge_declarations(first_stat,
							      last_stat);
}

struct node *make_formal_params_node(char *type, char *token)
{
	struct node *new_node2 = make_type_node("MethodParams");

	new_node2->c1 = ___make_formal_params_temp_node___(type, token);

	return new_node2;
}

struct node *___make_formal_params_temp_node___(char *type, char *token)
{
	struct node *new_node = make_type_node("ParamDecl");

	new_node->c1 = make_type_node(type);
	new_node->c2 = make_terminal_node("Id", token);

	return new_node;
}

struct node *make_formal_params_temp_node(struct node *type, char *token)
{
	struct node *new_node = make_type_node("ParamDecl");

	new_node->c1 = type;
	new_node->c2 = make_terminal_node("Id", token);

	return new_node;
}

struct node *make_call_operator_node(char *token, struct node *arguments)
{
	struct node *new_node = make_type_node("Call");
	new_node->c1 = make_terminal_node("Id", token);
	new_node->c2 = arguments;

	return new_node;
}

struct node *make_call_operator_another_node(char *token, struct node *arguments, struct node *token2)
{
	struct node *new_node = make_type_node("Call");
	new_node->c1 = make_terminal_node("Id", token);
	new_node->c2 = arguments;
	new_node->c3 = token2;

	return new_node;
}

struct node *make_statement_node(char *token, struct node *stat, struct node *stat2, struct node *stat3)
{
	struct node *new_node = make_type_node(token);
	new_node->c1 = stat;
	if (stat2) {
		new_node->c2 = stat2;
	}
	if (stat3) {
		new_node->c3 = stat3;
	}
	return new_node;
}

struct node *make_statement_node_str(char *token, char *str)
{
	struct node *new_node = make_type_node(token);
	new_node->c1 = make_terminal_node("StrLit", str);
	return new_node;
}

struct node *make_ifstatement_node(char *token, struct node *stat, struct node *stat2, struct node *stat3)
{
	struct node *new_node = make_type_node(token);
	new_node->c1 = stat;
	if (stat2) {
		new_node->c2 = stat2;
	} else {
		new_node->c2 = make_type_node("Block");
	}
	if (stat3) {
		new_node->c3 = stat3;
	} else {
		new_node->c3 = make_type_node("Block");
	}
	return new_node;
}

struct node *make_whilestatement_node(char *token, struct node *stat, struct node *stat2, struct node *stat3)
{
	struct node *new_node = make_type_node(token);
	new_node->c1 = stat;
	if (stat2) {
		new_node->c2 = stat2;
	} else {
		new_node->c2 = make_type_node("Block");
	}

	return new_node;
}

struct node *make_do_whilestatement_node(char *token, struct node *stat, struct node *stat2)
{
	struct node *new_node = make_type_node(token);
	if (!stat) {
		new_node->c1 = make_type_node("Block");
	} else {
		new_node->c1 = stat;
	}
	if (stat2) {
		new_node->c2 = stat2;
	} else {
		new_node->c2 = make_type_node("Block");
	}

	return new_node;
}

struct node *make_method_header_node(struct node *type, char *token, struct node *formalpars)
{
	struct node *new_node = make_type_node("MethodHeader");

	if (!type) {
		new_node->c1 = make_terminal_node("Void", NULL);
	} else {
		new_node->c1 = type;
	}
	new_node->c2 = make_terminal_node("Id", token);

	if (formalpars) {
		new_node->c3 = formalpars;
	}
	return new_node;
}

struct node *make_method_header_empty_node(struct node *type, char *token, char *last)
{
	struct node *new_node = make_type_node("MethodHeader");

	if (!type) {
		new_node->c1 = make_terminal_node("Void", NULL);
	} else {
		new_node->c1 = type;
	}
	new_node->c2 = make_terminal_node("Id", token);

	new_node->c3 = make_type_node(last);
	return new_node;
}

struct node *make_method_decl_node(struct node *token, struct node *token2)
{
	struct node *new_node = make_type_node("MethodDecl");

	new_node->c1 = token;
	new_node->c2 = token2;

	return new_node;
}

struct node *make_varDecl_node(char *node, struct node *type, char *token)
{
	struct node *new_node = make_type_node(node);

	new_node->c1 = type;
	new_node->c2 = make_terminal_node("Id", token);

	return new_node;
}

struct node *make_assignment_store_node(char *token, struct node *expression)
{
	struct node *new_node = make_type_node("Assign");
	new_node->c1 = make_terminal_node("Id", token);
	new_node->c2 = expression;
	return new_node;
}

int count_nodes(struct node *a)
{
	int nodes = 0;
	while (a) {
		nodes++;
		a = a->next;
	}
	return nodes;
}

struct node *make_statement_temp_node(struct node *arg)
{
	if (!arg) {
		return NULL;
	}

	if (count_nodes(arg) != 1) {
		struct node *new_node = make_type_node("Block");
		new_node->c1 = arg;
		return new_node;
	} else {
		return arg;
	}
}

struct node *make_varDecl_node_comma(char *node, char *terminal, char *token)
{
	struct node *new_node = make_type_node(node);

	new_node->c1 = make_terminal_node(terminal, NULL);
	new_node->c2 = make_terminal_node("Id", token);

	if (!strcmp(terminal, "Int") || !strcmp(terminal, "Void") ||
	    !strcmp(terminal, "Double") || !strcmp(terminal, "Bool")) {
		new_node->flag = 1;
	}

	return new_node;
}

struct node *make_mbody_vardecl_node(char *token, struct node *token1, struct node *token2)
{
	struct node *new_node = make_type_node(token);

	new_node->c1 = token1;
	new_node->c2 = token2;

	return new_node;
}

struct node *make_mbody_node(char *token, struct node *token1)
{
	struct node *new_node = make_type_node(token);

	new_node->c1 = token1;

	return new_node;
}

struct node *make_formal_params_type_node(char *node, struct node *type, char *token)
{
	struct node *new_node = make_type_node(node);

	new_node->c1 = ___make_formal_params_type_temp_node___(type, token);

	return new_node;
}

struct node *make_formal_params_type_temp_node(char *node, struct node *type, char *token, struct node *token2)
{
	struct node *new_node = make_type_node(node);

	new_node->c1 = ___make_formal_params_type_temp_node___(type, token);
	new_node->c2 = token2;

	return new_node;
}

struct node *___make_formal_params_type_temp_node___(struct node *type, char *token)
{
	struct node *new_node = make_type_node("ParamDecl");

	new_node->c1 = type;
	new_node->c2 = make_terminal_node("Id", token);

	return new_node;
}

struct node *make_varDeclTemp_temp_node(char *type, char *token)
{
	struct node *new_node = make_type_node("VarDecl");

	new_node->c1 = make_terminal_node(type, NULL);
	new_node->c2 = make_terminal_node("Id", token);

	return new_node;
}

struct node *make_FieldDeclTemp_temp_node(char *type, char *token)
{
	struct node *new_node = make_type_node("FieldDecl");

	new_node->c1 = make_terminal_node(type, NULL);
	new_node->c2 = make_terminal_node("Id", token);

	return new_node;
}

void print_ast(struct node *tree, int d)
{
	indent_tree(d);

	if (tree->terminal && tree->flag == 1) {
		printf("%s\n", tree->terminal);

	} else if (tree->terminal && tree->flag != 1) {
		printf("%s(%s)\n", tree->terminal, tree->type);

	} else {
		printf("%s\n", tree->type);
	}

	if (tree->c1) {
		print_ast(tree->c1, d + 1);
	}
	if (tree->c2) {
		print_ast(tree->c2, d + 1);
	}
	if (tree->c3) {
		print_ast(tree->c3, d + 1);
	}
	if (tree->next) {
		print_ast(tree->next, d);
	}
}

void indent_tree(int n)
{
	int i;
	for (i = 0; i < n; i++) {
		printf("..");
	}
}
