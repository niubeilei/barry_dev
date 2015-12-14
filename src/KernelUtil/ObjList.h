////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjList.h
// Description:
//	This file is a modification of include/linux/list.h to manage 
//  objects.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelUtil_ObjList_h
#define aos_KernelUtil_ObjList_h


#include <KernelSimu/system.h>
#include <KernelSimu/prefetch.h>

#include "KernelUtil/MgdObj.h"


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

struct aosObjListHead 
{
    struct aosList      	*listprev;          
    struct aosList      	*listnext;          
    struct aosObjListHead 	*prevobj;           
    struct aosObjListHead 	*nextobj;           
    u16                  	objtype;

};

#define AOS_OBJ_LIST_HEAD_INIT(name) { &(name), &(name) }

#define AOS_OBJ_LIST_HEAD(name) \
	struct aosObjListHead name = LIST_HEAD_INIT(name)

#define AOS_INIT_OBJ_LIST_HEAD(ptr) do { \
	(ptr)->nextobj = (ptr); (ptr)->prevobj = (ptr); \
} while (0)


/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prevobj/nextobj entries already!
 */
static inline void __aosObjList_add(
				struct aosObjListHead *newentry,
			    struct aosObjListHead *prev,
			    struct aosObjListHead *next)
{
	next->prevobj = newentry;
	newentry->nextobj = next;
	newentry->prevobj = prev;
	prev->nextobj = newentry;
}


/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void aosObjList_insertAfter(struct aosObjListHead *newentry, struct aosObjListHead *crt)
{
	__aosObjList_add(newentry, crt, crt->nextobj);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void aosObjList_append(struct aosObjListHead *newentry, struct aosObjListHead *head)
{
	__aosObjList_add(newentry, head->prevobj, head);
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prevobj/nextobj entries already!
 */
static inline void __aosObjList_add_rcu(
			struct aosObjListHead * newentry,
			struct aosObjListHead * prev, 
			struct aosObjListHead * next)
{
	newentry->nextobj = next;
	newentry->prevobj = prev;
	smp_wmb();
	next->prevobj = newentry;
	prev->nextobj = newentry;
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
static inline void aosObjList_add_rcu(struct aosObjListHead *newentry, struct aosObjListHead *head)
{
	__aosObjList_add_rcu(newentry, head, head->nextobj);
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
static inline void aosObjList_add_tail_rcu(
					struct aosObjListHead *newentry,
					struct aosObjListHead *head)
{
	__aosObjList_add_rcu(newentry, head->prevobj, head);
}

/*
 * Delete a list entry by making the prevobj/nextobj entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prevobj/nextobj entries already!
 */
static inline void __aosObjList_del(struct aosObjListHead * prev, struct aosObjListHead * next)
{
	next->prevobj = prev;
	prev->nextobj = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void aosObjList_del(struct aosObjListHead *entry)
{
	__aosObjList_del(entry->prevobj, entry->nextobj);
	entry->nextobj = (struct aosObjListHead *)AOS_LIST_POISON1;
	entry->prevobj = (struct aosObjListHead *)AOS_LIST_POISON2;
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
static inline void aosObjList_del_rcu(struct aosObjListHead *entry)
{
	__aosObjList_del(entry->prevobj, entry->nextobj);
	entry->prevobj = (struct aosObjListHead *)AOS_LIST_POISON2;

	// 
	// It does not poison nextobj because it wants the concurrency
	// with traversal functions.
	//
}

//
// list_del_init - deletes entry from list and reinitialize it.
// Reinitializing it means to make the entry to point to itself.
//
// @entry: the element to delete from the list.
//
static inline void aosObjList_del_init(struct aosObjListHead *entry)
{
	__aosObjList_del(entry->prevobj, entry->nextobj);
	AOS_INIT_OBJ_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void aosObjList_move(struct aosObjListHead *list, struct aosObjListHead *head)
{
    __aosObjList_del(list->prevobj, list->nextobj);
    aosObjList_insertAfter(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void aosObjList_move_tail(
				struct aosObjListHead *list,
				struct aosObjListHead *head)
{
        __aosObjList_del(list->prevobj, list->nextobj);
        aosObjList_append(list, head);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int aosObjList_empty(const struct aosObjListHead *head)
{
	return head->nextobj == head;
}

/**
 * list_empty_careful - tests whether a list is
 * empty _and_ checks that no other CPU might be
 * in the process of still modifying either member
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 *
 * @head: the list to test.
 */
static inline int aosObjList_empty_careful(const struct aosObjListHead *head)
{
	struct aosObjListHead *next = head->nextobj;
	return (next == head) && (next == head->prevobj);
}


/*
static inline void __aosObjList_splice(
				struct aosObjListHead *list,
				struct aosObjListHead *head)
{
	struct aosObjListHead *first = list->nextobj;
	struct aosObjListHead *last = list->prevobj;
	struct aosObjListHead *at = head->nextobj;

	first->prevobj = head;
	head->nextobj = first;

	last->nextobj = at;
	at->prevobj = last;
}


//
// list_splice - join two lists
// @list: the new list to add.
// @head: the place to add it in the first list.
//
static inline void list_splice(struct aosObjListHead *list, struct aosObjListHead *head)
{
	if (!list_empty(list))
		__list_splice(list, head);
}

//
// list_splice_init - join two lists and reinitialise the emptied list.
// @list: the new list to add.
// @head: the place to add it in the first list.
//
// The list at @list is reinitialised
//
static inline void list_splice_init(struct aosObjListHead *list,
				    struct aosObjListHead *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head);
		INIT_LIST_HEAD(list);
	}
}
*/


//
// list_entry - get the struct for this entry
// @ptr:	the &struct aosObjListHead pointer.
// @type:	the type of the struct this is embedded in.
// @member:	the name of the list_struct within the struct.
//
#define aosObjList_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct aosObjListHead to use as a loop counter.
 * @head:	the head for your list.
 */
#define aosObjList_for_each(pos, head) \
	for (pos = (head)->nextobj, prefetch(pos->nextobj); pos != (head); \
        	pos = pos->nextobj, prefetch(pos->nextobj))

/**
 * __list_for_each	-	iterate over a list
 * @pos:	the &struct aosObjListHead to use as a loop counter.
 * @head:	the head for your list.
 *
 * This variant differs from list_for_each() in that it's the
 * simplest possible list iteration code, no prefetching is done.
 * Use this for code that knows the list to be very short (empty
 * or 1 entry) most of the time.
 */
#define __aosObjList_for_each(pos, head) \
	for (pos = (head)->nextobj; pos != (head); pos = pos->nextobj)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct aosObjListHead to use as a loop counter.
 * @head:	the head for your list.
 */
#define aosObjList_for_each_prev(pos, head) \
	for (pos = (head)->prevobj, prefetch(pos->prevobj); pos != (head); \
        	pos = pos->prevobj, prefetch(pos->prevobj))

/*
//
// list_for_each_safe	-	iterate over a list safe against removal of list entry
// @pos:	the &struct aosObjListHead to use as a loop counter.
// @n:		another &struct aosObjListHead to use as temporary storage
// @head:	the head for your list.
//
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->nextobj, n = pos->nextobj; pos != (head); \
		pos = n, n = pos->nextobj)

//
// list_for_each_entry	-	iterate over list of given type
// @pos:	the type * to use as a loop counter.
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
//
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->nextobj, typeof(*pos), member),	\
		     prefetch(pos->member.nextobj);			\
	     &pos->member != (head); 					\
	     pos = list_entry(pos->member.nextobj, typeof(*pos), member),	\
		     prefetch(pos->member.nextobj))

//
// list_for_each_entry_reverse - iterate backwards over list of given type.
// @pos:	the type * to use as a loop counter.
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
//
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member),	\
		     prefetch(pos->member.prev);			\
	     &pos->member != (head); 					\
	     pos = list_entry(pos->member.prev, typeof(*pos), member),	\
		     prefetch(pos->member.prev))

//
// list_prepare_entry - prepare a pos entry for use as a start point in
//			list_for_each_entry_continue
// @pos:	the type * to use as a start point
// @head:	the head of the list
// @member:	the name of the list_struct within the struct.
//
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))

//
// list_for_each_entry_continue -	iterate over list of given type
//			continuing after existing point
// @pos:	the type * to use as a loop counter.
// @head:	the head for your list.
// @member:	the name of the list_struct within the struct.
//
#define list_for_each_entry_continue(pos, head, member) 		\
	for (pos = list_entry(pos->member.nextobj, typeof(*pos), member),	\
		     prefetch(pos->member.nextobj);			\
	     &pos->member != (head);					\
	     pos = list_entry(pos->member.next, typeof(*pos), member),	\
		     prefetch(pos->member.next))

//
// list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
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

//
// list_for_each_rcu	-	iterate over an rcu-protected list
// @pos:	the &struct aosObjListHead to use as a loop counter.
// @head:	the head for your list.
//
// This list-traversal primitive may safely run concurrently with
// the _rcu list-mutation primitives such as list_add_rcu()
// as long as the traversal is guarded by rcu_read_lock().
//
#define list_for_each_rcu(pos, head) \
	for (pos = (head)->next, prefetch(pos->next); pos != (head); \
        	pos = pos->next, ({ smp_read_barrier_depends(); 0;}), prefetch(pos->next))

#define __list_for_each_rcu(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next, ({ smp_read_barrier_depends(); 0;}))

//
// list_for_each_safe_rcu	-	iterate over an rcu-protected list safe
//					against removal of list entry
// @pos:	the &struct aosObjListHead to use as a loop counter.
// @n:		another &struct aosObjListHead to use as temporary storage
// @head:	the head for your list.
//
// This list-traversal primitive may safely run concurrently with
// the _rcu list-mutation primitives such as list_add_rcu()
// as long as the traversal is guarded by rcu_read_lock().
//
#define list_for_each_safe_rcu(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, ({ smp_read_barrier_depends(); 0;}), n = pos->next)

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
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		     prefetch(pos->member.next);			\
	     &pos->member != (head); 					\
	     pos = list_entry(pos->member.next, typeof(*pos), member),	\
		     ({ smp_read_barrier_depends(); 0;}),		\
		     prefetch(pos->member.next))


//
// list_for_each_continue_rcu	-	iterate over an rcu-protected list
//			continuing after existing point.
// @pos:	the &struct aosObjListHead to use as a loop counter.
// @head:	the head for your list.
//
// This list-traversal primitive may safely run concurrently with
// the _rcu list-mutation primitives such as list_add_rcu()
// as long as the traversal is guarded by rcu_read_lock().
//
#define list_for_each_continue_rcu(pos, head) \
	for ((pos) = (pos)->next, prefetch((pos)->next); (pos) != (head); \
        	(pos) = (pos)->next, ({ smp_read_barrier_depends(); 0;}), prefetch((pos)->next))
*/

#endif
