#include <stdlib.h>
#include <basic_tree.h>

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


btec basic_tree_insert(btnode *node, btnode *parent, int pos) {
	btnode *ptr;

	/* special cases */
	if (pos == 0) {
		basic_tree_prepend(node, parent);
		return 0;
	}
	if (pos == -1) {
		basic_tree_append(node, parent);
		return 0;
	}

	/* other cases */
	if (pos < 0)
		ptr = __basic_tree_nth_child(parent, pos+1);
	else
		ptr = __basic_tree_nth_child(parent, pos-1);

	/* can't find the right position, return error */
	if (ptr == NULL)
		return BASIC_TREE_INDEX_ERROR;

	/* insert node */
	if (pos < 0)
		list_add_after(&(node->siblings), ptr->siblings.prev);
	else
		list_add_after(&(node->siblings), &(ptr->siblings));

	/* set new node's parent */
	node->parent = parent;
	return 0;
}

btnode *__basic_tree_nth_child(btnode *parent, int pos) {
	btnode *ptr;
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

int __basic_tree_num_nodes(btnode *node) {
	btnode *ptr;
	int num = 1;

	if (basic_tree_is_leaf(node))
		return 1;

	/* count nodes recursively */
	list_for_each_entry(ptr, &(node->children), siblings) {
		num += __basic_tree_num_nodes(ptr);
	}
	return num;
}

int __basic_tree_height(btnode *node) {
	btnode *ptr;
	int max_height = 1;
	int height;

	if (basic_tree_is_leaf(node))
		return 1;

	list_for_each_entry(ptr, &(node->children), siblings) {
		height = __basic_tree_height(ptr);
		if (max_height < height)
			max_height = height;
	}
	return max_height+1;
}

void __basic_tree_destroy_tree(btnode *node, basic_tree_data_clean_func func, basic_tree_data_clean_args args) {
	btnode *ptr;

	list_for_each_entry(ptr, &(node->children), siblings) {
		__basic_tree_destroy_tree(ptr, func, args);
	}
	basic_tree_destroy(node, func, args);
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