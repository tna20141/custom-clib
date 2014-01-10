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
#include <basic_list.h>

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

static inline void assert_list(const struct list_head *head, int *array) {
	int i = 0;
	struct list_head *ptr = head->next;

	while (ptr != head) {
		assert_int_equal(array[i], CONTAINER_OF(ptr, struct entry, ptrs)->num);
		i++;
		ptr = ptr->next;
	}

	i--;
	ptr = head->prev;
	while (ptr != head) {
		assert_int_equal(array[i], CONTAINER_OF(ptr, struct entry, ptrs)->num);
		i--;
		ptr = ptr->prev;
	}
}

static void test_initiation(void **state) {
	// test LIST_HEAD
	LIST_HEAD(l1);
	assert_int_equal(&l1, l1.next);
	assert_int_equal(&l1, l1.prev);

	// test LIST_HEAD_INIT
	struct list_head l2 = LIST_HEAD_INIT(l2);
	assert_int_equal(&l2, l2.next);
	assert_int_equal(&l2, l2.prev);

	// test INIT_LIST_HEAD
	struct list_head l3;
	INIT_LIST_HEAD(&l3);
	assert_int_equal(&l3, l3.next);
	assert_int_equal(&l3, l3.prev);
}

static void test_basic_operations(void **state) {
	// test list_add, list_add_tail, list_add_after
	BUILD_LIST(l);
	int array1[4] = {1, 2, 3, 4};
	assert_list(&l, array1);

	// test list_del
	list_del(&entry3.ptrs);
	int array2[3] = {1, 2, 4};
	assert_list(&l, array2);

	// test list_del_init
	list_del_init(&entry4.ptrs);
	int array3[2] = {1, 2};
	assert_list(&l, array3);
	assert_int_equal(&entry4.ptrs, entry4.ptrs.next);
	assert_int_equal(&entry4.ptrs, entry4.ptrs.prev);
}

static void test_advanced_operations(void **state) {
	// test list_rotate_left, list_rotate_right
	BUILD_LIST(l);
	list_rotate_left(&l);
	int array1[4] = {2, 3, 4, 1};
	assert_list(&l, array1);
	list_rotate_right(&l);
	int array2[4] = {1, 2, 3, 4};
	assert_list(&l, array2);

	// test list_cut_position, list_splice_init, list_splice_tail
	struct list_head l2;
	list_cut_position(&l2, &l, &entry2.ptrs);
	int array3[2] = {1, 2};
	int array4[2] = {3, 4};
	assert_list(&l2, array3);
	assert_list(&l, array4);

	list_splice_init(&l2, &l);
	int array5[0] = {};
	int array6[4] = {1, 2, 3, 4};
	assert_list(&l2, array5);
	assert_list(&l, array6);

	list_cut_position(&l2, &l, &entry3.ptrs);
	list_splice_tail(&l2, &l);
	int array7[4] = {4, 1, 2, 3};
	assert_list(&l, array7);
}

static void test_macros(void **state) {
	BUILD_LIST(l);

	struct list_head *pos;
	struct entry *epos;
	int i = 0;

	// test list_for_each, list_for_each_prev
	list_for_each(pos, &l) {
		i++;
		assert_int_equal(i, CONTAINER_OF(pos, struct entry, ptrs)->num);
	}
	list_for_each_prev(pos, &l) {
		assert_int_equal(i, CONTAINER_OF(pos, struct entry, ptrs)->num);
		i--;
	}

	// test list_for_each_entry, list_for_each_entry_reverse
	list_for_each_entry(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
	}
	list_for_each_entry_reverse(epos, &l, ptrs) {
		assert_int_equal(i, epos->num);
		i--;
	}

	// test list_prepare_entry, list_for_each_entry_continue, list_for_each_entry_from
	epos = NULL;
	epos = list_prepare_entry(epos, &l, ptrs);
	list_for_each_entry_continue(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
		if (i == 3) break;
	}
	epos = list_prepare_entry(epos, &l, ptrs);
	list_for_each_entry_continue(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
	}

	epos = NULL;
	i = 0;
	epos = list_prepare_entry(epos, &l, ptrs);
	list_for_each_entry_continue(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
		if (i == 2) break;
	}
	epos = list_prepare_entry(epos, &l, ptrs);
	i--;
	list_for_each_entry_from(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
	}
}

int main(void) {
	const UnitTest tests[] = {
		unit_test(test_initiation),
		unit_test(test_basic_operations),
		unit_test(test_advanced_operations),
		unit_test(test_macros)
	};

	return run_tests(tests);
}

#undef BUILD_LIST