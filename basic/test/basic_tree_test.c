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
#include <basic_list.h>
#include <basic_general.h>
#include <basic_tree.h>

/*
 * convenient macros, will be undefined at the end
 */
#define BUILD_NODE(N, S)		\
	data = (sd *)malloc(sizeof(sd));	\
	data->n = N;		\
	data->str = (char *)malloc(strlen(S)+1);	\
	strcpy(data->str, S);		\
	node = basic_tree_new((void *)data);

/* root node of the tree used for testing */
btnode *test_root;

int test_num;

/* node data struct */
struct simple_data {
	int n;
	char *str;
};
typedef struct simple_data sd;

/* function prototypes (for the ones that need one) */
void assert_node(btnode *node, int n, const char *str);
void assert_children(btnode *parent, ...);
void cleanup_callback(void *node_data, void *data);

/* fixture set up function */
static void setup_tree(void **state) {
	sd* data;
	btnode* node;
	btnode* parent;
	btnode* sibling;

	BUILD_NODE(1, "one");
	test_root = node;
	parent = test_root;

	BUILD_NODE(2, "two");
	basic_tree_prepend(node, parent);

	BUILD_NODE(3, "three");
	basic_tree_prepend(node, parent);

	BUILD_NODE(4, "four");
	basic_tree_append(node, parent);
	parent = node;

	BUILD_NODE(5, "five");
	basic_tree_append(node, parent);
	parent = node;

	BUILD_NODE(6, "six");
	assert_int_equal(0, basic_tree_insert(node, parent, 0));
	sibling = node;

	BUILD_NODE(7, "seven");
	basic_tree_insert_after(node, sibling);
	sibling = node;

	BUILD_NODE(8, "eight");
	assert_int_equal(0, basic_tree_insert(node, parent, 2));

	BUILD_NODE(9, "nine");
	basic_tree_insert_after(node, sibling);

	BUILD_NODE(10, "ten");
	assert_int_equal(0, basic_tree_insert(node, parent, 0));
	parent = basic_tree_nth_child(test_root, 0);

	BUILD_NODE(11, "eleven");
	assert_int_equal(0, basic_tree_insert(node, parent, -1));

	BUILD_NODE(12, "twelve");
	assert_int_equal(0, basic_tree_insert(node, parent, -2));
	parent = basic_tree_next_sibling(node);

	BUILD_NODE(13, "thirteen");
	assert_int_equal(0, basic_tree_insert(node, parent, 0));
	sibling = node;

	BUILD_NODE(14, "fourteen");
	basic_tree_insert_before(node, sibling);

	BUILD_NODE(15, "fifteen");
	assert_int_equal(0, basic_tree_insert(node, parent, -2));
	sibling = node;

	BUILD_NODE(16, "sixteen");
	basic_tree_insert_before(node, sibling);
}

/* fixture tear down function */
static void teardown_tree(void **state) {
	test_num = 0;
	basic_tree_destroy_tree(test_root, cleanup_callback, (void *)16);
	assert_int_equal(16, test_num);
}

/*
 *tests
 */
static void test_new(void **state) {
	btnode *node;
	void *data = (void *)1;

	node = basic_tree_new(data);

	assert_int_equal(NULL, node->parent);
	assert_int_equal(1, node->data);
	assert_int_equal(&(node->siblings), node->siblings.prev);
	assert_int_equal(&(node->siblings), node->siblings.next);
	assert_int_equal(&(node->children), node->children.prev);
	assert_int_equal(&(node->children), node->children.next);
}

static void test_insert_get_destroy_nodes(void **state) {
	btnode *current = test_root;
	btnode *node;
	sd *data;

	assert_int_equal(test_root, basic_tree_get_root(current));
	assert_int_equal(NULL, basic_tree_parent(current));
	assert_children(current, 3, 3, "three", 2, "two", 4, "four");

	current = basic_tree_nth_child(current, 0);
	assert_children(current, 2, 12, "twelve", 11, "eleven");
	assert_int_equal(NULL, basic_tree_prev_sibling(current));

	current = basic_tree_nth_child(current, -2);
	assert_node(current, 12, "twelve");
	assert_children(current, 0);

	current = basic_tree_next_sibling(current);
	assert_int_equal(test_root, basic_tree_get_root(current));
	assert_children(current, 4, 14,
		"fourteen", 16, "sixteen", 15, "fifteen", 13, "thirteen");
	assert_int_equal(NULL, basic_tree_nth_child(current, 4));
	assert_int_equal(NULL, basic_tree_nth_child(current, -5));

	current = basic_tree_ancestor(current, 2);
	assert_int_equal(test_root, current);

	current = basic_tree_nth_child(current, 1);
	assert_node(current, 2, "two");
	assert_int_equal(basic_tree_ancestor(current, 1), basic_tree_parent(current));
	assert_int_equal(NULL, basic_tree_nth_sibling(current, 3));
	assert_int_equal(NULL, basic_tree_nth_sibling(current, -4));
	assert_children(current, 0);

	current = basic_tree_nth_sibling(current, -1);
	assert_node(current, 4, "four");
	assert_int_equal(current, basic_tree_ancestor(current, 0));
	assert_int_equal(NULL, basic_tree_next_sibling(current));

	current = basic_tree_nth_child(current, -1);
	assert_int_equal(current, basic_tree_nth_sibling(current, -1));
	assert_children(current, 5, 10, "ten", 6, "six",
		7, "seven", 9, "nine", 8, "eight");

	current = basic_tree_nth_child(current, 0);
	assert_node(current, 10, "ten");
	assert_int_equal(current, basic_tree_nth_sibling(current, 0));

	current = basic_tree_nth_sibling(current, 2);
	assert_node(current, 7, "seven");
	assert_int_equal(test_root, basic_tree_get_root(current));

	current = basic_tree_prev_sibling(current);
	assert_node(current, 6, "six");

	current = basic_tree_nth_sibling(current, -2);
	assert_node(current, 9, "nine");

	/* abnormal cases */
	BUILD_NODE(17, "seventeen");
	current = basic_tree_nth_child(test_root, 2);
	assert_int_equal(BASIC_TREE_INDEX_ERROR, basic_tree_insert(node, current, -3));
	assert_int_equal(BASIC_TREE_INDEX_ERROR, basic_tree_insert(node, current, 2));
	free(data);
	free(node);
}

static void test_child_position(void **state) {
	btnode *current;

	assert_int_equal(0, basic_tree_child_position(test_root));

	current = basic_tree_nth_child(test_root, 2);
	assert_int_equal(2, basic_tree_child_position(current));

	current = basic_tree_prev_sibling(current);
	assert_int_equal(1, basic_tree_child_position(current));

	current = basic_tree_prev_sibling(current);
	assert_int_equal(0, basic_tree_child_position(current));
}

static void test_depth(void **state) {
	btnode *current;

	assert_int_equal(1, basic_tree_depth(test_root));

	current = basic_tree_nth_child(test_root, 1);
	assert_int_equal(2, basic_tree_depth(current));

	current = basic_tree_prev_sibling(current);
	current = basic_tree_nth_child(
		basic_tree_nth_child(current, -1), 2
	);
	assert_int_equal(4, basic_tree_depth(current));
}

static void test_num_children(void **state) {
	btnode *current;

	assert_int_equal(3, basic_tree_num_children(test_root));

	current = basic_tree_nth_child(test_root, 1);
	assert_int_equal(0, basic_tree_num_children(current));

	current = basic_tree_prev_sibling(current);
	assert_int_equal(2, basic_tree_num_children(current));

	current = basic_tree_nth_child(current, 1);
	assert_int_equal(4, basic_tree_num_children(current));
}

static void test_num_nodes(void **state) {
	btnode *current;

	assert_int_equal(16, basic_tree_num_nodes(test_root));
	assert_int_equal(0, basic_tree_num_nodes(NULL));

	current = basic_tree_nth_child(test_root, 0);
	assert_int_equal(7, basic_tree_num_nodes(current));

	current = basic_tree_next_sibling(current);
	assert_int_equal(1, basic_tree_num_nodes(current));
}

static void test_height(void **state) {
	btnode *current;

	assert_int_equal(4, basic_tree_height(test_root));
	assert_int_equal(0, basic_tree_height(NULL));

	current = basic_tree_nth_child(test_root, 0);
	assert_int_equal(3, basic_tree_height(current));

	current = basic_tree_next_sibling(current);
	assert_int_equal(1, basic_tree_height(current));
}

static void test_is_parent(void **state) {
	btnode *current;

	assert_int_equal(0, basic_tree_is_parent(NULL, test_root));

	current = basic_tree_nth_child(test_root, 0);
	assert_int_equal(1, basic_tree_is_parent(test_root, current));
	assert_int_equal(0, basic_tree_is_parent(current, current));
	assert_int_equal(0, basic_tree_is_parent(current, test_root));

	current = basic_tree_nth_child(current, 1);
	assert_int_equal(0, basic_tree_is_parent(current, test_root));
	assert_int_equal(0, basic_tree_is_parent(
		basic_tree_nth_child(test_root, 2), current
	));
}

static void test_is_ancestor(void **state) {
	btnode *current;

	assert_int_equal(0, basic_tree_is_ancestor(NULL, test_root));

	current = basic_tree_nth_child(test_root, 0);
	assert_int_equal(1, basic_tree_is_ancestor(test_root, current));
	assert_int_equal(0, basic_tree_is_ancestor(current, current));
	assert_int_equal(0, basic_tree_is_ancestor(current, test_root));
	assert_int_equal(0, basic_tree_is_ancestor(NULL, current));

	current = basic_tree_nth_child(current, 1);
	assert_int_equal(1, basic_tree_is_ancestor(test_root, current));
	assert_int_equal(0, basic_tree_is_ancestor(
		basic_tree_nth_child(test_root, 2), current
	));

	current = basic_tree_nth_child(current, 2);
	assert_int_equal(1, basic_tree_is_ancestor(test_root, current));
	assert_int_equal(1, basic_tree_is_ancestor(
		basic_tree_nth_child(test_root, 0), current
	));
}

static void test_is_siblings(void **state) {
	btnode *current1;
	btnode *current2;

	assert_int_equal(0, basic_tree_is_siblings(test_root, test_root));

	current1 = basic_tree_nth_child(test_root, 0);
	current2 = basic_tree_nth_child(test_root, -1);
	assert_int_equal(1, basic_tree_is_siblings(current1, current2));
	assert_int_equal(1, basic_tree_is_siblings(current2, current1));
	assert_int_equal(0, basic_tree_is_siblings(current1, current1));
	assert_int_equal(0, basic_tree_is_siblings(test_root, current2));

	current2 = basic_tree_prev_sibling(current2);
	assert_int_equal(1, basic_tree_is_siblings(current1, current2));

	current1 = basic_tree_nth_child(current1, 0);
	assert_int_equal(0, basic_tree_is_siblings(current1, current2));

	current2 = basic_tree_nth_child(basic_tree_next_sibling(current2), 0);
	assert_int_equal(0, basic_tree_is_siblings(current1, current2));
}

static void test_is_root(void **state) {
	btnode *current;
	sd *data;
	btnode *node;

	assert_int_equal(1, basic_tree_is_root(test_root));
	assert_int_equal(0, basic_tree_is_root(NULL));

	current = basic_tree_nth_child(test_root, 0);
	assert_int_equal(0, basic_tree_is_root(current));

	current = basic_tree_next_sibling(current);
	assert_int_equal(0, basic_tree_is_root(current));

	BUILD_NODE(17, "seventeen");
	assert_int_equal(1, basic_tree_is_root(node));
	free(data);
	free(node);
}

static void test_is_leaf(void **state) {
	btnode *current;
	sd *data;
	btnode *node;

	assert_int_equal(0, basic_tree_is_leaf(test_root));
	assert_int_equal(0, basic_tree_is_leaf(NULL));

	current = basic_tree_nth_child(test_root, -2);
	assert_int_equal(1, basic_tree_is_leaf(current));

	current = basic_tree_prev_sibling(current);
	assert_int_equal(0, basic_tree_is_leaf(current));

	current = basic_tree_nth_child(current, 0);
	assert_int_equal(1, basic_tree_is_leaf(current));

	BUILD_NODE(17, "seventeen");
	assert_int_equal(1, basic_tree_is_leaf(node));
	free(data);
	free(node);
}

static void test_is_alone(void **state) {
	btnode *current;
	sd *data;
	btnode *node;

	assert_int_equal(0, basic_tree_is_alone(test_root));
	assert_int_equal(0, basic_tree_is_alone(NULL));

	current = basic_tree_nth_child(test_root, -2);
	assert_int_equal(0, basic_tree_is_alone(current));

	current = basic_tree_prev_sibling(current);
	assert_int_equal(0, basic_tree_is_alone(current));

	current = basic_tree_nth_child(current, 0);
	assert_int_equal(0, basic_tree_is_alone(current));

	BUILD_NODE(17, "seventeen");
	assert_int_equal(1, basic_tree_is_alone(node));
	free(data);
	free(node);
}

static void test_unlink(void **state) {
	btnode *current;
	btnode *node;

	current = basic_tree_nth_child(test_root, 2);
	assert_int_equal(current, basic_tree_unlink(current));

	assert_children(test_root, 2, 3, "three", 2, "two");
	assert_int_equal(1, basic_tree_is_root(current));
	assert_children(current, 1, 5, "five");

	current = basic_tree_nth_child(current, -1);
	node = basic_tree_unlink(basic_tree_nth_child(current, 1));
	assert_children(current, 4, 10, "ten", 7, "seven", 9, "nine", 8, "eight");

	assert_node(node, 6, "six");
	assert_int_equal(1, basic_tree_is_alone(node));
	basic_tree_unlink(node);
	assert_int_equal(1, basic_tree_is_alone(node));
	free(((sd *)node->data)->str);
	basic_tree_destroy(node, NULL, (void *)current);

	basic_tree_destroy_tree(current, cleanup_callback, (void *)0);
}

/*
 * private functions
 */
void assert_node(btnode *node, int n, const char *str) {
	assert_int_equal(n, ((sd *)node->data)->n);
	assert_string_equal(str, ((sd *)node->data)->str);
}

void assert_children(btnode *parent, ...) {
	btnode *ptr = CONTAINER_OF(parent->children.next, btnode, siblings);
	va_list a_list;
	int num;

	va_start(a_list, parent);
	num = va_arg(a_list, int);

	list_for_each_entry_from(ptr, &(parent->children), siblings) {
		assert_int_equal(va_arg(a_list, int), ((sd *)ptr->data)->n);
		assert_string_equal(va_arg(a_list, char *), ((sd *)ptr->data)->str);
		num--;
	}

	assert_int_equal(0, num);
}

void cleanup_callback(void *node_data, void *data) {
	if (((sd *)node_data)->n == (int64_t)data)
		test_num = (int64_t)data;
	free(((sd *)node_data)->str);
	free(node_data);
}

/* main function */
int main(void) {
	const UnitTest tests[] = {
		unit_test(test_new),
		unit_test_setup_teardown(test_insert_get_destroy_nodes, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_child_position, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_depth, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_num_children, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_num_nodes, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_height, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_is_parent, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_is_ancestor, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_is_siblings, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_is_root, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_is_leaf, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_is_alone, setup_tree, teardown_tree),
		unit_test_setup_teardown(test_unlink, setup_tree, teardown_tree)
	};

	return run_tests(tests);
}