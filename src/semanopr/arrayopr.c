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
// 12/11/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semanopr/arrayopr_defs.h"

#include "debug_c/rc.h"
#include "semantics_c/objentry.h"
#include "semantics_c/objdict.h"
#include "util_c/errmgr.h"


int __aos_array_decl(const char * const filename, 
					int lineno, 
					void *array, 
					const char * const data_type, 
					const aos_mat_t alloc_type,
					const int size)
{
	aos_objentry_t *entry = aos_objdict_get(array);
	if (entry)
	{
		// 
		// The array was specified in the dictionary. Compare it.
		//
		if (entry->size != size)
		{
			char errmsg[200];
			sprintf("Array specified with different size: %s:%d:%d vs %s:%d:%d", 
				filename, lineno, size, 
				entry->creation_loc.filename, 
				entry->creation_loc.lineno, 
				entry->size);
			aos_errmgr_add_err(__FILE__, __LINE__, errmsg);
			return -eAosRc_TypeInconsistent;
		}

		/* Compiling error. Temporarily commented out by Chen Ding, 
		 * 02/13/2008
		 *
		if (strcmp(data_type, entry->data_type))
		{
			char errmsg[200];
			sprintf("Array specified with different data type: %s:%d:%s vs %s:%d:%s",
				filename, lineno, data_type, 
				entry->creation_loc.filename, 
				entry->creation_loc.lineno, 
				entry->data_type);
			aos_errmgr_add_err(__FILE__, __LINE__, errmsg);
			return -eAosRc_TypeInconsistent;
		}
		*/

		if (alloc_type != entry->alloc_type)
		{
			char errmsg[200];
			sprintf("Array specified with different memory type: "
					"%s:%d:%s vs %s:%d:%s",
				filename, lineno, aos_mat_to_str(alloc_type), 
				entry->creation_loc.filename, 
				entry->creation_loc.lineno, 
				aos_mat_to_str(entry->alloc_type));
			aos_errmgr_add_err(__FILE__, __LINE__, errmsg);
			return -eAosRc_TypeInconsistent;
		}

		return 0;
	}

	// 
	// The entry was not in the dictionary yet. Create one.
	//
	
	return 0;
}


int __aos_array_delete(char *filename, 
					int lineno, 
					char *array)
{
	return 0;
}


int __aos_array_modify(char *filename, 
					int lineno, 
					void *array, 
					int index) 
{
	return 0;
}


int __aos_array_read(char *filename, 
					int lineno, 
					void *array, 
					int index)
{
	return 0;
}


int __aos_array_stop(char *filename, 
					int lineno, 
					char *array)
{
	return 0;
}


int __aos_array_resume(char *filename, 
					int lineno, 
					char *array)
{
	return 0;
}


int __aos_array_read_only(char *filename, 
					int lineno, 
					char *array)
{
	return 0;
}


int __aos_array_modify_only(char *filename, 
					int lineno, 
					char *array)
{
	return 0;
}


int __aos_array_read_modify(char *filename, 
							int lineno, 
							char *array)
{
	return 0;
}

