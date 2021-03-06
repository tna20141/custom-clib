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
typedef void * bs_data;

struct bs_elem {
	bs_data data;
	struct bs_elem *next;
};
typedef struct bs_elem bs_elem;

struct bs_stack {
	bs_elem *top;
	int num;
};

typedef void bs_cleanup_ret;
typedef void * bs_cleanup_args;
typedef bs_cleanup_ret (*bs_cleanup_func)(bs_data, bs_cleanup_args);

/*
 * API functions
 */
static inline void bs_init(struct bs_stack *stack);

static inline int bs_is_empty(struct bs_stack *stack);

static inline int bs_num_elem(struct bs_stack *stack);

static inline void bs_push(bs_data data, struct bs_stack *stack);

static inline bs_data bs_pop(struct bs_stack *stack);

static inline bs_data bs_peek(struct bs_stack *stack);

static inline void bs_destroy(struct bs_stack *stack, bs_cleanup_func func, bs_cleanup_args args);

/*
 * API macros
 */
#define BS_FOREACH(pos, type, stack)		\
	for(pos = (type **)MEMBER_OF_SAFE((stack)->top, bs_data, OFFSET_OF(bs_elem, data));	\
		pos != NULL;		\
		pos = (type **)MEMBER_OF_SAFE(CONTAINER_OF((bs_data *)pos, bs_elem, data)->next, \
			bs_data, OFFSET_OF(bs_elem, data)))

#define BS_FOREACH_SAFE(pos, n, type, stack)		\
	for(pos = (type **)MEMBER_OF_SAFE((stack)->top, bs_data, OFFSET_OF(bs_elem, data)),	\
		n = (type **)MEMBER_OF_SAFE(DEREF(MEMBER_OF_SAFE(			\
			CONTAINER_OF_SAFE((bs_data *)pos, bs_elem, data), bs_elem *, OFFSET_OF(bs_elem, next))), \
			bs_data, OFFSET_OF(bs_elem, data));	\
		pos != NULL;		\
		pos = n, n = (type **)MEMBER_OF_SAFE(DEREF(MEMBER_OF_SAFE(			\
			CONTAINER_OF_SAFE((bs_data *)n, bs_elem, data), bs_elem *, OFFSET_OF(bs_elem, next))), \
			bs_data, OFFSET_OF(bs_elem, data)))

/*
 * inline function definitions
 */
static inline void bs_init(struct bs_stack *stack) {
	stack->top = NULL;
	stack->num = 0;
}

static inline int bs_is_empty(struct bs_stack *stack) {
	return (stack->num == 0);
}

static inline int bs_num_elem(struct bs_stack *stack) {
	return (stack->num);
}

static inline void bs_push(bs_data data, struct bs_stack *stack) {
	bs_elem *neww = (bs_elem *)malloc(sizeof(bs_elem));

	neww->data = data;
	neww->next = stack->top;
	stack->top = neww;
	stack->num++;
}

static inline bs_data bs_pop(struct bs_stack *stack) {
	bs_elem *popped = stack->top;
	bs_data data;

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

static inline bs_data bs_peek(struct bs_stack *stack) {

	if (stack->num > 0) {
		return stack->top->data;
	} else {
		return NULL;
	}
}

static inline void bs_destroy(struct bs_stack *stack, bs_cleanup_func func, bs_cleanup_args args) {
	bs_data data;

	while (stack->num > 0) {
		data = bs_pop(stack);
		if (func != NULL)
			func(data, args);
	}
}

#endif