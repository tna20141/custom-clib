#include "cloak_10.h"
#include "gtest/gtest.h"

#define M_REPEAT1(S, N, ...) S

#define M_REPEAT2(S, N, ...) \
	EXPECT_EQ((N)-(S), CLOAK_CAT(int, S));

#define M_REPEAT3(S, N, X, Y) (X) += (Y);

#define M_REPEAT_COMMA(S, N, ...) \
	CLOAK_COMMA_IF(CLOAK_DEC(CLOAK_DEC(S))) S

int test_function() {
	return 4;
}

TEST(Pasting, Cat) {
	EXPECT_EQ(12345, CLOAK_CAT(123, 45));
	EXPECT_EQ(4, CLOAK_CAT(test_, CLOAK_CAT(funct, ion)) ());
	EXPECT_EQ("2233 , 55", CLOAK_STRINGIFY(CLOAK_CAT(22, 33 , 55)));
}

TEST(Stringify, Stringify) {
	EXPECT_EQ("(abcd)", CLOAK_STRINGIFY((abcd)));
	EXPECT_EQ("1bb cc", CLOAK_STRINGIFY(CLOAK_CAT(1, bb cc)));
	EXPECT_EQ("a\nb", CLOAK_STRINGIFY(a\nb));
}

TEST(Operations, Inc) {
	EXPECT_EQ(1, CLOAK_INC(0));
	EXPECT_EQ(3, CLOAK_INC(2));
}

TEST(Operations, Dec) {
	EXPECT_EQ(0, CLOAK_DEC(0));
	EXPECT_EQ(3, CLOAK_DEC(4));
}

TEST(Loop, Repeat) {
	int int1 = 4, int2 = 3, int3 = 2, int4 = 1, int5 = 0;

	EXPECT_EQ("0 1 2 3 4 5 6 7 8 9", CLOAK_STRINGIFY(
		CLOAK_RUN_LOOP(CLOAK_REPEAT(0, 10, M_REPEAT1, ~))
	));
	CLOAK_RUN_LOOP(CLOAK_REPEAT(1, 5, M_REPEAT2, ~));

	int1 = 0;
	CLOAK_RUN_LOOP(CLOAK_REPEAT(2, 6, M_REPEAT3, int1, 2 * 3));
	EXPECT_EQ(36, int1);
}

TEST(Comma, CommaInLoop) {
	int a[4] = {CLOAK_RUN_LOOP(CLOAK_REPEAT(2, 4, M_REPEAT_COMMA, ~))};
	int i;

	for (i = 0; i < 4; i++) {
		EXPECT_EQ(i+2, a[i]);
	}
}

#undef M_REPEAT_COMMA
#undef M_REPEAT3
#undef M_REPEAT2
#undef M_REPEAT1