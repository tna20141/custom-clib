/*
 * Copyright 2008 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>
#include <basic_general.h>
#include <basic_queue.h>

struct test_entry {
	int num;
	char *str;
};
typedef struct test_entry teste;

int test_num1;
int test_num2;
int test_num3;

static void cleanup_func(void *e, void *args) {
	int data = *((int *)args);

	test_num1 = 1;
	if (((teste *)e)->num == data%10)
		test_num2 = ((teste *)e)->num;
	free (((teste *)e)->str);

	if (((teste *)e)->num == data/10) {
		test_num3 = 2;
		free(e);
	}
}

static void verify_queue(struct basic_queue *queue, int n, ...) {
	bq_elem *elem;
	va_list a_list;
	int num = n;

	va_start(a_list, n);

	assert_int_equal(num, queue->num);
	bl_for_each_entry(elem, &queue->head, list) {
		assert_int_equal(va_arg(a_list, int), ((teste *)(elem->data))->num);
		num--;
	}
	assert_int_equal(0, num);
}

static void test_init(void **state) {
	struct basic_queue queue;

	basic_queue_init(&queue);
	assert_int_equal(&queue.head, queue.head.next);
	assert_int_equal(&queue.head, queue.head.prev);
	assert_int_equal(0, queue.num);
}

static void test_push_pop_peek(void **state) {
	struct basic_queue queue;
	teste e1, e2, e3, e4, *e;

	basic_queue_init(&queue);
	e1.num = 1;
	e2.num = 2;
	e3.num = 3;
	e4.num = 4;
	verify_queue(&queue, 0);

	e = (teste *)basic_queue_pop_head(&queue);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = (teste *)basic_queue_pop_tail(&queue);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = (teste *)basic_queue_pop(&queue);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = (teste *)basic_queue_peek_head(&queue);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = (teste *)basic_queue_peek_tail(&queue);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = (teste *)basic_queue_peek(&queue);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);

	basic_queue_push_tail((void *)&e1, &queue);
	verify_queue(&queue, 1, 1);
	basic_queue_push((void *)&e2, &queue);
	verify_queue(&queue, 2, 1, 2);
	basic_queue_push_head((void *)&e3, &queue);
	verify_queue(&queue, 3, 3, 1, 2);
	basic_queue_push_tail((void *)&e4, &queue);
	verify_queue(&queue, 4, 3, 1, 2, 4);

	e = (teste *)basic_queue_peek(&queue);
	verify_queue(&queue, 4, 3, 1, 2, 4);
	assert_int_equal(3, e->num);
	e = (teste *)basic_queue_peek_tail(&queue);
	verify_queue(&queue, 4, 3, 1, 2, 4);
	assert_int_equal(4, e->num);
	e = (teste *)basic_queue_peek_head(&queue);
	verify_queue(&queue, 4, 3, 1, 2, 4);
	assert_int_equal(3, e->num);

	e = (teste *)basic_queue_pop(&queue);
	verify_queue(&queue, 3, 1, 2, 4);
	assert_int_equal(3, e->num);
	e = (teste *)basic_queue_pop_tail(&queue);
	verify_queue(&queue, 2, 1, 2);
	assert_int_equal(4, e->num);
	e = (teste *)basic_queue_pop_head(&queue);
	verify_queue(&queue, 1, 2);
	assert_int_equal(1, e->num);
	e = (teste *)basic_queue_pop_head(&queue);
	verify_queue(&queue, 0);
	assert_int_equal(2, e->num);

	basic_queue_push_head((void *)&e2, &queue);
	verify_queue(&queue, 1, 2);

	e = (teste *)basic_queue_peek(&queue);
	verify_queue(&queue, 1, 2);
	assert_int_equal(2, e->num);
	e = (teste *)basic_queue_peek_tail(&queue);
	verify_queue(&queue, 1, 2);
	assert_int_equal(2, e->num);
	e = (teste *)basic_queue_peek_head(&queue);
	verify_queue(&queue, 1, 2);
	assert_int_equal(2, e->num);
	
	e = (teste *)basic_queue_pop_tail(&queue);
	verify_queue(&queue, 0);
	assert_int_equal(2, e->num);
}

static void test_is_empty(void **state) {
	teste e;
	struct basic_queue queue;

	basic_queue_init(&queue);
	assert_int_equal(1, basic_queue_is_empty(&queue));
	basic_queue_push((void *)&e, &queue);
	assert_int_equal(0, basic_queue_is_empty(&queue));
}

static void test_reverse(void **state) {
	struct basic_queue queue;
	teste e1, e2, e3, e4;

	basic_queue_init(&queue);
	e1.num = 1;
	e2.num = 2;
	e3.num = 3;
	e4.num = 4;

	basic_queue_reverse(&queue);
	verify_queue(&queue, 0);

	basic_queue_push((void *)&e1, &queue);
	basic_queue_reverse(&queue);
	verify_queue(&queue, 1, 1);

	basic_queue_push((void *)&e2, &queue);
	basic_queue_push((void *)&e3, &queue);
	basic_queue_push((void *)&e4, &queue);
	basic_queue_reverse(&queue);
	verify_queue(&queue, 4, 4, 3, 2, 1);
	basic_queue_reverse(&queue);
	verify_queue(&queue, 4, 1, 2, 3, 4);

	basic_queue_pop(&queue);
	basic_queue_reverse(&queue);
	verify_queue(&queue, 3, 4, 3, 2);

	basic_queue_pop_tail(&queue);
	basic_queue_reverse(&queue);
	verify_queue(&queue, 2, 3, 4);
}

static void test_foreach(void **state) {
	struct basic_queue queue;
	teste e1, e2, e3, e4, **pos, **tmp;
	int n = 0;

	basic_queue_init(&queue);
	e1.num = 1;
	e2.num = 2;
	e3.num = 3;
	e4.num = 4;

	BASIC_QUEUE_FOREACH_HEAD(pos, teste, &queue)
		n++;
	assert_int_equal(0, n);
	BASIC_QUEUE_FOREACH_HEAD_SAFE(pos, tmp, teste, &queue)
		n++;
	assert_int_equal(0, n);
	BASIC_QUEUE_FOREACH_TAIL(pos, teste, &queue)
		n++;
	assert_int_equal(0, n);
	BASIC_QUEUE_FOREACH_TAIL_SAFE(pos, tmp, teste, &queue)
		n++;
	assert_int_equal(0, n);
	BASIC_QUEUE_FOREACH(pos, teste, &queue)
		n++;
	assert_int_equal(0, n);
	BASIC_QUEUE_FOREACH_SAFE(pos, tmp, teste, &queue)
		n++;
	assert_int_equal(0, n);

	basic_queue_push((void *)&e1, &queue);
	BASIC_QUEUE_FOREACH_HEAD(pos, teste, &queue)
		n += (*pos)->num;
	assert_int_equal(1, n);
	BASIC_QUEUE_FOREACH_HEAD_SAFE(pos, tmp, teste, &queue)
		n += (*pos)->num;
	assert_int_equal(2, n);
	BASIC_QUEUE_FOREACH_TAIL(pos, teste, &queue)
		n += (*pos)->num;
	assert_int_equal(3, n);
	BASIC_QUEUE_FOREACH_TAIL_SAFE(pos, tmp, teste, &queue)
		n += (*pos)->num;
	assert_int_equal(4, n);
	BASIC_QUEUE_FOREACH(pos, teste, &queue)
		n += (*pos)->num;
	assert_int_equal(5, n);
	BASIC_QUEUE_FOREACH_SAFE(pos, tmp, teste, &queue)
		n += (*pos)->num;
	assert_int_equal(6, n);

	basic_queue_push((void *)&e2, &queue);
	basic_queue_push((void *)&e3, &queue);
	basic_queue_push((void *)&e4, &queue);
	n = 0;
	BASIC_QUEUE_FOREACH_HEAD(pos, teste, &queue) {
		n++;
		assert_int_equal(n, (*pos)->num);
	}
	n = 0;
	BASIC_QUEUE_FOREACH_HEAD_SAFE(pos, tmp, teste, &queue) {
		n++;
		assert_int_equal(n, (*pos)->num);
	}
	n = 5;
	BASIC_QUEUE_FOREACH_TAIL(pos, teste, &queue) {
		n--;
		assert_int_equal(n, (*pos)->num);
	}
	n = 5;
	BASIC_QUEUE_FOREACH_TAIL_SAFE(pos, tmp, teste, &queue) {
		n--;
		assert_int_equal(n, (*pos)->num);
	}
	n = 0;
	BASIC_QUEUE_FOREACH(pos, teste, &queue) {
		n++;
		assert_int_equal(n, (*pos)->num);
	}
	n = 0;
	BASIC_QUEUE_FOREACH_SAFE(pos, tmp, teste, &queue) {
		n++;
		assert_int_equal(n, (*pos)->num);
	}

	BASIC_QUEUE_FOREACH_HEAD_SAFE(pos, tmp, teste, &queue) {
		basic_queue_pop_head(&queue);
	}
	assert_int_equal(1, basic_queue_is_empty(&queue));

	basic_queue_push((void *)&e2, &queue);
	basic_queue_push((void *)&e3, &queue);
	basic_queue_push((void *)&e4, &queue);
	BASIC_QUEUE_FOREACH_TAIL_SAFE(pos, tmp, teste, &queue) {
		basic_queue_pop_tail(&queue);
	}
	assert_int_equal(1, basic_queue_is_empty(&queue));

	basic_queue_push((void *)&e2, &queue);
	basic_queue_push((void *)&e3, &queue);
	basic_queue_push((void *)&e4, &queue);
	BASIC_QUEUE_FOREACH_SAFE(pos, tmp, teste, &queue) {
		basic_queue_pop(&queue);
	}
	assert_int_equal(1, basic_queue_is_empty(&queue));
}

static void test_num_elem(void **state) {
	struct basic_queue queue;
	teste e1, e2, e3, e4;

	basic_queue_init(&queue);
	assert_int_equal(0, basic_queue_num_elem(&queue));

	basic_queue_pop(&queue);
	assert_int_equal(0, basic_queue_num_elem(&queue));
	basic_queue_peek(&queue);
	assert_int_equal(0, basic_queue_num_elem(&queue));

	basic_queue_push_tail((void *)&e1, &queue);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	basic_queue_push((void *)&e2, &queue);
	assert_int_equal(2, basic_queue_num_elem(&queue));
	basic_queue_push_head((void *)&e3, &queue);
	assert_int_equal(3, basic_queue_num_elem(&queue));
	basic_queue_push_tail((void *)&e4, &queue);
	assert_int_equal(4, basic_queue_num_elem(&queue));

	basic_queue_peek(&queue);
	assert_int_equal(4, basic_queue_num_elem(&queue));
	basic_queue_peek_tail(&queue);
	assert_int_equal(4, basic_queue_num_elem(&queue));
	basic_queue_peek_head(&queue);
	assert_int_equal(4, basic_queue_num_elem(&queue));

	basic_queue_pop(&queue);
	assert_int_equal(3, basic_queue_num_elem(&queue));
	basic_queue_pop_tail(&queue);
	assert_int_equal(2, basic_queue_num_elem(&queue));
	basic_queue_pop_head(&queue);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	basic_queue_pop_head(&queue);
	assert_int_equal(0, basic_queue_num_elem(&queue));

	basic_queue_push_head((void *)&e2, &queue);
	assert_int_equal(1, basic_queue_num_elem(&queue));

	basic_queue_peek(&queue);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	basic_queue_peek_tail(&queue);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	basic_queue_peek_head(&queue);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	
	basic_queue_pop_tail(&queue);
	assert_int_equal(0, basic_queue_num_elem(&queue));
}

static void test_destroy(void **state) {
	struct basic_queue queue;
	teste e1, e2, e3, *e;
	int temp;

	basic_queue_init(&queue);
	e1.num = 1;
	e1.str = (char *)malloc(sizeof("one")+1);
	strcpy(e1.str, "one");
	e2.num = 2;
	e2.str = (char *)malloc(sizeof("two")+1);
	strcpy(e2.str, "two");
	e3.num = 3;
	e3.str = (char *)malloc(sizeof("three")+1);
	strcpy(e3.str, "three");
	e = (teste *)malloc(sizeof(teste));
	e->num = 4;
	e->str = (char *)malloc(sizeof("four")+1);
	strcpy(e->str, "four");

	test_num1 = 0;
	test_num2 = 0;
	test_num3 = 0;
	temp = 43;

	basic_queue_destroy(&queue, cleanup_func, (void *)&temp);
	assert_int_equal(0, test_num1);
	assert_int_equal(0, test_num2);
	assert_int_equal(1, basic_queue_is_empty(&queue));

	basic_queue_push((void *)&e1, &queue);
	basic_queue_push((void *)&e2, &queue);
	basic_queue_push((void *)&e3, &queue);
	basic_queue_push((void *)e, &queue);
	basic_queue_destroy(&queue, cleanup_func, (void *)&temp);
	assert_int_equal(1, test_num1);
	assert_int_equal(3, test_num2);
	assert_int_equal(2, test_num3);
	assert_int_equal(1, basic_queue_is_empty(&queue));

	e = (teste *)malloc(sizeof(teste));
	e->num = 5;
	e->str = (char *)malloc(sizeof("five")+1);
	strcpy(e->str, "five");

	test_num1 = 0;
	basic_queue_push((void *)e, &queue);
	basic_queue_destroy(&queue, NULL, (void *)&temp);
	assert_int_equal(0, test_num1);
	assert_int_equal(1, basic_queue_is_empty(&queue));
}

/* main function */
int main(void) {
	const UnitTest tests[] = {
		unit_test(test_init),
		unit_test(test_push_pop_peek),
		unit_test(test_is_empty),
		unit_test(test_reverse),
		unit_test(test_num_elem),
		unit_test(test_foreach),
		unit_test(test_destroy)
	};

	return run_tests(tests);
}