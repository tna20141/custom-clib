#include <stdlib.h>
#include <basic_tree.h>

void basic_tree_insert(tnode *node, tnode *parent, int pos) {
	tnode *ptr;

	/* if this is the first child */
	if (parent->children == NULL) {
		parent->children = node;
		goto ret;
	}

	/* else */

	ptr = tnode * __basic_tree_nth_child(tnode *parent, pos);
	if (ptr == NULL)
		return;

	/* insert node */
	if (pos < 0)
		__basic_tree_insert(node, ptr, ptr->next);
	else
		__basic_tree_insert(node, ptr->prev, ptr);

	/* update parent's children pointer if necessary */
	if (pos == 0) {
		parent->children = node;
	}

	ret:

	/* set new node's parent */
	node->parent = parent;
}

tnode *__basic_tree_nth_child(tnode *parent, int pos) {
	tnode *ptr;
	int i = 0;

	if (pos < 0) {
		list_for_each_entry_reverse(ptr, &(parent->children), siblings) {
			i--;
			if (i == pos)
				return ptr;
		}
	} else {
		list_for_each_entry(ptr, &(parent->children), siblings) {
			if (i == pos)
				return ptr;
			i++;
		}
	}

	/* if pos is out of bound, return NULL */
	return NULL;
}

int __basic_tree_num_nodes(tnode *node) {
	tnode *ptr;
	int num = 1;

	if (ptr == NULL)
		return 1;

	/* count nodes recursively */
	list_for_each_entry(ptr, &(node->children), siblings) {
		num += __basic_tree_num_nodes(ptr);
	}
	return num;
}

int __basic_tree_height(tnode *node) {
	tnode *ptr = node->children;
	int max_height = 1;
	int height;

	if (ptr == NULL)
		return 1;

	list_for_each_entry(ptr, &(node->children), siblings) {
		height = __basic_tree_height(ptr);
		if (max_height < height)
			max_height = height;
	}
	return max_height+1;
}

void __basic_tree_destroy_tree(tnode *node, basic_tree_data_clean_func func, basic_tree_data_clean_args args) {
	tnode *ptr;

	if (basic_tree_is_leaf(node)) {
		basic_tree_destroy(node, func, args);
		return;
	}

	list_for_each_entry(ptr, &(node->children), siblings) {
		__basic_tree_destroy_tree(ptr, func, args);
	}
}