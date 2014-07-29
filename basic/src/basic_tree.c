#include <stdlib.h>
#include <basic.h>
#include <basic_tree.h>
#include <basic_queue.h>

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

/*
 * function definitions
 */
basic_ec bt_insert(btnode_t *node, btnode_t *parent, int pos)
{
	btnode_t *ptr;

	/* special cases */
	if (pos == 0) {
		bt_prepend(node, parent);
		return 0;
	}
	if (pos == -1) {
		bt_append(node, parent);
		return 0;
	}

	/* other cases */
	if (pos < 0)
		ptr = __bt_nth_child(parent, pos+1);
	else
		ptr = __bt_nth_child(parent, pos-1);

	/* can't find the right position, return error */
	if (ptr == NULL)
		return -BE_INVAL;

	/* insert node */
	if (pos < 0)
		bl_add_after(&(node->siblings), ptr->siblings.prev);
	else
		bl_add_after(&(node->siblings), &(ptr->siblings));

	/* set new node's parent */
	node->parent = parent;
	return 0;
}

btnode_t *bt_remove(btnode_t *parent, int pos)
{
	btnode_t *ptr;

	ptr = __bt_nth_child(parent, pos);

	if (ptr == NULL)
		return NULL;

	bl_del_init(&(ptr->siblings));
	return ptr;
}

btnode_t *__bt_nth_child(btnode_t *parent, int pos)
{
	btnode_t *ptr;
	int i = 0;

	if (pos < 0)
		bl_for_each_entry_reverse(ptr, &(parent->children), siblings) {
			i--;
			if (i == pos)
				return ptr;
		}
	else
		bl_for_each_entry(ptr, &(parent->children), siblings) {
			if (i == pos)
				return ptr;
			i++;
		}

	/* if pos is out of bound, return NULL */
	return NULL;
}

int __bt_num_nodes(btnode_t *node)
{
	btnode_t *ptr;
	int num = 1;

	if (bt_is_leaf(node))
		return 1;

	/* count nodes recursively */
	bl_for_each_entry(ptr, &(node->children), siblings)
		num += __bt_num_nodes(ptr);
	return num;
}

int __bt_height(btnode_t *node)
{
	btnode_t *ptr;
	int max_height = 0;
	int height;

	if (bt_is_leaf(node))
		return 0;

	bl_for_each_entry(ptr, &(node->children), siblings) {
		height = __bt_height(ptr);
		if (max_height < height)
			max_height = height;
	}
	return max_height+1;
}

void __bt_destroy_tree(btnode_t *node, bt_data_clean_func func, bt_data_clean_args args)
{
	btnode_t *ptr;
	btnode_t *temp;

	bl_for_each_entry_safe(ptr, temp, &(node->children), siblings)
		__bt_destroy_tree(ptr, func, args);

	bt_destroy(node, func, args);
}

int __bt_traverse_dfs(btnode_t *node, int depth,
		bttf meet_func, btta meet_args, bttf done_func, btta done_args)
{
	btnode_t *ptr, *n;

	if (node == NULL)
		return 0;

	if (meet_func != NULL)
		if (meet_func(node, meet_args))
			return 1;

	if (depth == 0)
		goto done;

	bl_for_each_entry_safe(ptr, n, &(node->children), siblings)
		if (__bt_traverse_dfs(ptr, depth-1,
				meet_func, meet_args, done_func, done_args))
			return 1;

done:
	if (done_func != NULL)
		if(done_func(node, done_args))
			return 1;

	return 0;
}

void __bt_traverse_bfs(btnode_t *root, int max_depth,
		bttf meet_func, btta meet_args, bttf done_func, btta done_args)
{
	btnode_t *current, *ptr, *n;
	int cur_nodes = 0, next_nodes = 0;
	struct bq_queue queue;
	int depth = 0;

	if (root == NULL)
		return;

	bq_init(&queue);

	if (meet_func != NULL)
		if (meet_func(root, meet_args))
			goto ret;

	bq_in((void *)root, &queue);
	cur_nodes = 1;
	depth = 0;

	while (!bq_is_empty(&queue)) {
		current = (btnode_t *)bq_out(&queue);
		cur_nodes--;

		if (depth == max_depth)
			goto done;

		bl_for_each_entry_safe(ptr, n, &(current->children), siblings) {
			if (meet_func != NULL)
				if (meet_func(ptr, meet_args))
					goto ret;

			bq_in((void *)ptr, &queue);
			next_nodes++;
		}

		if (cur_nodes == 0) {
			depth++;
			cur_nodes = next_nodes;
			next_nodes = 0;
		}

done:
		if (done_func != NULL)
			if (done_func(current, done_args))
				goto ret;
	}

ret:
	bq_destroy(&queue, NULL, NULL);
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
