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

struct entry {
	int num;
	char *str;
	struct list_head ptrs;
};

int test_num1;
int test_num2;
int test_num3;

static void cleanup_func(struct entry *e, void *args) {
	int data = *((int *)args);

	test_num1 = 1;
	if (e->num == data%10)
		test_num2 = e->num;
	free (e->str);

	if (e->num == data/10) {
		test_num3 = 2;
		free(e);
	}
}

static void verify_queue(struct basic_queue *queue, int n, ...) {
	struct entry *tmp;
	va_list a_list;
	int num = n;

	va_start(a_list, n);

	assert_int_equal(num, queue->num);
	list_for_each_entry(tmp, &queue->list, ptrs) {
		assert_int_equal(va_arg(a_list, int), tmp->num);
		num--;
	}
	assert_int_equal(0, num);
}

static void test_init(void **state) {
	struct basic_queue queue;

	basic_queue_init(&queue);
	assert_int_equal(&queue.list, queue.list.next);
	assert_int_equal(&queue.list, queue.list.prev);
	assert_int_equal(0, queue.num);
}

static void test_push_pop_peek(void **state) {
	struct basic_queue queue;
	struct entry e1, e2, e3, e4, *e;

	basic_queue_init(&queue);
	e1.num = 1;
	e2.num = 2;
	e3.num = 3;
	e4.num = 4;
	verify_queue(&queue, 0);

	e = BASIC_QUEUE_POP_HEAD(struct entry, &queue, ptrs);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = BASIC_QUEUE_POP_TAIL(struct entry, &queue, ptrs);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = BASIC_QUEUE_POP(struct entry, &queue, ptrs);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = BASIC_QUEUE_PEEK_HEAD(struct entry, &queue, ptrs);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = BASIC_QUEUE_PEEK_TAIL(struct entry, &queue, ptrs);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);
	e = BASIC_QUEUE_PEEK(struct entry, &queue, ptrs);
	assert_int_equal(NULL, e);
	verify_queue(&queue, 0);

	BASIC_QUEUE_PUSH_TAIL(&e1, &queue, ptrs);
	verify_queue(&queue, 1, 1);
	BASIC_QUEUE_PUSH(&e2, &queue, ptrs);
	verify_queue(&queue, 2, 1, 2);
	BASIC_QUEUE_PUSH_HEAD(&e3, &queue, ptrs);
	verify_queue(&queue, 3, 3, 1, 2);
	BASIC_QUEUE_PUSH_TAIL(&e4, &queue, ptrs);
	verify_queue(&queue, 4, 3, 1, 2, 4);

	e = BASIC_QUEUE_PEEK(struct entry, &queue, ptrs);
	verify_queue(&queue, 4, 3, 1, 2, 4);
	assert_int_equal(3, e->num);
	e = BASIC_QUEUE_PEEK_TAIL(struct entry, &queue, ptrs);
	verify_queue(&queue, 4, 3, 1, 2, 4);
	assert_int_equal(4, e->num);
	e = BASIC_QUEUE_PEEK_HEAD(struct entry, &queue, ptrs);
	verify_queue(&queue, 4, 3, 1, 2, 4);
	assert_int_equal(3, e->num);

	e = BASIC_QUEUE_POP(struct entry, &queue, ptrs);
	verify_queue(&queue, 3, 1, 2, 4);
	assert_int_equal(3, e->num);
	e = BASIC_QUEUE_POP_TAIL(struct entry, &queue, ptrs);
	verify_queue(&queue, 2, 1, 2);
	assert_int_equal(4, e->num);
	e = BASIC_QUEUE_POP_HEAD(struct entry, &queue, ptrs);
	verify_queue(&queue, 1, 2);
	assert_int_equal(1, e->num);
	e = BASIC_QUEUE_POP_HEAD(struct entry, &queue, ptrs);
	verify_queue(&queue, 0);
	assert_int_equal(2, e->num);

	BASIC_QUEUE_PUSH_HEAD(&e2, &queue, ptrs);
	verify_queue(&queue, 1, 2);

	e = BASIC_QUEUE_PEEK(struct entry, &queue, ptrs);
	verify_queue(&queue, 1, 2);
	assert_int_equal(2, e->num);
	e = BASIC_QUEUE_PEEK_TAIL(struct entry, &queue, ptrs);
	verify_queue(&queue, 1, 2);
	assert_int_equal(2, e->num);
	e = BASIC_QUEUE_PEEK_HEAD(struct entry, &queue, ptrs);
	verify_queue(&queue, 1, 2);
	assert_int_equal(2, e->num);
	
	e = BASIC_QUEUE_POP_TAIL(struct entry, &queue, ptrs);
	verify_queue(&queue, 0);
	assert_int_equal(2, e->num);
}

static void test_is_empty(void **state) {
	struct entry e;
	struct basic_queue queue;

	basic_queue_init(&queue);
	assert_int_equal(1, basic_queue_is_empty(&queue));
	BASIC_QUEUE_PUSH(&e, &queue, ptrs);
	assert_int_equal(0, basic_queue_is_empty(&queue));
}

static void test_num_elem(void **state) {
	struct basic_queue queue;
	struct entry e1, e2, e3, e4;

	basic_queue_init(&queue);
	assert_int_equal(0, basic_queue_num_elem(&queue));

	BASIC_QUEUE_POP(struct entry, &queue, ptrs);
	assert_int_equal(0, basic_queue_num_elem(&queue));
	BASIC_QUEUE_PEEK(struct entry, &queue, ptrs);
	assert_int_equal(0, basic_queue_num_elem(&queue));

	BASIC_QUEUE_PUSH_TAIL(&e1, &queue, ptrs);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	BASIC_QUEUE_PUSH(&e2, &queue, ptrs);
	assert_int_equal(2, basic_queue_num_elem(&queue));
	BASIC_QUEUE_PUSH_HEAD(&e3, &queue, ptrs);
	assert_int_equal(3, basic_queue_num_elem(&queue));
	BASIC_QUEUE_PUSH_TAIL(&e4, &queue, ptrs);
	assert_int_equal(4, basic_queue_num_elem(&queue));

	BASIC_QUEUE_PEEK(struct entry, &queue, ptrs);
	assert_int_equal(4, basic_queue_num_elem(&queue));
	BASIC_QUEUE_PEEK_TAIL(struct entry, &queue, ptrs);
	assert_int_equal(4, basic_queue_num_elem(&queue));
	BASIC_QUEUE_PEEK_HEAD(struct entry, &queue, ptrs);
	assert_int_equal(4, basic_queue_num_elem(&queue));

	BASIC_QUEUE_POP(struct entry, &queue, ptrs);
	assert_int_equal(3, basic_queue_num_elem(&queue));
	BASIC_QUEUE_POP_TAIL(struct entry, &queue, ptrs);
	assert_int_equal(2, basic_queue_num_elem(&queue));
	BASIC_QUEUE_POP_HEAD(struct entry, &queue, ptrs);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	BASIC_QUEUE_POP_HEAD(struct entry, &queue, ptrs);
	assert_int_equal(0, basic_queue_num_elem(&queue));

	BASIC_QUEUE_PUSH_HEAD(&e2, &queue, ptrs);
	assert_int_equal(1, basic_queue_num_elem(&queue));

	BASIC_QUEUE_PEEK(struct entry, &queue, ptrs);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	BASIC_QUEUE_PEEK_TAIL(struct entry, &queue, ptrs);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	BASIC_QUEUE_PEEK_HEAD(struct entry, &queue, ptrs);
	assert_int_equal(1, basic_queue_num_elem(&queue));
	
	BASIC_QUEUE_POP_TAIL(struct entry, &queue, ptrs);
	assert_int_equal(0, basic_queue_num_elem(&queue));
}

static void test_destroy(void **state) {
	struct basic_queue queue;
	struct entry e1, e2, e3, *e;
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
	e = (struct entry *)malloc(sizeof(struct entry));
	e->num = 4;
	e->str = (char *)malloc(sizeof("four")+1);
	strcpy(e->str, "four");

	test_num1 = 0;
	test_num2 = 0;
	test_num3 = 0;
	temp = 43;

	BASIC_QUEUE_DESTROY(struct entry, &queue, ptrs, cleanup_func, (void *)&temp);
	assert_int_equal(0, test_num1);
	assert_int_equal(0, test_num2);
	assert_int_equal(1, basic_queue_is_empty(&queue));

	BASIC_QUEUE_PUSH(&e1, &queue, ptrs);
	BASIC_QUEUE_PUSH(&e2, &queue, ptrs);
	BASIC_QUEUE_PUSH(&e3, &queue, ptrs);
	BASIC_QUEUE_PUSH(e, &queue, ptrs);
	BASIC_QUEUE_DESTROY(struct entry, &queue, ptrs, cleanup_func, (void *)&temp);
	assert_int_equal(1, test_num1);
	assert_int_equal(3, test_num2);
	assert_int_equal(2, test_num3);
	assert_int_equal(1, basic_queue_is_empty(&queue));

	e = (struct entry *)malloc(sizeof(struct entry));
	e->num = 5;
	e->str = (char *)malloc(sizeof("five")+1);
	strcpy(e->str, "five");

	test_num1 = 0;
	BASIC_QUEUE_PUSH(e, &queue, ptrs);
	BASIC_QUEUE_DESTROY(struct entry, &queue, ptrs, NULL, (void *)&temp);
	assert_int_equal(0, test_num1);
	assert_int_equal(1, basic_queue_is_empty(&queue));
}

/* main function */
int main(void) {
	const UnitTest tests[] = {
		unit_test(test_init),
		unit_test(test_push_pop_peek),
		unit_test(test_is_empty),
		unit_test(test_num_elem),
		unit_test(test_destroy)
	};

	return run_tests(tests);
}