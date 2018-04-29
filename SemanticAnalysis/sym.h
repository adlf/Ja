#ifndef SYM_H
#define SYM_H

#include "node.h"



extern char all_funcs[256][256];
extern struct sym *root2;

struct sym {
    char *element_type;         /* class table, method table, variable, method */
    char *name;                 /* element name (main, gcd...*/
    char *type;                 /* element type: int, double , void */
    int flag;                   /* if the element is a formal parameter */
    struct sym *next;           /* next item in the linked list*/
    struct sym *table_method;   /* method symbol table */
    struct node *method_body;   /* pointer to the statemens in the method */
};


struct search_table {
    char *element_type;                 /* class table, method table, variable, method */
    char *name;                         /* element name (main, gcd...*/
    char *type;                         /* element type: int, double , void */
    int flag;                           /* if the element is a formal parameter */
    struct search_table *next;          /* next item in the linked list*/
    struct search_table *table_method;  /* method symbol table */
    struct node *method_body;           /* pointer to the statemens in the method */
};

struct lookup_array {
    char *name;
    char *type;
    char *scope;
    int isMethod;
};


struct sym *aux_table;

struct node *temp_method_ast;

char * to_local_search;

void test_thing();


char *search_global_scope(char *);
char * search_global_array(char *, char *, int);

void print_global_lookup_table(struct sym *);
int valid_types(char *);

char * search_table(struct sym *, char *, char *);
char *get_function_name(char *);
char *to_lower(char *);


int check_node(char *);
int check_all_nodes(char *);

int check_symbol(struct sym *, char *);

struct search_table *global_lookup_table;

struct sym *parse_ast(struct node *, int);
struct sym *make_node(char *, char *);
struct sym *make_table(char *, char *);
struct sym *make_variable(char *, char *);
struct sym *make_method(char *, struct node * );
struct sym *make_table_method(struct node *, int);

void check_semantics(struct node *, struct sym *);
void add_element_to_symbol_table(struct sym *, struct sym *);
void print_element(struct sym *, struct node *);
void print_table(struct sym *, struct node *);
int is_symbol_in_table(struct sym *, struct node *, int);
void add_param_decl(struct sym *, struct node *);
void add_vardecls(struct sym *, struct node *);

void search_and_fill_ast(struct node *, char *, char *);

int search_variable(char *, char *);
void analyze_ast_semantic(struct node *, struct sym *, struct sym *);

int __strcmp(char *, char *);

#endif /* _SYM_H_ */
