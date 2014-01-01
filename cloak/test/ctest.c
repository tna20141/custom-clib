#include "../src/cloak_template.h"

// #define A(X, Y) 1 B X () (OBSTRUCT X (), X 1)
// #define B() 2 A

// A(OBSTRUCT(), 1)

// EXPAND(A(OBSTRUCT(), 1))

// EXPAND(EXPAND(A(OBSTRUCT(), 1)))

// #define C() 1 D DEFER() ()
// #define D() 2 C DEFER() ()

// C()

// EXPAND(C())

// EXPAND(EXPAND(C()))

// EXPAND(EXPAND(EXPAND(C())))

// EXPAND(EXPAND(EXPAND(EXPAND(C()))))

// #define G() 1 G DEFER() ()

// G()
// EXPAND(G())
// EXPAND(EXPAND(G()))

// 1
#define M(s, i, ...) i

// EXPR_S(9)(REPEAT_I(OBSTRUCT(), 0, 3, M, ~))

// EXPR_0(WHEN _(3)(EXPR_S(1)(REPEAT_INDIRECT _()(OBSTRUCT _(), INC _(1), \
// 	DEC _(3), M, ~))))
// EXPR_0(REPEAT_S(0, 1, M, ~)) // 0 1 2 3 4 5 6 7

// A(2, 3)

// #define A(X) (X+1)
// #define B(X) (X+2_)

// A(A(A(1)))

// A(B(A(1)))
