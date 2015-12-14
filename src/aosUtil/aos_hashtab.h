#ifndef _AOS_CORE_HASHTAB_H_
#define _AOS_CORE_HASHTAB_H_

#include <linux/types.h>

#define AOS_HASHTAB_MAX_NODES	0xffffffff

struct aos_hashtab_node {
	void *key;
	void *datum;
	struct aos_hashtab_node *next;
};

struct aos_hashtab {
	struct aos_hashtab_node **htable;	/* hash table */
	u32 size;			/* number of slots in hash table */
	u32 nel;			/* number of elements in hash table */
	u32 (*hash_value)(struct aos_hashtab *h, void *key);
					/* hash function */
	int (*keycmp)(struct aos_hashtab *h, void *key1, void *key2);
					/* key comparison function */
};

struct aos_hashtab_info {
	u32 slots_used;
	u32 max_chain_len;
};

static inline int aos_hashtab_elements(struct aos_hashtab *h) 
{
		return h->nel;
}

/*
 * Creates a new hash table with the specified characteristics.
 *
 * Returns NULL if insufficent space is available or
 * the new hash table otherwise.
 */
struct aos_hashtab *aos_hashtab_create(u32 (*hash_value)(struct aos_hashtab *h, void *key),
                               int (*keycmp)(struct aos_hashtab *h, void *key1, void *key2),
                               u32 size);

/*
 * Inserts the specified (key, datum) pair into the specified hash table.
 *
 * Returns -ENOMEM on memory allocation error,
 * -EEXIST if there is already an entry with the same key,
 * -EINVAL for general errors or
 * 0 otherwise.
 */
int aos_hashtab_insert(struct aos_hashtab *h, void *k, void *d);

/*
 * Removes the entry with the specified key from the hash table.
 * Applies the specified destroy function to (key,datum,args) for
 * the entry.
 
 * Returns -ENOENT if no entry has the specified key,
 * -EINVAL for general errors or
 *0 otherwise.
 */
int aos_hashtab_remove(struct aos_hashtab *h, void *k,
		   void (*destroy)(void *k, void *d, void *args),
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
void *aos_hashtab_search(struct aos_hashtab *h, void *k);

/*
 * Destroys the specified hash table.
 */
void aos_hashtab_destroy(struct aos_hashtab *h);

/*
 * Applies the specified apply function to (key,datum,args)
 * for each entry in the specified hash table.
 *
 * The order in which the function is applied to the entries
 * is dependent upon the internal structure of the hash table.
 *
 * If apply returns a non-zero status, then aos_hashtab_map will cease
 * iterating through the hash table and will propagate the error
 * return to its caller.
 */
int aos_hashtab_map(struct aos_hashtab *h,
		int (*apply)(void *k, void *d, void *args),
		void *args);

/*
 * Same as aos_hashtab_map, except that if apply returns a non-zero status,
 * then the (key,datum) pair will be removed from the aos_hashtab and the
 * destroy function will be applied to (key,datum,args).
 */
void aos_hashtab_map_remove_on_error(struct aos_hashtab *h,
                                 int (*apply)(void *k, void *d, void *args),
                                 void (*destroy)(void *k, void *d, void *args),
                                 void *args);


/* Fill info with some hash table statistics */
void aos_hashtab_stat(struct aos_hashtab *h, struct aos_hashtab_info *info);

#endif	/* _SS_HASHTAB_H */
