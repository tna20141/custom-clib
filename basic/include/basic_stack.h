#ifndef _BASIC_STACK_H
#define _BASIC_STACK_H

/*
 * Simple FIFO stack implementation.
 *
 */

struct basic_stack_list_head {
	struct basic_stack_list_head *next;
};
typedef struct basic_stack_list_head bs_elem;

struct basic_stack {
	bs_elem *top;
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
#define BASIC_STACK_PUSH(entry, type, member, stack) ({		\
	const size_t offset = OFFSET_OF(type, member);		\
	__basic_stack_push(MEMBER_OF(entry, type, offset), stack); })

#define BASIC_STACK_POP(type, member, stack) ({		\
	bs_elem *elem = __basic_stack_pop(stack);		\
	((elem == NULL) ? NULL : CONTAINER_OF(elem, type, member)); })

#define BASIC_STACK_PEEK(type, member, stack) ({	\
	bs_elem *lem = __basic_stack_peek(stack);		\									\
	((elem == NULL) ? NULL : CONTAINER_OF(elem, type, member)); })

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
	stack->top = NULL;
	stack->num = 0;
}

static inline int basic_stack_is_empty(struct basic_stack *stack) {
	return (stack->num == 0);
}

static inline int basic_stack_num_elem(struct basic_stack *stack) {
	return (stack->num);
}

static inline void __basic_stack_push(bs_elem *elem, struct basic_stack *stack) {
	elem->next = stack->top;
	stack->top = elem;
	stack->num++;
}

static inline bs_elem *__basic_stack_pop(struct basic_stack *stack) {
	bs_elem *popped = stack->top;
	if (num > 0) {
		stack->top = popped->next;
		popped->next = NULL;
		stack->num--;
	}
	return popped;
}

static inline bs_elem *__basic_stack_peek(struct basic_stack *stack) {
	return (stack->top);
}

#endif