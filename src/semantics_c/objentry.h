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
#ifndef aos_semantics_objentry_h
#define aos_semantics_objentry_h

#include "debug_c/debug_util.h"
#include "semantics_c/rco.h"
#include "semantics_c/memory.h"
#include "semantics_c/types.h"
#include "semantics_c/defs.h"
#include "util_c/types.h"
#include "util_c/loc.h"


typedef struct
{
	aos_rco_define;
	void *				key;	// Starting address
	void *				user_data;
	char				name[AOS_MAX_OBJ_NAME_LEN];
	aos_entry_type_e	entry_type;
	u32					size;
	aos_location_t		creation_loc;
	aos_location_t		deletion_loc;
	aos_time_t			create_time;
	aos_time_t			delete_time;
	aos_mat_t			alloc_type;
	aos_data_type_e		data_type;
} aos_objentry_t;

extern aos_objentry_t *
aos_objentry_create(const char * const filename, 
				    const int lineno, 
				    void *key, 
					void *user_data,
					const char * const name,
					const aos_entry_type_e entry_type,
				    const aos_data_type_e data_type, 
				    aos_mat_t alloc_type, 
				    const u32 size);
extern char * aos_objentry_2str(const aos_objentry_t * const entry);

#endif // End of Include

