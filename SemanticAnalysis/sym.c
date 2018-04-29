#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sym.h"

char all_funcs[256][256];

char all_nodes[36][16] = {
    "eq",
    "gt",
    "if",
    "lt",
    "or",
    "add",
    "and",
    "div",
    "geq",
    "leq",
    "mod",
    "mul",
    "neq",
    "not",
    "sub",
    "call",
    "plus",
    "block",
    "minus",
    "length",
    "print",
    "while",
    "assign",
    "assign",
    "return",
    "dowhile",
    "program",
    "vardecl",
    "fielddecl",
    "paramdecl",
    "parseargs",
    "methodbody",
    "methoddecl",
    "methodheader",
    "methodparams",
};


struct lookup_array global_array[1024];

int global_array_stack_pointer = 0;

char *__func_name_initial__;

int sp = 0;

int check_node(char *type)
{
    char types[5][10] = {
        "Void", "String[]", "Double", "Int", "Bool",
    };

    int i;
    for (i = 0; i < 5; i++) {
        if (!strcmp(types[i], type)) {
            return 1;
        }
    }
    return 0;
}

int check_all_nodes(char *type)
{
    int i;
    for (i = 0; i < 35; i++) {
        if (!strcmp(all_nodes[i], type)) {
            return 1;
        }
    }
    return 0;
}

struct sym *make_node(char *type, char *name)
{
    struct sym* new_sym = malloc(sizeof(struct sym));
    new_sym->element_type = type;
    new_sym->name = name;
    new_sym->type = "undef";
    new_sym->flag = 0;
    new_sym->next = NULL;
    new_sym->table_method = NULL;
    new_sym->method_body = NULL;
    return new_sym;
}

struct sym *make_table(char *type, char *name)
{
    struct sym *table = make_node(type, name);
    return table;
}

struct sym *make_variable(char *name, char *type)
{
    struct sym *variable = make_node("VAR", name);
    variable->type = type;
    return variable;
}

struct sym *make_method(char *name, struct node * new_method)
{
    struct sym *method = make_node("METHOD", name);
    method->method_body = new_method->c2;
    return method;
}

void add_element_to_symbol_table(struct sym *table, struct sym *ele)
{
    struct sym *temp = table;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = ele;
}

char *search_table(struct sym *table, char *function_name, char *what_to_look)
{
    table->name = get_function_name(table->name);

    struct sym *table_copy = table->next;
    table = table->next;
    while (table_copy) {

        printf("table->name = %s function_name =  %s table_copy->name =  %s what_to_look =  %s\n", table->name, function_name, table_copy->name, what_to_look);
        //printf("table_copy->element_type = %s | table->name = %s | table_copy->name = %s | table_copy->type = %s
        //>>>>>>> searching for %s in function %s\n\n  ",table_copy->element_type, table->name,table_copy->name,
        //to_lower(table_copy->type), what_to_look, function_name);
        if (!strcmp(table->name, function_name) && !strcmp(table_copy->name, what_to_look)) {
            // printf("--------------------------------------------------------------------------------------------FOUND\n");
            return table_copy->type;
        }

        table_copy = table_copy->next;
        table = table->next;
    }

    // variable not found
    return NULL;
}

struct search_table *init_search_table()
{
    struct search_table* new_sym = malloc(sizeof(struct search_table));
    new_sym->element_type = NULL;
    new_sym->name = NULL;
    new_sym->type = NULL;
    new_sym->next = NULL;
    new_sym->table_method = NULL;
    new_sym->method_body = NULL;
    return new_sym;
}

void test_thing()
{
    int i;
    for (i = 0; i < global_array_stack_pointer; ++i) {
        printf("global_array[%d].name = %s\n", i, global_array[i].name);
        printf("global_array[%d].type = %s\n", i, global_array[i].type);
        printf("global_array[%d].scope = %s\n", i, global_array[i].scope);
        puts("");
    }
}

char *get_function_name(char *str)
{
    if (!str) {
        return NULL;
    }
    int pos = 0;
    int count = 0;
    char *ret = NULL;
    int flag = 0;
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != '(') {
            count++;
        } else {
            flag = 1;
            break;
        }
    }

    if (flag == 0) {
        return str;
    }

    ret = malloc(count + 1);

    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != '(') {
            ret[i] = str[i];
        } else {
            pos = i;
            break;
        }
    }

    ret[pos] = '\0';
    return ret;
}

char *search_global_scope(char *variable_name)
{
    int i;
    for (i = 0; i < global_array_stack_pointer; ++i) {
        if (strcmp(global_array[i].scope, "__CLASS__") == 0) {
            //printf("comparing variable_name = %s, with global_array[i].name = %s\n", variable_name, global_array[i].name);
            if (strcmp(global_array[i].name, variable_name) == 0) {
                //printf("returning: %s\n", global_array[i].type);
                if (valid_types(global_array[i].type)) {
                    return global_array[i].type;
                } else {
                    return NULL;
                }
            }
        } else {
            //printf("global_array[i].scope = %s\n", global_array[i].scope);
        }
    }
    return NULL;
}

int valid_types(char *str)
{
    return !strcmp(str, "Int") || !strcmp(str, "Double") ||
           !strcmp(str, "Bool") || !strcmp(str, "String[]");
}

char *search_global_array(char *variable_name, char *function_name, int flag)
{
    char *func_name = NULL;
    int i;

    //printf(":: [searching %s in %s] ", variable_name, function_name);

    int find = 0;
    // searching locallly first
    for (i = 0; i < global_array_stack_pointer; ++i) {
        func_name = get_function_name(global_array[i].scope);
        if (strcmp(global_array[i].scope, "__CLASS__") != 0 && strcmp(func_name, function_name) == 0) { // && global_array[i].isMethod == 0 && find == 1) {

            // printf("0==========?=?=?=?=?=?=?\n");
            if (strcmp(global_array[i].name, variable_name) == 0 && strcmp(func_name, function_name) == 0) {
                if (valid_types(global_array[i].type)) {
                    //printf("----------------------------> FOUDDSDD %s\n", global_array[i].type);
                    return global_array[i].type;
                } else {
                    return NULL;
                }
            }
        } else if (global_array[i].isMethod == 1 && to_local_search != NULL) {

            find = 0;
            char teste[256];

            //to_local_search ="";

            //printf("->>>>>>> %s\n",to_local_search);

            if (!strcmp(to_local_search, ")")) {
                to_local_search = "()";

            }

            strcpy(teste, function_name);
            strcat(teste, to_local_search);

            //printf("->>>>>!>> %s\n",teste);

            if (!strcmp(teste, global_array[i].name)) {
                find = 1;
            }

            //printf("METHOD - >>  var name = %s --> find %d\n", global_array[i].name,find);
        }
    }
    //}

    //if (flag == 2 && flag != 1) {

    // then globally
    for (i = 0; i < global_array_stack_pointer; ++i) {
        if (strcmp(global_array[i].scope, "__CLASS__") == 0) {
            //printf("[GLOBAL] comparing variable_name = %s, with global_array[i].name = %s with type = %s in function %s\n", variable_name, global_array[i].name,global_array[i].type,func_name);
            if (strcmp(global_array[i].name, variable_name) == 0) {
                //printf("----------------------------> FOUDDSDD %s\n",global_array[i].type);
                if (valid_types(global_array[i].type)) {
                    // printf("----------------------------> FOUDDSDD %s\n", global_array[i].type);

                    return global_array[i].type;
                } else {
                    return NULL;
                }
            }
        }
    }
    //}

    // nothing found
    return NULL;
}

void print_global_lookup_table(struct sym *root2)
{

    global_lookup_table = NULL;
    global_array_stack_pointer = 0;

    struct sym *__temp1;
    struct sym *__temp2;
    struct sym *__temp3;

    if (global_lookup_table == NULL) {
        global_lookup_table = init_search_table();
    }

    global_lookup_table->name = root2->name;
    global_lookup_table->element_type = root2->element_type;

    global_lookup_table->next = init_search_table();

    __temp1 = root2->next;
    while (__temp1) {


        if (!strcmp(__temp1->element_type, "VAR")) {
            //printf("var name = %s, var type = %s\n", __temp1->name, __temp1->type);
            global_lookup_table->name = __temp1->name;
            global_lookup_table->type = __temp1->type;
            //printf("glob var = %s, glob var = %s\n", global_lookup_table->name, global_lookup_table->type);

            global_array[global_array_stack_pointer].name = global_lookup_table->name;
            global_array[global_array_stack_pointer].type = global_lookup_table->type;
            global_array[global_array_stack_pointer].scope = "__CLASS__";
            global_array[global_array_stack_pointer].isMethod = 0;
            global_array_stack_pointer++;



        }

        global_lookup_table->next = init_search_table();

        __temp1 = __temp1->next;
    }

    __temp2 = root2;

    char *variable_scope = malloc(128);

    while (__temp2) {

        if (!strcmp(__temp2->element_type, "METHOD")) {

            //printf("var name = %s\n", __temp2->table_method->name);
            //printf("__temp2->table_method->name = %s\n", __temp2->table_method->name);
            global_lookup_table->table_method = init_search_table();
            global_lookup_table->table_method->name = __temp2->table_method->name;
            // printf("global_lookup_table->table_method->name = %s\n", global_lookup_table->table_method->name);

            global_array[global_array_stack_pointer].name = global_lookup_table->table_method->name;
            global_array[global_array_stack_pointer].type = global_lookup_table->table_method->name;
            global_array[global_array_stack_pointer].scope = global_lookup_table->table_method->name;
            global_array[global_array_stack_pointer].isMethod = 1;
            global_array_stack_pointer++;

            variable_scope = global_lookup_table->table_method->name;


            __temp3 = __temp2->table_method->next;

            global_lookup_table->table_method->next = init_search_table();

            while (__temp3) {

                //printf("__temp3->next = %s\n", __temp3->name);
                global_lookup_table->table_method->name = __temp3->name;
                global_lookup_table->table_method->type = __temp3->type;

                /// printf("global_lookup_table->table_method->name = %s, type = %s\n", global_lookup_table->table_method->name, global_lookup_table->table_method->type);
                global_array[global_array_stack_pointer].name = global_lookup_table->table_method->name;
                global_array[global_array_stack_pointer].type = global_lookup_table->table_method->type;
                global_array[global_array_stack_pointer].scope = variable_scope;
                global_array[global_array_stack_pointer].isMethod = 0;
                global_array_stack_pointer++;

                global_lookup_table->next = init_search_table();
                __temp3 = __temp3->next;
            }

        }

        __temp2 = __temp2->next;
    }



}

void print_element(struct sym *ele, struct node *root)
{
    char *type = ele->type;
    char *low = NULL;
    int allowed = 0;


    if (type == NULL) {
        return;
    }

    type = to_lower(type);

    if (!__strcmp(ele->element_type, "VAR")) {

        if (check_node(ele->type)) {
            printf("%s", ele->name);
            allowed = 1;
        }

        if (!__strcmp(type, "bool")) {
            printf("\t\tboolean");
        } else if (!__strcmp(type, "string[]")) {
            printf("\t\tString[]");
        } else {
            if (allowed == 1) {
                printf("\t\t%s", type);
            }
        }

        if (ele->flag == 1) {
            printf("\tparam");
        }

        if (allowed) {
            puts("");
        }
    } else if (!__strcmp(ele->element_type, "METHOD")) {

        int i;
        int pos = 0;
        int flag = 0;
        int index = 0;

        char func_id[128];
        char func_pars[128];

        for (i = 0; ele->table_method->name[i]; ++i) {
            if (ele->table_method->name[i] != '(') {
                printf("%c", ele->table_method->name[i]);
                func_id[i] = ele->table_method->name[i];
            } else {
                flag = 1;
                pos = i;
                printf("\t");
                break;
            }
        }


        func_id[pos] = '\0';
        /*global_lookup_table->table_method->name = func_id;*/


        for (i = pos; ele->table_method->name[i]; ++i) {
            if (flag == 1) {
                printf("%c", ele->table_method->name[i]);
                func_pars[index] = ele->table_method->name[i];
                ++index;
            }
        }

        func_pars[index] = '\0';

        if (!__strcmp(ele->table_method->next->type, "String[]")) {
            low = ele->table_method->next->type;
        } else {
            low = to_lower(ele->table_method->next->type);
        }

        if (!__strcmp(low, "bool")) {
            printf("\tboolean\n");
        } else {
            printf("\t%s\n", low);
        }

        strcpy(all_funcs[sp], low);
        sp++;
        strcpy(all_funcs[sp], func_id);
        sp++;
        strcpy(all_funcs[sp], func_pars);
        sp++;
        strcpy(all_funcs[sp], "-1"); /*  to indicate the end*/

    }
}

void print_table(struct sym *table, struct node *root)
{

    struct sym *temp;

    if (!__strcmp(table->element_type, "CLASS")) {
        printf("===== Class %s Symbol Table =====\n", table->name);



    } else if (!__strcmp(table->element_type, "METHOD_TABLE")) {
        printf("===== Method %s Symbol Table =====\n", table->name);


    }


    temp = table->next;
    while (temp) {
        print_element(temp, root);
        temp = temp->next;
    }




    // search_table(table, "funcao2", "global2");
    // root2 -> symbol table
    // root-> node table
    //search_table(root2, root);


    //  search_variable("tttt", "bar");
}

int check_symbol(struct sym *table, char *name)
{
    struct sym *temp = table->next;

    while (temp) {
        if (__strcmp(temp->element_type, "CLASS") && !__strcmp(name, temp->name)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

int is_symbol_in_table(struct sym *table, struct node *var, int option)
{
    if (option == 1) {
        if (check_symbol(table, var->c2->type)) {
            printf("Line %d, col %d: Symbol %s already defined\n", var->line, var->col, var->c2->type);
            return 1;
        }
    } else if (option == 2) {
        if (check_symbol(table, var->type)) {
            printf("Line %d, col %d: Symbol %s already defined\n", var->line, var->col, var->type);
            return 1;
        }
    }
    return 0;
}

void add_param_decl(struct sym *root, struct node *vardecl)
{
    struct sym *temp;

    struct node *current = vardecl->c1;
    struct node *dummy = vardecl->c2;

    if (current) {

        //is_symbol_in_table(root, current, 1);

        temp = make_variable(current->c2->type, NULL);
        if (current->c1->terminal == NULL) {

            temp->type = "String[]";
            //current->note = "String[]";
        } else {
            temp->type = current->c1->terminal;
            //current->note = current->c1->terminal;
        }
        temp->flag = 1;
        add_element_to_symbol_table(root, temp);

        /*if (current->c1->terminal == NULL) {
            current->note = "Stringggggggg[]";
        }*/

        //printf("current->c2->type = %s current->c1->terminal = %s\n",current->c2->type, current->c1->terminal  );
        current = current->next;

        if (! current) {
            while (dummy) {
                //is_symbol_in_table(root, dummy, 1);

                //dummy->note = dummy->c1->terminal;
                //printf("dummy->c2->type = %s, dummy->c1->terminal = %s dummy->note = %s\n",dummy->c2->type, dummy->c1->terminal,dummy->note );
                temp = make_variable(dummy->c2->type, dummy->c1->terminal);
                temp->flag = 1;
                add_element_to_symbol_table(root, temp);

                dummy = dummy->next;
            }
        }
    }
}

void add_vardecls(struct sym *root, struct node *vardecl)
{
    struct node *current;
    struct node *current_var;
    struct sym *temp;

    char *var_type;

    current = vardecl->c1; // vardecl

    while (current) {

        current_var = current->c2; // statement

        if (!__strcmp(current->type, "Return")) {
            return;
        }
        var_type = current->c1->terminal; // int, double, bool

        while (current_var) {
            // avoid checking unnecessary nodes

            if (!check_all_nodes(current_var->type)) {

                //if (is_symbol_in_table(root, current->c2, 2) == 0) {
                //printf("[[[[[[[[[[[[[[[[[ adding %s of type %s\n", current_var->type, var_type);
                temp = make_variable(current_var->type, var_type);

                add_element_to_symbol_table(root, temp);
                //}

            }
            current_var = current_var->next;
        }
        current = current->next;
    }
}

struct sym *make_table_method(struct node *method, int flag)
{
    char *type;
    struct sym *root;
    struct sym *temp;
    struct sym *lamb;

    root = make_table("METHOD_TABLE", method->c1->c2->type);
    temp = make_variable("return", method->c1->c1->terminal);

    add_element_to_symbol_table(root, temp);

    add_param_decl(root, method->c1->c3);

    if (flag == 1) {
        add_vardecls(root, method->c2);
    }

    lamb = root;

    char *arguments_to_print_in_table = malloc(256); //Lol
    memset(arguments_to_print_in_table, 0, 256);
    strcpy(arguments_to_print_in_table, lamb->name);
    strcat(arguments_to_print_in_table, "(");

    while (lamb) {
        if (lamb->flag == 1) {
            if (!__strcmp(lamb->type, "String[]")) {
                type = lamb->type;
            } else {
                type = to_lower(lamb->type);
            }
            if (!__strcmp(type, "bool")) {
                strcat(arguments_to_print_in_table, "boolean");
            } else {
                strcat(arguments_to_print_in_table, type);
            }
            if (lamb->next && lamb->next->flag == 1) {
                strcat(arguments_to_print_in_table, ",");
            }
        }
        lamb = lamb->next;
    }
    strcat(arguments_to_print_in_table, ")");
    root->name = arguments_to_print_in_table;
    return root;
}

struct sym *parse_ast(struct node * root, int flag)
{
    struct node *current_node;
    struct node *current_method;

    struct sym *current;
    struct sym *method_symbol_table;
    struct sym *main_table;

    if (!root) {
        return NULL;
    }
    main_table = make_table("CLASS", root->c1->type);
    current_node = root->c2;// methodDecl

    while (current_node) {
        if (!__strcmp(current_node->type, "FieldDecl")) {
            //printf("------> %s %s\n", current_node->c2->type, current_node->c1->terminal);
            //current_node->c2->func_name = "GLOBAL_SCOPE";
            current = make_variable(current_node->c2->type, current_node->c1->terminal);
            //is_symbol_in_table(root, current_node->c2, 2);
            //printf("ADDING %s TO TABLE\n", current->name);
            //if (is_symbol_in_table(main_table, current_node->c2, 2) == 0) {
            add_element_to_symbol_table(main_table, current);
            //}
        }

        else if (!__strcmp(current_node->type, "MethodDecl")) {

            current_method = current_node->c1;

            // make a method with the name "main"
            current = make_method(current_method->c2->type, current_node);

            //printf("passing to function as argument_::: :: : : : :: : : : %s\n", current_node->type);
            method_symbol_table = make_table_method(current_node, flag);

            current->table_method = method_symbol_table;

            add_element_to_symbol_table(main_table, current);

        }
        current_node = current_node->next;
    }
    //root2 = main_table;
    return main_table;
}

char *to_lower(char *str)
{
    size_t i;
    size_t length = strlen(str);

    char* lower = malloc(length + 1);
    lower[length] = '\0';

    for (i = 0; i < length; i++ ) {
        lower[i] = tolower(str[i]);
    }

    return lower;
}

void analyze_ast_semantic(struct node *node, struct sym *class_table, struct sym *curr_method_table)
{
    struct node *temp = node;


    while (temp != NULL) {


        if (strcmp(temp->type, "Block")) {
            // printf("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk %s\n", temp->type);
            //analyze_ast(temp->c1, class_table, curr_method_table);
        }

        temp = temp->next;
    }
}


// check ast for semantic errors
void check_semantics(struct node *root, struct sym *class)
{
    if (!root || !class) {
        return;
    }
    struct sym *temp = class;

    // printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\   %s\n", temp->next->name);

    while (temp) {
        if (!strcmp(temp->element_type, "METHOD")) {


            //printf("============> method: %s,  %s\n", temp->name, temp->method_body->c1->type);
            analyze_ast_semantic(temp->method_body->c1, class, temp->table_method);
        }
        temp = temp->next;
    }
}


// debug proposes
int __strcmp( char *s1, char *s2 )
{
    printf("=====>[comparing %s with %s]<========\n", s1, s2);
    unsigned char *p1 = (unsigned char *)s1;
    unsigned char *p2 = (unsigned char *)s2;

    while (*p1 && *p1 == *p2) {
        ++p1, ++p2;
    }

    return (*p1 > *p2) - (*p2  > *p1);
}

