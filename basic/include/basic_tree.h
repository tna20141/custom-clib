#ifndef _BASIC_TREE_H
#define _BASIC_TREE_H

#include <stdlib.h>
#include "basic_general.h"
#include "basic_list.h"

/*
 * Simple n-ary tree implementation.
 *
 * Based on GLib's n-ary tree. The tree structure implemented in this
 * library should be constructed in the heap (in other words, nodes
 * and data are built with dynamic memory allocation and destroy with
 * memory deallocation).
 */

/*
 * convenient macros for shortening code lines, will be undefined at the end
 */
#define bttr basic_tree_traverse_ret
#define btta basic_tree_traverse_args
#define bttf basic_tree_traverse_func
#define btdcr basic_tree_data_clean_ret
#define btdca basic_tree_data_clean_args
#define btdcf basic_tree_data_clean_func
#define btto basic_tree_traverse_order
#define btec basic_tree_error_code

/*
 * type definitions
 */

/* data of each node in a tree */
typedef void * btnode_data;

/* tree node struct */
struct basic_tree_node {
	btnode_data data;
	struct basic_tree_node *next;
	struct basic_tree_node *parent;
	struct list_head siblings;
	struct list_head children;
};
typedef struct basic_tree_node btnode;

/* tree traverse function */
typedef int bttr;
typedef void * btta;
typedef bttr (*bttf)(btnode *, btta *);

/* node data clean up function */
typedef void btdcr;
typedef void * btdca;
typedef btdcr (*btdcf)(btnode_data, btdca);

/*
 * enumarations
 */

/* enum for tree traverse order */
typedef enum {
	BASIC_TREE_ORDER_DFS,
	BASIC_TREE_ORDER_BFS
} btto;

/* enum for error codes */
typedef enum {
	BASIC_TREE_INDEX_ERROR = -1
} btec;

/*
 * API functions
 */
static inline btnode *basic_tree_new(btnode_data data);

btec basic_tree_insert(btnode *node, btnode *parent, int pos);

static inline void basic_tree_insert_after(btnode *node, btnode *sibling);

static inline void basic_tree_insert_before(btnode *node, btnode *sibling);

static inline void basic_tree_append(btnode *node, btnode *parent);

static inline void basic_tree_prepend(btnode *node, btnode *parent);

static inline btnode *basic_tree_unlink(btnode *node);

static inline btnode *basic_tree_get_root(btnode *node);

static inline btnode *basic_tree_nth_child(btnode *parent, int pos);

static inline btnode *basic_tree_prev_sibling(btnode *node);

static inline btnode *basic_tree_next_sibling(btnode *node);

static inline btnode *basic_tree_nth_sibling(btnode *node, int pos);

static inline btnode *basic_tree_parent(btnode *node);

static inline btnode *basic_tree_ancestor(btnode *node, int levels);

static inline int basic_tree_child_position(btnode *node);

static inline int basic_tree_depth(btnode *node);

static inline int basic_tree_num_children(btnode *parent);

static inline int basic_tree_num_nodes(btnode *root);

static inline int basic_tree_height(btnode *root);

void basic_tree_traverse(btnode *root, int max_depth, btto order, bttf func, btta args);

static inline int basic_tree_is_parent(btnode *parent, btnode *child);

static inline int basic_tree_is_ancestor(btnode *ancestor, btnode *descendant);

static inline int basic_tree_is_siblings(btnode *node1, btnode *node2);

static inline int basic_tree_is_root(btnode *node);

static inline int basic_tree_is_leaf(btnode *node);

static inline int basic_tree_is_alone(btnode *node);

static inline void basic_tree_destroy(btnode *node, btdcf func, btdca args);

static inline void basic_tree_destroy_tree(btnode *node, btdcf func, btdca args);

/*
 * private functions
 */
btnode *__basic_tree_nth_child(btnode *parent, int pos);
int __basic_tree_num_nodes(btnode *node);
int __basic_tree_height(btnode *node);
void __basic_tree_destroy_tree(btnode *node, btdcf func, btdca args);

/*
 * inline function definitions
 */
static inline btnode *basic_tree_new(btnode_data data) {
	btnode *node = (btnode *)malloc(sizeof(btnode));
	INIT_LIST_HEAD(&(node->siblings));
	INIT_LIST_HEAD(&(node->children));
	node->parent = NULL;
	node->data = data;
	return node;
}

static inline void basic_tree_insert_after(btnode *node, btnode *sibling) {
	list_add_after(&(node->siblings), &(sibling->siblings));
	node->parent = sibling->parent;
}

static inline void basic_tree_insert_before(btnode *node, btnode *sibling) {
	list_add_after(&(node->siblings), sibling->siblings.prev);
	node->parent = sibling->parent;
}

static inline void basic_tree_append(btnode *node, btnode *parent) {
	list_add_after(&(node->siblings), parent->children.prev);
	node->parent = parent;
}

static inline void basic_tree_prepend(btnode *node, btnode *parent) {
	list_add_after(&(node->siblings), &(parent->children));
	node->parent = parent;
}

static inline btnode *basic_tree_unlink(btnode *node) {
	node->parent = NULL;
	list_del(&(node->siblings));
	return node;
}

static inline btnode *basic_tree_get_root(btnode *node) {
	btnode *ptr = node;
	while (ptr->parent != NULL) {
		ptr = ptr->parent;
	}
	return ptr;
}

static inline int basic_tree_child_position(btnode *node) {
	btnode *ptr;
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

static inline btnode *basic_tree_nth_child(btnode *parent, int pos) {
	if (basic_tree_is_leaf(parent))
		return NULL;
	return __basic_tree_nth_child(parent, pos);
}

static inline btnode *basic_tree_prev_sibling(btnode *node) {
	if (node->parent == NULL)
		return NULL;

	if (node->siblings.prev == &(node->parent->children))
		return NULL;

	return CONTAINER_OF(node->siblings.prev, btnode, siblings);
}

static inline btnode *basic_tree_next_sibling(btnode *node) {
	if (node->parent == NULL)
		return NULL;

	if (node->siblings.next == &(node->parent->children))
		return NULL;

	return CONTAINER_OF(node->siblings.next, btnode, siblings);
}

static inline btnode *basic_tree_nth_sibling(btnode *node, int pos) {
	/* if it has no parent, only itself is the 0th sibling */
	if (node->parent == NULL)
		if (pos == 0)
			return node;
		else
			return NULL;

	return __basic_tree_nth_child(node->parent, pos);
}

static inline btnode *basic_tree_parent(btnode *node) {
	return (node->parent);
}

static inline btnode *basic_tree_ancestor(btnode *node, int levels) {
	btnode *ptr = node;

	while (ptr != NULL && levels > 0) {
		ptr = ptr->parent;
		levels--;
	}
	return ptr;
}

static inline int basic_tree_depth(btnode *node) {
	btnode *ptr = node;
	int depth = 1;

	while (ptr->parent != NULL) {
		depth++;
		ptr = ptr->parent;
	}
	return depth;
}

static inline int basic_tree_num_children(btnode *parent) {
	btnode *ptr;
	int num = 0;

	list_for_each_entry(ptr, &(parent->children), siblings) {
		num++;
	}
	return num;
}

static inline int basic_tree_num_nodes(btnode *root) {
	if (root == NULL)
		return 0;

	return __basic_tree_num_nodes(root);
}

static inline int basic_tree_height(btnode *root) {
	if (root == NULL)
		return 0;

	return __basic_tree_height(root);
}

static inline int basic_tree_is_parent(btnode *parent, btnode *child) {
	if (child->parent == NULL)
		return 0;
	return (child->parent == parent);
}

static inline int basic_tree_is_ancestor(btnode *ancestor, btnode *descendant) {
	btnode *ptr = descendant->parent;

	while (ptr != NULL) {
		if (ptr == ancestor)
			return 1;
		ptr = ptr->parent;
	}
	return 0;
}

static inline int basic_tree_is_siblings(btnode *node1, btnode *node2) {
	btnode *ptr;

	if (node1->parent == NULL || node2->parent == NULL)
		return 0;

	list_for_each_entry(ptr, &(node1->parent->children), siblings) {
		if (ptr == node2)
			if (ptr != node1)
				return 1;
	}
	return 0;
}

static inline int basic_tree_is_root(btnode *node) {
	if (node == NULL)
		return 0;
	return (node->parent == NULL);
}

static inline int basic_tree_is_leaf(btnode *node) {
	if (node == NULL)
		return 0;
	return (list_empty(&(node->children)));
}

static inline int basic_tree_is_alone(btnode *node) {
	return (basic_tree_is_root(node) && basic_tree_is_leaf(node));
}

static inline void basic_tree_destroy(btnode *node, btdcf func, btdca args) {
	if (func != NULL)
		func(node->data, args);
	free(node);
}

static inline void basic_tree_destroy_tree(btnode *node, btdcf func, btdca args) {
	__basic_tree_destroy_tree(node, func, args);
}

/*
 * undefining the convenient macros
 */
#undef bttr
#undef btta
#undef bttf
#undef btdcr
#undef btdca
#undef btdcf
#undef btto
#undef btec

#endif