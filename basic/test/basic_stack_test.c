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
#include <basic_stack.h>

struct entry {
	int data;
	bs_elem next;
};

static void test_init(void **state) {
	int i = 0;
	struct entry *a, b;
	struct basic_stack stack;
	basic_stack_init(&stack);
	BASIC_STACK_FOREACH(a, &stack, next) {
		i++;
	}
	// BASIC_STACK_DESTROY(struct entry, next, &stack, NULL, NULL);
	assert_int_equal(0, i);
	return;
}

/* main function */
int main(void) {
	const UnitTest tests[] = {
		unit_test(test_init)
	};

	return run_tests(tests);
}