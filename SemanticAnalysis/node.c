#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

int to_print = 1;

struct to_call_ {
    int pos;
    int flag;
    int nargs;
    int real_args;
    int points;
};

char *func_name = NULL;
char *func_args;

char *to_change_call;

char func_args_param_decl[128][128];
int func_args_param_decl_sp = 0;

int check_args_flag = 0;

struct to_call_ to_call[1] = {
    { -1, -1, -1, -1, -1}
};

char fill_parameters[256];

struct node *make_operator_node_loc(struct node *expr_left, char *operation, struct node *expr_right, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(operation, loc);

    if (expr_left) {
        new_node->c1 = expr_left;
    }

    new_node->c2 = expr_right;

    if (is_binary_operator(operation)) {
        new_node->note = "boolean";
    }

    return new_node;
}

struct node *make_type_node_loc(char *type, struct loc *loc)
{
    struct node *new_node = malloc(sizeof(struct node));
    new_node->c1 = NULL;
    new_node->c2 = NULL;
    new_node->c3 = NULL;
    new_node->next = NULL;
    new_node->terminal = NULL;
    new_node->type = type;
    new_node->flag = 0;
    new_node->note = NULL;
    new_node->func_name = NULL;
    if (loc != NULL) {
        new_node->line = loc->line;
        new_node->col = loc->col;
    } else {
        // error
        new_node->line = -2;
        new_node->line = -2;
    }
    return new_node;
}

struct node *make_terminal_node_loc(char *terminal, char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(NULL, loc);
    new_node->terminal = terminal;
    new_node->type = token; /* the token read from yacc */

    if (!strcmp(terminal, "DecLit")) {
        new_node->note = "int";
    } else if (!strcmp(terminal, "RealLit")) {
        new_node->note = "double";
    } else if (!strcmp(terminal, "StrLit")) {
        new_node->note = "String";
    }  else if (!strcmp(terminal, "BoolLit")) {
        new_node->note = "boolean";
    }

    if (!strcmp(terminal, "Int") || !strcmp(terminal, "Void") ||
            !strcmp(terminal, "Double") || !strcmp(terminal, "Bool")) {
        new_node->flag = 1;
    }

    return new_node;
}

struct node *make_length_node_loc(char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("Length", loc);
    new_node->c1 = make_terminal_node_loc("Id", token, loc);

    new_node->note = "int";

    return new_node;
}

struct node *make_call_operator_another_node_loc(char *token, struct node *arguments, struct node *token2, struct loc *l)
{
    struct node *new_node = make_type_node_loc("Call", l);
    new_node->c1 = make_terminal_node_loc("Id", token, l);
    new_node->c2 = arguments;
    new_node->c3 = token2;


    return new_node;
}

struct node *make_call_operator_node_loc(char *token, struct node *arguments, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("Call", loc);
    new_node->c1 = make_terminal_node_loc("Id", token, loc);
    new_node->c2 = arguments;


    return new_node;
}

struct node *make_assignment_store_node_loc(char *token, struct node *expression, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("Assign", loc);
    new_node->c1 = make_terminal_node_loc("Id", token, loc);
    new_node->c2 = expression;
    //new_node->note = "undef";
    return new_node;
}

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
    new_node->note = NULL;

    return new_node;
}

struct node *make_program_node(char *token, struct node *declarations)
{
    struct node *new_node = make_type_node("Program");
    new_node->c1 = make_terminal_node("Id", token); /* e.g. Id(abc) */
    if (declarations) {
        new_node->c2 = declarations;
    }
    return new_node;
}

struct node *make_terminal_node(char *terminal, char *token)
{
    struct node *new_node = make_type_node(NULL);
    new_node->terminal = terminal;
    new_node->type = token; /* the token read from yacc */

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
    struct node *new_node = make_type_node(operation);  /* the token read from yacc */

    if (expr_left) {
        new_node->c1 = expr_left;
        //printf("chield one type: %s\n", expr_left->note);
        //new_node->c1->note = expr_left->note;
    }

    new_node->c2 = expr_right;
    //new_node->c2->note = expr_left->note;

    if (!strcmp(operation, "ParseArgs")) {
        new_node->note = "int";

    }

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
    return (!first_stat) ? last_stat : merge_declarations(first_stat, last_stat);
}

struct node *make_formal_params_node(char *type, char *token, struct loc *loc)
{
    struct node *new_node2 = make_type_node_loc("MethodParams", loc);

    new_node2->c1 = ___make_formal_params_temp_node___(type, token, loc);

    return new_node2;
}

struct node *___make_formal_params_temp_node___(char *type, char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("ParamDecl", loc);

    new_node->c1 = make_type_node_loc(type, loc);
    new_node->c2 = make_terminal_node_loc("Id", token, loc);
    if (!strcmp(type, "StringArray")) { new_node->c2->note = "String[]"; }
    else { new_node->c2->note = to_lower(type); }

    return new_node;
}

struct node *make_formal_params_temp_node(struct node *type, char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("ParamDecl", loc);

    new_node->c1 = type;
    new_node->c2 = make_terminal_node_loc("Id", token, loc);
    //new_node->c2->note = to_lower(type->terminal);

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

struct node *make_statement_node(char *token, struct node *stat, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(token, loc);
    new_node->c1 = stat;
    return new_node;
}

struct node *make_statement_node_str(char *token, char *str, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(token, loc);
    new_node->c1 = make_terminal_node_loc("StrLit", str, loc);
    return new_node;
}

struct node *make_ifstatement_node(char *token, struct node *stat, struct node *stat2, struct node *stat3, struct loc *l)
{
    struct node *new_node = make_type_node_loc(token, l);
    new_node->c1 = stat;
    if (stat2) {
        new_node->c2 = stat2;
    } else {
        new_node->c2 = make_type_node_loc("Block", l);
    }
    if (stat3) {
        new_node->c3 = stat3;
    } else {
        new_node->c3 = make_type_node_loc("Block", l);
    }
    return new_node;
}

struct node *make_whilestatement_node(char *token, struct node *stat, struct node *stat2, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(token, loc);
    new_node->c1 = stat;
    if (stat2) {
        new_node->c2 = stat2;
    } else {
        new_node->c2 = make_type_node_loc("Block", loc);
    }

    return new_node;
}

struct node *make_do_whilestatement_node(char *token, struct node *stat, struct node *stat2, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(token, loc);
    if (!stat) {
        new_node->c1 = make_type_node_loc("Block", loc);
    } else {
        new_node->c1 = stat;
    }
    if (stat2) {
        new_node->c2 = stat2;
    } else {
        new_node->c2 = make_type_node_loc("Block", loc);
    }

    return new_node;
}

struct node *make_method_header_node(struct node *type, char *token, struct node *formalpars, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("MethodHeader", loc);

    if (!type) {
        new_node->c1 = make_terminal_node_loc("Void", NULL, loc);
    } else {
        new_node->c1 = type;
    }
    new_node->c2 = make_terminal_node_loc("Id", token, loc);

    if (formalpars) {
        new_node->c3 = formalpars;
    }
    return new_node;
}

struct node *make_method_header_empty_node(struct node *type, char *token, char *last, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("MethodHeader", loc);

    if (!type) {
        new_node->c1 = make_terminal_node_loc("Void", NULL, loc);
    } else {
        new_node->c1 = type;
    }
    new_node->c2 = make_terminal_node_loc("Id", token, loc);

    new_node->c3 = make_type_node_loc(last, loc);
    return new_node;
}

struct node *make_method_decl_node(struct node *token, struct node *token2)
{
    struct node *new_node = make_type_node("MethodDecl");

    new_node->c1 = token;
    new_node->c2 = token2;

    return new_node;
}

struct node *make_varDecl_node(char *node, struct node *type, char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(node, loc);

    new_node->c1 = type;
    new_node->c2 = make_terminal_node_loc("Id", token, loc);
    //new_node->c2->note = to_lower(type->terminal);
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

struct node *make_formal_params_type_node(char *node, struct node *type, char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(node, loc);

    new_node->c1 = ___make_formal_params_type_temp_node___(type, token, loc);

    return new_node;
}

struct node *make_formal_params_type_temp_node(char *node, struct node *type, char *token, struct node *token2, struct loc *loc)
{
    struct node *new_node = make_type_node_loc(node, loc);

    new_node->c1 = ___make_formal_params_type_temp_node___(type, token, loc);
    new_node->c2 = token2;
    new_node->c2->note = to_lower(type->terminal);

    return new_node;
}

struct node *___make_formal_params_type_temp_node___(struct node *type, char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("ParamDecl", loc);

    new_node->c1 = type;
    new_node->c2 = make_terminal_node_loc("Id", token, loc);
    //printf("[[[[[[[[[[[[[[[[[[[[[[[[[[%s]]]]]]]]]]]]]]]]]]]]]]]]]]\n", type->terminal);
    new_node->c2->note = to_lower(type->terminal);

    return new_node;
}

struct node *make_varDeclTemp_temp_node(char *type, char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("VarDecl", loc);

    new_node->c1 = make_terminal_node_loc(type, NULL, loc);
    new_node->c2 = make_terminal_node_loc("Id", token, loc);
    new_node->c2->note = to_lower(type);

    return new_node;
}

struct node *make_FieldDeclTemp_temp_node(char *type, char *token, struct loc *loc)
{
    struct node *new_node = make_type_node_loc("FieldDecl", loc);

    new_node->c1 = make_terminal_node_loc(type, NULL, loc);
    new_node->c2 = make_terminal_node_loc("Id", token, loc);

    return new_node;
}

void print_ast(struct node *tree, int d)
{
    indent_tree(d);

    if (tree->terminal && tree->flag == 1) {
        puts(tree->terminal);
    } else if (tree->terminal && tree->flag != 1) {
        printf("%s(%s)\n", tree->terminal, tree->type);
    } else {
        puts(tree->type);
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



int search_method_params2(char *name)
{
    int i;

    /*printf("\n");
    for (i = 0; strcmp(all_funcs[i], "-1"); i++) {
        printf("== > all_funcs[%d] = %s < ==\n", i, all_funcs[i]);
    }*/
    for (i = 0; strcmp(all_funcs[i], "-1"); i++) {
        //printf("== > all_funcs[%d] = %s < ==\n", i, all_funcs[i]);

        if (!strcmp(all_funcs[i], name)) {
            //printf("all_funcs[i+1] = %s\n", all_funcs[i + 1]);
            return i;
        }
    }
    return -1;
}

char *search_method_params(char *name, char *rest_of_string, int n_args)
{
    int i;
    char store[256][256];
    char store2[256][256];

    int return_flag = 0;

    memset(store2, 0, sizeof(store2));

    //  printf("rest_of_string = %s\n", rest_of_string);

    int m;
    int k = 0, pos = 0;
    for (m = 1; m < strlen(rest_of_string) - 1; ++m) {

        if (rest_of_string[m] != ',') {
            store2[pos][k] = rest_of_string[m];
        } else {
            k = -1;
            pos++;
        }
        k++;
    }

    int isvalid = 0;

    // store2 has the type of arguments passed when we call it, not when declared!!
    //printf("store2[0] ==> %s     |\n", store2[0]);
    //printf("store2[1] ==> %s     |\n", store2[1]);
    //printf("store2[2] ==> %s     |\n", store2[2]);

    k = 0, pos = 0;
    int var = 0;

    for (i = 1; strcmp(all_funcs[i], "-1"); i++) {

        memset(store, 0, sizeof(store));

        pos = 0;
        k = 0;

        if (!strcmp(all_funcs[i], name)) {

            isvalid = 1;

            for (m = 1; m < strlen(all_funcs[i + 1]) - 1; m++) {


                if (all_funcs[i + 1][m] != ',') {

                    store[pos][k] = all_funcs[i + 1][m];
                } else {
                    k = -1;
                    pos++;
                    isvalid = isvalid + 1;

                }

                k++;
            }

            //printf("store[0] ==> %s     |\n", store[0]);

            int flag_counter = 0; /* number of equal type arguments*/
            int diff_counter = 0;
            //printf("--\n");
            //puts("");



            // n_ars-> number of arguments passed when the function is called
            for (var = 0; var < n_args; var++) {
                //printf("comparing store[var] => %s with store2[var] => %s\n", store[var], store2[var] );
                if (!strcmp(store[var], store2[var]) || (!strcmp(store2[var], "int") && !strcmp(store[var], "double"))) {
                    flag_counter++;
                } else {
                    diff_counter++;
                }
            }

            //printf("flag_counter = %d n_args = %d\n", flag_counter,n_args);
            //printf("diff_counter = %d\n", diff_counter);


            if (flag_counter == n_args && isvalid == n_args) {


                return_flag = 1;
                to_change_call = all_funcs[i + 1];
                return all_funcs[i - 1];
            }

            if (strlen(store[0]) == 0 && n_args == 0) {
                return_flag = 1;
                to_change_call = all_funcs[i + 1];
                return all_funcs[i - 1];
            }
        }
    }
    if (!return_flag) {
        to_change_call = "undef";
    }
    return "undef";
}

char *recursionToCheckParams(struct node * tree, int real_args, int n_args)
{

    // -> ver nome da função
    int aux_args = 0;

    char * variable_func_name = tree->c1->type;
    //printf("function name= %s\n", tree->c1->type);

    //strcpy(fill_parameters, tree->c1->type);
    memset(fill_parameters, 0, sizeof(fill_parameters));
    strcat(fill_parameters, "(");

    /*if (real_args == 1 && n_args == 1) {
        if (!strcmp(tree->note, "bool")) {
            tree->note = "boolean";
        }

        strcat(fill_parameters, tree->note);
        strcat(fill_parameters, ")");

        return search_method_params(tree->c1->type, fill_parameters, n_args);
    }*/



    if (tree->c2 != NULL) {
        aux_args++;
        //printf("c2= %s\n", tree->c2->note);
        if (tree->c2->note == NULL) {


            //printf("tree->c2->type = %s, tree->c1->type = %s\n", tree->c2->type, tree->c1->type);
            tree->c2->note = search_global_array(tree->c2->type, tree->c1->type, 0);
            if (tree->c2->note == NULL) {
                tree->c2->note = "undef";
            }
        }
        strcat(fill_parameters, tree->c2->note);


        if (tree->c3 != NULL) {
            aux_args++;
            if (tree->c3->note == NULL) {
                tree->c3->note = search_global_array(tree->c3->type, tree->c1->type, 0);
                if (tree->c3->note == NULL) {


                    tree->c3->note = "undef";
                }
            }
            strcat(fill_parameters, ",");
            strcat(fill_parameters, tree->c3->note);


            struct node *copy_of_tree = tree->c3;

            while (copy_of_tree->next) {

                aux_args++;
                strcat(fill_parameters, ",");

                if (copy_of_tree->next->note == NULL) {
                    copy_of_tree->next->note = search_global_array(copy_of_tree->next->type, variable_func_name, 0);
                    if (copy_of_tree->next->note == NULL) {
                        copy_of_tree->next->note = "undef";
                    }
                }

                strcat(fill_parameters, copy_of_tree->next->note);

                copy_of_tree = copy_of_tree->next;

            }
            strcat(fill_parameters, ")");
        } else {
            strcat(fill_parameters, ")");
        }
    } else {
        strcat(fill_parameters, ")");
    }

    //    printf(" type = %s    fill_parameters = %s           aux_args = %d\n", tree->c1->type,fill_parameters, aux_args);
    //printf("final -> %s -> %d \n", fill_parameters,aux_args);

    // fill parameters has the type of arguments we pass when we call the function
    return search_method_params(tree->c1->type, fill_parameters, aux_args);
}


char *parameters_to_string()
{
    int pos = 0;
    char *str = calloc(256, sizeof(char));

    for (int i = 0; i < func_args_param_decl_sp; ++i) {
        for (int j = 0; j < strlen(func_args_param_decl[i]); ++j) {
            str[pos] = func_args_param_decl[i][j];
            pos++;
        }
    }

    return str;
}

// check all the arguments to call....
int check_next(struct node *tree, int d, int flag_to_check)
{
    int args = 0;


    if (flag_to_check == 1) {
        while (tree->next) {
            tree = tree->next;
            args++;
        }
    } else {
        while (tree->next) {
            indent_tree(d);
            if (tree->next->note == NULL) {
                tree->next->note = "undef";    // variable not declared
            }
            printf("%s(%s) - %s\n", tree->next->terminal, tree->next->type, tree->next->note);
            tree = tree->next;
            args++;
        }
    }
    //printf("args:::::::::: %d\n", args);
    return args;
}

int count_args(char *str)
{
    int i, count = 0;
    int flag = 0;
    for (i = 0, count = 0; str[i]; i++) {
        count += (str[i] == ',');
        flag = 1;
    }
    if (flag == 0) {
        return count;
    }
    return count + 1;
}


void print_an_ast(struct node *tree, int d)
{

    register int n_args = 0 ;
    register int temp = 0;

    char *callback = NULL;

    indent_tree(d);

    if (d <= to_call[1].points) {
        to_call[1].flag = 0;
    }

    if (tree->terminal && tree->flag == 1) {

        puts(tree->terminal);
    } else if (tree->terminal && tree->flag != 1) {
        if (to_print) {
            if (tree->note) {
                if (!strcmp(tree->note, "string[]")) {
                    printf("%s(%s) - String[]\n", tree->terminal, tree->type);
                } else {
                    if (to_call[0].flag == 1) {
                        /* the type of call */

                        /* if the number of arguments passed isnt the same as the one declared*/


                        //if (to_call[0].real_args != to_call[0].nargs) {
                        //    tree->note = "undef";
                        //}

                        printf("%s(%s) - %s\n", tree->terminal, tree->type, tree->note); //all_funcs[to_call[0].pos - 1]);
                        to_call[0].flag = 0;
                    } else {
                        if (!strcmp(tree->note, "undef")) {
                            printf("%s(%s) - %s\n", tree->terminal, tree->type, tree->note);
                        } else {
                            if (!strcmp(tree->note, "bool")) {
                                tree->note = "boolean";
                            }

                            printf("%s(%s) - %s\n", tree->terminal, tree->type, tree->note);
                        }

                        if (tree->next) {
                            check_next(tree, d, 0);
                        }
                    }
                }
            } else {
                // if the "note" field of a variable is null, it means the variable is not declared
                // so its undefined, because the "note" field if filled when the variable is declared.
                //to_local_search = parameters_to_string();
                callback = search_global_array(tree->type, func_name, 0);
                //printf("=========================================================>>> callback : %s\n", callback);
                if (callback == NULL) {
                    callback = "undef";
                }
                tree->note = to_lower(callback);

                if (!strcmp(tree->note, "string[]")) {
                    tree->note = "String[]";
                    printf("%s(%s) - %s\n", tree->terminal, tree->type, tree->note);
                } else if (!strcmp(tree->note, "bool")) {
                    tree->note = "boolean";
                    printf("%s(%s) - %s\n", tree->terminal, tree->type, tree->note);
                } else {
                    printf("%s(%s) - %s\n", tree->terminal, tree->type, tree->note);
                }
            }
        } else {
            printf("%s(%s)\n", tree->terminal, tree->type);

        }
    } else {
        to_print = is_node_anottated(tree->type);

        if (!strcmp(tree->type, "Call")) {

            to_call[0].flag = 1;
            to_call[0].real_args = 0;
            printf("%s", tree->type);
            if (tree->c1) {

                if (tree->c1->terminal && tree->c1->flag != 1) {
                    // search function name
                    to_call[0].pos = search_method_params2(tree->c1->type);

                    if (to_call[0].pos != -1) {

                        // fill the next child note to avoid null comparison
                        tree->note = all_funcs[to_call[0].pos - 1];

                        // method doesnt have parameters
                        // check the arguments of the fucntion

                        //printf("all_funcs[to_call[0].pos + 1] = %s\n", all_funcs[to_call[0].pos + 1]);
                        // number of arguments passed when the function is declared
                        n_args = count_args(all_funcs[to_call[0].pos + 1]);

                        //printf("n_args = %d\n", n_args);
                        to_call[0].nargs = n_args;



                        if (!strcmp(all_funcs[to_call[0].pos + 1], "()")) {
                            tree->c1->note = "()";
                            to_call[0].real_args++;
                        } else {
                            tree->c1->note = all_funcs[to_call[0].pos + 1];
                        }

                        if (tree->c2) {
                            to_call[0].real_args++;

                        }

                        if (tree->c3) {
                            to_call[0].real_args++;
                            if (tree->c3->next) {
                                temp = check_next(tree->c3, d, 1);
                            }
                        }





                        //  printf("temp-  ----->%d\n", temp);


                        to_call[0].real_args += temp;

                        if (to_call[0].real_args != to_call[0].nargs) {
                            tree->note = "undef";
                        }

                        //  printf("real args: %d, nargs: %d\n", to_call[0].real_args, to_call[0].nargs);


                        to_call[1].flag = 1;
                        to_call[1].points = d;

                        //if (to_call[0].real_args == 1 && to_call[0].nargs == 1) {
                        //if (!strcmp(tree->note, "bool")) {
                        //  tree->note = "boolean";
                        //}

                        //printf(" - %s\n", tree->note);
                        // } //else {



                        //if (to_call[0].real_args != 1 && to_call[0].nargs != 1) {
                        tree->note = recursionToCheckParams(tree, to_call[0].real_args, to_call[0].nargs);
                        tree->c1->note = to_change_call;
                        memset(fill_parameters, 0, sizeof(fill_parameters));
                        strcpy(fill_parameters, tree->c1->type);
                        strcat(fill_parameters, "(");
                        //}


                        if (!strcmp(tree->note, "bool")) {
                            tree->note = "boolean";
                        }

                        printf(" - %s\n", tree->note);
                        //}

                    } else {
                        tree->note = "undef";
                        tree->c1->note = "undef";
                        printf(" - %s\n", tree->note);
                    }
                    //search_variable(root2, "sdkljsdlfkjslkdjflsdkfjf", "barra");
                }
            }

            //check_call(tree, d);
        } else if (is_binary_operator(tree->type)) {
            printf("%s - %s\n", tree->type, tree->note);
        } else if (!strcmp(tree->type, "ParseArgs") || !strcmp(tree->type, "Length")) {
            printf("%s - %s\n", tree->type, tree->note);
        } else if (is_operation_node(tree->type)) {

            tree->note = NULL;
            //printf(" - %s [%s %s] ", tree->type, tree->c1->note, tree->c2->note);

            char *res = check_oper_childs(tree);



            //printf("%s - %s  [%s:%s, %s:%s]\n", tree->type, res, tree->c1->type, tree->c1->note, tree->c2->type, tree->c2->note);
            //printf("%s - %s [%s:%s %s:%s]<<<<<<<<<<<<<<<\n", tree->type, tree->note, tree->c1->type, tree->c1->note,tree->c2->type,tree->c2->note);
            printf("%s - %s\n", tree->type, res);
        }

        else if (is_unary_operator(tree->type)) {
            tree->note = NULL;
            if (!strcmp("Plus", tree->type) || !strcmp("Minus", tree->type)) {
                printf("%s", tree->type);

                if (tree->c2->terminal && tree->c2->flag != 1) {
                    if (tree->c2->note) {
                        if (!strcmp(tree->c2->note, "int") || !strcmp(tree->c2->note, "double")) {
                            tree->note = tree->c2->note;
                            printf(" - %s\n", tree->note);
                        } else {
                            tree->note = "undef";
                            puts(" - undef");
                        }
                    } else {

                        tree->note = search_global_array(tree->c2->type, func_name, 0);

                        if (tree->note == NULL) {
                            tree->note = "undef";
                        }

                        printf(" - %s\n", to_lower(tree->note));
                    }

                }
            } else if (!strcmp("Not", tree->type)) {

                //printf("%s", tree->type);
                tree->note = check_not(tree);

                if (!strcmp(tree->note, "undef")) {
                    //tree->note = tree->c2->note;
                }

                printf("%s - %s\n", tree->type, tree->note);
                //printf(" - %s [%s]\n", tree->note, tree->c2->note);

            }
        } else if (!strcmp(tree->type, "Assign")) {
            //printf("(._.)");
            check_assign(tree);


        } else if (!strcmp(tree->type, "Print")) {
            //printf("(._.)");
            //check_assign(tree);
            printf("%s\n", tree->type);
            if (tree->c1->note == NULL) {
                callback = search_global_array(tree->c1->type, func_name, 0);
                if (callback == NULL) {
                    callback = "undef";
                }

                tree->c1->note = to_lower(callback);

                // printf(":: %s\n", tree->c1->note);
            }



        }


        else {
            if (!strcmp(tree->type, "MethodBody")) {
                check_args_flag = 1;
                strcpy(func_args_param_decl[ func_args_param_decl_sp - 1], ")");
                to_local_search = NULL;
                to_local_search = parameters_to_string();
                temp_method_ast = tree;
            }

            if (!strcmp(tree->type, "MethodParams")) {
                func_args_param_decl_sp = 0;
                memset(func_args_param_decl, 0, sizeof(func_args_param_decl));
                strcpy(func_args_param_decl[ func_args_param_decl_sp ], "(");
                func_args_param_decl_sp++;
                check_args_flag = 0;
            }

            if (!strcmp(tree->type, "ParamDecl")) {
                if (tree->c1->terminal != NULL) {
                    char temp [256];
                    if (strcmp(tree->c1->terminal, "Bool") == 0) {
                        strcpy(temp, "boolean");
                    } else {
                        strcpy(temp, tree->c1->terminal);
                    }
                    strcpy(func_args_param_decl[ func_args_param_decl_sp ], to_lower(temp));

                    func_args_param_decl_sp++;
                    strcpy(func_args_param_decl[ func_args_param_decl_sp ], ",");
                    func_args_param_decl_sp++;
                }

                check_args_flag = 0;
            }

            if (!strcmp(tree->type, "MethodHeader")) {
                func_name = tree->c2->type;
            }
            if (!strcmp(tree->type, "FieldDecl")) {
                tree->c2->note = search_global_scope(tree->c2->type);
                puts(tree->type);

            } else if (!strcmp(tree->type, "VarDecl")) {
                puts(tree->type);
                struct sym *copy_aux_table = aux_table;


                char *aux_func_name_to_search;

                int loop_flag = 0;


                while (copy_aux_table && loop_flag == 0) {
                    aux_func_name_to_search = get_function_name(copy_aux_table->name);
                    if (!strcmp(aux_func_name_to_search, func_name) && copy_aux_table->table_method != NULL) {

                        struct sym *temp3;

                        temp3 = copy_aux_table->table_method;

                        while (temp3) {
                            if (temp3->next == NULL) {

                                temp3->next = make_variable(tree->c2->type, tree->c1->terminal);

                                print_global_lookup_table(aux_table);
                                break;
                            }
                            temp3 = temp3->next;
                        }
                    }
                    copy_aux_table = copy_aux_table->next;
                }
            } else {
                puts(tree->type);
            }
        }

        if (tree->c1) {
            print_an_ast(tree->c1, d + 1);
        }
        if (tree->c2) {
            print_an_ast(tree->c2, d + 1);
        }
        if (tree->c3) {
            print_an_ast(tree->c3, d + 1);
        }
        if (tree->next) {
            print_an_ast(tree->next, d);
        }
    }
}


void check_plus_minus_not(struct node *op)
{
    char *temp1 = NULL;
    char *temp2 = NULL;
    if (!strcmp(op->type, "Assign") && op->c1->terminal != NULL) {

        op->note = op->c1->note;
        if (!strcmp(op->note, "bool")) {
            op->note = "boolean";
        }

        printf("%s - %s\n", op->type, op->note);
    } else if (!strcmp("Plus", op->c2->type) || !strcmp("Minus", op->c2->type)) {
        // printf("\t\t[%s] :: type of child one: %s, [%s] :: type of child two: %s\n", op->c1->type,op->c1->note, op->c2->type,op->c2->note);
        if (op->c1->note == NULL) {
            temp1 = "undef";
        } else {
            temp1 = op->c1->note;
        }

        // child of Plus or Minus or Not
        if (op->c2->c2->note == NULL) {
            temp2 = "undef";
        } else {
            // the node type is defined based on its only child
            temp2 = op->c2->c2->note;
        }

        op->note = operators_result(temp1, temp2);
        printf(" - %s\n", op->note);
    }

}

char *check_oper_childs(struct node *tree)
{
    char *callback = NULL;
    if (!strcmp(tree->type, "Assign") && tree->c1->terminal != NULL) {
        tree->note = tree->c1->note;

        //printf("%s - %s\n", tree->type, tree->note);
    } else {
        if (tree->c2 != NULL) {

            if (tree->c1->note == NULL) {
                if (is_operation_node(tree->c1->type) == 0) {


                    callback = search_global_array(tree->c1->type, func_name, 0);

                    // printf("(((((callback of child 1 is %s)))))\n", callback);

                    //printf(" %s =======================>>> %s\n",tree->c1->type, callback);
                    if (callback == NULL) {
                        callback = "undef";
                    }

                    tree->c1->note = to_lower(callback);
                    if (!strcmp(tree->c1->note, "bool")) {
                        tree->c1->note = "boolean";
                    }
                }

            }

            if (tree->c2->note == NULL) {
                if (is_operation_node(tree->c2->type) == 0) {

                    callback = search_global_array(tree->c2->type, func_name, 0);

                    // printf("(((((callback of child 2 is %s)))))\n", callback);


                    //printf(" %s =======================>>> %s\n",tree->c2->type, callback);
                    if (callback == NULL) {
                        callback = "undef";
                    }

                    tree->c2->note = to_lower(callback);
                    if (!strcmp(tree->c2->note, "bool")) {
                        tree->c2->note = "boolean";
                    }
                    //tree->c2->note = "undef";
                }
            }

            //printf("[tree->c1->note = %s || tree->c1->type = %s || tree->c2->note = %s || tree->c2->type = %s]\\\\",
            //     tree->c1->note, tree->c1->type, tree->c2->note, tree->c2->type);


            if (tree->c1->note != NULL && tree->c2->note != NULL) {
                //printf(" (!! %s !! %s -- %s !!) ", tree->note ,tree->c1->note, tree->c2->note);
                tree->note = operators_result(tree->c1->note, tree->c2->note);
            }

            if (tree->note == NULL) {
                tree->note = check_oper_childs(tree->c2);
                //tree->note = operators_result(tree->c1->note, tree->c2->note);


                if (tree->note == NULL) {
                    tree->note = "undef";

                }
            }
        }
    }
    return tree->note;
}




char * check_not(struct node *tree)
{
    char *callback = NULL;
    if (tree->c2->terminal && tree->c2->flag != 1) {
        //printf("        tree->c2->type = %s ", tree->c2->type);
        callback = search_global_array(tree->c2->type, func_name, 0);

        if (callback == NULL) {
            callback = "undef";
        }

        if (!strcmp(callback, "Bool")) {
            callback = "boolean";
        }
        tree->c2->note = to_lower(callback);


        if (tree->c2->note) {
            if (!strcmp(tree->c2->note, "boolean")) {
                //tree->note = tree->c2->note;
                tree->note = "boolean";
                //printf(" - %s\n", tree->note);
            } else {
                tree->note = "undef";
                //puts(" - undef");
            }
        } else {
            tree->note = "undef"; /// changed this tree->c2->note
            //printf(" - %s\n", tree->note);

        }
    } else {
        //printf("%s\n", tree->terminal);
        if (tree->note == NULL) {
            tree->note = check_not(tree->c2);
            if (tree->note == NULL) {
                tree->note = "undef";
            }
        } else {
            //printf("aqu2i!");
            // printf("tree->notes !!! %s\n",tree->note);
        }

        //printf(" - %s\n",tree->note);

    }

    // printf("        returing: %s ", tree->note);
    return tree->note;


}





void check_assign(struct node *tree)
{
    //printf("\t\t[[[[tree->c1->type = %s,tree->c1->terminal = %s,tree->c1->note = %s,tree->c2->type = %s,tree->c2->terminal = %s,tree->c2->note = %s]]]]\n",  tree->c1->type, tree->c1->terminal, tree->c1->note, tree->c2->type, tree->c2->terminal, tree->c2->note);

    char *callback = NULL;
    char *res = NULL;
    if (tree->c1->note == NULL) {
        callback = search_global_array(tree->c1->type, func_name, 0);

        if (callback == NULL) {
            callback = "undef";
        }

        tree->c1->note = to_lower(callback);
    }

    if (tree->c2->note == NULL) {
        callback = search_global_array(tree->c2->type, func_name, 0);
        if (callback == NULL) {
            callback = "undef";
        }

        tree->c2->note = to_lower(callback);
    }

    /*  a = 123 */
    if (tree->c1->terminal) {
        tree->note = tree->c1->note;
    }

    if (tree->c1->terminal && tree->c2->terminal) {

        /*assign type is always the same as the type of the left operand*/
        tree->note = tree->c1->note;

        if (strcmp(tree->note, "bool") == 0) {
            tree->note = "boolean";
        }
        printf("%s - %s\n", tree->type, tree->note);
    }

    if (is_unary_operator(tree->c2->type)) {


        //printf("%s ", tree->type);
        check_plus_minus_not(tree);
        //res = check_plus_minus_not(tree);

        //printf("%s - %s\n", tree->type, res);

    }

    else if (is_operation_node(tree->c2->type)) {
        //printf("%s", tree->type);

        //printf("%s - %s", tree->type,tree->note);
        // printf("%s \\\\\\\\\\\\\\\\\\\\\\ ---------->%s<------------", tree->type,tree->note);

        //tree->note = check_oper_childs(tree);
        res = check_oper_childs(tree);
        printf("%s - %s\n", tree->type, res);
    }

    // parse args and length...
    else if (tree->c1->terminal && tree->c2->terminal == NULL) {

        callback = search_global_array(tree->c1->type, func_name, 0);
        tree->note = to_lower(callback);
        if (!strcmp(tree->note, "bool")) {
            tree->note = "boolean";
        }
        printf("%s - %s\n", tree->type, tree->note);
    }
}

int is_node_anottated(char *node_type)
{
    /* we're not supposed to print these type of nodes */
    return !(!strcmp(node_type, "Program") ||
             !strcmp(node_type, "VarDecl") ||
             !strcmp(node_type, "FieldDecl") ||
             !strcmp(node_type, "ParamDecl") ||
             !strcmp(node_type, "MethodHeader") ||
             !strcmp(node_type, "MethodParams") ||
             !strcmp(node_type, "StringArray"));
}

int is_binary_operator(char *bin_op)
{
    return (!strcmp(bin_op, "And") ||
            !strcmp(bin_op, "Or")  ||
            !strcmp(bin_op, "Eq")  ||
            !strcmp(bin_op, "Geq") ||
            !strcmp(bin_op, "Gt")  ||
            !strcmp(bin_op, "Leq") ||
            !strcmp(bin_op, "Lt")  ||
            !strcmp(bin_op, "Neq"));
}

int is_operation_node(char *oper)
{
    return (!strcmp(oper, "Sub") ||
            !strcmp(oper, "Add") ||
            !strcmp(oper, "Mul") ||
            !strcmp(oper, "Div") ||
            !strcmp(oper, "Mod"));
}

int is_unary_operator(char *un_op)
{
    return (!strcmp("Plus", un_op) ||
            !strcmp("Minus", un_op) ||
            !strcmp("Not", un_op));
}

// debug proposes
int _strcmp( char *s1, char *s2 )
{
    //printf("=====>[comparing %s with %s]<========\n", s1, s2);
    unsigned char *p1 = (unsigned char *)s1;
    unsigned char *p2 = (unsigned char *)s2;

    while (*p1 && *p1 == *p2) {
        ++p1, ++p2;
    }

    return (*p1 > *p2) - (*p2  > *p1);
}

void indent_tree(int n)
{
    int i;
    for (i = 0; i < n; i++) {
        printf("..");
    }
}

char *operators_result(char *first, char *second)
{
    if (!strcmp(first, second)) {
        return first;
    }

    if (!strcmp(first, "undef") || !strcmp(second, "undef")) {
        return "undef";
    }

    if ((!strcmp(first, "double") && !strcmp(second, "int")) ||
            (!strcmp(first, "int") && !strcmp(second, "double"))) {
        return "double";
    }

    if ((!strcmp(first, "int") && !strcmp(second, "boolean")) ||
            (!strcmp(first, "double") && !strcmp(second, "boolean")) ||
            (!strcmp(first, "boolean") && !strcmp(second, "int")) ||
            (!strcmp(first, "boolean") && !strcmp(second, "double"))) {
        return "undef";
    }
    return "undefined";
}