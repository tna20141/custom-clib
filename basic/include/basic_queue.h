#ifndef _BASIC_QUEUE_H
#define _BASIC_QUEUE_H

/*
 * Simple LIFO queue implementation.
 *
 */

#include "basic_list.h"
#include "basic_general.h"

struct basic_queue {
	struct list_head list;
	int num;
};

/*
 * API functions
 */
static inline void basic_queue_init(struct basic_queue *queue);

static inline int basic_queue_is_empty(struct basic_queue *queue);

static inline int basic_queue_num_elem(struct basic_queue *queue);

static inline void basic_queue_reverse(struct basic_queue *queue);

/*
 * private functions
 */
static inline void __basic_queue_push(struct list_head *e, struct basic_queue *queue, int head);
static inline struct list_head *__basic_queue_pop(struct basic_queue *queue, int head);
static inline struct list_head *__basic_queue_peek(struct basic_queue *queue, int head);

/*
 * API macros
 */
#define BASIC_QUEUE_PUSH_HEAD(entry, queue, member) ({	\
	const size_t __offset = OFFSET_OF(typeof(*entry), member);	\
	__basic_queue_push(MEMBER_OF(entry, struct list_head, __offset), queue, 1); })

#define BASIC_QUEUE_PUSH_TAIL(entry, queue, member) ({	\
	const size_t __offset = OFFSET_OF(typeof(*entry), member);	\
	__basic_queue_push(MEMBER_OF(entry, struct list_head, __offset), queue, 0); })

#define BASIC_QUEUE_PUSH BASIC_QUEUE_PUSH_TAIL

#define BASIC_QUEUE_POP_HEAD(type, queue, member) ({	\
	struct list_head *__elem = __basic_queue_pop(queue, 1);	\
	CONTAINER_OF_SAFE(__elem, type, member); })

#define BASIC_QUEUE_POP_TAIL(type, queue, member) ({	\
	struct list_head *__elem = __basic_queue_pop(queue, 0);	\
	CONTAINER_OF_SAFE(__elem, type, member); })

#define BASIC_QUEUE_POP BASIC_QUEUE_POP_HEAD

#define BASIC_QUEUE_PEEK_HEAD(type, queue, member) ({	\
	struct list_head *__elem = __basic_queue_peek(queue, 1);	\
	CONTAINER_OF_SAFE(__elem, type, member); })

#define BASIC_QUEUE_PEEK_TAIL(type, queue, member) ({	\
	struct list_head *__elem = __basic_queue_peek(queue, 0);	\
	CONTAINER_OF_SAFE(__elem, type, member); })

#define BASIC_QUEUE_PEEK BASIC_QUEUE_PEEK_HEAD

#define BASIC_QUEUE_FOREACH_HEAD(pos, queue, member)		\
	list_for_each_entry(pos, &(queue)->list, member)

#define BASIC_QUEUE_FOREACH_HEAD_SAFE(pos, n, queue, member)	\
	list_for_each_entry_safe(pos, n, &(queue)->list, member)

#define BASIC_QUEUE_FOREACH_TAIL(pos, queue, member)		\
	list_for_each_entry_reverse(pos, &(queue)->list, member)

#define BASIC_QUEUE_FOREACH_TAIL_SAFE(pos, n, queue, member)		\
	list_for_each_entry_safe_reverse(pos, n, &(queue)->list, member)

#define BASIC_QUEUE_FOREACH BASIC_QUEUE_FOREACH_HEAD

#define BASIC_QUEUE_FOREACH_SAFE BASIC_QUEUE_FOREACH_HEAD_SAFE

#define BASIC_QUEUE_DESTROY(type, queue, member, func, args) ({		\
	type *__pos, *__n;		\
	BASIC_QUEUE_FOREACH_SAFE(__pos, __n, queue, member) {	\
		__basic_queue_pop(queue, 1);	\
		if ((func) != NULL)		\
			((void (*)(type *, void *))(func))(__pos, args);	\
		else		\
			free(__pos);		\
	}; })

/*
 * static function definitions
 */
static inline void basic_queue_init(struct basic_queue *queue) {
	INIT_LIST_HEAD(&(queue->list));
	queue->num = 0;
}

static inline int basic_queue_is_empty(struct basic_queue *queue) {
	return (queue->num == 0);
}

static inline int basic_queue_num_elem(struct basic_queue *queue) {
	return (queue->num);
}

static inline void basic_queue_reverse(struct basic_queue *queue) {
	struct list_head *ptr, *n;

	list_for_each_safe(ptr, n, &queue->list) {
		SWAP(ptr->next, ptr->prev);
	}
}
static inline void __basic_queue_push(struct list_head *e, struct basic_queue *queue, int head) {
	if (head)
		list_add(e, &queue->list);
	else
		list_add_tail(e, &queue->list);

	queue->num++;
}

static inline struct list_head *__basic_queue_pop(struct basic_queue *queue, int head) {
	struct list_head *elem;

	if (head)
		elem = queue->list.next;
	else
		elem = queue->list.prev;

	if (elem != &queue->list) {
		list_del_init(elem);
		queue->num--;
		return elem;
	} else {
		return NULL;
	}
}

static inline struct list_head *__basic_queue_peek(struct basic_queue *queue, int head) {
	struct list_head *elem;

	if (head)
		elem = queue->list.next;
	else
		elem = queue->list.prev;

	if (elem != &queue->list) {
		return elem;
	} else {
		return NULL;
	}
}

#endif
