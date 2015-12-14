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
#ifndef aos_semanobj_array_so_h
#define aos_semanobj_array_so_h

#include "alarm_c/alarm.h"
#include "semanobj/so_type.h"
#include "semanobj/so.h"
#include "semanobj/array_priv.h"
#include "aosUtil/Memory.h"
#include "semantics_c/memory.h"
#include "semantics_c/objentry.h"
#include "thread_c/thread.h"
#include "util_c/global_data.h"
#include <stdio.h>

extern int aos_array_so_get_array_size(void *array, int *size);

#define AOS_ARRAY_MAX_NUM_DIMS 10
#define AOS_ARRAY_MAX_SIZE 1000000000

typedef struct aos_array_so
{
	aos_array_so_class_t *member_funcs;

	u32						elem_size;
	u32						dim_size[AOS_ARRAY_MAX_NUM_DIMS];
	u32						num_dims;
	char *					tname;
	char *					filename;
	int						lineno;
	aos_array_so_mode_e		mode;
	void *					array;
	aos_objentry_t *		dict_entry;
	u32						log_flags;
	u32						log_flush_flags;
	FILE *					file;
	char					cell_track;	// 0: no tracking on cell values
										// != 0: track on cell values
	void *					cell_tracker;	// For cell init tracking.

	// statistics
	u32		reads;
	u32		modifies;
	u32		adds;
	u32		deletes;

	u32		invalid_reads;
	u32		invalid_modifies;
	u32		invalid_adds;
	u32		invalid_deletes;

} aos_array_so_t;

extern void aos_array_print_stat(void * ptr);




#ifdef AOS_SEMANTICS
#define aos_array_decl(array, name, type, type_name, size) 	\
	type array[size]; 										\
	aos_array_stack_array_created(__FILE__, __LINE__, 		\
			name, array, sizeof(type), type_name, 1, size);

#define aos_array_decl2(array, name, type, type_name, size1, size2)\
	type array[size1][size2];								\
	aos_array_stack_array_created(__FILE__, __LINE__, 		\
			name, array, sizeof(type), type_name, 			\
			2, size1, size2);

#define aos_array_reset_value(array, size, value, flag)		\
{															\
	int i;													\
	int s = aos_array_get_size(array, 0);					\
	if (s != size)											\
	{														\
		aos_alarm("Failed to retrieve array size");			\
	}														\
	else													\
	{														\
		__aos_array_reset_value(array, value, flag);		\
	}														\
	for (i=0; i<size; i++) array[i] = value;				\
}
	
#define aos_array_reset_value2(array, size1, size2, value, flag)	\
{															\
	int i, j;												\
	int s1 = aos_array_get_size(array, 0);					\
	int s2 = aos_array_get_size(array, 1);					\
	if (s1 != size1 || s2 != size2)							\
	{														\
		aos_alarm("Array size mismatch: %d:%d, %d:%d", 		\
			size1, s1, size2, s2);							\
	}														\
	else													\
	{														\
		__aos_array_reset_value(array, value, flag);		\
	}														\
	for (i=0; i<size1; i++) 								\
		for (j=0; j<size2; j++)								\
			array[i][j] = value;							\
}

#define aos_array_reset_value3(array, size1, size2, size3, 	\
		value, flag)										\
{															\
	int i, j, k;											\
	int s1 = aos_array_get_size(array, 0);					\
	int s2 = aos_array_get_size(array, 1);					\
	int s3 = aos_array_get_size(array, 2);					\
	if (s1 != size1 || s2 != size2 || s3 != size3)			\
	{														\
		aos_alarm("Array size mismatch: %d:%d, %d:%d, %d:%d",	\
			size1, s1, size2, s2, s3, size3);				\
	}														\
	else													\
	{														\
		__aos_array_reset_value(array, value, flag);		\
	}														\
	for (i=0; i<size1; i++) 								\
		for (j=0; j<size2; j++)								\
			for (k=0; k<size3; k++)							\
				array[i][j][k] = value;						\
}

#define aos_array_reset_value4(array, size1, size2, size3, 	\
		size4, value, flag)									\
{															\
	int i, j, k, l;											\
	int s1 = aos_array_get_size(array, 0);					\
	int s2 = aos_array_get_size(array, 1);					\
	int s3 = aos_array_get_size(array, 2);					\
	int s4 = aos_array_get_size(array, 3);					\
	if (s1 != size1 || s2 != size2 || s3 != size3 ||		\
		s4 != size4)										\
	{														\
		aos_alarm("Array size mismatch: "					\
			"%d:%d, %d:%d, %d:%d, %d:%d",					\
			size1, s1, size2, s2, s3, size3, s4, size4);	\
	}														\
	else													\
	{														\
		__aos_array_reset_value(array, value, flag);		\
	}														\
	for (i=0; i<size1; i++) 								\
		for (j=0; j<size2; j++)								\
			for (k=0; k<size3; k++)							\
				for (l=0; l<size4; l++)						\
					array[i][j][k][l] = value;				\
}

#define aos_array_reset_value5(array, size1, size2, size3, 	\
		size4, size5, value, flag)							\
{															\
	int i, j, k, l, m;										\
	int s1 = aos_array_get_size(array, 0);					\
	int s2 = aos_array_get_size(array, 1);					\
	int s3 = aos_array_get_size(array, 2);					\
	int s4 = aos_array_get_size(array, 3);					\
	int s5 = aos_array_get_size(array, 4);					\
	if (s1 != size1 || s2 != size2 || s3 != size3 ||		\
		s4 != size4 || s5 != size5)							\
	{														\
		aos_alarm("Array size mismatch: "					\
			"%d:%d, %d:%d, %d:%d, %d:%d",					\
			size1, s1, size2, s2, size3, s3, size4, s4,		\
			size5, s5);										\
	}														\
	else													\
	{														\
		__aos_array_reset_value(array, value, flag);		\
	}														\
	for (i=0; i<size1; i++) 								\
		for (j=0; j<size2; j++)								\
			for (k=0; k<size3; k++)							\
				for (l=0; l<size4; l++)						\
					for (m=0; m<size5; m++)					\
						array[i][j][k][l][m] = value;		\
}
	
#define aos_array_create(name, type, type_name, size, 		\
		flag, init_value, features) 						\
		aos_thread_global_void_ptr[aos_get_current_tlid()]	\
			= __aos_create_array(__FILE__, __LINE__, 		\
				name, type_name, sizeof(type), flag, 		\
				init_value, features, 1, size);				\
		if (flag)											\
		{													\
			int i;											\
			type *array = (type*)aos_thread_global_void_ptr[\
				aos_get_current_tlid()];					\
			for (i=0; i<size; i++)							\
				array[i] = init_value;						\
		}


#define aos_array_create2(name, type, type_name, 			\
		size1, size2) 										\
	aos_create_array_so(__FILE__, __LINE__, name, type_name,\
		sizeof(type), 2, size1, size2)

#define aos_array_create3(name, type, type_name, 			\
		size1, size2, size3) 								\
	aos_create_array_so(__FILE__, __LINE__, name, type_name,\
		sizeof(type), 3, size1, size2, size3)

#define aos_array_create4(name, type, type_name, 			\
		size1, size2, size3, size4) 						\
	aos_create_array_so(__FILE__, __LINE__, name, type_name,\
		sizeof(type), 4, size1, size2, size3, size4)

#define aos_array_create5(name, type, type_name, 			\
		size1, size2, size3, size4, size5) 					\
	aos_create_array_so(__FILE__, __LINE__, name, type_name,\
		sizeof(type), 5, size1, size2, size3, size4, size5)

#define aos_array_set_log(array, events) 					\
	aos_arrayso_set_log(array, events);

#define aos_array_set_log_flush(array, events) 				\
	aos_arrayso_set_log_flush(array, events);

#define aos_array_track_cell_init(array)					\
	__aos_array_track_cell_init(array)

#define aos_array_delete(array, dstor) 						\
	if (dstor)												\
	{														\
		int size, i;										\
		if (aos_array_so_get_array_size(array, &size))		\
		{													\
			aos_alarm("Failed to retrieve array size");		\
		}													\
		else												\
		{													\
			for (i=0; i<size; i++)							\
			{												\
				dstor(array[i]);							\
			}												\
		}													\
	}														\
	aos_delete_array(__FILE__, __LINE__, array)		

#define aos_array_elem_read1(array, index)					\
	(aos_array_elem_check_read1(array, index)) ?			\
		array[index] : array[0] 

#define aos_array_elem_read2(array, idx1, idx2)				\
	(aos_array_elem_check_read2(array, idx1, idx2))?		\
		array[idx1][idx2] : array[0][0]

#define aos_array_elem_set1(array, index, value) 			\
	array[(aos_array_elem_check_set1(array, index, (void *)value)) ?	\
		index:0] = value

// #define aos_array_elem_set2(array, value, idx1, idx2) 		
// 	array[idx1][idx2] = value

#define aos_array_elem_set2(array, value, idx1, idx2) 		\
 	array[(aos_array_elem_check_set2(array, (void *)value, idx1, idx2))\
 				?idx1:0]						\
 		 [aos_thread_global_int[aos_get_current_tlid()][2]]\
 	= value

#define aos_array_remove_elems(array, array_size, pos, num_elems_remove)	\
{																			\
	int i;																	\
	if (array && pos >= 0 && num_elems_remove >= 0 &&						\
		pos + num_elems_remove < array_size)								\
	{																		\
		for (i=pos; i<array_size-num_elems_remove; i++)						\
		{																	\
			array[i] = array[i + num_elems_remove];							\
		}																	\
	}																		\
	else																	\
	{																		\
		aos_alarm("To remove elements from array but data incorrect: "		\
				"array: %x, array_size: %d, pos: %d, ner: %d", 				\
				(unsigned int)array, array_size, pos, num_elems_remove); 	\
	}																		\
}

#else
//
// No Semantics portion
//
#define aos_array_decl(name, type, type_name, size) 		\
	type name[size]

#define aos_array_decl2(name, type, type_name, size1, size2)\
	type name[size1][size2]

#define aos_array_create(name, type, type_name, size)		\
	malloc(sizeof(type) * size)


#define aos_array_create2(name, type, type_name, 			\
		size1, size2) 										\
	malloc(sizeof(type) * size1 * size2)

#define aos_array_create3(name, type, type_name, 			\
		size1, size2, size3) 								\
	malloc(sizeof(type) * size1 * size2 * size3)

#define aos_array_create4(name, type, type_name, 			\
		size1, size2, size3, size4)							\
	malloc(sizeof(type) * size1 * size2 * size3, * size4)

#define aos_array_create5(name, type, type_name, 			\
		size1, size2, size3, size4, size5)					\
	malloc(sizeof(type) * size1 * size2 * size3, * size4 * size5)

#endif
#endif // End of Include

