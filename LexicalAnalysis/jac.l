%{
	#include <string.h>

	#define PRINT_TOKEN(SYM)     do {col+=yyleng; if (cmd_flag) puts(SYM);} while (0)
	#define PRINT_TOKEN_LIT(SYM) do {col+=yyleng; if (cmd_flag) printf("%s(%s)\n",SYM,yytext);} while (0)

	int col = 1;
	int colComment = 1;
	int lineComment = 1;
	int colString = 1;
	int lineString = 1;
	int cmd_flag = 0;
	int line = 1;
%}

%x TRADITIONAL_COMMENT LAST EOL_COMMENT

letter [a-z]|[A-Z]|"_"|"$"
digit [0-9]
ID {letter}({letter}|{digit})*

REAL_OPTION1 {EXPRESSION}"."{EXPRESSION}*({EXPONENT})? 
REAL_OPTION2 ({EXPRESSION}{EXPONENT})
REAL_OPTION3 "."{EXPRESSION}({EXPONENT})?
EXPRESSION   ("_"*{digit}+)+
EXPONENT	 [eE][+-]?{EXPRESSION}

DECLIT  0|[1-9]+(_*[0-9]+)*
REALLIT ({REAL_OPTION1}|{REAL_OPTION2}|{REAL_OPTION3})
STRLIT  \"(\\[\\\"fnrt]|[^\n\r\"\\])*\"

%%

\"                     {BEGIN LAST;lineString=line;colString=col;col+=yyleng;}
<LAST>"\n"|"\r"|"\r\n" {printf("Line %d, col %d: unterminated string literal\n",lineString,colString); line++; col=1; BEGIN INITIAL;}
<LAST><<EOF>>          {printf("Line %d, col %d: unterminated string literal\n",lineString,colString); col=1; line++; BEGIN INITIAL;}
<LAST>\"               {col+=yyleng;BEGIN INITIAL;}
<LAST>\\f|\\t|\\n|\\r  {col+=yyleng;}
<LAST>\\\\|\\\"        {col+=yyleng;}
<LAST>\\.|\\           {printf("Line %d, col %d: invalid escape sequence (%s)\n",line,col,yytext);col += yyleng;}
<LAST>.                {col+=yyleng;};

"/*"                                  {BEGIN TRADITIONAL_COMMENT;lineComment=line;colComment=col;col+=yyleng;}
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
    PRINT_TOKEN_LIT("RESERVED");
}
"finally"|"float"|"for"|"goto"|"implements"|"import"|"instanceof"|"interface"|"long"|"native"|"new"|"null"|"package" {
    PRINT_TOKEN_LIT("RESERVED");
}
"private"|"protected"|"short"|"strictfp"|"super"|"switch"|"synchronized"|"this"|"throw"|"throws"|"transient"|"try" {
    PRINT_TOKEN_LIT("RESERVED");
}
"volatile"|"--"|"System"|"Integer" {
    PRINT_TOKEN_LIT("RESERVED");
}

"boolean"            {PRINT_TOKEN("BOOL");}
"true"               {PRINT_TOKEN_LIT("BOOLLIT");}
"false"              {PRINT_TOKEN_LIT("BOOLLIT");}
"class"              {PRINT_TOKEN("CLASS");}
"do"                 {PRINT_TOKEN("DO");}
".length"            {PRINT_TOKEN("DOTLENGTH");}
"double"             {PRINT_TOKEN("DOUBLE");}
"else"               {PRINT_TOKEN("ELSE");}
"if"                 {PRINT_TOKEN("IF");}
"int"                {PRINT_TOKEN("INT");}
"Integer.parseInt"   {PRINT_TOKEN("PARSEINT");}
"System.out.println" {PRINT_TOKEN("PRINT");}
"public"             {PRINT_TOKEN("PUBLIC");}
"return"             {PRINT_TOKEN("RETURN");}
"static"             {PRINT_TOKEN("STATIC");}
"String"             {PRINT_TOKEN("STRING");}
"void"               {PRINT_TOKEN("VOID");}
"while"              {PRINT_TOKEN("WHILE");}
"("                  {PRINT_TOKEN("OCURV");}
")"                  {PRINT_TOKEN("CCURV");}
"{"                  {PRINT_TOKEN("OBRACE");}
"}"                  {PRINT_TOKEN("CBRACE");}
"["                  {PRINT_TOKEN("OSQUARE");}
"]"                  {PRINT_TOKEN("CSQUARE");}
"&&"                 {PRINT_TOKEN("AND");}
"||"                 {PRINT_TOKEN("OR");}
"<"                  {PRINT_TOKEN("LT");}
">"                  {PRINT_TOKEN("GT");}
"=="                 {PRINT_TOKEN("EQ");}
"!="                 {PRINT_TOKEN("NEQ");}
"<="                 {PRINT_TOKEN("LEQ");}
">="                 {PRINT_TOKEN("GEQ");}
"+"                  {PRINT_TOKEN("PLUS");}
"-"                  {PRINT_TOKEN("MINUS");}
"*"                  {PRINT_TOKEN("STAR");}
"/"                  {PRINT_TOKEN("DIV");}
"%"                  {PRINT_TOKEN("MOD");}
"!"                  {PRINT_TOKEN("NOT");}
"="                  {PRINT_TOKEN("ASSIGN");}
";"                  {PRINT_TOKEN("SEMI");}
","                  {PRINT_TOKEN("COMMA");}
{ID}                 {PRINT_TOKEN_LIT("ID");}
{DECLIT}             {PRINT_TOKEN_LIT("DECLIT");}
{REALLIT}            {PRINT_TOKEN_LIT("REALLIT");}
{STRLIT}             {PRINT_TOKEN_LIT("STRLIT");}

[ \t\f]              {col+=yyleng;}
\n|\r|\r\n           {col=1;line++;}

.                    {printf("Line %d, col %d: illegal character (%s)\n",line,col,yytext);col+=yyleng;}

%%
int main(int argc, char **argv)
{
	if (argc >= 2) {
		cmd_flag = (!strcmp(argv[1], "-1") || !strcmp(argv[1], "-l"));
	}
	yylex();
	return 0;
}

int yywrap()
{
	return 1;
}
