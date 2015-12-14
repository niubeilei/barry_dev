#include "inet_ctrl_hash.h"
#include "aosUtil/Types.h"
#include "aosUtil/Memory.h"
#include "aosUtil/aos_hashtab.h"

// #include <linux/slab.h>

/* for my hash exported functions */
static struct aos_hashtab* entry_hash_tab = NULL;

static u32 inet_ctrl_hash(struct aos_hashtab *h, void *key)
{
	five_tuple_t *keyp = (five_tuple_t*)key;
	
	unsigned int val = (keyp->src_ip ^ keyp->src_port) ^ 
					   (keyp->dst_ip ^ keyp->dst_port) ^
					   keyp->proto;
	val ^= val >> 16;
	val ^= val >> 8;
	
	return val & (h->size - 1);
}

static int inet_ctrl_cmp(struct aos_hashtab *h, void *key1, void *key2)
{
	five_tuple_t *keyp1, *keyp2;

	keyp1 = (five_tuple_t*)key1;
	keyp2 = (five_tuple_t*)key2;
	
	if (keyp1->src_ip != keyp2->src_ip)
		return (keyp1->src_ip - keyp2->src_ip);

	if (keyp1->dst_ip != keyp2->dst_ip)
		return (keyp1->dst_ip - keyp2->dst_ip);

	if (keyp1->src_port != keyp2->src_port)
		return (keyp1->src_port - keyp2->src_port);
	
	if (keyp1->dst_port != keyp2->dst_port)
		return (keyp1->dst_port - keyp2->dst_port);
	
	return (keyp1->proto - keyp2->proto);
}

static int free_apply(void* k, void* data, void* arg)
{
	/* Simulate returning error for ever */
	return 1;
}

static void free_one_element(void* key, void* data, void* args)
{
	state_entry_t* entry = (state_entry_t*)data;

	if (key)
		aos_free(key);

	if (entry->st_data) {
		if (atomic_dec_and_test(&entry->st_data->data_refcnt)) { 
			/* no one holds this data, set this flag */
			FLAG_SET(entry->st_data->data_flag, DATA_PRE_DELETE);
		}
	}
	aos_free(entry);
}

int inet_ctrl_tabinit(unsigned int size)
{
	entry_hash_tab = aos_hashtab_create(inet_ctrl_hash, inet_ctrl_cmp, size);
	if (!entry_hash_tab)
		return -1;

	return 0;
}

int inet_ctrl_tabexit(void)
{
	inet_ctrl_cleartab();
	aos_hashtab_destroy(entry_hash_tab);
	return 0;
}

state_entry_t* inet_ctrl_hash_search(five_tuple_t* key, int is_insert)
{
	state_entry_t* entry = NULL;
	
	INETCTRL_INFO("Enter hash searching: Key %u.%u.%u.%u:%u->%u.%u.%u.%u:%u using %u\n", 
			NIPQUAD((key)->src_ip), (key)->src_port,
			NIPQUAD((key)->dst_ip), (key)->dst_port, (key)->proto);

	entry = (state_entry_t*)aos_hashtab_search(entry_hash_tab, key);
	if (entry) {

		INETCTRL_INFO("Found: Key %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n", 
			NIPQUAD((key)->src_ip), (key)->src_port,
			NIPQUAD((key)->dst_ip), (key)->dst_port);

		return entry;
	}
		
	if (is_insert) {
		five_tuple_t* tuple = (five_tuple_t*)aos_malloc(sizeof(*tuple));
		if (tuple == NULL) {
			INETCTRL_ERR("Failed to malloc key\n"); 
			return NULL;
		}
		
		INETCTRL_INFO("New: Key %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n", 
			NIPQUAD((key)->src_ip), (key)->src_port,
			NIPQUAD((key)->dst_ip), (key)->dst_port);
		
		entry = (state_entry_t*)aos_malloc(sizeof(*entry));
		if (entry == NULL) {
			INETCTRL_ERR("Failed to malloc entry\n"); 
			return NULL;
		}
		
		memset(entry, 0, sizeof(*entry));
		memcpy(&entry->st_tuple, key, sizeof(*key));
		memcpy(tuple, key, sizeof(*key));
		aos_hashtab_insert(entry_hash_tab, tuple, entry);
	}
	
	return entry;
}

int inet_ctrl_hash_remove(five_tuple_t* key)
{
	state_entry_t* entry;
	
	INETCTRL_INFO("Remove: Key %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n", 
			NIPQUAD((key)->src_ip), (key)->src_port,
			NIPQUAD((key)->dst_ip), (key)->dst_port);

	entry = (state_entry_t*)aos_hashtab_search(entry_hash_tab, key);
	if (entry)
		aos_hashtab_remove(entry_hash_tab, key, free_one_element, NULL);
	
	return 0;
}

int inet_ctrl_hash_map(int (*apply)(void *k, void *d, void *args), void *args)
{
	return aos_hashtab_map(entry_hash_tab, apply, args);
}

int inet_ctrl_cleartab(void)
{
	aos_hashtab_map_remove_on_error(entry_hash_tab, free_apply, free_one_element, NULL);
	return 0;
}


