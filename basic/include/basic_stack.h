#ifndef _BASIC_STACK_H
#define _BASIC_STACK_H

/*
 * Simple FIFO stack implementation.
 *
 */

#include <stdlib.h>
#include "basic_general.h"

/*
 * type definitions
 */
typedef void * basic_stack_data;

struct basic_stack_elem {
	basic_stack_data data;
	struct basic_stack_elem *next;
};
typedef struct basic_stack_elem bs_elem;

struct basic_stack {
	bs_elem *top;
	int num;
};

typedef void basic_stack_cleanup_ret;
typedef void * basic_stack_cleanup_args;
typedef basic_stack_cleanup_ret (*basic_stack_cleanup_func)(basic_stack_data, basic_stack_cleanup_args);

/*
 * API functions
 */
static inline void basic_stack_init(struct basic_stack *stack);

static inline int basic_stack_is_empty(struct basic_stack *stack);

static inline int basic_stack_num_elem(struct basic_stack *stack);

static inline void basic_stack_push(basic_stack_data data, struct basic_stack *stack);

static inline basic_stack_data basic_stack_pop(struct basic_stack *stack);

static inline basic_stack_data basic_stack_peek(struct basic_stack *stack);

static inline void basic_stack_destroy(struct basic_stack *stack, basic_stack_cleanup_func func, basic_stack_cleanup_args args);

/*
 * API macros
 */
#define BASIC_STACK_FOREACH(pos, type, stack)		\
	for(pos = (type **)MEMBER_OF_SAFE((stack)->top, basic_stack_data, OFFSET_OF(bs_elem, data));	\
		pos != NULL;		\
		pos = (type **)MEMBER_OF_SAFE(CONTAINER_OF((basic_stack_data *)pos, bs_elem, data)->next, \
			basic_stack_data, OFFSET_OF(bs_elem, data)))

#define BASIC_STACK_FOREACH_SAFE(pos, n, type, stack)		\
	for(pos = (type **)MEMBER_OF_SAFE((stack)->top, basic_stack_data, OFFSET_OF(bs_elem, data)),	\
		n = (type **)MEMBER_OF_SAFE(DEREF(MEMBER_OF_SAFE(			\
			CONTAINER_OF_SAFE((basic_stack_data *)pos, bs_elem, data), bs_elem *, OFFSET_OF(bs_elem, next))), \
			basic_stack_data, OFFSET_OF(bs_elem, data));	\
		pos != NULL;		\
		pos = n, n = (type **)MEMBER_OF_SAFE(DEREF(MEMBER_OF_SAFE(			\
			CONTAINER_OF_SAFE((basic_stack_data *)n, bs_elem, data), bs_elem *, OFFSET_OF(bs_elem, next))), \
			basic_stack_data, OFFSET_OF(bs_elem, data)))

/*
 * inline function definitions
 */
static inline void basic_stack_init(struct basic_stack *stack) {
	stack->top = NULL;
	stack->num = 0;
}

static inline int basic_stack_is_empty(struct basic_stack *stack) {
	return (stack->num == 0);
}

static inline int basic_stack_num_elem(struct basic_stack *stack) {
	return (stack->num);
}

static inline void basic_stack_push(basic_stack_data data, struct basic_stack *stack) {
	bs_elem *neww = (bs_elem *)malloc(sizeof(bs_elem));

	neww->data = data;
	neww->next = stack->top;
	stack->top = neww;
	stack->num++;
}

static inline basic_stack_data basic_stack_pop(struct basic_stack *stack) {
	bs_elem *popped = stack->top;
	basic_stack_data data;

	if (stack->num > 0) {
		data = popped->data;
		stack->top = popped->next;
		stack->num--;
		free(popped);
		return data;
	} else {
		return NULL;
	}
}

static inline basic_stack_data basic_stack_peek(struct basic_stack *stack) {

	if (stack->num > 0) {
		return stack->top->data;
	} else {
		return NULL;
	}
}

static inline void basic_stack_destroy(struct basic_stack *stack, basic_stack_cleanup_func func, basic_stack_cleanup_args args) {
	basic_stack_data data;

	while (stack->num > 0) {
		data = basic_stack_pop(stack);
		if (func != NULL)
			func(data, args);
	}
}

#endif