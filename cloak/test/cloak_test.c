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
#include <cloak_10.h>

#define M_REPEAT1(S, N, ...) S

#define M_REPEAT2(S, N, ...) \
	assert_int_equal((N)-(S), CLOAK_CAT(int, S));

#define M_REPEAT3(S, N, X, Y) (X) += (Y);

#define M_REPEAT_COMMA(S, N, ...) \
	CLOAK_COMMA_IF(CLOAK_DEC(CLOAK_DEC(S))) S

int test_function() {
	return 4;
}

static void test_pasting_cat(void **state) {
	assert_int_equal(12345, CLOAK_CAT(123, 45));
	assert_int_equal(4, CLOAK_CAT(test_, CLOAK_CAT(funct, ion)) ());
	assert_int_equal("2233 , 55", CLOAK_STRINGIFY(CLOAK_CAT(22, 33 , 55)));
}

static void test_stringify(void **state) {
	assert_int_equal("(abcd)", CLOAK_STRINGIFY((abcd)));
	assert_int_equal("1bb cc", CLOAK_STRINGIFY(CLOAK_CAT(1, bb cc)));
	assert_int_equal("a\nb", CLOAK_STRINGIFY(a\nb));
}

static void test_inc(void **state) {
	assert_int_equal(1, CLOAK_INC(0));
	assert_int_equal(3, CLOAK_INC(2));
}

static void test_dec(void **state) {
	assert_int_equal(0, CLOAK_DEC(0));
	assert_int_equal(3, CLOAK_DEC(4));
}

static void test_repeat(void **state) {
	int int1 = 4, int2 = 3, int3 = 2, int4 = 1, int5 = 0;

	assert_int_equal("0 1 2 3 4 5 6 7 8 9", CLOAK_STRINGIFY(
		CLOAK_RUN_LOOP(CLOAK_REPEAT(0, 10, M_REPEAT1, ~))
	));
	CLOAK_RUN_LOOP(CLOAK_REPEAT(1, 5, M_REPEAT2, ~));

	int1 = 0;
	CLOAK_RUN_LOOP(CLOAK_REPEAT(2, 6, M_REPEAT3, int1, 2 * 3));
	assert_int_equal(36, int1);
}

static void test_comma(void **state) {
	int a[4] = {CLOAK_RUN_LOOP(CLOAK_REPEAT(2, 4, M_REPEAT_COMMA, ~))};
	int i;

	for (i = 0; i < 4; i++) {
		assert_int_equal(i+2, a[i]);
	}
}

int main(void) {
	const UnitTest tests[] = {
		unit_test(test_pasting_cat),
		unit_test(test_stringify),
		unit_test(test_inc),
		unit_test(test_dec),
		unit_test(test_repeat),
		unit_test(test_comma)
	};

	return run_tests(tests);
}

#undef M_REPEAT_COMMA
#undef M_REPEAT3
#undef M_REPEAT2
#undef M_REPEAT1