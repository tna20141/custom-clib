#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gtest/gtest.h"
#include "customio.h"

TEST(IsWs, IsWs) {
	EXPECT_EQ(1, customio_is_ws('\n'));
	EXPECT_EQ(1, customio_is_ws('\t'));
	EXPECT_EQ(1, customio_is_ws('\r'));
	EXPECT_EQ(1, customio_is_ws('\f'));
	EXPECT_EQ(1, customio_is_ws('\v'));
	EXPECT_EQ(1, customio_is_ws(' '));
	EXPECT_EQ(0, customio_is_ws('a'));
	EXPECT_EQ(0, customio_is_ws('1'));
	EXPECT_EQ(0, customio_is_ws('\b'));
	EXPECT_EQ(0, customio_is_ws('\0'));
}

TEST(GetBefore, Ws) {
	char test_data1[] = "aabb cc\n dd";
	char test_data2[] = "aabbcc";
	char test_data3[] = "\taabbcc";
	char *buf;
	char ws;
	int num = 0;
	FILE *f = NULL;

	f = fmemopen(test_data1, strlen(test_data1), "rb");
	customio_get_before_ws(f, &buf, &num, &ws);
	EXPECT_STREQ("aabb", buf);
	EXPECT_EQ(4, num);
	EXPECT_EQ(' ', ws);
	free(buf);

	customio_get_before_ws_ignore(f, &buf, NULL, NULL);
	EXPECT_STREQ("cc", buf);
	free(buf);

	customio_get_before_ws_ignore(f, &buf, NULL, &ws);
	EXPECT_STREQ("dd", buf);
	EXPECT_EQ(EOF, ws);
	free(buf);
	fclose(f);

	f = fmemopen(test_data2, strlen(test_data2), "rb");
	customio_get_before_ws(f, &buf, &num, NULL);
	EXPECT_STREQ("aabbcc", buf);
	EXPECT_EQ(6, num);
	free(buf);
	fclose(f);

	f = fmemopen(test_data3, strlen(test_data3), "rb");
	customio_get_before_ws(f, &buf, &num, &ws);
	EXPECT_STREQ("", buf);
	EXPECT_EQ(0, num);
	EXPECT_EQ('\t', ws);
	free(buf);
	fclose(f);
}

TEST(GetBefore, Delim) {
	char test_data1[] = "aabbecc\n dd";
	char test_data2[] = "aabbcc";
	char test_data3[] = "\taabbcc";
	char *buf;
	char ws;
	int num = 0;
	FILE *f = NULL;

	f = fmemopen(test_data1, strlen(test_data1), "rb");
	customio_get_before_delim(f, "e", &buf, &num, &ws);
	EXPECT_STREQ("aabb", buf);
	EXPECT_EQ(4, num);
	EXPECT_EQ('e', ws);
	free(buf);

	customio_get_before_delim(f, "bd\t;=", &buf, NULL, NULL);
	EXPECT_STREQ("ecc\n ", buf);
	free(buf);

	customio_get_before_delim_ignore(f, "$*()", &buf, NULL, &ws);
	EXPECT_STREQ("dd", buf);
	EXPECT_EQ(EOF, ws);
	free(buf);
	fclose(f);

	f = fmemopen(test_data2, strlen(test_data2), "rb");
	customio_get_before_delim_ignore(f, "AV&EF", &buf, &num, NULL);
	EXPECT_STREQ("aabbcc", buf);
	EXPECT_EQ(6, num);
	free(buf);
	fclose(f);

	f = fmemopen(test_data3, strlen(test_data3), "rb");
	customio_get_before_delim(f, "cbd", &buf, &num, &ws);
	EXPECT_STREQ("\taa", buf);
	EXPECT_EQ(3, num);
	EXPECT_EQ('b', ws);
	free(buf);
	fclose(f);
}

TEST(GetBefore, DelimOrWs) {
	char test_data1[] = "aabbecc\n dd";
	char test_data2[] = " \n\taabb\n cc";
	char test_data3[] = "\taabbcc";
	char *buf;
	char ws;
	int num = 0;
	FILE *f = NULL;

	f = fmemopen(test_data1, strlen(test_data1), "rb");
	customio_get_before_delim_or_ws(f, "e", &buf, &num, &ws);
	EXPECT_STREQ("aabb", buf);
	EXPECT_EQ(4, num);
	EXPECT_EQ('e', ws);
	free(buf);

	customio_get_before_delim_or_ws(f, "bd;=", &buf, NULL, NULL);
	EXPECT_STREQ("ecc", buf);
	free(buf);

	customio_get_before_delim_or_ws_ignore(f, "$*()", &buf, NULL, &ws);
	EXPECT_STREQ("dd", buf);
	EXPECT_EQ(EOF, ws);
	free(buf);
	fclose(f);

	f = fmemopen(test_data2, strlen(test_data2), "rb");
	customio_get_before_delim_or_ws_ignore(f, "cDB", &buf, &num, NULL);
	EXPECT_STREQ("aabb", buf);
	EXPECT_EQ(4, num);
	free(buf);
	fclose(f);

	f = fmemopen(test_data3, strlen(test_data3), "rb");
	customio_get_before_delim_or_ws(f, "cbd", &buf, &num, &ws);
	EXPECT_STREQ("", buf);
	EXPECT_EQ(0, num);
	EXPECT_EQ('\t', ws);
	free(buf);

	customio_get_before_delim_or_ws(f, "cbd", &buf, NULL, NULL);
	EXPECT_STREQ("", buf);
	free(buf);
	fclose(f);
}

TEST(GetTill, Delim) {
	char test_data[] = "aabbecc\n dd";
	char *buf;
	int num = 0;
	FILE *f = NULL;

	f = fmemopen(test_data, strlen(test_data), "rb");
	customio_get_till_delim(f, "= cCe", &buf, &num);
	EXPECT_STREQ("aabbe", buf);
	EXPECT_EQ(5, num);
	free(buf);

	customio_get_till_delim(f, "b\t;=", &buf, NULL);
	EXPECT_STREQ("cc\n dd", buf);
	free(buf);

	fclose(f);
}

TEST(EatWs, EatWs) {
	char test_data[] = "a \tbbc \n \t  \v";
	int count = 0;
	FILE *f = NULL;

	f = fmemopen(test_data, strlen(test_data), "rb");
	customio_eat_ws(f, &count);
	EXPECT_EQ(0, ftell(f));
	EXPECT_EQ(0, count);

	fseek(f, 1, SEEK_CUR);
	customio_eat_ws(f, NULL);
	EXPECT_EQ(3, ftell(f));
	EXPECT_EQ('b', fgetc(f));

	fseek(f, 2, SEEK_CUR);
	customio_eat_ws(f, &count);
	EXPECT_EQ(1, feof(f));
	EXPECT_EQ(7, count);
	fclose(f);
}