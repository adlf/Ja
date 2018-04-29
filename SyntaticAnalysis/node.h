#ifndef NODE_H
#define NODE_H

/* linked list definition*/
struct node {

    /* children nodes */
    struct node *c1;
    struct node *c2;
    struct node *c3;

    /* next node in the linked list*/
    struct node *next;

    /* Program,FieldDecl,VarDecl,MethodDecl,MethodHeader,....  */
    /* basically everything not terminal goes here */
    char *type;

    /* terminals: Bool, Boollit, Double, DecLit, Id, Int, RealLit, StrLit, Void */
    char *terminal;

    int flag;
};

struct node *make_type_node(char *);
struct node *make_program_node(char *, struct node *);
struct node *make_terminal_node(char *, char *);
struct node *make_length_node(char *);
struct node *make_operator_node(struct node *, char *, struct node *);
struct node *merge_declarations(struct node *, struct node *);
struct node *merge_declarations_xxx(struct node *, struct node *);
struct node *merge_stat_declarations(struct node *, struct node *);
struct node *make_formal_params_node(char *, char *);
struct node *___make_formal_params_temp_node___(char *, char *);
struct node *make_formal_params_temp_node(struct node *, char *);
struct node *make_call_operator_node(char *, struct node *);
struct node *make_call_operator_another_node(char *, struct node *, struct node* );
struct node *make_statement_node(char *, struct node *, struct node *, struct node *);
struct node *make_statement_node_str(char *, char *);
struct node *make_ifstatement_node(char *, struct node *, struct node *, struct node *);
struct node *make_whilestatement_node(char *, struct node *, struct node *, struct node *);
struct node *make_do_whilestatement_node(char *, struct node *, struct node *);
struct node *make_method_header_node(struct node *, char *, struct node *);
struct node *make_method_header_empty_node(struct node *, char *, char *);
struct node *make_method_decl_node(struct node *, struct node *);
struct node *make_varDecl_node(char *, struct node *, char *);
struct node *make_assignment_store_node(char *, struct node *);
int count_nodes(struct node *);
struct node *make_statement_temp_node(struct node *);
struct node *make_varDecl_node_comma(char *, char *, char *);
struct node *make_mbody_vardecl_node(char*, struct node *, struct node *);
struct node *make_mbody_node(char*, struct node *);
struct node *make_formal_params_type_node(char *, struct node *, char *);
struct node *___make_formal_params_type_temp_node___(struct node *, char *);
struct node *make_formal_params_type_temp_node(char *, struct node *, char *, struct node *);
struct node *make_varDeclTemp_temp_node (char *, char *);
struct node *make_FieldDeclTemp_temp_node(char *, char *);
void print_ast(struct node *, int);
void indent_tree(int);

#endif /* _NODE_H_ */
