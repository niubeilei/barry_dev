////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 03/09/2008: copied from hashtab_gen.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_hashtab_gen_h
#define aos_util_hashtab_gen_h

#include "util_c/types.h"
#include <linux/types.h>

#define AOS_hashtab_gen_MAX_NODES	0xffffffff

typedef struct aos_hashtab_gen_node 
{
	void *obj;
	struct aos_hashtab_gen_node *next;
} aos_hashtab_gen_node_t;

struct aos_hashtab_gen;
struct aos_dyn_array;

typedef u32 (*aos_hashtab_gen_hash_func_t)(
		struct aos_hashtab_gen *h, 
		void *obj);

typedef int (*aos_hashtab_gen_key_cmp_func_t)(
		struct aos_hashtab_gen *h, 
		void *obj1, 
		void *obj2);

typedef int (*aos_hashtab_gen_destroy_func_t)(
		void *obj, 
		void *args);

typedef struct aos_hashtab_gen 
{
	aos_hashtab_gen_node_t **htable;	// hash table 
	u32 				size;		// number of slots in hash table
	u32 				nel;		// number of elements in hash table
	aos_hashtab_gen_hash_func_t		hash_func;	// hash function
	aos_hashtab_gen_key_cmp_func_t	keycmp;		// key compare function
} aos_hashtab_gen_t;

struct aos_hashtab_gen_info {
	u32 slots_used;
	u32 max_chain_len;
};


/*
 * Creates a new hash table with the specified characteristics.
 *
 * Returns NULL if insufficent space is available or
 * the new hash table otherwise.
 */
extern aos_hashtab_gen_t *
aos_hashtab_gen_create(aos_hashtab_gen_hash_func_t hash_func,
                   aos_hashtab_gen_key_cmp_func_t key_comp_func,
                   u32 size);

/*
 * Inserts the specified (key, datum) pair into the specified hash table.
 *
 * Returns -ENOMEM on memory allocation error,
 * -EEXIST if there is already an entry with the same key,
 * -EINVAL for general errors or
 * 0 otherwise.
 */
int aos_hashtab_gen_insert(
		struct aos_hashtab_gen *h, 
		void *obj, 
		const int override_flag, 
		int (*hold_func)(void *obj), 
		int (*put_func)(void *obj));

/*
 * Removes the entry with the specified key from the hash table.
 * Applies the specified destroy function to (key,datum,args) for
 * the entry.
 
 * Returns -ENOENT if no entry has the specified key,
 * -EINVAL for general errors or
 *0 otherwise.
 */
int aos_hashtab_gen_remove(struct aos_hashtab_gen *h, void *key,
		   aos_hashtab_gen_destroy_func_t destroy,
		   void *args);

/*
 * Insert or replace the specified (key, datum) pair in the specified
 * hash table.  If an entry for the specified key already exists,
 * then the specified destroy function is applied to (key,datum,args)
 * for the entry prior to replacing the entry's contents.
 *
 * Returns -ENOMEM if insufficient space is available,
 * -EINVAL for general errors or
 * 0 otherwise.
 */
int aos_hashtab_gen_replace(
		struct aos_hashtab_gen *h, 
		void *obj,
		aos_hashtab_gen_destroy_func_t destroy,
		void *args);

/*
 * Searches for the entry with the specified key in the hash table.
 *
 * Returns NULL if no entry has the specified key or
 * the datum of the entry otherwise.
 */
void * aos_hashtab_gen_search(struct aos_hashtab_gen *h, void *obj);


extern int aos_hashtab_gen_search_multiple(
		aos_hashtab_gen_t *h, 
		void *key, 
		struct aos_dyn_array *array);

/*
 * Destroys the specified hash table.
 */
void aos_hashtab_gen_destroy(
		struct aos_hashtab_gen *h, 
		aos_hashtab_gen_destroy_func_t destroy, 
		void *args);

/*
 * Applies the specified apply function to (key,datum,args)
 * for each entry in the specified hash table.
 *
 * The order in which the function is applied to the entries
 * is dependent upon the internal structure of the hash table.
 *
 * If apply returns a non-zero status, then aos_hashtab_gen_map will cease
 * iterating through the hash table and will propagate the error
 * return to its caller.
 */
int aos_hashtab_gen_map(
		struct aos_hashtab_gen *h,
		int (*apply)(void *d, void *args),
		void *args);

/*
 * Same as aos_hashtab_gen_map, except that if apply returns a non-zero status,
 * then the (key,datum) pair will be removed from the aos_hashtab_gen and the
 * destroy function will be applied to (key,datum,args).
 */
int aos_hashtab_gen_map_remove_on_error(
		struct aos_hashtab_gen *h,
        int (*apply)(void *d, void *args),
        aos_hashtab_gen_destroy_func_t destroy,
        void *args);


/* Fill info with some hash table statistics */
int aos_hashtab_gen_stat(
		struct aos_hashtab_gen *h, 
		struct aos_hashtab_gen_info *info);

#endif	/* _SS_hashtab_gen_H */
