#include <limits.h>
#include "list.h"
#include "gtest/gtest.h"

#define BUILD_LIST(l)	\
	LIST_HEAD(l);	\
	struct entry entry2;	\
	entry2.num = 2;		\
	list_add(&entry2.ptrs, &l);		\
	struct entry entry4;	\
	entry4.num = 4;		\
	list_add_tail(&entry4.ptrs, &l);	\
	struct entry entry3;	\
	entry3.num = 3;		\
	list_add_after(&entry3.ptrs, &entry2.ptrs);		\
	struct entry entry1;	\
	entry1.num = 1;		\
	list_add(&entry1.ptrs, &l);		\


struct entry {
	int num;
	struct list_head ptrs;
};

static inline void ASSERT_LIST(const struct list_head *head, int *array) {
	int i = 0;
	struct list_head *ptr = head->next;

	while (ptr != head) {
		EXPECT_EQ(array[i], container_of(ptr, entry, ptrs)->num);
		i++;
		ptr = ptr->next;
	}

	i--;
	ptr = head->prev;
	while (ptr != head) {
		EXPECT_EQ(array[i], container_of(ptr, entry, ptrs)->num);
		i--;
		ptr = ptr->prev;
	}
}

TEST(ListHeadTest, Initiation) {
	// test LIST_HEAD
	LIST_HEAD(l1);
	EXPECT_EQ(&l1, l1.next);
	EXPECT_EQ(&l1, l1.prev);

	// test LIST_HEAD_INIT
	struct list_head l2 = LIST_HEAD_INIT(l2);
	EXPECT_EQ(&l2, l2.next);
	EXPECT_EQ(&l2, l2.prev);

	// test INIT_LIST_HEAD
	struct list_head l3;
	INIT_LIST_HEAD(&l3);
	EXPECT_EQ(&l3, l3.next);
	EXPECT_EQ(&l3, l3.prev);
}

TEST(ListHeadTest, BasicOperations) {
	// test list_add, list_add_tail, list_add_after
	BUILD_LIST(l);
	int array1[4] = {1, 2, 3, 4};
	ASSERT_LIST(&l, array1);

	// test list_del
	list_del(&entry3.ptrs);
	int array2[3] = {1, 2, 4};
	ASSERT_LIST(&l, array2);

	// test list_del_init
	list_del_init(&entry4.ptrs);
	int array3[2] = {1, 2};
	ASSERT_LIST(&l, array3);
	EXPECT_EQ(&entry4.ptrs, entry4.ptrs.next);
	EXPECT_EQ(&entry4.ptrs, entry4.ptrs.prev);
}

TEST(ListHeadTest, AdvancedOperations) {
	// test list_rotate_left, list_rotate_right
	BUILD_LIST(l);
	list_rotate_left(&l);
	int array1[4] = {2, 3, 4, 1};
	ASSERT_LIST(&l, array1);
	list_rotate_right(&l);
	int array2[4] = {1, 2, 3, 4};
	ASSERT_LIST(&l, array2);

	// test list_cut_position, list_splice_init, list_splice_tail
	struct list_head l2;
	list_cut_position(&l2, &l, &entry2.ptrs);
	int array3[2] = {1, 2};
	int array4[2] = {3, 4};
	ASSERT_LIST(&l2, array3);
	ASSERT_LIST(&l, array4);

	list_splice_init(&l2, &l);
	int array5[0] = {};
	int array6[4] = {1, 2, 3, 4};
	ASSERT_LIST(&l2, array5);
	ASSERT_LIST(&l, array6);

	list_cut_position(&l2, &l, &entry3.ptrs);
	list_splice_tail(&l2, &l);
	int array7[4] = {4, 1, 2, 3};
	ASSERT_LIST(&l, array7);
}

TEST(ListHeadTest, Macros) {
	BUILD_LIST(l);

	struct list_head *pos;
	struct entry *epos;
	int i = 0;

	// test list_for_each, list_for_each_prev
	list_for_each(pos, &l) {
		i++;
		EXPECT_EQ(i, container_of(pos, entry, ptrs)->num);
	}
	list_for_each_prev(pos, &l) {
		EXPECT_EQ(i, container_of(pos, entry, ptrs)->num);
		i--;
	}

	// test list_for_each_entry, list_for_each_entry_reverse
	list_for_each_entry(epos, &l, ptrs) {
		i++;
		EXPECT_EQ(i, epos->num);
	}
	list_for_each_entry_reverse(epos, &l, ptrs) {
		EXPECT_EQ(i, epos->num);
		i--;
	}

	// test list_prepare_entry, list_for_each_entry_continue, list_for_each_entry_from
	
}