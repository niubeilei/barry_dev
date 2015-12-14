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
// 12/12/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semantics_obj_dict_h
#define aos_semantics_obj_dict_h

#include "util_c/types.h"
#include "semantics_c/memory.h"
#include "semantics_c/objentry.h"
#include "semantics_c/types.h"


extern int aos_objdict_init();
extern aos_objentry_t *aos_objdict_add(const char * const filename, 
						   const int lineno, 
						   void *key, 
						   void *user_data, 
						   const char * const name,
						   const aos_entry_type_e entry_type,
						   const aos_data_type_e data_type, 
						   aos_mat_t alloc_type, 
						   const u32 size);
extern int aos_objdict_remove(void *ptr);
extern aos_objentry_t * aos_objdict_get(void *ptr);


#define AOS_OBJDICT_TABLE_SIZE	20000
#define AOS_OBJDICT_TABLE_MAX	100000

#endif

