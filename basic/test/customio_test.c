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
#include <stdio.h>
#include <customio.h>

static void test_is_ws(void **state) {
	assert_int_equal(1, customio_is_ws('\n'));
	assert_int_equal(1, customio_is_ws('\t'));
	assert_int_equal(1, customio_is_ws('\r'));
	assert_int_equal(1, customio_is_ws('\f'));
	assert_int_equal(1, customio_is_ws('\v'));
	assert_int_equal(1, customio_is_ws(' '));
	assert_int_equal(0, customio_is_ws('a'));
	assert_int_equal(0, customio_is_ws('1'));
	assert_int_equal(0, customio_is_ws('\b'));
	assert_int_equal(0, customio_is_ws('\0'));
}

static void test_get_before_ws(void **state) {
	char test_data1[] = "aabb cc\n dd";
	char test_data2[] = "aabbcc";
	char test_data3[] = "\taabbcc";
	char test_data4[1024];
	char *buf = NULL;
	char ws;
	int num = 0, i;
	FILE *f = NULL;

	f = fmemopen(test_data1, strlen(test_data1), "rb");
	assert_int_equal(0, customio_get_before_ws(f, &buf, 0, &num, &ws));
	assert_string_equal("aabb", buf);
	assert_int_equal(4, num);
	assert_int_equal(' ', ws);

	assert_int_equal(0, customio_get_before_ws_ignore(f, &buf, num+1, NULL, NULL));
	assert_string_equal("cc", buf);

	assert_int_equal(0, customio_get_before_ws_ignore(f, &buf, num+1, &num, &ws));
	assert_string_equal("dd", buf);
	assert_int_equal(2, num);
	assert_int_equal(EOF, ws);
	free(buf);
	buf = NULL;
	fclose(f);

	f = fmemopen(test_data2, strlen(test_data2), "rb");
	assert_int_equal(0, customio_get_before_ws(f, &buf, 0, &num, NULL));
	assert_string_equal("aabbcc", buf);
	assert_int_equal(6, num);
	fclose(f);

	f = fmemopen(test_data3, strlen(test_data3), "rb");
	assert_int_equal(0, customio_get_before_ws(f, &buf, num+1, &num, &ws));
	assert_string_equal("", buf);
	assert_int_equal(0, num);
	assert_int_equal('\t', ws);
	free(buf);
	buf = NULL;
	fclose(f);

	for (i = 0; i < 1023; i++)
		test_data4[i] = 'a';
	test_data4[600] = ' ';
	test_data4[1023] = '\0';

	f = fmemopen(test_data4, strlen(test_data4), "rb");
	assert_int_equal(0, customio_get_before_ws(f, &buf, 0, &num, &ws));
	assert_int_equal(600, num);
	assert_int_equal(' ', ws);

	assert_int_equal(0, customio_get_before_ws_ignore(f, &buf, num+1, &num, &ws));
	assert_int_equal(422, num);
	assert_int_equal(EOF, ws);
	free(buf);
	buf = NULL;
	fclose(f);

	/*
	 * below is test for abnormal cases
	 * let's take get_before_ws to be the representative for everyone
	 */
	 buf = test_data1;
	 num = 1000;
	 ws = 'T';
	 f = fmemopen(test_data4, strlen(test_data4), "wb");
	 assert_int_equal(
	 	CUSTOMIO_READ_ERROR,
	 	customio_get_before_ws(f, &buf, 0, &num, &ws)
	 ),
	 assert_int_equal(test_data1, buf);
	 assert_int_equal(1000, num);
	 assert_int_equal('T', ws);
	 fclose(f);
}

static void test_get_before_delim(void **state) {
	char test_data1[] = "aabbecc\n dd";
	char test_data2[] = "aabbcc";
	char test_data3[] = "\taabbcc";
	char test_data4[1024];
	char *buf = NULL;
	char ws;
	int num = 0, i;
	FILE *f = NULL;

	f = fmemopen(test_data1, strlen(test_data1), "rb");
	assert_int_equal(0, customio_get_before_delim(f, "e", &buf, 0, &num, &ws));
	assert_string_equal("aabb", buf);
	assert_int_equal(4, num);
	assert_int_equal('e', ws);

	customio_get_before_delim(f, "bd\t;=", &buf, num+1, NULL, NULL);
	assert_string_equal("ecc\n ", buf);

	customio_get_before_delim_ignore(f, "$*()", &buf, num+1, NULL, &ws);
	assert_string_equal("dd", buf);
	assert_int_equal(EOF, ws);
	fclose(f);

	f = fmemopen(test_data2, strlen(test_data2), "rb");
	customio_get_before_delim_ignore(f, "AV&EF", &buf, num+1, &num, NULL);
	assert_string_equal("aabbcc", buf);
	assert_int_equal(6, num);
	fclose(f);

	f = fmemopen(test_data3, strlen(test_data3), "rb");
	customio_get_before_delim(f, "cbd", &buf, num+1, &num, &ws);
	assert_string_equal("\taa", buf);
	assert_int_equal(3, num);
	assert_int_equal('b', ws);
	free(buf);
	buf = NULL;
	fclose(f);

	for (i = 0; i < 1023; i++)
		test_data4[i] = 'a';
	test_data4[1000] = '$';
	test_data4[1023] = '\0';

	f = fmemopen(test_data4, strlen(test_data4), "rb");
	customio_get_before_delim(f, "bc$", &buf, 0, &num, &ws);
	assert_int_equal(1000, num);
	assert_int_equal('$', ws);
	free(buf);
	fclose(f);
}

static void test_get_before_delim_or_ws(void **state) {
	char test_data1[] = "aabbecc\n dd";
	char test_data2[] = " \n\taabb\n cc";
	char test_data3[] = "\taabbcc";
	char test_data4[1024];
	char *buf = NULL;
	char ws;
	int num = 0, i;
	FILE *f = NULL;

	f = fmemopen(test_data1, strlen(test_data1), "rb");
	assert_int_equal(0, customio_get_before_delim_or_ws(f, "e", &buf, 0, &num, &ws));
	assert_string_equal("aabb", buf);
	assert_int_equal(4, num);
	assert_int_equal('e', ws);

	customio_get_before_delim_or_ws(f, "bd;=", &buf, num+1, NULL, NULL);
	assert_string_equal("ecc", buf);

	customio_get_before_delim_or_ws_ignore(f, "$*()", &buf, num+1, NULL, &ws);
	assert_string_equal("dd", buf);
	assert_int_equal(EOF, ws);
	fclose(f);

	f = fmemopen(test_data2, strlen(test_data2), "rb");
	customio_get_before_delim_or_ws_ignore(f, "cDB", &buf, num+1, &num, NULL);
	assert_string_equal("aabb", buf);
	assert_int_equal(4, num);
	free(buf);
	buf = NULL;
	fclose(f);

	f = fmemopen(test_data3, strlen(test_data3), "rb");
	customio_get_before_delim_or_ws(f, "cbd", &buf, 0, &num, &ws);
	assert_string_equal("", buf);
	assert_int_equal(0, num);
	assert_int_equal('\t', ws);

	customio_get_before_delim_or_ws(f, "cbd", &buf, num+1, NULL, NULL);
	assert_string_equal("", buf);
	free(buf);
	buf = NULL;
	fclose(f);

	for (i = 0; i < 1023; i++)
		test_data4[i] = 'a';
	test_data4[768] = '$';
	test_data4[1023] = '\0';

	f = fmemopen(test_data4, strlen(test_data4), "rb");
	customio_get_before_delim_or_ws(f, "bc$", &buf, 1, &num, &ws);
	assert_int_equal(768, num);
	assert_int_equal('$', ws);
	free(buf);
	fclose(f);
}

static void test_get_till_delim(void **state) {
	char test_data1[] = "aabbecc\n dd";
	char test_data2[1024];
	char *buf = NULL;
	int num = 0, i;
	FILE *f = NULL;

	f = fmemopen(test_data1, strlen(test_data1), "rb");
	assert_int_equal(0, customio_get_till_delim(f, "= cCe", &buf, 0, &num));
	assert_string_equal("aabbe", buf);
	assert_int_equal(5, num);

	customio_get_till_delim(f, "b\t;=", &buf, num+1, NULL);
	assert_string_equal("cc\n dd", buf);
	free(buf);
	buf = NULL;
	fclose(f);

	for (i = 0; i < 1023; i++)
		test_data2[i] = 'a';
	test_data2[768] = '$';
	test_data2[1023] = '\0';

	f = fmemopen(test_data2, strlen(test_data2), "rb");
	customio_get_till_delim(f, "bc$", &buf, 2, &num);
	assert_int_equal(769, num);
	free(buf);
	buf = NULL;
	fclose(f);
}

static void test_eat_ws(void **state) {
	char test_data[] = "a \tbbc \n \t  \v";
	int count = 0;
	FILE *f = NULL;

	f = fmemopen(test_data, strlen(test_data), "rb");
	assert_int_equal(0, customio_eat_ws(f, &count));
	assert_int_equal(0, ftell(f));
	assert_int_equal(0, count);

	fseek(f, 1, SEEK_CUR);
	customio_eat_ws(f, NULL);
	assert_int_equal(3, ftell(f));
	assert_int_equal('b', fgetc(f));

	fseek(f, 2, SEEK_CUR);
	customio_eat_ws(f, &count);
	assert_int_equal(1, feof(f));
	assert_int_equal(7, count);
	fclose(f);
}

static void test_trim_before(void **state) {
	char str1[] = "  \n \t aa bb \n";
	char str2[] = "a b \n ccd e ";
	char str3[] = "\n\v\t  \n\r ";

	customio_trim_before(str1);
	assert_string_equal("aa bb \n", str1);

	customio_trim_before(str2);
	assert_string_equal("a b \n ccd e ", str2);

	customio_trim_before(str3);
	assert_string_equal("", str3);
}

static void test_trim_after(void **state) {
	char str1[] = "  \n \t aa bb \n";
	char str2[] = "  a b \n ccd e";
	char str3[] = "\n\v\t  \n\r ";

	customio_trim_after(str1);
	assert_string_equal("  \n \t aa bb", str1);

	customio_trim_after(str2);
	assert_string_equal("  a b \n ccd e", str2);

	customio_trim_after(str3);
	assert_string_equal("", str3);
}

static void test_trim(void **state) {
	char str1[] = "  \n \t aa bb \n";
	char str2[] = "  a b \n ccd e";
	char str3[] = "a b \n ccd e\t\t";
	char str4[] = "\n\v\t  \n\r ";

	customio_trim(str1);
	assert_string_equal("aa bb", str1);

	customio_trim(str2);
	assert_string_equal("a b \n ccd e", str2);

	customio_trim(str3);
	assert_string_equal("a b \n ccd e", str3);

	customio_trim(str4);
	assert_string_equal("", str4);
}

int main(void) {
	const UnitTest tests[] = {
		unit_test(test_is_ws),
		unit_test(test_get_before_ws),
		unit_test(test_get_before_delim),
		unit_test(test_get_before_delim_or_ws),
		unit_test(test_get_till_delim),
		unit_test(test_eat_ws),
		unit_test(test_trim_before),
		unit_test(test_trim_after),
		unit_test(test_trim)
	};

	return run_tests(tests);
}