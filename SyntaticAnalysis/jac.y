%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

int yylex(void);
void yyerror(const char *);

extern char *yytext;
extern int yyleng;

extern int col;
extern int line;
extern int l_flag;
extern int one_flag;

int ast_flag;
int error_flag = 0;
char *type_var;
struct node *root;

%}

%union {
    char *str;
    struct node *node;
}

%token <str> ID 
%token <str> DECLIT
%token <str> REALLIT 
%token <str> STRLIT
%token <str> BOOLLIT

%token BOOL CLASS DO DOTLENGTH DOUBLE ELSE IF INT PARSEINT PRINT PUBLIC RETURN STATIC STRING VOID WHILE OCURV CCURV
%token OBRACE CBRACE OSQUARE CSQUARE AND OR LT GT EQ NEQ LEQ GEQ PLUS MINUS STAR DIV MOD NOT ASSIGN SEMI COMMA RESERVED 

%type <node> Program FieldDecl MethodDecl MethodHeader FormalParams FormalParamsTemp VarDecl Type StatementTemp Expr1
%type <node> MethodInvocation MethodInvocationTemp VarDeclTemp Assignment ParseArgs Expr MethodBody MBody IDext
%type <node> Statement Declarations    

%nonassoc THEN
%right ASSIGN
%left OR
%left AND
%left EQ NEQ
%left GEQ GT LEQ LT 
%left PLUS MINUS
%left STAR DIV MOD
%right NOT
%nonassoc ELSE

%%

Program: CLASS ID OBRACE Declarations CBRACE              {$$ = root = make_program_node($2,$4);}
  | CLASS ID OBRACE CBRACE                                {$$ = root = make_program_node($2,NULL);}
  ;

Declarations:  FieldDecl                                  {$$ = $1;}
  | MethodDecl                                            {$$ = $1;}
  | SEMI                                                  {$$ = NULL;}
  | Declarations FieldDecl                                {$$ = merge_declarations_xxx($1,$2);}
  | Declarations MethodDecl                               {$$ = merge_declarations_xxx($1,$2);}
  | Declarations SEMI                                     {$$ = $1;}
  ;

FieldDecl: PUBLIC STATIC Type ID SEMI                     {$$ = make_varDecl_node("FieldDecl",$3,$4);}
  | PUBLIC STATIC Type ID IDext SEMI                      {$$ = merge_declarations(make_varDecl_node("FieldDecl",$3,$4),$5);}
  | error SEMI                                            {$$ = make_type_node("Error");}
  ;

IDext: COMMA ID IDext                                     {$$ = merge_declarations(make_FieldDeclTemp_temp_node(type_var,$2),$3);}
  | COMMA ID                                              {$$ = make_FieldDeclTemp_temp_node(type_var, $2);}
  ;

MethodDecl: PUBLIC STATIC MethodHeader MethodBody         {$$ = make_method_decl_node($3,$4);}
  ;

MethodHeader: Type ID OCURV FormalParams CCURV            {$$ = make_method_header_node($1,$2,$4);}
  | Type ID OCURV CCURV                                   {$$ = make_method_header_empty_node($1,$2,"MethodParams");}
  | VOID ID OCURV FormalParams CCURV                      {$$ = make_method_header_node(NULL,$2,$4);}
  | VOID ID OCURV CCURV                                   {$$ = make_method_header_empty_node(NULL,$2,"MethodParams");}
  ;   

FormalParams: Type ID                                     {$$ = make_formal_params_type_node("MethodParams",$1,$2);}
  | Type ID FormalParamsTemp                              {$$ = make_formal_params_type_temp_node("MethodParams",$1,$2,$3);}
  | STRING OSQUARE CSQUARE ID                             {$$ = make_formal_params_node("StringArray",$4);}
  ;

FormalParamsTemp: COMMA Type ID FormalParamsTemp          {$$ = merge_declarations(make_formal_params_temp_node($2, $3),$4);}
| COMMA Type ID                                           {$$ = make_formal_params_temp_node($2,$3);}
;

MethodBody: OBRACE CBRACE                                 {$$ = make_type_node("MethodBody");}
 | OBRACE MBody CBRACE                                    {$$ = make_mbody_node("MethodBody",$2);}

MBody: VarDecl                                            {$$ = $1;}
  | Statement                                             {$$ = $1;}
  | MBody VarDecl                                         {$$ = merge_declarations_xxx($1,$2);}
  | MBody Statement                                       {$$ = merge_declarations_xxx($1,$2);}
  ;

VarDecl: Type ID SEMI                                     {$$ = make_varDecl_node("VarDecl",$1,$2);}
  | Type ID VarDeclTemp SEMI                              {$$ = merge_declarations(make_varDecl_node("VarDecl",$1,$2),$3);}
  ;

VarDeclTemp: COMMA ID VarDeclTemp                         {$$ = merge_declarations(make_varDeclTemp_temp_node(type_var, $2),$3);}
  |   COMMA ID                                            {$$ = make_varDeclTemp_temp_node(type_var, $2);}
  ;
Statement: OBRACE CBRACE                                  {$$ = NULL;}
  | OBRACE StatementTemp CBRACE                           {$$ = make_statement_temp_node($2);}
  | IF OCURV Expr1 CCURV Statement %prec THEN             {$$ = make_ifstatement_node("If",$3,$5,NULL);}
  | IF OCURV Expr1 CCURV Statement ELSE Statement         {$$ = make_ifstatement_node("If",$3,$5,$7);}
  | WHILE OCURV Expr1 CCURV Statement                     {$$ = make_whilestatement_node("While",$3,$5,NULL);}
  | DO Statement WHILE OCURV Expr1 CCURV SEMI             {$$ = make_do_whilestatement_node("DoWhile",$2,$5);}
  | PRINT OCURV Expr1 CCURV SEMI                          {$$ = make_statement_node("Print",$3,NULL,NULL);}
  | PRINT OCURV STRLIT CCURV SEMI                         {$$ = make_statement_node_str("Print",$3);}
  | Assignment SEMI                                       {$$ = $1;}
  | MethodInvocation SEMI                                 {$$ = $1;}
  | ParseArgs SEMI                                        {$$ = $1;}
  | SEMI                                                  {$$ = NULL;}
  | RETURN SEMI                                           {$$ = make_statement_node("Return",NULL,NULL,NULL);}
  | RETURN Expr1 SEMI                                     {$$ = make_statement_node("Return",$2,NULL,NULL);}
  | error SEMI                                            {$$ = make_type_node("Error");}
  ;

Assignment: ID ASSIGN Expr1                               {$$ = make_assignment_store_node($1,$3);}
  ;

MethodInvocation: ID OCURV CCURV                          {$$ = make_call_operator_node($1,NULL);}
  | ID OCURV Expr1 CCURV                                  {$$ = make_call_operator_node($1,$3);}
  | ID OCURV Expr1 MethodInvocationTemp CCURV             {$$ = make_call_operator_another_node($1,$3,$4);}
  | ID OCURV error CCURV                                  {$$ = make_type_node("Error");}
  ;

MethodInvocationTemp: COMMA Expr1                         {$$ = $2;}
  | MethodInvocationTemp COMMA Expr1                      {$$ = merge_declarations($1,$3);}
  ;

ParseArgs: PARSEINT OCURV ID OSQUARE Expr1 CSQUARE CCURV  {$$ = make_operator_node(make_terminal_node("Id",$3),"ParseArgs",$5);}
  | PARSEINT OCURV error CCURV                            {$$ = make_type_node("Error");}
  ;

StatementTemp: StatementTemp Statement                    {$$ = merge_stat_declarations($1,$2);}
  | Statement                                             {$$ = $1;}
  ;

Expr1: Assignment                                         {$$ = $1;}
 | Expr                                                   {$$ = $1;}

Expr: MethodInvocation                                    {$$ = $1;}
  | ParseArgs                                             {$$ = $1;}
  | Expr AND Expr                                         {$$ = make_operator_node($1,"And",$3);}
  | Expr OR Expr                                          {$$ = make_operator_node($1,"Or", $3);}
  | Expr EQ Expr                                          {$$ = make_operator_node($1,"Eq", $3);}
  | Expr GEQ Expr                                         {$$ = make_operator_node($1,"Geq",$3);}
  | Expr GT Expr                                          {$$ = make_operator_node($1,"Gt", $3);}
  | Expr LEQ Expr                                         {$$ = make_operator_node($1,"Leq",$3);}
  | Expr LT Expr                                          {$$ = make_operator_node($1,"Lt", $3);}
  | Expr NEQ Expr                                         {$$ = make_operator_node($1,"Neq",$3);}
  | Expr PLUS Expr                                        {$$ = make_operator_node($1,"Add",$3);}
  | Expr MINUS Expr                                       {$$ = make_operator_node($1,"Sub",$3);}
  | Expr STAR Expr                                        {$$ = make_operator_node($1,"Mul",$3);}
  | Expr DIV Expr                                         {$$ = make_operator_node($1,"Div",$3);}
  | Expr MOD Expr                                         {$$ = make_operator_node($1,"Mod",$3);}
  | PLUS Expr %prec NOT                                   {$$ = make_operator_node(NULL,"Plus",$2);;}
  | MINUS Expr %prec NOT                                  {$$ = make_operator_node(NULL,"Minus",$2);;}
  | NOT Expr                                              {$$ = make_operator_node(NULL,"Not",$2);;}
  | BOOLLIT                                               {$$ = make_terminal_node("BoolLit",$1);;}
  | DECLIT                                                {$$ = make_terminal_node("DecLit", $1);;}
  | REALLIT                                               {$$ = make_terminal_node("RealLit",$1);}
  | ID                                                    {$$ = make_terminal_node("Id",$1);}
  | ID DOTLENGTH                                          {$$ = make_length_node($1);}
  | OCURV Expr1 CCURV                                     {$$ = $2;}
  | OCURV error CCURV                                     {$$ = make_type_node("Error");}
  ;

Type: BOOL                                                {$$ = make_terminal_node("Bool",NULL);type_var = "Bool";}
  | DOUBLE                                                {$$ = make_terminal_node("Double",NULL);type_var = "Double";}
  | INT                                                   {$$ = make_terminal_node("Int",NULL);type_var = "Int";}
  ;

%%

void yyerror(const char *err)
{
    error_flag = 1;
    printf("Line %d, col %lu: %s: %s\n",line,(col-strlen(yytext)),err,yytext);
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        if (!strcmp(argv[1],"-l")) {
            l_flag = 1;
        } else if (!strcmp(argv[1],"-t")) {
            ast_flag = 1;      
        }  else if (!strcmp(argv[1],"-1")) {
            yylex();
            return 0;
        }
    }

    if (!l_flag) {
        yyparse();
        yylex();
    }
    else 
        yylex();

    if (ast_flag && !error_flag)
        print_ast(root,0);

    return 0;
}
