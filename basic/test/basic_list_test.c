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
	BL_HEAD(l);	\
	struct entry entry2;	\
	entry2.num = 2;		\
	bl_add(&entry2.ptrs, &l);		\
	struct entry entry4;	\
	entry4.num = 4;		\
	bl_add_tail(&entry4.ptrs, &l);	\
	struct entry entry3;	\
	entry3.num = 3;		\
	bl_add_after(&entry3.ptrs, &entry2.ptrs);		\
	struct entry entry1;	\
	entry1.num = 1;		\
	bl_add(&entry1.ptrs, &l);		\

struct entry {
	int num;
	struct bl_head ptrs;
};

static inline void assert_list(const struct bl_head *head, int *array) {
	int i = 0;
	struct bl_head *ptr = head->next;

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
	// test BL_HEAD
	BL_HEAD(l1);
	assert_int_equal(&l1, l1.next);
	assert_int_equal(&l1, l1.prev);

	// test BL_HEAD_INIT
	struct bl_head l2 = BL_HEAD_INIT(l2);
	assert_int_equal(&l2, l2.next);
	assert_int_equal(&l2, l2.prev);

	// test BL_INIT_HEAD
	struct bl_head l3;
	BL_INIT_HEAD(&l3);
	assert_int_equal(&l3, l3.next);
	assert_int_equal(&l3, l3.prev);
}

static void test_basic_operations(void **state) {
	// test bl_add, bl_add_tail, bl_add_after
	BUILD_LIST(l);
	int array1[4] = {1, 2, 3, 4};
	assert_list(&l, array1);

	// test bl_del
	bl_del(&entry3.ptrs);
	int array2[3] = {1, 2, 4};
	assert_list(&l, array2);

	// test bl_del_init
	bl_del_init(&entry4.ptrs);
	int array3[2] = {1, 2};
	assert_list(&l, array3);
	assert_int_equal(&entry4.ptrs, entry4.ptrs.next);
	assert_int_equal(&entry4.ptrs, entry4.ptrs.prev);
}

static void test_advanced_operations(void **state) {
	// test bl_rotate_left, bl_rotate_right
	BUILD_LIST(l);
	bl_rotate_left(&l);
	int array1[4] = {2, 3, 4, 1};
	assert_list(&l, array1);
	bl_rotate_right(&l);
	int array2[4] = {1, 2, 3, 4};
	assert_list(&l, array2);

	// test bl_cut_position, bl_splice_init, bl_splice_tail
	struct bl_head l2;
	bl_cut_position(&l2, &l, &entry2.ptrs);
	int array3[2] = {1, 2};
	int array4[2] = {3, 4};
	assert_list(&l2, array3);
	assert_list(&l, array4);

	bl_splice_init(&l2, &l);
	int array5[0] = {};
	int array6[4] = {1, 2, 3, 4};
	assert_list(&l2, array5);
	assert_list(&l, array6);

	bl_cut_position(&l2, &l, &entry3.ptrs);
	bl_splice_tail(&l2, &l);
	int array7[4] = {4, 1, 2, 3};
	assert_list(&l, array7);
}

static void test_macros(void **state) {
	BUILD_LIST(l);

	struct bl_head *pos;
	struct entry *epos;
	int i = 0;

	// test bl_for_each, bl_for_each_prev
	bl_for_each(pos, &l) {
		i++;
		assert_int_equal(i, CONTAINER_OF(pos, struct entry, ptrs)->num);
	}
	bl_for_each_prev(pos, &l) {
		assert_int_equal(i, CONTAINER_OF(pos, struct entry, ptrs)->num);
		i--;
	}

	// test bl_for_each_entry, bl_for_each_entry_reverse
	bl_for_each_entry(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
	}
	bl_for_each_entry_reverse(epos, &l, ptrs) {
		assert_int_equal(i, epos->num);
		i--;
	}

	// test bl_prepare_entry, bl_for_each_entry_continue, bl_for_each_entry_from
	epos = NULL;
	epos = bl_prepare_entry(epos, &l, ptrs);
	bl_for_each_entry_continue(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
		if (i == 3) break;
	}
	epos = bl_prepare_entry(epos, &l, ptrs);
	bl_for_each_entry_continue(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
	}

	epos = NULL;
	i = 0;
	epos = bl_prepare_entry(epos, &l, ptrs);
	bl_for_each_entry_continue(epos, &l, ptrs) {
		i++;
		assert_int_equal(i, epos->num);
		if (i == 2) break;
	}
	epos = bl_prepare_entry(epos, &l, ptrs);
	i--;
	bl_for_each_entry_from(epos, &l, ptrs) {
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