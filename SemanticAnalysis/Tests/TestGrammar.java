/* Program -> CLASS ID OBRACE { FieldDecl | MethodDecl | SEMI } CBRACE */
class TestGrammar {
    /* Type -> BOOL | INT | DOUBLE */
    /* FieldDecl -> PUBLIC STATIC Type ID SEMI */
    public static int a,b,c,d,e;        // declit
    public static boolean a1,a2,a3,a4,a5;   // bool
    public static double q1,q2,q3,q4,q5;    // reallit


    /* MethodDecl -> PUBLIC STATIC MethodHeader MethodBody */
    /* MethodHeader -> (Type | VOID) ID OCURV CCURV */
    /* MethodBody -> OBRACE CBRACE */
    public static int foo1() {}
    public static boolean foo2() {}
    public static double foo3() {}
    public static void foo4() {}

    /* MethodDecl -> PUBLIC STATIC MethodHeader MethodBody */
    /* MethodHeader -> (Type | VOID) ID OCURV [ FormalParams ] CCURV */
    /* MethodBody -> OBRACE { VarDecl | Statement } CBRACE */
    /* FormalParams -> Type ID { COMMA Type ID } */
    public static int foo5(int a) {
        /* VarDecl -> Type ID {COMMA ID} SEMI */
        int temp, m, n, b;
        double q,w,e,r,t,y;
        /*
            Line 25, col 34: invalid escape sequence (\z)
            Line 25, col 35: syntax error: )


        */
        /* VarDecl -> Type ID SEMI */
        double temp2;
        boolean temp3;
        int temp4;
        /* Statement -> OBRACE CBRACE */
        {}
        /* Statement -> OBRACE { Statement } CBRACE */
        {{{{{{}}}}}}

    }
    public static boolean foo6(int a, double b, boolean c) {
        /* Statement -> IF OCURV Expr CCURV Statement */
        if (b > 5.0 && c == false) {
            System.out.println("Inside if");
        }

        /* Statement -> IF OCURV Expr CCURV Statement [ ELSE Statement ] */
        if (b > 239.0 && c == true) {
            System.out.println("Inside if");
        } else {
            System.out.println("Inside else");
        }
    }
    public static double foo7(int a, int b, int c, int d) {
        /* Statement -> WHILE OCURV Expr CCURV Statement */
        while (a > 10 && b < a && c + d == 20) {
            a = a - 1;
            b = b % 3;
            c = c / d;
            d = a + a;
        }
        int lel,lol,lil;
        int a;
        double what,the,beep;
    }
    public static void foo8() {
        /* Statement -> DO Statement WHILE OCURV Expr CCURV SEMI */
        do {
            while (a > 10 && b < a && c + d == 20) {

                /* Assignment -> ID ASSIGN Expr */
                a = a - 1;
                b = b % 3;
                c = c / d;
                d = a + a;
                a = b = c = d;
                if (b > 5.0 && c) {
                    System.out.println("Inside if");
                }

                if (a) {

                } else {
                    if (b) {

                    } else {

                    }
                }

                /* Statement -> IF OCURV Expr CCURV Statement [ ELSE Statement ] */
                if (b > 239.0 && c) {
                    System.out.println("Inside if foo 8");
                } else {
                    System.out.println("Inside else foo 8");
                }
            }
        } while (true == true || false == false && 32 > 94 || 66 < 2 && 56 != 22);

        /* Statement -> PRINT OCURV ( Expr ) CCURV SEMI */
        /* Statement -> PRINT OCURV ( STRLIT ) CCURV SEMI */
        System.out.println("End of function");
        System.out.println(a);
        System.out.println(b);
        System.out.println(c);
        System.out.println(d);
    }

    public static int caller() {
        /* MethodInvocation -> ID OCURV CCURV */
        foo8();
        /* MethodInvocation -> ID OCURV [ Expr { COMMA Expr } ] CCURV */
        foo7(2322, 22, 1, 1321);
        foo6(12,23.2222,false);
    }

    public static void bar(String []args) {
        int _a,_b,_c,_d,_e;
        double vars;

        /* ParseArgs -> PARSEINT OCURV ID OSQUARE Expr CSQUARE CCURV */
        _a = Integer.parseInt(args[0]);
        _b = Integer.parseInt(args[1]);
        _c = Integer.parseInt(args[args.length]);
        _d = Integer.parseInt(args[2+2]);
        _e = Integer.parseInt(args[1000-6]);

        /*int a;
        int b;
        boolean c;*/
        a = 1;
        b = 2;
        c = 3;
        /* Expr -> (PLUS | MINUS | NOT) Expr */
        a = +a;
        b = -a;
        c = !c;
        int d;
        d = a.length;
        d = (a+b)*c-(b+b+b)/d;
        //d = (a+) /* <------------------ */

        /*there are no empty args here...*/
        System.out.println("");
        b = c = h;
    }


    /* FieldDecl -> PUBLIC STATIC Type ID { COMMA ID } SEMI */
    public static int b, c, d, e;
    public static double qw;
    public static int comp;
    public static boolean aaaaaa;

}