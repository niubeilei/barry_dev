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
// 02/06/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_dyn_array_h
#define aos_util_dyn_array_h

struct aos_dyn_array;


typedef int (*aos_dyn_array_remove_all_t)(
		struct aos_dyn_array *array, 
		char **ptr); 

typedef int (*aos_dyn_array_add_element_t)(
		struct aos_dyn_array *array, 
		const int noe, 
		char **ptr);

typedef int (*aos_dyn_array_insert_t)(
		struct aos_dyn_array *array, 
		const int pos,
		const int noe, 
		char **ptr);

typedef int (*aos_dyn_array_del_element_t)(
		struct aos_dyn_array *array, 
		const int idx,
		const int noe, 
		char **ptr); 

typedef int (*aos_dyn_array_add_char_t)(
		struct aos_dyn_array *array, 
		const char c,
		char **ptr);

typedef int (*aos_dyn_array_add_chars_t)(
		struct aos_dyn_array *array, 
		const char c,
		const int repeat,
		char **ptr);

typedef int (*aos_dyn_array_add_str_t)(
		struct aos_dyn_array *array, 
		const char * const value,
		const int len,
		char **ptr);

typedef int (*aos_dyn_array_ic_t)(
		struct aos_dyn_array *array, 
		char **ptr); 

typedef int (*aos_dyn_array_release_memory_t)(
		struct aos_dyn_array *array); 


#define AOS_DYN_ARRAY_MEMFUNC_DECL					\
	aos_dyn_array_add_element_t 	add_element;	\
	aos_dyn_array_insert_t 			insert;			\
	aos_dyn_array_del_element_t 	del_element;	\
	aos_dyn_array_ic_t				integrity_check;\
	aos_dyn_array_release_memory_t	release_memory;	\
	aos_dyn_array_remove_all_t		remove_all;		\
	aos_dyn_array_add_char_t		add_char;		\
	aos_dyn_array_add_chars_t		add_chars;		\
	aos_dyn_array_add_str_t			add_str

#define AOS_DYN_ARRAY_MEMFUNC_INIT					\
	aos_dyn_array_add_element, 						\
	aos_dyn_array_insert, 							\
	aos_dyn_array_del_element, 						\
	aos_dyn_array_integrity_check,					\
	aos_dyn_array_release_memory,					\
	aos_dyn_array_remove_all,						\
	aos_dyn_array_add_char,							\
	aos_dyn_array_add_chars,						\
	aos_dyn_array_add_str

typedef struct aos_dyn_array_mf
{
	AOS_DYN_ARRAY_MEMFUNC_DECL;
} aos_dyn_array_mf_t;

typedef struct aos_dyn_array
{
	aos_dyn_array_mf_t *mf;

	char *		buffer;
	int			elem_size;
	int			buff_size;
	int			noe;
	int			inc_size;
	int			max_size;
} aos_dyn_array_t;

extern int aos_dyn_array_init(
		aos_dyn_array_t *array, 
		char **ptr,
		const int elem_size,
		const int inc_size,
		const int max_size);

extern int aos_dyn_array_release_memory(
		aos_dyn_array_t *array);

#endif // End of Include

