#ifndef INET_CTRL_HASH_H
#define INET_CTRL_HASH_H

#include "inet_ctrl.h"

int inet_ctrl_tabinit(unsigned int size);
int inet_ctrl_tabexit(void);

state_entry_t* inet_ctrl_hash_search(five_tuple_t* key, int is_insert);
int inet_ctrl_hash_remove(five_tuple_t* key);
int inet_ctrl_hash_map(int (*apply)(void *k, void *d, void *args), void *args);
int inet_ctrl_cleartab(void);

#endif

