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
typedef void * bq_data;

struct bq_elem {
	struct bl_head list;
	bq_data data;
};
typedef struct bq_elem bq_elem;

struct bq_queue {
	struct bl_head head;
	int num;
};

typedef void bq_cleanup_ret;
typedef void * bq_cleanup_args;
typedef bq_cleanup_ret (*bq_cleanup_func)(bq_data, bq_cleanup_args);

/*
 * API functions
 */
static inline void bq_init(struct bq_queue *queue);

static inline int bq_is_empty(struct bq_queue *queue);

static inline int bq_num_elem(struct bq_queue *queue);

static inline void bq_reverse(struct bq_queue *queue);

static inline void bq_push_head(bq_data data, struct bq_queue *queue);

static inline void bq_push_tail(bq_data data, struct bq_queue *queue);

static inline void bq_push(bq_data data, struct bq_queue *queue);

static inline bq_data bq_pop_head(struct bq_queue *queue);

static inline bq_data bq_pop_tail(struct bq_queue *queue);

static inline bq_data bq_pop(struct bq_queue *queue);

static inline bq_data bq_peek_head(struct bq_queue *queue);

static inline bq_data bq_peek_tail(struct bq_queue *queue);

static inline bq_data bq_peek(struct bq_queue *queue);

static inline void bq_destroy(struct bq_queue *queue, bq_cleanup_func func, bq_cleanup_args args);

/*
 * private functions
 */
static inline void __bq_push(bq_data data, struct bq_queue *queue, int head);
static inline bq_data __bq_pop(struct bq_queue *queue, int head);
static inline bq_data __bq_peek(struct bq_queue *queue, int head);

#define BQ_FOREACH_DIRECTION(pos, type, queue, direction)		\
	for(pos = (type **)MEMBER_OF(CONTAINER_OF((queue)->head.direction, bq_elem, list),	\
			bq_data, OFFSET_OF(bq_elem, data));	\
		MEMBER_OF(CONTAINER_OF((bq_data *)pos, bq_elem, data),		\
			struct bl_head, OFFSET_OF(bq_elem, list)) != &(queue)->head;	\
		pos = (type **)MEMBER_OF(CONTAINER_OF(CONTAINER_OF((bq_data *)pos,		\
			bq_elem, data)->list.direction, bq_elem, list), bq_data, OFFSET_OF(bq_elem, data)))

#define BQ_FOREACH_DIRECTION_SAFE(pos, n, type, queue, direction)	\
	for(pos = (type **)MEMBER_OF(CONTAINER_OF((queue)->head.direction, bq_elem, list),	\
			bq_data, OFFSET_OF(bq_elem, data)),	\
		n = (type **)MEMBER_OF(CONTAINER_OF(CONTAINER_OF((bq_data *)pos,		\
			bq_elem, data)->list.direction, bq_elem, list), bq_data, OFFSET_OF(bq_elem, data));		\
		MEMBER_OF(CONTAINER_OF((bq_data *)pos, bq_elem, data),		\
			struct bl_head, OFFSET_OF(bq_elem, list)) != &(queue)->head;	\
		pos = n, n = (type **)MEMBER_OF(CONTAINER_OF(CONTAINER_OF((bq_data *)n,		\
			bq_elem, data)->list.direction, bq_elem, list), bq_data, OFFSET_OF(bq_elem, data)))

/*
 * API macros
 */
#define BQ_FOREACH_HEAD(pos, type, queue)		\
	BQ_FOREACH_DIRECTION(pos, type, queue, next)

#define BQ_FOREACH_HEAD_SAFE(pos, n, type, queue)		\
	BQ_FOREACH_DIRECTION_SAFE(pos, n, type, queue, next)

#define BQ_FOREACH_TAIL(pos, type, queue)		\
	BQ_FOREACH_DIRECTION(pos, type, queue, prev)

#define BQ_FOREACH_TAIL_SAFE(pos, n, type, queue)		\
	BQ_FOREACH_DIRECTION_SAFE(pos, n, type, queue, prev)

#define BQ_FOREACH BQ_FOREACH_HEAD

#define BQ_FOREACH_SAFE BQ_FOREACH_HEAD_SAFE

/*
 * static function definitions
 */
static inline void bq_init(struct bq_queue *queue) {
	BL_INIT_HEAD(&queue->head);
	queue->num = 0;
}

static inline int bq_is_empty(struct bq_queue *queue) {
	return (queue->num == 0);
}

static inline int bq_num_elem(struct bq_queue *queue) {
	return (queue->num);
}

static inline void bq_reverse(struct bq_queue *queue) {
	struct bl_head *ptr, *n;

	bl_for_each_safe(ptr, n, &queue->head) {
		SWAP(ptr->next, ptr->prev);
	}
	SWAP(queue->head.next, queue->head.prev);
}

static inline void bq_push_head(bq_data data, struct bq_queue *queue) {
	__bq_push(data, queue, 1);
}

static inline void bq_push_tail(bq_data data, struct bq_queue *queue) {
	__bq_push(data, queue, 0);
}

static inline void bq_push(bq_data data, struct bq_queue *queue) {
	__bq_push(data, queue, 0);
}

static inline bq_data bq_pop_head(struct bq_queue *queue) {
	return __bq_pop(queue, 1);
}

static inline bq_data bq_pop_tail(struct bq_queue *queue) {
	return __bq_pop(queue, 0);
}

static inline bq_data bq_pop(struct bq_queue *queue) {
	return __bq_pop(queue, 1);
}

static inline bq_data bq_peek_head(struct bq_queue *queue) {
	return __bq_peek(queue, 1);
}

static inline bq_data bq_peek_tail(struct bq_queue *queue) {
	return __bq_peek(queue, 0);
}

static inline bq_data bq_peek(struct bq_queue *queue) {
	return __bq_peek(queue, 1);
}

static inline void bq_destroy(struct bq_queue *queue, bq_cleanup_func func, bq_cleanup_args args) {
	bq_data data;

	while (queue->num > 0) {
		data = bq_pop(queue);
		if (func != NULL)
			func(data, args);
	}
}

static inline void __bq_push(bq_data data, struct bq_queue *queue, int head) {
	bq_elem *elem = (bq_elem *)malloc(sizeof(bq_elem));

	BL_INIT_HEAD(&elem->list);
	elem->data = data;

	if (head)
		bl_add(&elem->list, &queue->head);
	else
		bl_add_tail(&elem->list, &queue->head);

	queue->num++;
}

static inline bq_data __bq_pop(struct bq_queue *queue, int head) {
	struct bl_head *node;
	bq_data data;
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

static inline bq_data __bq_peek(struct bq_queue *queue, int head) {
	struct bl_head *node;
	bq_data data;

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
