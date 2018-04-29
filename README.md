
# Dependencies
* Lex (A Lexical Analyzer Generator )
* Yacc (Yet Another Compiler-Compiler)
* Clang 3.8 compiler


#### Lexical Analysis
```
lex jac.l
clang-3.8 lex.yy.c -o jac

```

#### Syntatic Analysis
```
lex jac.l
yacc -d jac.y
clang-3.8 -Wall -Wno-unused-function *.c -o jac
```

#### Semantic Analysis
```
lex jac.l
yacc -d jac.y
clang-3.8 -Wall -Wno-unused-function *.c -o jac -g

```


#### EBNF Grammar

```
Program → CLASS ID OBRACE { FieldDecl | MethodDecl | SEMI } CBRACE
FieldDecl → PUBLIC STATIC Type ID { COMMA ID } SEMI
MethodDecl → PUBLIC STATIC MethodHeader MethodBody
MethodHeader → ( Type | VOID ) ID OCURV [ FormalParams ] CCURV
MethodBody → OBRACE { VarDecl | Statement } CBRACE
FormalParams → Type ID { COMMA Type ID }
FormalParams → STRING OSQUARE CSQUARE ID
VarDecl → Type ID { COMMA ID } SEMI
Type → BOOL | INT | DOUBLE
Statement → OBRACE { Statement } CBRACE
Statement → IF OCURV Expr CCURV Statement [ ELSE Statement ]
Statement → WHILE OCURV Expr CCURV Statement
Statement → DO Statement WHILE OCURV Expr CCURV SEMI
Statement → PRINT OCURV ( Expr | STRLIT ) CCURV SEMI
Statement → [ ( Assignment | MethodInvocation | ParseArgs ) ] SEMI
Statement → RETURN [ Expr ] SEMI
Assignment → ID ASSIGN Expr
MethodInvocation → ID OCURV [ Expr { COMMA Expr } ] CCURV
ParseArgs → PARSEINT OCURV ID OSQUARE Expr CSQUARE CCURV
Expr → Assignment | MethodInvocation | ParseArgs
Expr → Expr ( AND | OR ) Expr
Expr → Expr ( EQ | GEQ | GT | LEQ | LT | NEQ ) Expr
Expr → Expr ( PLUS | MINUS | STAR | DIV | MOD ) Expr
Expr → ( PLUS | MINUS | NOT ) Expr
Expr → ID [ DOTLENGTH ]
Expr → OCURV Expr CCURV
Expr → BOOLLIT | DECLIT | REALLIT
FieldDecl → error SEMI
Statement → error SEMI
ParseArgs → PARSEINT OCURV error CCURV
MethodInvocation → ID OCURV error CCURV
Expr → OCURV error CCURV
```
