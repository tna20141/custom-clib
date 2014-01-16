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
#include <basic_stack.h>

struct test_entry {
	int num;
	char *str;
};
typedef struct test_entry teste;

int test_num1;
int test_num2;

static void cleanup_func(void *e, void *args) {
	int data = *((int *)args);

	test_num1 = 1;
	if (((teste *)e)->num == data%10)
		test_num2 = ((teste *)e)->num;
	free (((teste *)e)->str);

	if (((teste *)e)->num == data/10) {
		test_num1 = 2;
		free(e);
	}
}

static void verify_stack(struct basic_stack *stack, int n, ...) {
	int i;
	bs_elem *ptr;
	va_list a_list;

	va_start(a_list, n);

	assert_int_equal(n, stack->num);
	for (i = 0, ptr = stack->top; i < n; i++, ptr = ptr->next) {
		assert_int_equal(va_arg(a_list, int), ((teste *)(ptr->data))->num);
	}
	assert_int_equal(NULL, ptr);
}

static void test_init(void **state) {
	struct basic_stack stack;

	basic_stack_init(&stack);
	assert_int_equal(NULL, stack.top);
	assert_int_equal(0, stack.num);
}

static void test_push_pop_peek(void **state) {
	struct basic_stack stack;
	teste e1, e2, e3, e4, *e;

	basic_stack_init(&stack);
	e1.num = 1;
	e2.num = 2;
	e3.num = 3;
	e4.num = 4;

	basic_stack_push((void *)&e1, &stack);
	verify_stack(&stack, 1, 1);

	e = (teste *)basic_stack_peek(&stack);
	verify_stack(&stack, 1, 1);
	assert_int_equal(1, e->num);

	basic_stack_push((void *)&e2, &stack);
	verify_stack(&stack, 2, 2, 1);

	e = (teste *)basic_stack_peek(&stack);
	verify_stack(&stack, 2, 2, 1);
	assert_int_equal(2, e->num);

	e = (teste *)basic_stack_pop(&stack);
	verify_stack(&stack, 1, 1);
	assert_int_equal(2, e->num);

	e = (teste *)basic_stack_peek(&stack);
	verify_stack(&stack, 1, 1);
	assert_int_equal(1, e->num);

	basic_stack_push((void *)&e3, &stack);
	verify_stack(&stack, 2, 3, 1);

	basic_stack_push((void *)&e4, &stack);
	verify_stack(&stack, 3, 4, 3, 1);

	e = (teste *)basic_stack_peek(&stack);
	verify_stack(&stack, 3, 4, 3, 1);
	assert_int_equal(4, e->num);

	e = (teste *)basic_stack_pop(&stack);
	verify_stack(&stack, 2, 3, 1);
	assert_int_equal(4, e->num);

	e = (teste *)basic_stack_peek(&stack);
	verify_stack(&stack, 2, 3, 1);
	assert_int_equal(3, e->num);

	e = (teste *)basic_stack_pop(&stack);
	verify_stack(&stack, 1, 1);
	assert_int_equal(3, e->num);

	e = (teste *)basic_stack_pop(&stack);
	verify_stack(&stack, 0);
	assert_int_equal(1, e->num);

	e = (teste *)basic_stack_pop(&stack);
	assert_int_equal(NULL, e);

	e = (teste *)basic_stack_peek(&stack);
	assert_int_equal(NULL, e);
}

static void test_is_empty(void **state) {
	teste e;
	struct basic_stack stack;

	basic_stack_init(&stack);
	assert_int_equal(1, basic_stack_is_empty(&stack));
	basic_stack_push((void *)&e, &stack);
	assert_int_equal(0, basic_stack_is_empty(&stack));
}

static void test_foreach(void **state) {
	struct basic_stack stack;
	teste e1, e2, e3, **e, **ee;
	int n = 0;

	basic_stack_init(&stack);
	e1.num = 1;
	e2.num = 2;
	e3.num = 3;

	BASIC_STACK_FOREACH(e, teste, &stack)
		n++;
	assert_int_equal(0, n);
	BASIC_STACK_FOREACH_SAFE(e, ee, teste, &stack)
		n++;
	assert_int_equal(0, n);

	basic_stack_push((void *)&e1, &stack);
	BASIC_STACK_FOREACH(e, teste, &stack) {
		n++;
		n += (*e)->num;
	}
	assert_int_equal(2, n);
	BASIC_STACK_FOREACH_SAFE(e, ee, teste, &stack) {
		n++;
		n += (*e)->num;
		n += (*e)->num;
	}
	assert_int_equal(5, n);

	basic_stack_push((void *)&e2, &stack);
	n = 2;
	BASIC_STACK_FOREACH(e, teste, &stack) {
		assert_int_equal(n, (*e)->num);
		n--;
	}
	n = 2;
	BASIC_STACK_FOREACH_SAFE(e, ee, teste, &stack) {
		assert_int_equal(n, (*e)->num);
		n--;
	}

	basic_stack_push((void *)&e3, &stack);
	n = 3;
	BASIC_STACK_FOREACH(e, teste, &stack) {
		assert_int_equal(n, (*e)->num);
		n--;
	}
	n = 3;
	BASIC_STACK_FOREACH_SAFE(e, ee, teste, &stack) {
		assert_int_equal(n, (*e)->num);
		n--;
	}

	BASIC_STACK_FOREACH_SAFE(e, ee, teste, &stack) {
		basic_stack_pop(&stack);
	}
	assert_int_equal(1, basic_stack_is_empty(&stack));
}

static void test_num_elem(void **state) {
	struct basic_stack stack;
	teste e1, e2, e3, e4, *e;

	basic_stack_init(&stack);
	assert_int_equal(0, basic_stack_num_elem(&stack));

	basic_stack_push((void *)&e1, &stack);
	assert_int_equal(1, basic_stack_num_elem(&stack));

	basic_stack_peek(&stack);
	assert_int_equal(1, basic_stack_num_elem(&stack));

	basic_stack_push((void *)&e2, &stack);
	assert_int_equal(2, basic_stack_num_elem(&stack));

	basic_stack_pop(&stack);
	assert_int_equal(1, basic_stack_num_elem(&stack));

	basic_stack_peek(&stack);
	assert_int_equal(1, basic_stack_num_elem(&stack));

	basic_stack_push((void *)&e3, &stack);
	basic_stack_push((void *)&e4, &stack);
	assert_int_equal(3, basic_stack_num_elem(&stack));

	basic_stack_pop(&stack);
	basic_stack_pop(&stack);
	assert_int_equal(1, basic_stack_num_elem(&stack));

	basic_stack_pop(&stack);
	assert_int_equal(0, basic_stack_num_elem(&stack));

	basic_stack_pop(&stack);
	assert_int_equal(0, basic_stack_num_elem(&stack));

	basic_stack_peek(&stack);
	assert_int_equal(0, basic_stack_num_elem(&stack));
}

static void test_destroy(void **state) {
	struct basic_stack stack;
	teste e1, e2, e3, e4, *e;
	int temp = 1;

	basic_stack_init(&stack);
	e1.num = 1;
	e1.str = (char *)malloc(sizeof("one")+1);
	strcpy(e1.str, "one");
	e2.num = 2;
	e2.str = (char *)malloc(sizeof("two")+1);
	strcpy(e2.str, "two");
	e3.num = 3;
	e3.str = (char *)malloc(sizeof("three")+1);
	strcpy(e3.str, "three");
	e4.num = 4;
	e4.str = (char *)malloc(sizeof("four")+1);
	strcpy(e4.str, "four");
	e = (teste *)malloc(sizeof(teste));
	e->num = 5;
	e->str = (char *)malloc(sizeof("five")+1);
	strcpy(e->str, "five");

	basic_stack_destroy(&stack, cleanup_func, (void *)&temp);
	assert_int_equal(0, test_num1);
	assert_int_equal(0, test_num2);
	assert_int_equal(1, basic_stack_is_empty(&stack));

	basic_stack_push((void *)&e1, &stack);
	basic_stack_destroy(&stack, cleanup_func, (void *)&temp);
	assert_int_equal(1, test_num1);
	assert_int_equal(1, test_num2);
	assert_int_equal(NULL, stack.top);
	assert_int_equal(1, basic_stack_is_empty(&stack));

	basic_stack_push((void *)e, &stack);
	basic_stack_push((void *)&e2, &stack);
	basic_stack_push((void *)&e3, &stack);
	basic_stack_push((void *)&e4, &stack);
	temp = 53;
	basic_stack_destroy(&stack, cleanup_func, (void *)&temp);
	assert_int_equal(3, test_num2);
	assert_int_equal(2, test_num1);
	assert_int_equal(NULL, stack.top);
	assert_int_equal(1, basic_stack_is_empty(&stack));

	e = (teste *)malloc(sizeof(teste));
	e->num = 5;
	e->str = (char *)malloc(sizeof("five")+1);
	strcpy(e->str, "five");

	test_num1 = 0;
	basic_stack_push((void *)e, &stack);
	basic_stack_destroy(&stack, NULL, NULL);
	assert_int_equal(0, test_num1);
	assert_int_equal(1, basic_stack_is_empty(&stack));
}

/* main function */
int main(void) {
	const UnitTest tests[] = {
		unit_test(test_init),
		unit_test(test_push_pop_peek),
		unit_test(test_is_empty),
		unit_test(test_foreach),
		unit_test(test_num_elem),
		unit_test(test_destroy)
		
	};

	return run_tests(tests);
}