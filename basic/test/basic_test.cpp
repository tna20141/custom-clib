#include <limits.h>
#include "list.h"
#include "gtest/gtest.h"

TEST(ListHeadTest, Basic) {
	LIST_HEAD(l);
	EXPECT_EQ(&l, l.next);
	EXPECT_EQ(&l, l.prev);
}
