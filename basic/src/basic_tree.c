#include <stdlib.h>
#include <basic_tree.h>
#include <basic_queue.h>

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
 * function definitions
 */
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
	btnode *temp;

	list_for_each_entry_safe(ptr, temp, &(node->children), siblings) {
		__basic_tree_destroy_tree(ptr, func, args);
	}
	basic_tree_destroy(node, func, args);
}

int __basic_tree_traverse_dfs(btnode *node, int depth, bttf meet_func, btta meet_args, bttf done_func, btta done_args) {
	btnode *ptr, *n;

	if (depth == 0)
		return 0;

	if (node == NULL)
		return 0;

	if (meet_func != NULL)
		if (meet_func(node->data, meet_args))
			return 1;

	list_for_each_entry_safe(ptr, n, &(node->children), siblings) {
		if (__basic_tree_traverse_dfs(ptr, depth-1, meet_func, meet_args, done_func, done_args))
			return 1;
	}

	if (done_func != NULL)
		if(done_func(node->data, done_args))
			return 1;

	return 0;
}

void __basic_tree_traverse_bfs(btnode *root, int max_depth, bttf meet_func, btta meet_args, bttf done_func, btta done_args) {
	btnode *current, *ptr, *n;
	int cur_nodes = 0, next_nodes = 0;
	struct basic_queue queue;
	int depth = 0;

	if (root == NULL)
		return;

	if (max_depth == 0)
		return;

	basic_queue_init(&queue);

	if (meet_func != NULL)
		if (meet_func(root->data, meet_args))
			goto ret;
	basic_queue_push((void *)root, &queue);
	cur_nodes = 1;
	depth = 1;

	while (!basic_queue_is_empty(&queue)) {
		current = (btnode *)basic_queue_pop(&queue);
		cur_nodes--;

		if (depth == max_depth)
			goto done;

		list_for_each_entry_safe(ptr, n, &(current->children), siblings) {
			if (meet_func != NULL)
				if (meet_func(ptr->data, meet_args))
					goto ret;
			basic_queue_push((void *)ptr, &queue);
			next_nodes++;
		}

		if (cur_nodes == 0) {
			depth++;
			cur_nodes = next_nodes;
			next_nodes = 0;
		}

		done:

		if (done_func != NULL)
			if (done_func(current->data, done_args))
				goto ret;
	}

	ret:

	basic_queue_destroy(&queue, NULL, NULL);
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