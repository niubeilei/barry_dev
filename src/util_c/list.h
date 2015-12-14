////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: List.h
// Description:
//   
//
// Modification History:
// 01/18/2008: Copied from aosUtil by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_list_h
#define aos_util_list_h

#include "KernelSimu/prefetch.h"
#include "KernelSimu/system.h"
#include "KernelSimu/kernel.h"

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)1)->MEMBER - 1)

/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define AOS_LIST_POISON1  ((void *) 0x00100100)
#define AOS_LIST_POISON2  ((void *) 0x00200200)

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

typedef struct aos_list_head 
{
	struct aos_list_head *next, *prev;
} aos_list_head_t;

#ifndef AOS_LIST_HEAD_INIT
#define AOS_LIST_HEAD_INIT(name) { &(name), &(name) }
#endif

#ifndef AOS_LIST_HEAD
#define AOS_LIST_HEAD(name) \
	struct aos_list_head name = AOS_LIST_HEAD_INIT(name)
#endif

#ifndef AOS_INIT_LIST_HEAD
#define AOS_INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)
#endif

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __aos_list_add(struct aos_list_head *new_entry,
			      struct aos_list_head *prev,
			      struct aos_list_head *next)
{
	next->prev = new_entry;
	new_entry->next = next;
	new_entry->prev = prev;
	prev->next = new_entry;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void aos_list_add(struct aos_list_head *new_entry, 
								struct aos_list_head *head)
{
	__aos_list_add(new_entry, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void aos_list_add_tail(struct aos_list_head *new_entry, 
							struct aos_list_head *head)
{
	__aos_list_add(new_entry, head->prev, head);
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __aos_list_add_rcu(struct aos_list_head * new_entry,
		struct aos_list_head * prev, 
		struct aos_list_head * next)
{
	new_entry->next = next;
	new_entry->prev = prev;
	smp_wmb();
	next->prev = new_entry;
	prev->next = new_entry;
}

/**
 * list_add_rcu - add a new entry to rcu-protected list
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as list_add_rcu()
 * or list_del_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * list_for_each_entry_rcu().
 */
static inline void aos_list_add_rcu(struct aos_list_head *new_entry, 
						struct aos_list_head *head)
{
	__aos_list_add_rcu(new_entry, head, head->next);
}

/**
 * list_add_tail_rcu - add a new entry to rcu-protected list
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as list_add_tail_rcu()
 * or list_del_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * list_for_each_entry_rcu().
 */
static inline void aos_list_add_tail_rcu(struct aos_list_head *new_entry,
					struct aos_list_head *head)
{
	__aos_list_add_rcu(new_entry, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __aos_list_del(struct aos_list_head * prev, 
						struct aos_list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void aos_list_del(struct aos_list_head *entry)
{
	__aos_list_del(entry->prev, entry->next);
	entry->next = (struct aos_list_head *)AOS_LIST_POISON1;
	entry->prev = (struct aos_list_head *)AOS_LIST_POISON2;
}

/**
 * list_del_rcu - deletes entry from list without re-initialization
 * @entry: the element to delete from the list.
 *
 * Note: list_empty on entry does not return true after this,
 * the entry is in an undefined state. It is useful for RCU based
 * lockfree traversal.
 *
 * In particular, it means that we can not poison the forward
 * pointers that may still be used for walking the list.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as list_del_rcu()
 * or list_add_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * list_for_each_entry_rcu().
 *
 * Note that the caller is not permitted to immediately free
 * the newly deleted entry.  Instead, either synchronize_kernel()
 * or call_rcu() must be used to defer freeing until an RCU
 * grace period has elapsed.
 */
static inline void aos_list_del_rcu(struct aos_list_head *entry)
{
	__aos_list_del(entry->prev, entry->next);
	entry->prev = (struct aos_list_head *)AOS_LIST_POISON2;
}

/*
 * list_replace_rcu - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * The old entry will be replaced with the new entry atomically.
 */
static inline void aos_list_replace_rcu(struct aos_list_head *old, 
					struct aos_list_head *new_entry){
	new_entry->next = old->next;
	new_entry->prev = old->prev;
	smp_wmb();
	new_entry->next->prev = new_entry;
	new_entry->prev->next = new_entry;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void aos_list_del_init(struct aos_list_head *entry)
{
	__aos_list_del(entry->prev, entry->next);
	entry->next = entry;
	entry->prev = entry;
	// AOS_INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void aos_list_move(struct aos_list_head *list, 
					struct aos_list_head *head)
{
        __aos_list_del(list->prev, list->next);
        aos_list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void aos_list_move_tail(struct aos_list_head *list,
				  struct aos_list_head *head)
{
        __aos_list_del(list->prev, list->next);
        aos_list_add_tail(list, head);
}

//
// list_empty - tests whether a list is empty
// @head: the list to test.
//
static inline int aos_list_empty(const struct aos_list_head *head)
{
	return head->next == head;
}

//
// list_empty_careful - tests whether a list is
// empty _and_ checks that no other CPU might be
// in the process of still modifying either member
//
// NOTE: using list_empty_careful() without synchronization
// can only be safe if the only activity that can happen
// to the list entry is list_del_init(). Eg. it cannot be used
// if another CPU could re-list_add() it.
//
// @head: the list to test.
//
static inline int aos_list_empty_careful(const struct aos_list_head *head)
{
	struct aos_list_head *next = head->next;
	return (next == head) && (next == head->prev);
}


static inline void __aos_list_splice(struct aos_list_head *list,
				 struct aos_list_head *head)
{
	struct aos_list_head *first = list->next;
	struct aos_list_head *last = list->prev;
	struct aos_list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}


//
// list_splice - join two lists
// @list: the new list to add.
// @head: the place to add it in the first list.
//
static inline void aos_list_splice(struct aos_list_head *list, 
					struct aos_list_head *head)
{
	if (!aos_list_empty(list))
		__aos_list_splice(list, head);
}


//
// list_splice_init - join two lists and reinitialise the emptied list.
// @list: the new list to add.
// @head: the place to add it in the first list.
//
// The list at @list is reinitialised
//
static inline void aos_list_splice_init(struct aos_list_head *list,
				    struct aos_list_head *head)
{
	if (!aos_list_empty(list)) {
		__aos_list_splice(list, head);
		AOS_INIT_LIST_HEAD(list);
	}
}


//
// list_entry - get the struct for this entry
// @ptr:	the &struct aos_list_head pointer.
// @type:	the type of the struct this is embedded in.
// @member:	the name of the list_struct within the struct.
//
#define aos_list_entry(ptr, type, member) \
	container_of(ptr, type, member)

//
// list_for_each	-	iterate over a list
// @pos:	the &struct aos_list_head to use as a loop counter.
// @head:	the head for your list.
//
#define aos_list_for_each(pos, head) \
	for (pos = (head)->next; prefetch(pos->next), pos != (head); \
        	pos = pos->next)

//
// __list_for_each	-	iterate over a list
// @pos:	the &struct aos_list_head to use as a loop counter.
// @head:	the head for your list.
//
// This variant differs from list_for_each() in that it's the
// simplest possible list iteration code, no prefetching is done.
// Use this for code that knows the list to be very short (empty
// or 1 entry) most of the time.
//
#define __aos_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

//
// list_for_each_prev	-	iterate over a list backwards
// @pos:	the &struct aos_list_head to use as a loop counter.
// @head:	the head for your list.
//
#define aos_list_for_each_prev(pos, head) \
	for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
        	pos = pos->prev)

//
// list_for_each_safe	-	iterate over a list safe against removal of list entry
// @pos:	the &struct aos_list_head to use as a loop counter.
// @n:		another &struct aos_list_head to use as temporary storage
// @head:	the head for your list.
//
#define aos_list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

//
// Iterate over list of given type safe against removal of list entry.
// This means that during the looping, one can safely remove the 
// element 'pos' from the list. 
//
// @pos:	the type * to use as a loop counter.
// @n:		another type * to use as temporary storage
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
// 
#define aos_list_for_each_entry_safe(pos, n, head, member)				\
		for (pos = aos_list_entry((head)->next, typeof(*pos), member),	\
			 n=aos_list_entry(pos->member.next, typeof(*pos), member);	\
			 &pos->member != (head); 									\
			 pos=n, n=aos_list_entry(n->member.next, typeof(*n), member))

//
// Iterate over list of given type. 
// !!!!!!!!IMPORTANT!!!!!: Do not delete the element from the list.
// If need to delete the element, use 'aos_list_for_each_entry_safe(...)'
//
// @pos:	the type * to use as a loop counter.
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
//
#define aos_list_for_each_entry(pos, head, member)						\
	for (pos = aos_list_entry((head)->next, typeof(*pos), member);		\
	     prefetch(pos->member.next), &pos->member != (head); 			\
	     pos = aos_list_entry(pos->member.next, typeof(*pos), member))


//
// list_for_each_entry_reverse - iterate backwards over list of given type.
// @pos:	the type * to use as a loop counter.
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
//
#define aos_list_for_each_entry_reverse(pos, head, member)			\
	for (pos = aos_list_entry((head)->prev, typeof(*pos), member);	\
	     prefetch(pos->member.prev), &pos->member != (head); 	\
	     pos = aos_list_entry(pos->member.prev, typeof(*pos), member))


/*
//
// list_prepare_entry - prepare a pos entry for use as a start point in
//			list_for_each_entry_continue
// @pos:	the type * to use as a start point
// @head:	the head of the list
// @member:	the name of the list_struct within the struct.
//
#define aos_list_prepare_entry(pos, head, member) \
	((pos) ? : aos_list_entry(head, typeof(*pos), member))
*/

/*
//
// list_for_each_entry_continue -	iterate over list of given type
//			continuing after existing point
// @pos:	the type * to use as a loop counter.
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
//
#define aos_list_for_each_entry_continue(pos, head, member) 		\
	for (pos = list_entry(pos->member.next, typeof(*pos), member);	\
	     prefetch(pos->member.next), &pos->member != (head);	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))
*/

/*
//
// list_for_each_entry_safe - iterate over list of given type safe against 
// removal of list entry
// @pos:	the type * to use as a loop counter.
// @n:		another type * to use as temporary storage
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
//
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))
*/

/*
//
// list_for_each_rcu	-	iterate over an rcu-protected list
// @pos:	the &struct aos_list_head to use as a loop counter.
// @head:	the head for your list.
//
// This list-traversal primitive may safely run concurrently with
// the _rcu list-mutation primitives such as list_add_rcu()
// as long as the traversal is guarded by rcu_read_lock().
//
#define list_for_each_rcu(pos, head) \
	for (pos = (head)->next; prefetch(pos->next), pos != (head); \
        	pos = rcu_dereference(pos->next))

#define __list_for_each_rcu(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = rcu_dereference(pos->next))
*/

/*
//
// list_for_each_safe_rcu	-	iterate over an rcu-protected list safe
//					against removal of list entry
// @pos:	the &struct aos_list_head to use as a loop counter.
// @n:		another &struct aos_list_head to use as temporary storage
// @head:	the head for your list.
//
// This list-traversal primitive may safely run concurrently with
// the _rcu list-mutation primitives such as list_add_rcu()
// as long as the traversal is guarded by rcu_read_lock().
//
#define list_for_each_safe_rcu(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = rcu_dereference(n), n = pos->next)
*/

/*
//
// list_for_each_entry_rcu	-	iterate over rcu list of given type
// @pos:	the type * to use as a loop counter.
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
//
// This list-traversal primitive may safely run concurrently with
// the _rcu list-mutation primitives such as list_add_rcu()
// as long as the traversal is guarded by rcu_read_lock().
//
#define list_for_each_entry_rcu(pos, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     prefetch(pos->member.next), &pos->member != (head); 	\
	     pos = rcu_dereference(list_entry(pos->member.next, 	\
					typeof(*pos), member)))
*/


/*
//
// list_for_each_continue_rcu	-	iterate over an rcu-protected list
//			continuing after existing point.
// @pos:	the &struct aos_list_head to use as a loop counter.
// @head:	the head for your list.
//
// This list-traversal primitive may safely run concurrently with
// the _rcu list-mutation primitives such as list_add_rcu()
// as long as the traversal is guarded by rcu_read_lock().
//
#define list_for_each_continue_rcu(pos, head) \
	for ((pos) = (pos)->next; prefetch((pos)->next), (pos) != (head); \
        	(pos) = rcu_dereference((pos)->next))
*/

#endif

