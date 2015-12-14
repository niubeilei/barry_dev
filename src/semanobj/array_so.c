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
// 12/11/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semanobj/array_so.h"

#include "alarm_c/alarm.h"
#include "util_c/memory.h"
#include "porting_c/mutex.h"
#include "semantics_c/semantics.h"
#include "semantics_c/objdict.h"
#include "thread_c/thread.h"
#include "util_c/strutil.h"
#include "util_c/rc.h"
#include "util_c/global_data.h"

#include <stdio.h>
#include <stdarg.h>


void testfunc()
{
	aos_lock_t lock;
	aos_lock(&lock);
}


static struct aos_array_so_class sg_member_funcs = 
{
	AOS_SO_MEMBER_FUNC_INIT,
	AOS_ARRAYSO_MEMBER_FUNC_INIT
};


void * aos_create_array_recursive(
			const u32 * const dims, 
			const int num_dims, 
			const int elem_size)
{
	void **array;
	int i;
	aos_assert_r(elem_size > 0, 0);
	aos_assert_r(num_dims > 0, 0);

	if (num_dims == 1)
	{
		array = aos_malloc(dims[0] * elem_size);
		aos_assert_r(array, 0);
		return array;
	}

	array = aos_malloc(sizeof(void *) * dims[0]);
	aos_assert_r(array, 0);

	for (i=0; i<dims[0]; i++)
	{
		array[i] = aos_create_array_recursive(&dims[1], num_dims-1, elem_size);
		if (!array[i])
		{
			aos_alarm("Failed to allocate memory");
			aos_free(array);
			return 0;
		}
	}

	return (void *)array;
}


void * 
aos_create_array_so(
				const char * const filename, 
				const int lineno, 
				const char * const name, 
				const char * const type_name, 
				const u32 elem_size,
				const u32 num_dims,
				...)
{
	int size;
	void *array;
	u32 dims[AOS_ARRAY_MAX_NUM_DIMS];
	int dim = 0;
	u32 total_size = 1;

	// 
	// Retrieving all the dimensions
	//
	va_list argptr;
	va_start(argptr, num_dims);
	aos_assert_r(num_dims > 0, 0);
	aos_assert_r(num_dims < AOS_ARRAY_MAX_NUM_DIMS, 0);
	while (dim < num_dims)
	{
		size = va_arg(argptr, int);
		if (size < 0)
		{
			aos_alarm("Invalid size: %d:%d", dim, size);
			return 0;
		}

		total_size *= size;
		dims[dim++] = size;
	}
	va_end(argptr);

	array = aos_create_array_recursive(dims, num_dims, elem_size);
	aos_assert_r(array, 0);

	aos_array_so_t *obj = 
		(aos_array_so_t *)aos_malloc(sizeof(aos_array_so_t));
	if (!obj)
	{
		return 0;
	}

	memset(obj, 0, sizeof(aos_array_so_t));

	if (total_size > AOS_ARRAY_MAX_SIZE)
	{
		aos_alarm("Array too big: %d. Maximum allowed: %d", 
				total_size, AOS_ARRAY_MAX_SIZE);
		aos_free(obj);
		return 0;
	}

	obj->array = array;
	obj->num_dims = dim;
	aos_str_set(&obj->tname, type_name, strlen(type_name));
	aos_str_set(&obj->filename, filename, strlen(filename));
	obj->lineno = lineno;
	obj->member_funcs = &sg_member_funcs;
	obj->elem_size = elem_size;
	obj->mode = eAosArraySoMode_ReadWrite;

	obj->dict_entry = aos_objdict_add(filename, lineno, obj->array, obj, name,
			eAosEntryType_Array, eAosDataType_ARRAY, 
			eAosMAT_Heap, total_size);

	return obj->array;
}


int 
aos_array_so_is_read_only(struct aos_array_so *obj)
{
	return obj->mode == eAosArraySoMode_ReadOnly;
}


int 
aos_array_so_is_modify_only(struct aos_array_so *obj)
{
	return obj->mode == eAosArraySoMode_ModifyOnly;
}


int 
aos_array_so_is_read_modify(struct aos_array_so *obj)
{
	return obj->mode == eAosArraySoMode_ReadWrite;
}


char * 
aos_array_so_get_typename(struct aos_array_so *obj)
{
	return obj->tname;
}


u32 
aos_array_so_get_size(struct aos_array_so *obj)
{
	int i;
	u32 total_size = 1;
	for (i=0; i<obj->num_dims; i++)
	{
		total_size *= obj->dim_size[i];
	}

	return total_size;
}


// 
// One tries to read an array 'array'. This function checks whether
// the array was defined and, if yes, whether the index is correct.
//
// Return: 1 if index is good. Otherwise, it returns 0.
//
int aos_array_elem_check_read1(void *array, const int index)
{

	aos_objentry_t *def = aos_objdict_get(array);
	if (!def)
	{
		// 
		// Did not find the definition. This can be an error.
		//
		aos_alarm("To access an array, but the array is not defined: %x: %d",
				array, index);
		return 1;
	}

	if (def->entry_type != eAosEntryType_Array)
	{
		// 
		// This is not correct. 
		//
		aos_alarm("Found an entry for array, but it is not an array type: %s",
				aos_objentry_2str(def));
		return 1;
	}

	aos_array_so_t *arraydef = (aos_array_so_t *)def->user_data;
	aos_assert_r(arraydef, index);

	if (index < 0 || index >= arraydef->dim_size[0])
	{
		aos_alarm("To access array: %s, but its index is invalid: %d:%d", 
			def->name, arraydef->dim_size[0], index);
		arraydef->invalid_reads++;
		return 0;
	}

	if (arraydef->mode == eAosArraySoMode_ModifyOnly)
	{
		aos_alarm("To access a modify only array: %s %d", 
			def->name, index);
		arraydef->invalid_reads++;
		return index;
	}

	arraydef->reads++;
	return index;
}


int aos_array_elem_check_set1(void *array, const int index, void *value)
{
	aos_objentry_t *def = aos_objdict_get(array);
	if (!def)
	{
		// 
		// Did not find the definition. This can be an error.
		//
		aos_alarm("To access an array, but the array is not defined: %x: %d",
				array, index);
		return 1;
	}

	if (def->entry_type != eAosEntryType_Array)
	{
		// 
		// This is not correct. 
		//
		aos_alarm("Found an entry for array, but it is not an array type: %s",
				aos_objentry_2str(def));
		return 1;
	}

	aos_array_so_t *arraydef = (aos_array_so_t *)def->user_data;
	aos_assert_r(arraydef, index);

	if (index < 0 || index >= arraydef->dim_size[0])
	{
		aos_alarm("To modify array: %s, but its index is invalid: %d:%d", 
			def->name, index, arraydef->dim_size[0]);
		arraydef->invalid_modifies++;
		return 0;
	}

	if (arraydef->mode == eAosArraySoMode_ReadOnly)
	{
		aos_alarm("To modify a read-only array: %s %d", 
			def->name, index);
		arraydef->invalid_modifies++;
		return 1;
	}

	arraydef->modifies++;
	return 1;
}


void aos_array_print_stat(void * ptr)
{
	aos_objentry_t *entry = aos_objdict_get(ptr);
	if (!entry)
	{
		printf("Entry not found: %x\n", (unsigned int)ptr);
		return;
	}

	if (entry->entry_type != eAosEntryType_Array)
	{
		printf("It is not an array entry: %d\n", entry->entry_type);
		return;
	}

	aos_array_so_t *array = (aos_array_so_t *)entry->user_data;
	if (!array)
	{
		printf("No array entry\n");
		return;
	}

	printf("Array Entry: %s Statistics:\n", entry->name);
	printf("    Reads:            %d\n", array->reads);
	printf("    Modifies:         %d\n", array->modifies);
	printf("    Adds:             %d\n", array->adds);
	printf("    Deletes:          %d\n", array->deletes);
	printf("    Invalid Reads:    %d\n", array->invalid_reads);
	printf("    Invalid Modifies: %d\n", array->invalid_modifies);
	printf("    Invalid Adds:     %d\n", array->invalid_adds);
	printf("    Invalid Deletes:  %d\n", array->invalid_deletes);
}


// 
// Delete an array. 
//
int aos_delete_array(const char * const filename, 
				const int lineno, 
				void *array) 
{
	aos_assert_r(array, -eAosRc_NullPtr);
	aos_objentry_t *entry = aos_objdict_get(array);
	if (!entry)
	{
		// 
		// This is not allocated by the right operator. 
		// An error.
		//
		aos_alarm("Array not allocated by semantic operations: %x", 
			(unsigned int)array);
		aos_free(array);
		return -eAosRc_EntryNotFound;
	}

	if (entry->entry_type != eAosEntryType_Array)
	{
		aos_alarm("It is not an array entry: %d\n", entry->entry_type);
		aos_free(array);
		return -eAosRc_InvalidEntry;
	}

	aos_array_so_t *arraydef = entry->user_data;
	aos_assert_r(arraydef, -eAosRc_NullPtr);
	aos_assert_r(arraydef->member_funcs->log, -eAosRc_NullPtr);
	arraydef->member_funcs->log(
			arraydef, filename, lineno, eAosArraySoEvent_Delete);

	return 0;
}	


char * 
aos_arrayso_get_name(aos_array_so_t *obj)
{
	aos_assert_r(obj, "invalid");
	aos_assert_r(obj->dict_entry, "no_name");
	return obj->dict_entry->name;
}


int aos_arrayso_write_log(
			aos_array_so_t *obj, 
			const char * const log, 
			const u32 events)
{
	aos_assert_r(obj, -eAosRc_NullPtr);
	aos_assert_r(log, -eAosRc_NullPtr);
	if (!obj->file)
	{
		char filename[50];
		sprintf(filename, "%s/%s_%x", 
			aos_semantics_get_logdir(), 
			aos_arrayso_get_name(obj), 
			(unsigned int)obj->array);
		obj->file = fopen(filename, "w");
		if (!obj->file)
		{
			aos_alarm("Failed to open log file: %s", filename);
			return -eAosRc_FailedOpenFile;
		}
	}

	if (fputs(log, obj->file) <= 0)
	{
		aos_alarm("Failed to write to file: %s:%x", 
			aos_arrayso_get_name(obj), 
			(unsigned int)obj->array);
		return -eAosRc_FailedWriteFile;
	}

	if (obj->log_flush_flags & events)
	{
		fflush(obj->file);
	}

	return 0;
}


int aos_array_so_log(
				struct aos_array_so *obj, 
				const char * const filename, 
				const int lineno, 
				const aos_array_so_event_e event)
{
	aos_assert_r(obj, -eAosRc_NullPtr);
	if (obj->log_flags & event)
	{
		char buff[100];
		sprintf("Array: %s is deleted at: %s:%d\n", 
			aos_arrayso_get_name(obj), 
			filename, lineno);
		aos_arrayso_write_log(obj, buff, event);
	}
	return 0;
}

int aos_arrayso_set_log(struct aos_array_so *obj, u32 events)
{
	aos_assert_r(obj, -eAosRc_NullPtr);
	obj->log_flags = events;
	return 0;
}


int aos_arrayso_set_log_flush(struct aos_array_so *obj, u32 events)
{
	aos_assert_r(obj, -eAosRc_NullPtr);
	obj->log_flush_flags = events;
	return 0;
}


static int aos_array_check_set(void *array, 
		void *value, 
		const u32 tlid, 
		const int dim)
{
	int i;
	aos_objentry_t *def = aos_objdict_get(array);
	if (!def)
	{
		// 
		// Did not find the definition. This can be an error.
		//
		aos_alarm("To access an array, but the array is not defined: %x: %d",
				array, index);
		return 1;
	}

	if (def->entry_type != eAosEntryType_Array)
	{
		// 
		// This is not correct. 
		//
		aos_alarm("Found an entry for array, but it is not an array type: %s",
				aos_objentry_2str(def));
		return 1;
	}

	aos_array_so_t *arraydef = (aos_array_so_t *)def->user_data;
	aos_assert_r(arraydef, 1);
	aos_assert_r(arraydef->num_dims == dim, 1);

	for (i=0; i<dim; i++)
	{
		int idx = aos_thread_global_int[tlid][i];
		if (idx < 0 || idx >= arraydef->dim_size[i])
		{
			aos_alarm("To modify array: %s, but index invalid: " "%d:%d:%d", 
				def->name, i, idx, arraydef->dim_size[i]);
			arraydef->invalid_modifies++;
			aos_thread_global_int[tlid][i] = 0;
		}
	}

	if (arraydef->mode == eAosArraySoMode_ReadOnly)
	{
		aos_alarm("To modify a read-only array: %s", def->name);
		arraydef->invalid_modifies++;
		return 1;
	}

	arraydef->modifies++;
	return 1;
}


int aos_array_elem_check_set2(void *array, void *value, int idx1, int idx2) 
{
	u32 tlid = aos_get_current_tlid();
	aos_thread_global_int[tlid][0] = idx1;
	aos_thread_global_int[tlid][1] = idx2;

	return aos_array_check_set(array, value, tlid, 2);
}



int aos_array_check_read(void *array, const u16 tlid, const int dim)
{
	int i;

	aos_objentry_t *def = aos_objdict_get(array);
	if (!def)
	{
		// 
		// Did not find the definition. This can be an error.
		//
		aos_alarm("To access an array, but the array is not defined: %x: %d",
				array, index);
		return 1;
	}

	if (def->entry_type != eAosEntryType_Array)
	{
		// 
		// This is not correct. 
		//
		aos_alarm("Found an entry for array, but it is not an array type: %s",
				aos_objentry_2str(def));
		return 1;
	}

	aos_array_so_t *arraydef = (aos_array_so_t *)def->user_data;
	aos_assert_r(arraydef, 1);
	aos_assert_r(arraydef->num_dims == dim, 1);

	for (i=0; i<dim; i++)
	{
		int idx = aos_thread_global_int[tlid][i];
		if (idx < 0 || idx >= arraydef->dim_size[i])
		{
			aos_alarm("To access array: %s, but index invalid: %d:%d:%d", 
				def->name, i, arraydef->dim_size[i], idx);
			arraydef->invalid_reads++;
			aos_thread_global_int[tlid][i] = 0;
		}
	}

	if (arraydef->mode == eAosArraySoMode_ModifyOnly)
	{
		aos_alarm("To access a modify only array: %s", def->name);
		arraydef->invalid_reads++;
		return 1;
	}

	arraydef->reads++;
	return 1;
}


int aos_array_elem_check_read2(void *array, int idx1, int idx2) 
{
	u32 tlid = aos_get_current_tlid();
	aos_thread_global_int[tlid][0] = idx1;
	aos_thread_global_int[tlid][1] = idx2;

	return aos_array_check_read(array, tlid, 2);
}


int aos_array_stack_array_created(
				const char * const filename, 
				const int lineno, 
				const char * const name,
				void *array, 
				const u32 elem_size, 
				const char * const type_name, 
				const u32 num_dims, 
				...)
{
	int dim, size, total_size;

	aos_array_so_t *obj = 
		(aos_array_so_t *)aos_malloc(sizeof(aos_array_so_t));
	if (!obj)
	{
		return -eAosRc_OutOfMemory;
	}

	memset(obj, 0, sizeof(aos_array_so_t));

	va_list argptr;
	aos_assert_r(num_dims > 0, -eAosRc_InvalidParm);
	aos_assert_r(num_dims < AOS_ARRAY_MAX_NUM_DIMS, -eAosRc_InvalidParm);
	va_start(argptr, num_dims);
	total_size = 1;
	for (dim=0; dim<num_dims; dim++)
	{
		size = va_arg(argptr, int);
		if (size < 0)
		{
			aos_alarm("Invalid size: %d:%d", dim, size);
			return -eAosRc_InvalidParm;
		}

		total_size *= size;
		obj->dim_size[dim++] = size;
	}
	va_end(argptr);

	obj->array = array;
	obj->num_dims = num_dims;
	aos_str_set(&obj->tname, type_name, strlen(type_name));
	aos_str_set(&obj->filename, filename, strlen(filename));
	obj->lineno = lineno;
	obj->member_funcs = &sg_member_funcs;
	obj->elem_size = elem_size;
	obj->mode = eAosArraySoMode_ReadWrite;

	obj->dict_entry = aos_objdict_add(filename, lineno, array, obj, name,
			eAosEntryType_Array, eAosDataType_ARRAY, 
			eAosMAT_Stack, total_size);

	aos_assert_r(obj->dict_entry, -eAosRc_OutOfMemory);
	return 0;
}


aos_array_so_t * aos_array_get_def(void *array)
{
	aos_objentry_t *def = aos_objdict_get(array);
	aos_assert_r(def, 0);
	aos_assert_r(def->user_data, 0);
	aos_assert_r(def->entry_type == eAosEntryType_Array, 0);
	return (aos_array_so_t *)def->user_data;
}

// 
// Description:
// 	It retrieves the 'dim'-th size. 
//
// Returns:
// 	-1 if anything wrong. Otherwise, it returns the size of the
// 	specified dimension.
//
int aos_array_get_size(void *array, const int dim)
{
	aos_array_so_t *arraydef = aos_array_get_def(array);
	aos_assert_r(arraydef, -1);
	aos_assert_r(dim >= 0 && dim < arraydef->num_dims, -1);
	return arraydef->dim_size[dim];
}


static int aos_array_reset_cell_tracker(
			void *cell_tracker, 
			const u32 * const dims, 
			const int num_dims, 
			const char init)
{
	int i;
	aos_assert_r(cell_tracker, -1);
	aos_assert_r(dims, -1);
	aos_assert_r(num_dims > 0, -1);

	if (num_dims == 1)
	{
		char *array = (char *)cell_tracker;
		for (i=0; i<dims[0]; i++)
		{
			array[i] = init;
		}
		return 0;
	}

	void **tracker = (void **)cell_tracker;
	for (i=0; i<dims[0]; i++)
	{
		aos_assert_r(aos_array_reset_cell_tracker(
		 		tracker[i], &dims[1], num_dims-1, init) == 0, -1);
	}
	return 0;
}


// 
// Description:
// 	All the elements of the array 'array' is set to the value 'value'. 
// 	This function does not actually set the arrays, which should have
// 	been handled by the macro. Instead, this funciton marks the cell
// 	tracker as needed. The flag indicates whether to treat elements 
// 	as being initialized or not.
//
// Parameters:
// 	array:	the array to be reset.
// 	value:	the value to be reset to.
// 	flag:	indicates whether elements should be considered initialized
// 			(1) or not (0).
//
int __aos_array_reset_value(void *array, void *value, int flag)
{
	aos_array_so_t *arraydef = aos_array_get_def(array);
	if (!arraydef)
	{
		// The definition of the array is not found. Do nothing.
		return 0;
	}

	if (!arraydef->cell_track)
	{
		// Tracking cell is not turned on. Do nothing.
		return 0;
	}

	aos_assert_r(arraydef->cell_tracker, -1);
	return aos_array_reset_cell_tracker(arraydef->cell_tracker, 
			arraydef->dim_size, arraydef->num_dims, flag);

}



static int aos_array_create_cell_tracking(
			aos_array_so_t *def, 
			const char init)
{
	aos_assert_r(def, -1);
	aos_assert_r(def->cell_track == 0, -1);
	aos_assert_r(def->cell_tracker == 0, -1);
	def->cell_tracker = aos_create_array_recursive(
			def->dim_size, def->num_dims, sizeof(char));
	if (!def->cell_tracker)
	{
		aos_alarm("Failed to allocate memory for cell tracker: %s", 
			aos_arrayso_get_name(def));
		return -1;
	}
	def->cell_track = 1;

	// 
	// The tracker has been created. Need to initialize it.
	//
	return aos_array_reset_cell_tracker(def->cell_tracker, 
			def->dim_size, def->num_dims, init);
}


int __array_array_track_cell_init(void *array, const char init)
{
	aos_array_so_t *arraydef = aos_array_get_def(array);
	aos_assert_r(arraydef, -1);

	// Check whether cell tracking is already on
	if (arraydef->cell_track) return 0;

	// Need to create the cell tracker
	aos_assert_r(aos_array_create_cell_tracking(arraydef, init) == 0, -1);
	return 0;
}

