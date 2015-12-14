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
// Object Dictionary keeps track of objects. Objects are identified
// by their starting addresses. 
//   
//
// Modification History:
// 12/14/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semantics_c/objentry.h"

#include "alarm_c/alarm.h"
#include "aosUtil/Memory.h"
#include "debug_c/debug_util.h"
#include "util_c/time.h"
#include "util_seman/seman_defs.h"

aos_objentry_t *
aos_objentry_create(const char * const filename, 
				    const int lineno, 
				    void *key, 
					void *user_data,
					const char * const name,
					const aos_entry_type_e entry_type,
				    const aos_data_type_e data_type, 
				    aos_mat_t alloc_type, 
				    const u32 size)
{
	aos_objentry_t *entry = 0;
   
	aos_assert_r(filename, 0);
	aos_assert_r(lineno > 0, 0);
	aos_assert_r(key, 0);
	aos_assert_r(data_type, 0); 
	aos_assert_r(size, 0);

	entry = aos_malloc(sizeof(aos_objentry_t));
	aos_assert_r(entry, 0);
	memset(entry, 0, sizeof(aos_objentry_t));

	aos_set_location(&entry->creation_loc, filename, lineno);
	entry->key = key;
	entry->user_data = user_data;
	entry->create_time = aos_get_crt_time();
	entry->alloc_type = alloc_type;
	entry->entry_type = entry_type;	
	entry->data_type = data_type;
	entry->size = size;

	if (name)
	{
		if (strlen(name) >= AOS_MAX_OBJ_NAME_LEN)
		{
			strncpy(entry->name, name, AOS_MAX_OBJ_NAME_LEN);
			entry->name[AOS_MAX_OBJ_NAME_LEN] = 0;
		}
		else
		{
			strcpy(entry->name, name);
		}
	}
	else
	{
		entry->name[0] = 0;
	}

	return entry;
}


char * aos_objentry_2str(const aos_objentry_t * const entry)
{
	static char ls_buff[200];

	sprintf(ls_buff, "Object entry: %s %d %d", 
		entry->name, entry->entry_type, entry->data_type);
	return ls_buff;
}


