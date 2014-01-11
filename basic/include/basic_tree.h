#ifndef _BASIC_TREE_H
#define _BASIC_TREE_H

#include "basic_general.h"
#include "basic_list.h"

/*
 * type definitions
 */

/* tree node struct */
struct basic_tree_node {
	node_data data;
	struct basic_tree_node *next;
	struct basic_tree_node *parent;
	struct list_head *siblings;
	struct list_head *children;
};

typedef struct basic_tree_node tnode;

/* data of each node in a tree */
typedef void * node_data;

/* tree traverse function */
typedef int basic_tree_traverse_ret;
typedef void * basic_tree_traverse_args;
typedef basic_tree_traverse_ret (*basic_tree_traverse_func)(tnode *, basic_tree_traverse_args *);

/* node data clean up function */
typedef void basic_tree_data_clean_ret;
typedef void * basic_tree_data_clean_args;
typedef basic_tree_data_clean_ret (*basic_tree_data_clean_func)(node_data *, basic_tree_data_clean_args);

/*
 * enumarations
 */

/* enum for tree traverse order */
enum basic_tree_traverse_order {
	ORDER_DFS,
	ORDER_BFS
}

/*
 * API functions
 */
static inline tnode *basic_tree_new(node_data data);

basic_tree_insert(tnode *node, tnode *parent, int pos);

static inline void basic_tree_insert_after(tnode *node, tnode *sibling);

static inline void basic_tree_insert_before(tnode *node, tnode *sibling);

static inline void basic_tree_append(tnode *node, tnode *parent);

static inline void basic_tree_prepend(tnode *node, tnode *parent);

static inline void basic_tree_unlink(tnode *node);

void basic_tree_traverse(tnode *root, int max_depth, basic_tree_traverse_order order, basic_tree_traverse_func func, basic_tree_traverse_args args);

static inline tnode *basic_tree_get_root(tnode *node);

static inline int basic_tree_child_position(tnode *node);

static inline tnode *basic_tree_nth_child(tnode *parent, int pos);

static inline tnode *basic_tree_prev_sibling(tnode *node);

static inline tnode *basic_tree_next_sibling(tnode *node);

static inline tnode *basic_tree_nth_sibling(tnode *node, int pos);

static inline int basic_tree_depth(tnode *node);

static inline int basic_tree_num_children(tnode *parent);

static inline int basic_tree_num_nodes(tnode *root);

static inline int basic_tree_height(tnode *root);

static inline int basic_tree_is_parent(tnode *parent, tnode *child);

static inline int basic_tree_is_ancestor(tnode *ancestor, tnode *descendant);

static inline int basic_tree_is_siblings(tnode *node1, tnode *node2);

static inline int basic_tree_is_root(tnode *node);

static inline int basic_tree_is_leaf(tnode *node);

static inline int basic_tree_is_alone(tnode *node);

static inline void basic_tree_destroy(tnode *node, basic_tree_data_clean_func func, basic_tree_data_clean_args args);

static inline void basic_tree_destroy_tree(tnode *node, basic_tree_data_clean_func func,  basic_tree_data_clean_args args);

/*
 * private functions
 */
tnode *__basic_tree_nth_child(tnode *parent, int pos);

int __basic_tree_num_nodes(tnode *node);

int __basic_tree_height(tnode *node);

void __basic_tree_destroy_tree(tnode *node, basic_tree_data_clean_func func,  basic_tree_data_clean_args args);

/*
 * inline function definitions
 */
static inline tnode *basic_tree_new(node_data data) {
	tnode * node = (tnode *)malloc(sizeof(tnode));
	INIT_LIST_HEAD(&(node->siblings));
	INIT_LIST_HEAD(&(node->children));
	node->parent = NULL;
	node->data = data;
	return node;
}

static inline void basic_tree_insert_after(tnode *node, tnode *sibling) {
	list_add_after(&(node->siblings), &(sibling->siblings));
	node->parent = sibling->parent;
}

static inline void basic_tree_insert_before(tnode *node, tnode *sibling) {
	list_add_after(&(node->siblings), &(sibling->siblings->prev));
	node->parent = sibling->parent;
}

static inline void basic_tree_append(tnode *node, tnode *parent) {
	list_add_after(&(node->siblings), &(parent->children->prev));
	node->parent = parent;
}

static inline void basic_tree_prepend(tnode *node, tnode *parent) {
	list_add_after(&(node->siblings), &(parent->children));
	node->parent = parent;
}

static inline void basic_tree_unlink(tnode *node) {
	node->parent = NULL;
	list_del(&(node->siblings));
}

static inline tnode *basic_tree_get_root(tnode *node) {
	tnode *ptr = node;
	while (ptr->parent != NULL) {
		ptr = ptr->parent;
	}
	return ptr;
}

static inline int basic_tree_child_position(tnode *node) {
	tnode *ptr;
	int pos = 0;

	/* if this node is a root, consider it a first child */
	if (node->parent == NULL)
		return 0;

	list_for_each_entry(ptr, &(node->parent->children), siblings) {
		if (ptr == node)
			break;
		pos++;
	}
	return pos;
}

static inline tnode *basic_tree_nth_child(tnode *parent, int pos) {
	if (parent->children == NULL)
		return NULL;
	return __basic_tree_nth_child(parent, pos);
}

static inline tnode *basic_tree_prev_sibling(tnode *node) {
	tnode *sibling;

	if (node->parent == NULL)
		return NULL;

	sibling = container_of(node->siblings->prev, tnode, siblings);
	if (sibling == node->parent)
		return NULL;
	else
		return sibling;
}

static inline tnode *basic_tree_next_sibling(tnode *node) {
	tnode *sibling;

	if (node->parent == NULL)
		return NULL;

	sibling = container_of(node->siblings->next, tnode, siblings);
	if (sibling == node->parent)
		return NULL;
	else
		return sibling;
}

static inline tnode *basic_tree_nth_sibling(tnode *node, int pos) {
	/* if it has no parent, only itself is the 0th sibling */
	if (node->parent == NULL)
		if (pos == 0)
			return node;
		else
			return NULL;

	return __basic_tree_nth_child(node->parent, pos);
}

static inline int basic_tree_depth(tnode *node) {
	tnode *ptr = node;
	int depth = 1;

	while (ptr->parent != NULL) {
		depth++;
		ptr = ptr->parent;
	}
	return depth;
}

static inline int basic_tree_num_children(tnode *parent) {
	tnode *ptr;
	int num = 0;

	if (ptr == NULL)
		return 0;

	list_for_each_entry(ptr, &(parent->children), siblings) {
		num++;
	}
	return num;
}

static inline int basic_tree_num_nodes(tnode *root) {
	if (root == NULL)
		return 0;

	return __basic_tree_num_nodes(root);
}

static inline int basic_tree_height(tnode *root) {
	if (root == NULL)
		return 0;

	return __basic_tree_height(root);
}

static inline int basic_tree_is_parent(tnode *parent, tnode *child) {
	return (child->parent == parent);
}

static inline int basic_tree_is_ancestor(tnode *ancestor, tnode *descendant) {
	tnode *ptr = descendant->parent;

	while (ptr != NULL) {
		if (ptr == ancestor)
			return 1;
		ptr = ptr->parent;
	}
	return 0;
}

static inline int basic_tree_is_siblings(tnode *node1, tnode *node2) {
	tnode *ptr;

	list_for_each_entry(ptr, &(node1->parent->children), siblings) {
		if (ptr == node2)
			if (ptr != node1)
				return 1;
	}
	return 0;
}

static inline int basic_tree_is_root(tnode *node) {
	return (node->parent == NULL);
}

static inline int basic_tree_is_leaf(tnode *node) {
	return (list_empty(&(node->children)));
}

static inline int basic_tree_is_alone(tnode *node) {
	return (basic_tree_is_root(node) && basic_tree_is_leaf(node));
}

static inline void basic_tree_destroy(tnode *node, basic_tree_data_clean_func func, basic_tree_data_clean_args args) {
	if (func != NULL)
		func(node->data, args);
	free(node);
}

static inline void basic_tree_destroy_tree(tnode *node, basic_tree_data_clean_func func, basic_tree_data_clean_args args) {
	__basic_tree_destroy_tree(node, basic_tree_data_clean_func func, basic_tree_data_clean_args args);
}

#endif