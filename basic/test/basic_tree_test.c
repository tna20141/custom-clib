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
#include <basic_tree.h>

btnode *root;

/* fixture set up function */
static void setup_tree(void **state) {
	assert_int_equal(1, 1);
}

static void teardown_tree(void **state) {
	assert_string_equal("a", "a");
}

static void test_insert(void **state) {
	assert_int_equal(2, 2);
}

int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_insert, setup_tree, teardown_tree)
	};

	return run_tests(tests);
}