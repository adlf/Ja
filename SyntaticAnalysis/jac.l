%{
    #include <string.h>
    #include "y.tab.h"

    /* flags */
    int l_flag = 0;
    int one_flag = 0;

    int col = 1;
    int line = 1;
    int colString = 1;
    int lineString = 1;
    int colComment = 1;
    int lineComment = 1;

    #define PRINT_TOKEN(SYM,RET) \
        do {col+=yyleng; if (l_flag == 1) {puts(SYM);} else return RET;} while (0)
    
    #define PRINT_TOKEN_LIT(SYM,RET) \
        do {col+=yyleng; if (l_flag == 1) printf("%s(%s)\n",SYM,yytext); else {yylval.str=strdup(yytext); return RET;}} while (0)
%}

%x TRADITIONAL_COMMENT LAST EOL_COMMENT

letter [a-z]|[A-Z]|"_"|"$"
digit [0-9]
ID {letter}({letter}|{digit})*

REAL_OPTION1 {EXPRESSION}"."{EXPRESSION}*({EXPONENT})? 
REAL_OPTION2 ({EXPRESSION}{EXPONENT})
REAL_OPTION3 "."{EXPRESSION}({EXPONENT})?
EXPRESSION   ("_"*{digit}+)+
EXPONENT     [eE][+-]?{EXPRESSION}

DECLIT  0|[1-9]+(_*[0-9]+)*
REALLIT ({REAL_OPTION1}|{REAL_OPTION2}|{REAL_OPTION3})
STRLIT  \"(\\[\\\"fnrt]|[^\n\r\"\\])*\"

%%

\"                     {BEGIN LAST; lineString=line; colString=col; col+=yyleng;}
<LAST>"\n"|"\r"|"\r\n" {line++; col=1; printf("Line %d, col %d: unterminated string literal\n",lineString,colString); BEGIN INITIAL;}
<LAST><<EOF>>          {line++; col=1; printf("Line %d, col %d: unterminated string literal\n",lineString,colString); BEGIN INITIAL;}
<LAST>\"               {col+=yyleng; BEGIN INITIAL;}
<LAST>\\f|\\t|\\n|\\r  {col+=yyleng;}
<LAST>\\\\|\\\"        {col+=yyleng;}
<LAST>\\.|\\           {printf("Line %d, col %d: invalid escape sequence (%s)\n",line,col,yytext); col+=yyleng; }
<LAST>.                {col+=yyleng;}

"/*"                                  {BEGIN TRADITIONAL_COMMENT; lineComment=line; colComment=col; col+=yyleng;}
<TRADITIONAL_COMMENT><<EOF>>          {printf("Line %d, col %d: unterminated comment\n",lineComment,colComment);BEGIN INITIAL;}
<TRADITIONAL_COMMENT>"\n"|"\r"|"\r\n" {line++;col=1;}
<TRADITIONAL_COMMENT>"\f"             {col+=yyleng;}
<TRADITIONAL_COMMENT>" "              {col+=yyleng;}
<TRADITIONAL_COMMENT>"*/"             {BEGIN INITIAL;col+=yyleng;}
<TRADITIONAL_COMMENT>.                {col+=yyleng;}

"//"                          {BEGIN EOL_COMMENT; lineComment=line;colComment=col;col+=yyleng;}
<EOL_COMMENT>"\n"|"\r"|"\r\n" {BEGIN INITIAL;col=1; line++;}
<EOL_COMMENT>.                {col++;}


"abstract"|"assert"|"break"|"byte"|"case"|"catch"|"char"|"const"|"continue"|"default"|"enum"|"extends"|"final"|"++" {
    PRINT_TOKEN_LIT("RESERVED",RESERVED);
}
"finally"|"float"|"for"|"goto"|"implements"|"import"|"instanceof"|"interface"|"long"|"native"|"new"|"null"|"package" {
    PRINT_TOKEN_LIT("RESERVED",RESERVED);
}
"private"|"protected"|"short"|"strictfp"|"super"|"switch"|"synchronized"|"this"|"throw"|"throws"|"transient"|"try" {
    PRINT_TOKEN_LIT("RESERVED",RESERVED);
}
"volatile"|"--"|"System"|"Integer" {
    PRINT_TOKEN_LIT("RESERVED",RESERVED);
}

"boolean"            {PRINT_TOKEN("BOOL",BOOL);}
"true"               {PRINT_TOKEN_LIT("BOOLLIT",BOOLLIT);}
"false"              {PRINT_TOKEN_LIT("BOOLLIT",BOOLLIT);}
"class"              {PRINT_TOKEN("CLASS",CLASS);}
"do"                 {PRINT_TOKEN("DO",DO);}
".length"            {PRINT_TOKEN("DOTLENGTH",DOTLENGTH);}
"double"             {PRINT_TOKEN("DOUBLE",DOUBLE);}
"else"               {PRINT_TOKEN("ELSE",ELSE);}
"if"                 {PRINT_TOKEN("IF",IF);}
"int"                {PRINT_TOKEN("INT",INT);}
"Integer.parseInt"   {PRINT_TOKEN("PARSEINT",PARSEINT);}
"System.out.println" {PRINT_TOKEN("PRINT",PRINT);}
"public"             {PRINT_TOKEN("PUBLIC",PUBLIC);}
"return"             {PRINT_TOKEN("RETURN",RETURN);}
"static"             {PRINT_TOKEN("STATIC",STATIC);}
"String"             {PRINT_TOKEN("STRING",STRING);}
"void"               {PRINT_TOKEN("VOID",VOID);}
"while"              {PRINT_TOKEN("WHILE",WHILE);}
"("                  {PRINT_TOKEN("OCURV",OCURV);}
")"                  {PRINT_TOKEN("CCURV",CCURV);}
"{"                  {PRINT_TOKEN("OBRACE",OBRACE);}
"}"                  {PRINT_TOKEN("CBRACE",CBRACE);}
"["                  {PRINT_TOKEN("OSQUARE",OSQUARE);}
"]"                  {PRINT_TOKEN("CSQUARE",CSQUARE);}
"&&"                 {PRINT_TOKEN("AND",AND);}
"||"                 {PRINT_TOKEN("OR",OR);}
"<"                  {PRINT_TOKEN("LT",LT);}
">"                  {PRINT_TOKEN("GT",GT);}
"=="                 {PRINT_TOKEN("EQ",EQ);}
"!="                 {PRINT_TOKEN("NEQ",NEQ);}
"<="                 {PRINT_TOKEN("LEQ",LEQ);}
">="                 {PRINT_TOKEN("GEQ",GEQ);}
"+"                  {PRINT_TOKEN("PLUS",PLUS);}
"-"                  {PRINT_TOKEN("MINUS",MINUS);}
"*"                  {PRINT_TOKEN("STAR",STAR);}
"/"                  {PRINT_TOKEN("DIV",DIV);}
"%"                  {PRINT_TOKEN("MOD",MOD);}
"!"                  {PRINT_TOKEN("NOT",NOT);}
"="                  {PRINT_TOKEN("ASSIGN",ASSIGN);}
";"                  {PRINT_TOKEN("SEMI",SEMI);}
","                  {PRINT_TOKEN("COMMA",COMMA);}
{ID}                 {PRINT_TOKEN_LIT("ID",ID);}
{DECLIT}             {PRINT_TOKEN_LIT("DECLIT",DECLIT);}
{REALLIT}            {PRINT_TOKEN_LIT("REALLIT",REALLIT);}
{STRLIT}             {PRINT_TOKEN_LIT("STRLIT",STRLIT);}

[ \t\f]              {col+=yyleng;}
\n|\r|\r\n           {col=1;line++;}

.                    {printf("Line %d, col %d: illegal character (%s)\n",line,col,yytext);col+=yyleng; }

%%

int yywrap()
{
    return 1;
}
