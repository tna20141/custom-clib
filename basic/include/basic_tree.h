#ifndef _BASIC_TREE_H
#define _BASIC_TREE_H

/*
 * Simple n-ary tree implementation.
 *
 * Based on GLib's n-ary tree. The tree structure implemented in this
 * library should be constructed in the heap (in other words, nodes
 * and data are built with dynamic memory allocation and destroy with
 * memory deallocation).
 */

#include <stdlib.h>
#include "basic_general.h"
#include "basic_list.h"

/*
 * convenient macros for shortening code lines, will be undefined at the end
 */
#define bttr bt_traverse_ret
#define btta bt_traverse_args
#define bttf bt_traverse_func
#define btdcr bt_data_clean_ret
#define btdca bt_data_clean_args
#define btdcf bt_data_clean_func
#define btto bt_traverse_order
#define btec bt_error_code

/*
 * type definitions
 */

/* data of each node in a tree */
typedef void * btnode_data;

/* tree node struct */
struct bt_node {
	btnode_data data;
	struct bt_node *next;
	struct bt_node *parent;
	struct bl_head siblings;
	struct bl_head children;
};
typedef struct bt_node btnode;

/* control struct for traversal, contains info about current node */
struct bt_node_info {
	/* TODO: currently not implemented yet */
	int is_leaf;
};
typedef struct bt_node_info btinfo;

/* tree traverse function */
typedef int bttr;
typedef void * btta;
typedef bttr (*bttf)(btnode_data, btta, btinfo *);

/* node data clean up function */
typedef void btdcr;
typedef void * btdca;
typedef btdcr (*btdcf)(btnode_data, btdca);

/*
 * enumarations
 */

/* enum for tree traverse order */
typedef enum {
	BT_ORDER_DFS,
	BT_ORDER_BFS
} btto;

/* enum for error codes */
typedef enum {
	BT_INDEX_ERROR = -1
} btec;

/*
 * API functions
 */
static inline btnode *bt_new(btnode_data data);

btec bt_insert(btnode *node, btnode *parent, int pos);

static inline void bt_insert_after(btnode *node, btnode *sibling);

static inline void bt_insert_before(btnode *node, btnode *sibling);

static inline void bt_append(btnode *node, btnode *parent);

static inline void bt_prepend(btnode *node, btnode *parent);

static inline btnode *bt_unlink(btnode *node);

static inline btnode *bt_get_root(btnode *node);

static inline btnode *bt_nth_child(btnode *parent, int pos);

static inline btnode *bt_prev_sibling(btnode *node);

static inline btnode *bt_next_sibling(btnode *node);

static inline btnode *bt_nth_sibling(btnode *node, int pos);

static inline btnode *bt_parent(btnode *node);

static inline btnode *bt_ancestor(btnode *node, int levels);

static inline int bt_child_position(btnode *node);

static inline int bt_depth(btnode *node);

static inline int bt_num_children(btnode *parent);

static inline int bt_num_nodes(btnode *root);

static inline int bt_height(btnode *root);

static inline void bt_traverse(btnode *root, int max_depth, btto order, bttf meet_func, btta meet_args, bttf done_func, btta done_args);

static inline int bt_is_parent(btnode *parent, btnode *child);

static inline int bt_is_ancestor(btnode *ancestor, btnode *descendant);

static inline int bt_is_siblings(btnode *node1, btnode *node2);

static inline int bt_is_root(btnode *node);

static inline int bt_is_leaf(btnode *node);

static inline int bt_is_alone(btnode *node);

static inline void bt_destroy(btnode *node, btdcf func, btdca args);

static inline void bt_destroy_tree(btnode *node, btdcf func, btdca args);

/*
 * private functions
 */
btnode *__bt_nth_child(btnode *parent, int pos);
int __bt_num_nodes(btnode *node);
int __bt_height(btnode *node);
void __bt_destroy_tree(btnode *node, btdcf func, btdca args);
int __bt_traverse_dfs(btnode *node, int depth, bttf meet_func, btta meet_args, bttf done_func, btta done_args);
void __bt_traverse_bfs(btnode *root, int max_depth, bttf meet_func, btta meet_args, bttf done_func, btta done_args);

/*
 * inline function definitions
 */
static inline btnode *bt_new(btnode_data data) {
	btnode *node = (btnode *)malloc(sizeof(btnode));
	BL_INIT_HEAD(&(node->siblings));
	BL_INIT_HEAD(&(node->children));
	node->parent = NULL;
	node->data = data;
	return node;
}

static inline void bt_insert_after(btnode *node, btnode *sibling) {
	bl_add_after(&(node->siblings), &(sibling->siblings));
	node->parent = sibling->parent;
}

static inline void bt_insert_before(btnode *node, btnode *sibling) {
	bl_add_after(&(node->siblings), sibling->siblings.prev);
	node->parent = sibling->parent;
}

static inline void bt_append(btnode *node, btnode *parent) {
	bl_add_after(&(node->siblings), parent->children.prev);
	node->parent = parent;
}

static inline void bt_prepend(btnode *node, btnode *parent) {
	bl_add_after(&(node->siblings), &(parent->children));
	node->parent = parent;
}

static inline btnode *bt_unlink(btnode *node) {
	node->parent = NULL;
	bl_del(&(node->siblings));
	return node;
}

static inline btnode *bt_get_root(btnode *node) {
	btnode *ptr = node;
	while (ptr->parent != NULL) {
		ptr = ptr->parent;
	}
	return ptr;
}

static inline int bt_child_position(btnode *node) {
	btnode *ptr;
	int pos = 0;

	/* if this node is a root, consider it a first child */
	if (node->parent == NULL)
		return 0;

	bl_for_each_entry(ptr, &(node->parent->children), siblings) {
		if (ptr == node)
			break;
		pos++;
	}
	return pos;
}

static inline btnode *bt_nth_child(btnode *parent, int pos) {
	if (bt_is_leaf(parent))
		return NULL;
	return __bt_nth_child(parent, pos);
}

static inline btnode *bt_prev_sibling(btnode *node) {
	if (node->parent == NULL)
		return NULL;

	if (node->siblings.prev == &(node->parent->children))
		return NULL;

	return CONTAINER_OF(node->siblings.prev, btnode, siblings);
}

static inline btnode *bt_next_sibling(btnode *node) {
	if (node->parent == NULL)
		return NULL;

	if (node->siblings.next == &(node->parent->children))
		return NULL;

	return CONTAINER_OF(node->siblings.next, btnode, siblings);
}

static inline btnode *bt_nth_sibling(btnode *node, int pos) {
	/* if it has no parent, only itself is the 0th sibling */
	if (node->parent == NULL)
		if (pos == 0)
			return node;
		else
			return NULL;

	return __bt_nth_child(node->parent, pos);
}

static inline btnode *bt_parent(btnode *node) {
	return (node->parent);
}

static inline btnode *bt_ancestor(btnode *node, int levels) {
	btnode *ptr = node;

	while (ptr != NULL && levels > 0) {
		ptr = ptr->parent;
		levels--;
	}
	return ptr;
}

static inline int bt_depth(btnode *node) {
	btnode *ptr = node;
	int depth = 1;

	while (ptr->parent != NULL) {
		depth++;
		ptr = ptr->parent;
	}
	return depth;
}

static inline int bt_num_children(btnode *parent) {
	btnode *ptr;
	int num = 0;

	bl_for_each_entry(ptr, &(parent->children), siblings) {
		num++;
	}
	return num;
}

static inline int bt_num_nodes(btnode *root) {
	if (root == NULL)
		return 0;

	return __bt_num_nodes(root);
}

static inline int bt_height(btnode *root) {
	if (root == NULL)
		return 0;

	return __bt_height(root);
}

static inline void bt_traverse(btnode *root, int max_depth, btto order, bttf meet_func, btta meet_args, bttf done_func, btta done_args) {
	switch (order) {
		case BT_ORDER_BFS:
			__bt_traverse_bfs(root, max_depth, meet_func, meet_args, done_func, done_args);
			break;
		case BT_ORDER_DFS:
			__bt_traverse_dfs(root, max_depth, meet_func, meet_args, done_func, done_args);
			break;
		default:
			break;
	}
}

static inline int bt_is_parent(btnode *parent, btnode *child) {
	if (child->parent == NULL)
		return 0;
	return (child->parent == parent);
}

static inline int bt_is_ancestor(btnode *ancestor, btnode *descendant) {
	btnode *ptr = descendant->parent;

	while (ptr != NULL) {
		if (ptr == ancestor)
			return 1;
		ptr = ptr->parent;
	}
	return 0;
}

static inline int bt_is_siblings(btnode *node1, btnode *node2) {
	btnode *ptr;

	if (node1->parent == NULL || node2->parent == NULL)
		return 0;

	bl_for_each_entry(ptr, &(node1->parent->children), siblings) {
		if (ptr == node2)
			if (ptr != node1)
				return 1;
	}
	return 0;
}

static inline int bt_is_root(btnode *node) {
	if (node == NULL)
		return 0;
	return (node->parent == NULL);
}

static inline int bt_is_leaf(btnode *node) {
	if (node == NULL)
		return 0;
	return (bl_empty(&(node->children)));
}

static inline int bt_is_alone(btnode *node) {
	return (bt_is_root(node) && bt_is_leaf(node));
}

static inline void bt_destroy(btnode *node, btdcf func, btdca args) {
	if (func != NULL)
		func(node->data, args);
	free(node);
}

static inline void bt_destroy_tree(btnode *node, btdcf func, btdca args) {
	__bt_destroy_tree(node, func, args);
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