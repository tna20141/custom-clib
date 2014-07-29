#ifndef _BASIC_LIST_H
#define _BASIC_LIST_H

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

#include "basic.h"

struct bl_head {
	struct bl_head *next, *prev;
};

#define BL_HEAD_INIT(name) { &(name), &(name) }

#define BL_HEAD(name) \
	struct bl_head name = BL_HEAD_INIT(name)

static inline void BL_INIT_HEAD(struct bl_head *list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Insert a neww entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __bl_add(struct bl_head *neww,
			      struct bl_head *prev,
			      struct bl_head *next)
{
	next->prev = neww;
	neww->next = next;
	neww->prev = prev;
	prev->next = neww;
}

/**
 * bl_add - add a neww entry
 * @neww: neww entry to be added
 * @head: list head to add it after
 *
 * Insert a neww entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void bl_add(struct bl_head *neww, struct bl_head *head)
{
	__bl_add(neww, head, head->next);
}


/**
 * bl_add_tail - add a neww entry
 * @neww: neww entry to be added
 * @head: list head to add it before
 *
 * Insert a neww entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void bl_add_tail(struct bl_head *neww, struct bl_head *head)
{
	__bl_add(neww, head->prev, head);
}

/**
 * bl_add_after - add a neww entry after a given entry
 * @neww: neww entry to be added
 * @next: the entry in a list after which the neww entry is added
 *
 * This function is added by me. It might be useful sometimes.
 */
static inline void bl_add_after(struct bl_head *neww, struct bl_head *next)
{
	__bl_add(neww, next, next->next);
}
/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __bl_del(struct bl_head *prev, struct bl_head *next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * bl_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: bl_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void __bl_del_entry(struct bl_head *entry)
{
	__bl_del(entry->prev, entry->next);
}

static inline void bl_del(struct bl_head *entry)
{
	__bl_del(entry->prev, entry->next);
	/* not necessary right now
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
	*/
}

/**
 * bl_replace - replace old entry by neww one
 * @old : the element to be replaced
 * @neww : the neww element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void bl_replace(struct bl_head *old,
				struct bl_head *neww)
{
	neww->next = old->next;
	neww->next->prev = neww;
	neww->prev = old->prev;
	neww->prev->next = neww;
}

static inline void bl_replace_init(struct bl_head *old,
					struct bl_head *neww)
{
	bl_replace(old, neww);
	BL_INIT_HEAD(old);
}

/**
 * bl_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void bl_del_init(struct bl_head *entry)
{
	__bl_del_entry(entry);
	BL_INIT_HEAD(entry);
}

/**
 * bl_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void bl_move(struct bl_head *list, struct bl_head *head)
{
	__bl_del_entry(list);
	bl_add(list, head);
}

/**
 * bl_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void bl_move_tail(struct bl_head *list,
				  struct bl_head *head)
{
	__bl_del_entry(list);
	bl_add_tail(list, head);
}

/**
 * bl_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int bl_is_last(const struct bl_head *list,
				const struct bl_head *head)
{
	return list->next == head;
}

/**
 * bl_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int bl_empty(const struct bl_head *head)
{
	return head->next == head;
}

/**
 * bl_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using bl_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is bl_del_init(). Eg. it cannot be used
 * if another CPU could re-bl_add() it.
 */
static inline int bl_empty_careful(const struct bl_head *head)
{
	struct bl_head *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * bl_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void bl_rotate_left(struct bl_head *head)
{
	struct bl_head *first;

	if (!bl_empty(head)) {
		first = head->next;
		bl_move_tail(first, head);
	}
}

/**
 * bl_rotate_right - rotate the list to the right
 * @head: the head of the list
 *
 * added by me, since it doesn't feel right with bl_rotate_left alone
 */
static inline void bl_rotate_right(struct bl_head *head)
{
	struct bl_head *last;

	if (!bl_empty(head)) {
		last = head->prev;
		bl_move(last, head);
	}
}

/**
 * bl_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int bl_is_singular(const struct bl_head *head)
{
	return !bl_empty(head) && (head->next == head->prev);
}

static inline void __bl_cut_position(struct bl_head *list,
		struct bl_head *head, struct bl_head *entry)
{
	struct bl_head *neww_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = neww_first;
	neww_first->prev = head;
}

/**
 * bl_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void bl_cut_position(struct bl_head *list,
		struct bl_head *head, struct bl_head *entry)
{
	if (bl_empty(head))
		return;
	if (bl_is_singular(head) &&
		(head->next != entry && head != entry))
		return;
	if (entry == head)
		BL_INIT_HEAD(list);
	else
		__bl_cut_position(list, head, entry);
}

static inline void __bl_splice(const struct bl_head *list,
				 struct bl_head *prev,
				 struct bl_head *next)
{
	struct bl_head *first = list->next;
	struct bl_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * bl_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void bl_splice(const struct bl_head *list,
				struct bl_head *head)
{
	if (!bl_empty(list))
		__bl_splice(list, head, head->next);
}

/**
 * bl_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void bl_splice_tail(struct bl_head *list,
				struct bl_head *head)
{
	if (!bl_empty(list))
		__bl_splice(list, head->prev, head);
}

/**
 * bl_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void bl_splice_init(struct bl_head *list,
				    struct bl_head *head)
{
	if (!bl_empty(list)) {
		__bl_splice(list, head, head->next);
		BL_INIT_HEAD(list);
	}
}

/**
 * bl_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the neww list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void bl_splice_tail_init(struct bl_head *list,
					 struct bl_head *head)
{
	if (!bl_empty(list)) {
		__bl_splice(list, head->prev, head);
		BL_INIT_HEAD(list);
	}
}

/**
 * bl_entry - get the struct for this entry
 * @ptr:	the &struct bl_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define bl_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * bl_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define bl_first_entry(ptr, type, member) \
	bl_entry((ptr)->next, type, member)

/**
 * bl_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define bl_first_entry_or_null(ptr, type, member) \
	(!bl_empty(ptr) ? bl_first_entry(ptr, type, member) : NULL)

/**
 * bl_for_each	-	iterate over a list
 * @pos:	the &struct bl_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define bl_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * bl_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct bl_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define bl_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * bl_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct bl_head to use as a loop cursor.
 * @n:		another &struct bl_head to use as temporary storage
 * @head:	the head for your list.
 */
#define bl_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * bl_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct bl_head to use as a loop cursor.
 * @n:		another &struct bl_head to use as temporary storage
 * @head:	the head for your list.
 */
#define bl_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * bl_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bl_for_each_entry(pos, head, member)				\
	for (pos = bl_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = bl_entry(pos->member.next, typeof(*pos), member))

/**
 * bl_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bl_for_each_entry_reverse(pos, head, member)			\
	for (pos = bl_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = bl_entry(pos->member.prev, typeof(*pos), member))

/**
 * bl_prepare_entry - prepare a pos entry for use in bl_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in bl_for_each_entry_continue().
 */
#define bl_prepare_entry(pos, head, member) \
	((pos) ? : bl_entry(head, typeof(*pos), member))

/**
 * bl_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define bl_for_each_entry_continue(pos, head, member) 		\
	for (pos = bl_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = bl_entry(pos->member.next, typeof(*pos), member))

/**
 * bl_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define bl_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = bl_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = bl_entry(pos->member.prev, typeof(*pos), member))

/**
 * bl_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define bl_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = bl_entry(pos->member.next, typeof(*pos), member))

/**
 * bl_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bl_for_each_entry_safe(pos, n, head, member)			\
	for (pos = bl_entry((head)->next, typeof(*pos), member),	\
		n = bl_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = bl_entry(n->member.next, typeof(*n), member))

/**
 * bl_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define bl_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = bl_entry(pos->member.next, typeof(*pos), member), 		\
		n = bl_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = bl_entry(n->member.next, typeof(*n), member))

/**
 * bl_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define bl_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = bl_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = bl_entry(n->member.next, typeof(*n), member))

/**
 * bl_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define bl_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = bl_entry((head)->prev, typeof(*pos), member),	\
		n = bl_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = bl_entry(n->member.prev, typeof(*n), member))

/**
 * bl_safe_reset_next - reset a stale bl_for_each_entry_safe loop
 * @pos:	the loop cursor used in the bl_for_each_entry_safe loop
 * @n:		temporary storage used in bl_for_each_entry_safe
 * @member:	the name of the list_struct within the struct.
 *
 * bl_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and bl_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define bl_safe_reset_next(pos, n, member)				\
	n = bl_entry(pos->member.next, typeof(*pos), member)

#endif
