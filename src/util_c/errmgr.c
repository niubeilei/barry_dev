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
//
// Modification History:
// 12/12/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/errmgr.h"

#include "alarm_c/alarm.h"
#include "aos_util/hashtab.h"
#include "util_c/errmgr_priv.h"
#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"
#include "porting_c/mutex.h"
#include "semantics_c/semantics.h"
#include "semantics_c/objflags.h"
#include <string.h>

static aos_hashtab_t * sg_hashtab = 0;
static aos_lock_t sgLock;
static int sg_errmgr_init_flag = 0;

#define AOS_ERRMGR_TABLE_SIZE 	5000
#define AOS_ERRMGR_TABLE_MAXERR 5000

static u32 aos_errmgr_hashfunc(aos_hashtab_t *table, void *key)
{
	aos_assert_r(key, 0);
	aos_errmgr_entry_t *entry = (aos_errmgr_entry_t *)key;
	aos_verify_cast(entry, AOS_OBJFLAG_ERRMGR_ENTRY_T);

	return (unsigned long)((unsigned long)key + (unsigned long)entry->lineno);
}


static int aos_errmgr_keycmp(aos_hashtab_t *table, void *key1, void *key2)
{
	aos_errmgr_entry_t *entry1;
	aos_errmgr_entry_t *entry2;

	aos_assert_r(key1, -eAosRc_NullPointer);
	aos_assert_r(key2, -eAosRc_NullPointer);

	entry1 = (aos_errmgr_entry_t *)key1;
	entry2 = (aos_errmgr_entry_t *)key2;
	aos_verify_cast(entry1, AOS_OBJFLAG_ERRMGR_ENTRY_T);
	aos_verify_cast(entry2, AOS_OBJFLAG_ERRMGR_ENTRY_T);

	if (entry1->lineno < entry2->lineno) return -1;
	if (entry1->lineno > entry2->lineno) return 1;

	return strcmp(entry1->filename, entry2->filename);
}


int aos_errmgr_init()
{
	if (!sg_errmgr_init_flag)
	{
		aos_init_lock(&sgLock);
		sg_hashtab = aos_hashtab_create(aos_errmgr_hashfunc, 
						aos_errmgr_keycmp, 
						AOS_ERRMGR_TABLE_SIZE, 
						AOS_ERRMGR_TABLE_MAXERR);
		aos_assert_r(sg_hashtab, -eAosRc_OutOfMemory);
		sg_errmgr_init_flag = 1;
		return 0;
	}

	return 0;
}


int aos_errmgr_add_err(const char * const filename, 
					   const int lineno, 
					   const char * const errmsg)
{
	aos_errmgr_entry_t entry;
	aos_errmgr_entry_t *found, *new_entry;	
	int ret = 0;

	entry.filename = (char *)filename;
	entry.lineno = lineno;
	found = (aos_errmgr_entry_t *)aos_hashtab_search(sg_hashtab, &entry);
	if (found)
	{
		found->count++;
		return 0;
	}

	// 
	// Add the entry into the table
	//
	new_entry = aos_errmgr_create_errentry(filename, lineno, errmsg);
	aos_assert_r(new_entry, -eAosRc_OutOfMemory);
	ret = aos_hashtab_insert(sg_hashtab, new_entry, new_entry);
	aos_assert_r(!ret, -eAosRc_FailedAddEntry);
	return 0;
}


aos_errmgr_entry_t *
aos_errmgr_create_errentry(const char * const filename, 
						   const int lineno,
						   const char * const errmsg)
{
	aos_assert_r(filename, 0);
	aos_assert_r(errmsg, 0);

	aos_errmgr_entry_t *entry = aos_malloc(sizeof(aos_errmgr_entry_t));
	aos_assert_r(entry, 0);
	memset(entry, 0, sizeof(aos_errmgr_entry_t));

	entry->filename = aos_malloc(strlen(filename));
	aos_assert_r(entry->filename, 0);
	strcpy(entry->filename, filename);

	entry->errmsg = aos_malloc(strlen(errmsg));
	aos_assert_r(entry->errmsg, 0);
	strcpy(entry->errmsg, errmsg);

	entry->count = 1;
	entry->__aos_semantic_iden = AOS_OBJFLAG_ERRMGR_ENTRY_T;
	entry->lineno = lineno;

	return entry;
}

