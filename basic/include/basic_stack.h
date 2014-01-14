#ifndef _BASIC_STACK_H
#define _BASIC_STACK_H

/*
 * Simple FIFO stack implementation.
 *
 */

#include "basic_general.h"

struct basic_stack_list_head {
	struct basic_stack_list_head *next;
};
typedef struct basic_stack_list_head bs_elem;

struct basic_stack {
	bs_elem top;
	int num;
};

/*
 * API functions
 */
static inline void basic_stack_init(struct basic_stack *stack);

static inline int basic_stack_is_empty(struct basic_stack *stack);

static inline int basic_stack_num_elem(struct basic_stack *stack);

/*
 * API macros
 */
#define BASIC_STACK_PUSH(entry, stack, member) ({		\
	const size_t __offset = OFFSET_OF(typeof(*entry), member);		\
	__basic_stack_push(MEMBER_OF(entry, typeof((entry)->member), __offset), stack); })

#define BASIC_STACK_POP(type, stack, member) ({		\
	bs_elem *__elem = __basic_stack_pop(stack);		\
	CONTAINER_OF_SAFE(__elem, type, member); })

#define BASIC_STACK_PEEK(type, stack, member) ({	\
	bs_elem *__elem = __basic_stack_peek(stack);		\
	CONTAINER_OF_SAFE(__elem, type, member); })

#define BASIC_STACK_FOREACH(pos, stack, member)		\
	for(pos = CONTAINER_OF_SAFE((stack)->top.next, typeof(*pos), member);	\
		pos != NULL;		\
		pos = CONTAINER_OF_SAFE(pos->member.next, typeof(*pos), member))

#define BASIC_STACK_FOREACH_SAFE(pos, n, stack, member)		\
	for(pos = CONTAINER_OF_SAFE((stack)->top.next, typeof(*pos), member),	\
		n = (pos == NULL) ? NULL : CONTAINER_OF_SAFE(pos->member.next, typeof(*pos), member);		\
		pos != NULL;		\
		pos = n, n = (n == NULL) ? NULL : CONTAINER_OF_SAFE(n->member.next, typeof(*n), member))

#define BASIC_STACK_DESTROY(type, stack, member, func, args) ({		\
	type *__entry, *__temp;		\
	BASIC_STACK_FOREACH_SAFE(__entry, __temp, stack, member) {	\
		__basic_stack_pop(stack);		\
		if (func != NULL)		\
			func(__entry, args);		\
		else		\
			free(__entry);		\
	}; })

/*
 * private functions
 */
static inline void __basic_stack_push(bs_elem *elem, struct basic_stack *stack);
static inline bs_elem *__basic_stack_pop(struct basic_stack *stack);
static inline bs_elem *__basic_stack_peek(struct basic_stack *stack);

/*
 * inline function definitions
 */
static inline void basic_stack_init(struct basic_stack *stack) {
	stack->top.next = NULL;
	stack->num = 0;
}

static inline int basic_stack_is_empty(struct basic_stack *stack) {
	return (stack->num == 0);
}

static inline int basic_stack_num_elem(struct basic_stack *stack) {
	return (stack->num);
}

static inline void __basic_stack_push(bs_elem *elem, struct basic_stack *stack) {
	elem->next = stack->top.next;
	stack->top.next = elem;
	stack->num++;
}

static inline bs_elem *__basic_stack_pop(struct basic_stack *stack) {
	bs_elem *popped = stack->top.next;
	if (stack->num > 0) {
		stack->top.next = popped->next;
		popped->next = NULL;
		stack->num--;
	}
	return popped;
}

static inline bs_elem *__basic_stack_peek(struct basic_stack *stack) {
	return (stack->top.next);
}

#endif