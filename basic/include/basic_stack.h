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
 * private functions
 */
static inline void __basic_stack_push(bs_elem *elem, struct basic_stack *stack);

static inline bs_elem *__basic_stack_pop(struct basic_stack *stack);

#endif