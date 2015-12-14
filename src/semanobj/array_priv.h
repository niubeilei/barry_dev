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
#ifndef aos_semanobj_array_priv_h
#define aos_semanobj_array_priv_h

struct aos_array_so;

typedef enum
{
	eAosArraySoEvent_Unknown = 0, 
	eAosArraySoEvent_Create = 0x01,
	eAosArraySoEvent_AddElem = 0x02, 
	eAosArraySoEvent_RemoveElem = 0x04, 
	eAosArraySoEvent_Read = 0x08, 
	eAosArraySoEvent_Modify = 0x10, 
	eAosArraySoEvent_Swap = 0x20, 
	eAosArraySoEvent_Delete = 0x40 
} aos_array_so_event_e;


// 
// Global functions
//
int aos_array_elem_check_read1(void *arra, const int index);
int aos_array_elem_check_read2(void *arra, const int, const int);
int aos_array_elem_check_set1(void *arra, const int index, void *value);
int aos_array_elem_check_set2(void *arra, void *value, 
			const int idx1, const int idx2);

// 
// Member function definitions
//
extern int aos_array_so_is_read_only(struct aos_array_so *obj);
extern int aos_array_so_is_modify_only(struct aos_array_so *obj);
extern int aos_array_so_is_read_modify(struct aos_array_so *obj);
extern char * aos_array_so_get_typename(struct aos_array_so *obj);
extern u32 aos_array_so_get_size(struct aos_array_so *obj);
extern int aos_array_so_log(
				struct aos_array_so *obj, 
				const char * const filename, 
				const int lineno, 
				const aos_array_so_event_e event);
extern int aos_arrayso_set_log(struct aos_array_so *obj, u32 events);
				
typedef int (*aos_arrayso_mf_log_t)(
				struct aos_array_so *obj, 
				const char * const filename, 
				const int lineno, 
				const aos_array_so_event_e event);
typedef int (*aos_arrayso_mf_readonly_t)(struct aos_array_so *obj);
typedef int (*aos_arrayso_mf_modifyonly_t)(struct aos_array_so *obj);
typedef int (*aos_arrayso_mf_readmodify_t)(struct aos_array_so *obj);
typedef char * (*aos_arrayso_mf_get_typename_t)(struct aos_array_so *obj);
typedef u32 (*aos_arrayso_mf_get_size_t)(struct aos_array_so *obj);


struct aos_array_so_func
{
	aos_arrayso_mf_readonly_t		is_read_only;
	aos_arrayso_mf_modifyonly_t		is_modify_only;
	aos_arrayso_mf_readmodify_t 	is_readmodify;
	aos_arrayso_mf_get_typename_t	get_typename;
	aos_arrayso_mf_get_size_t		get_size;
	aos_arrayso_mf_log_t			log;
};

#define AOS_ARRAYSO_MEMBER_FUNC_INIT							\
	aos_array_so_is_read_only, 									\
	aos_array_so_is_modify_only,								\
	aos_array_so_is_read_modify, 								\
	aos_array_so_get_typename, 									\
	aos_array_so_get_size,										\
	aos_array_so_log


#define AOS_ARRAYSO_MEMBER_FUNC_DECL							\
	aos_arrayso_mf_readonly_t		is_read_only;				\
	aos_arrayso_mf_modifyonly_t		is_modify_only;				\
	aos_arrayso_mf_readmodify_t 	is_readmodify;				\
	aos_arrayso_mf_get_typename_t	get_typename;				\
	aos_arrayso_mf_get_size_t		get_size;					\
	aos_arrayso_mf_log_t			log;

typedef struct aos_array_so_class
{
	AOS_SO_MEMBER_FUNC_DECL;
	AOS_ARRAYSO_MEMBER_FUNC_DECL;
} aos_array_so_class_t;

typedef enum 
{
	eAosArraySoMode_ReadWrite,
	eAosArraySoMode_ReadOnly,
	eAosArraySoMode_ModifyOnly
} aos_array_so_mode_e;

extern int aos_array_stack_array_created(
				const char * const filename, 
				const int lineno, 
				const char *name, 
				void *array, 
				const u32 elem_size, 
				const char * const type_name, 
				const u32 num_dims, 
				...);

// 
// Array related global functions
//
extern void * 
__aos_create_array(
  				const char * const filename, 
  				const int lineno, 
				const char * const name,
  				const char * const type_name, 
				const u32 elem_size,
				const u32 num_dims,
				const char flag, 
				const void *init_value,
				const u32 features,
  				...);

extern int aos_delete_array(const char * const filename, 
				const int lineno, 
				void *array); 
extern int aos_array_get_size(void *array, const int dim);
extern int __aos_array_reset_value(void *array, void *value, int flag);


#endif // End of Include

