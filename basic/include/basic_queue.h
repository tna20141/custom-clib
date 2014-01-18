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
	struct bl_head list;
	basic_queue_data data;
};
typedef struct basic_queue_elem bq_elem;

struct basic_queue {
	struct bl_head head;
	int num;
};

typedef void basic_queue_cleanup_ret;
typedef void * basic_queue_cleanup_args;
typedef basic_queue_cleanup_ret (*basic_queue_cleanup_func)(basic_queue_data, basic_queue_cleanup_args);

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

static inline void basic_queue_destroy(struct basic_queue *queue, basic_queue_cleanup_func func, basic_queue_cleanup_args args);

/*
 * private functions
 */
static inline void __basic_queue_push(basic_queue_data data, struct basic_queue *queue, int head);
static inline basic_queue_data __basic_queue_pop(struct basic_queue *queue, int head);
static inline basic_queue_data __basic_queue_peek(struct basic_queue *queue, int head);

#define BASIC_QUEUE_FOREACH_DIRECTION(pos, type, queue, direction)		\
	for(pos = (type **)MEMBER_OF(CONTAINER_OF((queue)->head.direction, bq_elem, list),	\
			basic_queue_data, OFFSET_OF(bq_elem, data));	\
		MEMBER_OF(CONTAINER_OF((basic_queue_data *)pos, bq_elem, data),		\
			struct bl_head, OFFSET_OF(bq_elem, list)) != &(queue)->head;	\
		pos = (type **)MEMBER_OF(CONTAINER_OF(CONTAINER_OF((basic_queue_data *)pos,		\
			bq_elem, data)->list.direction, bq_elem, list), basic_queue_data, OFFSET_OF(bq_elem, data)))

#define BASIC_QUEUE_FOREACH_DIRECTION_SAFE(pos, n, type, queue, direction)	\
	for(pos = (type **)MEMBER_OF(CONTAINER_OF((queue)->head.direction, bq_elem, list),	\
			basic_queue_data, OFFSET_OF(bq_elem, data)),	\
		n = (type **)MEMBER_OF(CONTAINER_OF(CONTAINER_OF((basic_queue_data *)pos,		\
			bq_elem, data)->list.direction, bq_elem, list), basic_queue_data, OFFSET_OF(bq_elem, data));		\
		MEMBER_OF(CONTAINER_OF((basic_queue_data *)pos, bq_elem, data),		\
			struct bl_head, OFFSET_OF(bq_elem, list)) != &(queue)->head;	\
		pos = n, n = (type **)MEMBER_OF(CONTAINER_OF(CONTAINER_OF((basic_queue_data *)n,		\
			bq_elem, data)->list.direction, bq_elem, list), basic_queue_data, OFFSET_OF(bq_elem, data)))

/*
 * API macros
 */
#define BASIC_QUEUE_FOREACH_HEAD(pos, type, queue)		\
	BASIC_QUEUE_FOREACH_DIRECTION(pos, type, queue, next)

#define BASIC_QUEUE_FOREACH_HEAD_SAFE(pos, n, type, queue)		\
	BASIC_QUEUE_FOREACH_DIRECTION_SAFE(pos, n, type, queue, next)

#define BASIC_QUEUE_FOREACH_TAIL(pos, type, queue)		\
	BASIC_QUEUE_FOREACH_DIRECTION(pos, type, queue, prev)

#define BASIC_QUEUE_FOREACH_TAIL_SAFE(pos, n, type, queue)		\
	BASIC_QUEUE_FOREACH_DIRECTION_SAFE(pos, n, type, queue, prev)

#define BASIC_QUEUE_FOREACH BASIC_QUEUE_FOREACH_HEAD

#define BASIC_QUEUE_FOREACH_SAFE BASIC_QUEUE_FOREACH_HEAD_SAFE

/*
 * static function definitions
 */
static inline void basic_queue_init(struct basic_queue *queue) {
	BL_INIT_HEAD(&queue->head);
	queue->num = 0;
}

static inline int basic_queue_is_empty(struct basic_queue *queue) {
	return (queue->num == 0);
}

static inline int basic_queue_num_elem(struct basic_queue *queue) {
	return (queue->num);
}

static inline void basic_queue_reverse(struct basic_queue *queue) {
	struct bl_head *ptr, *n;

	bl_for_each_safe(ptr, n, &queue->head) {
		SWAP(ptr->next, ptr->prev);
	}
	SWAP(queue->head.next, queue->head.prev);
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

static inline void basic_queue_destroy(struct basic_queue *queue, basic_queue_cleanup_func func, basic_queue_cleanup_args args) {
	basic_queue_data data;

	while (queue->num > 0) {
		data = basic_queue_pop(queue);
		if (func != NULL)
			func(data, args);
	}
}

static inline void __basic_queue_push(basic_queue_data data, struct basic_queue *queue, int head) {
	bq_elem *elem = (bq_elem *)malloc(sizeof(bq_elem));

	BL_INIT_HEAD(&elem->list);
	elem->data = data;

	if (head)
		bl_add(&elem->list, &queue->head);
	else
		bl_add_tail(&elem->list, &queue->head);

	queue->num++;
}

static inline basic_queue_data __basic_queue_pop(struct basic_queue *queue, int head) {
	struct bl_head *node;
	basic_queue_data data;
	bq_elem *elem;

	if (head)
		node = queue->head.next;
	else
		node = queue->head.prev;

	if (node != &queue->head) {
		elem = CONTAINER_OF(node, bq_elem, list);
		data = elem->data;
		bl_del(node);
		queue->num--;
		free(elem);
		return data;
	} else {
		return NULL;
	}
}

static inline basic_queue_data __basic_queue_peek(struct basic_queue *queue, int head) {
	struct bl_head *node;
	basic_queue_data data;

	if (head)
		node = queue->head.next;
	else
		node = queue->head.prev;

	if (node != &queue->head) {
		data = CONTAINER_OF(node, bq_elem, list)->data;
		return data;
	} else {
		return NULL;
	}
}

#endif
