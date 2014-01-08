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
#include <string.h>
#include <cmocka.h>
#include "cmocka_sample.h"

int __wrap_return_int() {
	return mock_type(int);
}

int mock_sample() {
	return return_int();
}



static void test_sample(void **state) {
    assert_int_equal(1, mock_sample());
}

static void test_sample2(void **state) {
    (void) state;
    will_return(__wrap_return_int, 2);
    assert_int_equal(2, mock_sample());
}

int main(void) {
    const UnitTest tests[] = {
        unit_test(test_sample),
        unit_test(test_sample2),
    };
    return run_tests(tests);
}