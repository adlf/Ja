class teste {
       public static void foo8(String []args) {
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
                    ;
                } else {
                    if (b) {
                        ;
                    } else {
                        ;
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
        foo("abcd\h\jkkkkkkkkkkkkk")

    }
}