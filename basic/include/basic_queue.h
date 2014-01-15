#ifndef _BASIC_QUEUE_H
#define _BASIC_QUEUE_H

/*
 * Simple LIFO queue implementation.
 *
 */

#include "basic_list.h"
#include "basic_general.h"

/*
 * type definitions
 */
typedef void * basic_queue_data;

struct basic_queue_elem {
	struct list_head list;
	basic_queue_data data;
};
typedef struct basic_queue_elem bq_elem;

struct basic_queue {
	struct list_head head;
	int num;
};

/*
 * API functions
 */
static inline void basic_queue_init(struct basic_queue *queue);

static inline int basic_queue_is_empty(struct basic_queue *queue);

static inline int basic_queue_num_elem(struct basic_queue *queue);

static inline void basic_queue_reverse(struct basic_queue *queue);

static inline void basic_queue_push_head(basic_queue_data data, struct basic_queue *queue);

static inline void basic_queue_push_tail(basic_queue_data data, struct basic_queue *queue);

static inline void basic_queue_push(basic_queue_data data, struct basic_queue *queue);

static inline basic_queue_data basic_queue_pop_head(struct basic_queue *queue);

static inline basic_queue_data basic_queue_pop_tail(struct basic_queue *queue);

static inline basic_queue_data basic_queue_pop(struct basic_queue *queue);

static inline basic_queue_data basic_queue_peek_head(struct basic_queue *queue);

static inline basic_queue_data basic_queue_peek_tail(struct basic_queue *queue);

static inline basic_queue_data basic_queue_peek(struct basic_queue *queue);

/*
 * private functions
 */
static inline void __basic_queue_push(basic_queue_data data, struct basic_queue *queue, int head);
static inline basic_queue_data __basic_queue_pop(struct basic_queue *queue, int head);
static inline basic_queue_data __basic_queue_peek(struct basic_queue *queue, int head);

/*
 * API macros
 */
#define BASIC_QUEUE_FOREACH_HEAD(pos, queue)		\
	for(pos = MEMBER_OF(CONTAINER_OF((queue)->head.next, bs_elem, list), basic_queue_data, OFFSET_OF(bs_elem, data));	\
		MEMBER_OF(CONTAINER_OF(pos, bs_elem, data), struct list_head, OFFSET_OF(bs_elem, list)) != &(queue)->head;	\
		pos = MEMBER_OF(CONTAINER_OF(CONTAINER_OF(pos, bs_elem, data)->list.next, bs_elem, list), basic_queue_data, OFFSET_OF(bs_elem, data)))

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
	INIT_LIST_HEAD(&queue->head);
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

	list_for_each_safe(ptr, n, &queue->head) {
		SWAP(ptr->next, ptr->prev);
	}
}

static inline void basic_queue_push_head(basic_queue_data data, struct basic_queue *queue) {
	__basic_queue_push(data, queue, 1);
}

static inline void basic_queue_push_tail(basic_queue_data data, struct basic_queue *queue) {
	__basic_queue_push(data, queue, 0);
}

static inline void basic_queue_push(basic_queue_data data, struct basic_queue *queue) {
	__basic_queue_push(data, queue, 0);
}

static inline basic_queue_data basic_queue_pop_head(struct basic_queue *queue) {
	return __basic_queue_pop(queue, 1);
}

static inline basic_queue_data basic_queue_pop_tail(struct basic_queue *queue) {
	return __basic_queue_pop(queue, 0);
}

static inline basic_queue_data basic_queue_pop(struct basic_queue *queue) {
	return __basic_queue_pop(queue, 1);
}

static inline basic_queue_data basic_queue_peek_head(struct basic_queue *queue) {
	return __basic_queue_peek(queue, 1);
}

static inline basic_queue_data basic_queue_peek_tail(struct basic_queue *queue) {
	return __basic_queue_peek(queue, 0);
}

static inline basic_queue_data basic_queue_peek(struct basic_queue *queue) {
	return __basic_queue_peek(queue, 1);
}

static inline void __basic_queue_push(basic_queue_data data, struct basic_queue *queue, int head) {
	bq_elem *elem = (bq_elem *)malloc(sizeof(bq_elem));

	INIT_LIST_HEAD(&elem->list);
	elem->data = data;

	if (head)
		list_add(&elem->list, &queue->list);
	else
		list_add_tail(&elem->list, &queue->list);

	queue->num++;
}

static inline basic_queue_data __basic_queue_pop(struct basic_queue *queue, int head) {
	struct list_head *node;
	basic_queue_data data;
	bs_elem *elem;

	if (head)
		node = queue->head.next;
	else
		node = queue->head.prev;

	if (node != &queue->head) {
		elem = container_of(node, bs_elem, list);
		data = elem->data;
		list_del_init(node);
		queue->num--;
		free(elem);
		return data;
	} else {
		return NULL;
	}
}

static inline basic_queue_data __basic_queue_peek(struct basic_queue *queue, int head) {
	struct list_head *node;
	basic_queue_data data;

	if (head)
		node = queue->head.next;
	else
		node = queue->head.prev;

	if (node != &queue->head) {
		data = container_of(node, bs_elem, list)->data;
		return data;
	} else {
		return NULL;
	}
}

#endif
