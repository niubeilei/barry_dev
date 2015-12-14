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
//   A hash table maintains associations between
//   key values and data values.  The type of the key values
//   and the type of the data values is arbitrary.  The
//   functions for hash computation and key comparison are
//   provided by the creator of the table.
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_aos_util_hashtab_h
#define Aos_aos_util_hashtab_h

#include "aosUtil/Types.h"

struct aos_hashtab;

typedef u32 (*aos_hash_func)(struct aos_hashtab *h, void *key);
typedef int (*aos_hash_comp_func)(struct aos_hashtab *h, 
								  void *key1, void *key2);
struct aos_hashtab_node 
{
	void *key;
	void *data;
	struct aos_hashtab_node *next;
};

typedef struct aos_hashtab
{
	struct aos_hashtab_node **htable;	// hash table
	u32 					size;		// number of slots in hash table
	u32 					noe;		// number of elements in hash table 
	u32						max;		// maximum elements
	aos_hash_func			func;		// hash func.
	aos_hash_comp_func	keycmp; 	// key comparison function
} aos_hashtab_t;

struct aos_hashtab_info 
{
	u32 slots_used;
	u32 max_chain_len;
};

static inline u32 aos_hashtab_get_noe(struct aos_hashtab *h) 
{
	return h->noe;
}

//
// Creates a new hash table with the specified characteristics.
// Returns NULL if insufficent space is available
//
struct aos_hashtab *
aos_hashtab_create(aos_hash_func hashfunc,
               aos_hash_comp_func keycomp,
               const u32 size,
			   const u32 max);


//
// Inserts data into a hash table. If the data already exists in 
// the table, it is an error.
//
// Parameters:
// table: 	The hashtable
// key:		The key
// data:	The data to be inserted
//
// Returns 
// 0					if success
//
// Errors:
// -AosRc_OutOfMemory 	if memory allocation error,
// -AosRc_EntryExist	if the entry already exist,
// -AosRc_ProgError     for general errors 
//
int aos_hashtab_insert(struct aos_hashtab *table, 
						 void *key, 
						 void *data);

//
// Removes the entry with the specified key from the hash table.
// If the entry is found, apply 'destroy' function to (key, data, args)
// 
// Parameters:
// table:	the table
// key:		the key
// destroy:	the destroy function (optional)
// args:	the args passed to 'destroy'. 
//
// Returns:
// 0		Upon success
//
int aos_hashtab_remove(struct aos_hashtab *h, void *key,
		   				 void (*destroy)(void *key, 
							 			 void *data, 
										 void *args),
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
int aos_hashtab_replace(struct aos_hashtab *h, void *k, void *d,
		    void (*destroy)(void *k, void *d, void *args),
		    void *args);

/*
 * Searches for the entry with the specified key in the hash table.
 *
 * Returns NULL if no entry has the specified key or
 * the datum of the entry otherwise.
 */
void * aos_hashtab_search(struct aos_hashtab *h, void *k);

/*
 * Destroys the specified hash table.
 */
void aos_hashtab_delete(struct aos_hashtab *h);

/*
 * Applies the specified apply function to (key,datum,args)
 * for each entry in the specified hash table.
 *
 * The order in which the function is applied to the entries
 * is dependent upon the internal structure of the hash table.
 *
 * If apply returns a non-zero status, then hashtab_map will cease
 * iterating through the hash table and will propagate the error
 * return to its caller.
 */
int aos_hashtab_map(struct aos_hashtab *h,
		int (*apply)(void *k, void *d, void *args),
		void *args);

/*
 * Same as aos_hashtab_map, except that if apply returns a non-zero status,
 * then the (key,datum) pair will be removed from the hashtab and the
 * destroy function will be applied to (key,datum,args).
 */
u32 aos_hashtab_map_remove_on_error(struct aos_hashtab *h,
                                 int (*apply)(void *k, void *d, void *args),
                                 void (*destroy)(void *k, void *d, void *args),
                                 void *args);


/* Fill info with some hash table statistics */
// void aos_hashtab_stat(struct aos_hashtab *h, struct aos_hashtab_info *info);

#endif

