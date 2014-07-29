#ifndef _BASIC_QUEUE_H
#define _BASIC_QUEUE_H

/*
 * Simple LIFO queue implementation.
 *
 */

#include "basic_list.h"
#include "basic.h"

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

static inline basic_ec bq_in_head(bq_data data, struct bq_queue *queue);

static inline basic_ec bq_in_tail(bq_data data, struct bq_queue *queue);

static inline basic_ec bq_in(bq_data data, struct bq_queue *queue);

static inline bq_data bq_out_head(struct bq_queue *queue);

static inline bq_data bq_out_tail(struct bq_queue *queue);

static inline bq_data bq_out(struct bq_queue *queue);

static inline bq_data bq_peek_head(struct bq_queue *queue);

static inline bq_data bq_peek_tail(struct bq_queue *queue);

static inline bq_data bq_peek(struct bq_queue *queue);

static inline void bq_destroy(struct bq_queue *queue, bq_cleanup_func func, bq_cleanup_args args);

/*
 * private functions
 */
static inline basic_ec __bq_in(bq_data data, struct bq_queue *queue, int head);
static inline bq_data __bq_out(struct bq_queue *queue, int head);
static inline bq_data __bq_peek(struct bq_queue *queue, int head);

#define bq_for_each_direction(pos, type, queue, direction)		\
	for(pos = (type **)member_of(container_of((queue)->head.direction, bq_elem, list),	\
			bq_data, offset_of(bq_elem, data));	\
		member_of(container_of((bq_data *)pos, bq_elem, data),		\
			struct bl_head, offset_of(bq_elem, list)) != &(queue)->head;	\
		pos = (type **)member_of(container_of(container_of((bq_data *)pos,		\
			bq_elem, data)->list.direction, bq_elem, list), bq_data, offset_of(bq_elem, data)))

#define bq_for_each_direction_safe(pos, n, type, queue, direction)	\
	for(pos = (type **)member_of(container_of((queue)->head.direction, bq_elem, list),	\
			bq_data, offset_of(bq_elem, data)),	\
		n = (type **)member_of(container_of(container_of((bq_data *)pos,		\
			bq_elem, data)->list.direction, bq_elem, list), bq_data, offset_of(bq_elem, data));		\
		member_of(container_of((bq_data *)pos, bq_elem, data),		\
			struct bl_head, offset_of(bq_elem, list)) != &(queue)->head;	\
		pos = n, n = (type **)member_of(container_of(container_of((bq_data *)n,		\
			bq_elem, data)->list.direction, bq_elem, list), bq_data, offset_of(bq_elem, data)))

/*
 * API macros
 */
#define bq_for_each_head(pos, type, queue)		\
	bq_for_each_direction(pos, type, queue, next)

#define bq_for_each_head_safe(pos, n, type, queue)		\
	bq_for_each_direction_safe(pos, n, type, queue, next)

#define bq_for_each_tail(pos, type, queue)		\
	bq_for_each_direction(pos, type, queue, prev)

#define bq_for_each_tail_safe(pos, n, type, queue)		\
	bq_for_each_direction_safe(pos, n, type, queue, prev)

#define bq_for_each bq_for_each_head

#define bq_for_each_safe bq_for_each_head_safe

/*
 * static function definitions
 */
static inline void bq_init(struct bq_queue *queue)
{
	BL_INIT_HEAD(&queue->head);
	queue->num = 0;
}

static inline int bq_is_empty(struct bq_queue *queue)
{
	return queue->num == 0;
}

static inline int bq_num_elem(struct bq_queue *queue)
{
	return queue->num;
}

static inline void bq_reverse(struct bq_queue *queue)
{
	struct bl_head *ptr, *n;

	bl_for_each_safe(ptr, n, &queue->head)
		swap(ptr->next, ptr->prev);

	swap(queue->head.next, queue->head.prev);
}

static inline basic_ec bq_in_head(bq_data data, struct bq_queue *queue)
{
	return __bq_in(data, queue, 1);
}

static inline basic_ec bq_in_tail(bq_data data, struct bq_queue *queue)
{
	return __bq_in(data, queue, 0);
}

static inline basic_ec bq_in(bq_data data, struct bq_queue *queue)
{
	return __bq_in(data, queue, 0);
}

static inline bq_data bq_out_head(struct bq_queue *queue)
{
	return __bq_out(queue, 1);
}

static inline bq_data bq_out_tail(struct bq_queue *queue)
{
	return __bq_out(queue, 0);
}

static inline bq_data bq_out(struct bq_queue *queue)
{
	return __bq_out(queue, 1);
}

static inline bq_data bq_peek_head(struct bq_queue *queue)
{
	return __bq_peek(queue, 1);
}

static inline bq_data bq_peek_tail(struct bq_queue *queue)
{
	return __bq_peek(queue, 0);
}

static inline bq_data bq_peek(struct bq_queue *queue)
{
	return __bq_peek(queue, 1);
}

static inline void bq_destroy(struct bq_queue *queue, bq_cleanup_func func, bq_cleanup_args args)
{
	bq_data data;

	while (queue->num > 0) {
		data = bq_out(queue);
		if (func != NULL)
			func(data, args);
	}
}

static inline basic_ec __bq_in(bq_data data, struct bq_queue *queue, int head)
{
	bq_elem *elem = (bq_elem *)malloc(sizeof(bq_elem));
	if (elem == NULL)
		return -BE_FAULT;

	BL_INIT_HEAD(&elem->list);
	elem->data = data;

	if (head)
		bl_add(&elem->list, &queue->head);
	else
		bl_add_tail(&elem->list, &queue->head);

	queue->num++;

	return BE_OK;
}

static inline bq_data __bq_out(struct bq_queue *queue, int head)
{
	struct bl_head *node;
	bq_data data;
	bq_elem *elem;

	if (head)
		node = queue->head.next;
	else
		node = queue->head.prev;

	if (node != &queue->head) {
		elem = container_of(node, bq_elem, list);
		data = elem->data;
		bl_del(node);
		queue->num--;
		free(elem);
		return data;
	} else
		return NULL;
}

static inline bq_data __bq_peek(struct bq_queue *queue, int head)
{
	struct bl_head *node;
	bq_data data;

	if (head)
		node = queue->head.next;
	else
		node = queue->head.prev;

	if (node != &queue->head) {
		data = container_of(node, bq_elem, list)->data;
		return data;
	} else
		return NULL;
}

#endif
