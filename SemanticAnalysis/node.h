#ifndef NODE_H
#define NODE_H

#include "sym.h"

/* Abstract Syntax Tree */
struct node {
    char *note;
    char *type;         /* Program,FieldDecl,VarDecl,MethodDecl,MethodHeader,....  */
    char *func_name;
    char *terminal;     /* Bool, Boollit, Double, DecLit, Id, Int, RealLit, StrLit, Void */
    int flag;
    int line;           /*to report line and column of error*/
    int col;
    struct node *c1; /* children nodes */
    struct node *c2;
    struct node *c3;
    struct node *next;  /* next node in the linked list*/
};

/* Token read from lex->yacc */
struct loc {
    char *str; /* token */
    int line; /*to report line and column of error*/
    int col;
};

typedef struct loc l_t;

extern char all_funcs[256][256];
extern struct sym *root2;

char *check_not(struct node *);
char * recursionToCheckParams(struct node *, int , int);

int search_method_params2(char *);
int __search_variable__(char *, char *);
void check_call(struct node *, int);
char *check_oper_childs(struct node *);

void check_assign(struct node *);
void print_ast(struct node *, int);
void print_an_ast(struct node *, int);
void indent_tree(int);
void check_plus_minus_not(struct node *);

int __strcmp__(char *s1, char *s2);
char *search_method_params(char *, char *, int);
int count_nodes(struct node *);
int is_node_anottated(char *);
int is_binary_operator(char *);
int is_operation_node(char *);
int is_unary_operator(char *);
int parse_int(char *str);
int parse_double(char *str);

char *operators_result(char *, char *);
char *check_literal(char *s);
int check_next(struct node *tree, int d, int);
int count_args(char *str);


/* refactor later */
struct node *make_operator_node_loc(struct node *, char *, struct node *, struct loc *);
struct node *make_type_node_loc(char *, struct loc *);
struct node *make_terminal_node_loc(char *, char *, struct loc *);
struct node *make_length_node_loc(char *, struct loc *);
struct node *make_call_operator_another_node_loc(char *, struct node *, struct node *, struct loc *);
struct node *make_call_operator_node_loc(char *, struct node *, struct loc *);
struct node *make_assignment_store_node_loc(char *, struct node *, struct loc *);
/* refactor later */

struct node *make_type_node(char *);
struct node *make_program_node(char *, struct node *);
struct node *make_terminal_node(char *, char *);
struct node *make_length_node(char *);
struct node *make_operator_node(struct node *, char *, struct node *);
struct node *merge_declarations(struct node *, struct node *);
struct node *merge_declarations_xxx(struct node *, struct node *);
struct node *merge_stat_declarations(struct node *, struct node *);
struct node *make_formal_params_node(char *, char *, struct loc *);
struct node *___make_formal_params_temp_node___(char *, char *, struct loc *);
struct node *make_formal_params_temp_node(struct node *, char *, struct loc *);
struct node *make_call_operator_node(char *, struct node *);
struct node *make_call_operator_another_node(char *, struct node *, struct node* );
struct node *make_statement_node(char *, struct node *, struct loc *);
struct node *make_statement_node_str(char *, char *, struct loc *);
struct node *make_ifstatement_node(char *, struct node *, struct node *, struct node *, struct loc *);
struct node *make_whilestatement_node(char *, struct node *, struct node *, struct loc *);
struct node *make_do_whilestatement_node(char *, struct node *, struct node *, struct loc *);
struct node *make_method_header_node(struct node *, char *, struct node *, struct loc *);
struct node *make_method_header_empty_node(struct node *, char *, char *, struct loc *);
struct node *make_method_decl_node(struct node *, struct node *);
struct node *make_varDecl_node(char *, struct node *, char *, struct loc *);
struct node *make_assignment_store_node(char *, struct node *);
struct node *make_statement_temp_node(struct node *);
struct node *make_varDecl_node_comma(char *, char *, char *);
struct node *make_mbody_vardecl_node(char*, struct node *, struct node *);
struct node *make_mbody_node(char*, struct node *);
struct node *make_formal_params_type_node(char *, struct node *, char *, struct loc *);
struct node *___make_formal_params_type_temp_node___(struct node *, char *, struct loc *);
struct node *make_formal_params_type_temp_node(char *, struct node *, char *, struct node *, struct loc *);
struct node *make_varDeclTemp_temp_node (char *, char *, struct loc *);
struct node *make_FieldDeclTemp_temp_node(char *, char *, struct loc *);

#endif /* _NODE_H_ */
