#ifndef _CUSTOM_CLIB_CLOAK_20_H
#define _CUSTOM_CLIB_CLOAK_20_H

/*
 * Pasting macro
 */
#define CLOAK_CAT(a, ...) CLOAK_PRIMITIVE_CAT(a, __VA_ARGS__)
#define CLOAK_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

/*
 * Stringify macro
 */
#define CLOAK_STRINGIFY(...) CLOAK_PRIMITIVE_STRINGIFY(__VA_ARGS__)
#define CLOAK_PRIMITIVE_STRINGIFY(...) # __VA_ARGS__

/*
 * The maximum number of loops is defined here.
 * Increase the maximum number of loops by adding
 * more macros for each of the types below
 */
#define CLOAK_INC(x) CLOAK_PRIMITIVE_CAT(CLOAK_INC_, x)
#define CLOAK_INC_0 1
#define CLOAK_INC_1 2
#define CLOAK_INC_2 3
#define CLOAK_INC_3 4
#define CLOAK_INC_4 5
#define CLOAK_INC_5 6
#define CLOAK_INC_6 7
#define CLOAK_INC_7 8
#define CLOAK_INC_8 9
#define CLOAK_INC_9 10
#define CLOAK_INC_10 11
#define CLOAK_INC_11 12
#define CLOAK_INC_12 13
#define CLOAK_INC_13 14
#define CLOAK_INC_14 15
#define CLOAK_INC_15 16
#define CLOAK_INC_16 17
#define CLOAK_INC_17 18
#define CLOAK_INC_18 19
#define CLOAK_INC_19 20
#define CLOAK_INC_20 21


#define CLOAK_DEC(x) CLOAK_PRIMITIVE_CAT(CLOAK_DEC_, x)
#define CLOAK_DEC_0 0
#define CLOAK_DEC_1 0
#define CLOAK_DEC_2 1
#define CLOAK_DEC_3 2
#define CLOAK_DEC_4 3
#define CLOAK_DEC_5 4
#define CLOAK_DEC_6 5
#define CLOAK_DEC_7 6
#define CLOAK_DEC_8 7
#define CLOAK_DEC_9 8
#define CLOAK_DEC_10 9
#define CLOAK_DEC_11 10
#define CLOAK_DEC_12 11
#define CLOAK_DEC_13 12
#define CLOAK_DEC_14 13
#define CLOAK_DEC_15 14
#define CLOAK_DEC_16 15
#define CLOAK_DEC_17 16
#define CLOAK_DEC_18 17
#define CLOAK_DEC_19 18
#define CLOAK_DEC_20 19


#define CLOAK_EXPR_S(s) CLOAK_PRIMITIVE_CAT(CLOAK_EXPR_, s)
#define CLOAK_EXPR_0(...) __VA_ARGS__
#define CLOAK_EXPR_1(...) __VA_ARGS__
#define CLOAK_EXPR_2(...) __VA_ARGS__
#define CLOAK_EXPR_3(...) __VA_ARGS__
#define CLOAK_EXPR_4(...) __VA_ARGS__
#define CLOAK_EXPR_5(...) __VA_ARGS__
#define CLOAK_EXPR_6(...) __VA_ARGS__
#define CLOAK_EXPR_7(...) __VA_ARGS__
#define CLOAK_EXPR_8(...) __VA_ARGS__
#define CLOAK_EXPR_9(...) __VA_ARGS__
#define CLOAK_EXPR_10(...) __VA_ARGS__
#define CLOAK_EXPR_11(...) __VA_ARGS__
#define CLOAK_EXPR_12(...) __VA_ARGS__
#define CLOAK_EXPR_13(...) __VA_ARGS__
#define CLOAK_EXPR_14(...) __VA_ARGS__
#define CLOAK_EXPR_15(...) __VA_ARGS__
#define CLOAK_EXPR_16(...) __VA_ARGS__
#define CLOAK_EXPR_17(...) __VA_ARGS__
#define CLOAK_EXPR_18(...) __VA_ARGS__
#define CLOAK_EXPR_19(...) __VA_ARGS__
#define CLOAK_EXPR_20(...) __VA_ARGS__

/*
 * end of macros for defining maximum number of loops
 */

/*
 * Conditional macros (boolean stuff)
 */
#define CLOAK_CHECK_N(x, n, ...) n
#define CLOAK_CHECK(...) CLOAK_CHECK_N(__VA_ARGS__, 0,)

#define CLOAK_NOT(x) CLOAK_CHECK(CLOAK_PRIMITIVE_CAT(CLOAK_NOT_, x))
#define CLOAK_NOT_0 ~, 1,

#define CLOAK_COMPL(b) CLOAK_PRIMITIVE_CAT(CLOAK_COMPL_, b)
#define CLOAK_COMPL_0 1
#define CLOAK_COMPL_1 0

#define CLOAK_BOOL(x) CLOAK_COMPL(CLOAK_NOT(x))

#define CLOAK_IIF(c) CLOAK_PRIMITIVE_CAT(CLOAK_IIF_, c)
#define CLOAK_IIF_0(t, ...) __VA_ARGS__
#define CLOAK_IIF_1(t, ...) t

#define CLOAK_IF(c) CLOAK_IIF(CLOAK_BOOL(c))

/*
 * When macro
 */
#define CLOAK_EAT(...)
#define CLOAK_EXPAND(...) __VA_ARGS__
#define CLOAK_WHEN(c) CLOAK_IF(c)(CLOAK_EXPAND, CLOAK_EAT)

/*
 * Deferation macros
 */
#define CLOAK_EMPTY()
#define CLOAK_DEFER(id) id CLOAK_EMPTY()
#define CLOAK_OBSTRUCT(id) id CLOAK_DEFER(CLOAK_EMPTY)()
     
//#define REPEAT_S(s, n, m, ...) \
//        IF(n)(REPEAT_I, EAT)(OBSTRUCT(), INC(s), DEC(n), m, __VA_ARGS__)
//        
//#define REPEAT_INDIRECT() REPEAT_S
//#define REPEAT_I(_, s, n, m, ...) \
//        EXPR_S _(s)( \
//            REPEAT_INDIRECT _()(s, n, m, __VA_ARGS__) \
//        )\
//        m _(s, n, __VA_ARGS__)

/*
 * Run loop macro
 * Macro to start the loop, it basically expands the loop macro 1 more time
 */
#define CLOAK_RUN_LOOP(...) __VA_ARGS__

/*
 * Repeat macro
 *	@s: the start number for the iterator of the loop
 *	@n: the number of the loop (the loop will run n times)
 *	@m: the name of the body macro of the loop
 *
 * You need to define the body macro like so:
 *		#define <M>(S, N, ...) <BODY>
 * with <M> is the macro name, S = s+i, N is the total number of loops,
 * i is the loop number starting from 0. The rest of the
 * repeat arguments are passed to the body macro
 *
 * The macro can be called like so:
 *		CLOAK_RUN_LOOP(CLOAK_REPEAT_S(0, 8, M, ~))
 */
#define CLOAK_REPEAT(s, n, m, ...) \
        CLOAK_REPEAT_I(CLOAK_OBSTRUCT(), s, CLOAK_DEC(n), m, n,__VA_ARGS__)
        
#define CLOAK_REPEAT_INDIRECT() CLOAK_REPEAT_I
#define CLOAK_REPEAT_I(_, s, n, m, N, ...) \
        m _(s, N,__VA_ARGS__) \
        CLOAK_WHEN _(n)(CLOAK_EXPR_S _(s)( \
            CLOAK_REPEAT_INDIRECT _() \
            	(CLOAK_OBSTRUCT _(), CLOAK_INC _(s), CLOAK_DEC _(n), m, N, __VA_ARGS__) \
        ))

/*
 * Comma macro
 * used when you need to insert commas in between the expanded macros
 */
#define CLOAK_COMMA() ,
#define CLOAK_COMMA_IF(n) CLOAK_IF(n)(CLOAK_COMMA, CLOAK_EAT)()

#endif