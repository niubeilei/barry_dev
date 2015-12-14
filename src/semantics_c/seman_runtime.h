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
// 11/24/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_semantic_operator_array_operator_h
#define Aos_semantic_operator_array_operator_h

#ifndef AOS_SEMANTICS

#define aos_array_decl(array, type, size)
#define aos_array_delete(array)
#define aos_array_modify(array, index, value)	array[index] = value
#define aos_array_read(array, index)			array[index]
#define aos_array_stop(array)
#define aos_array_resume(array)
#define aos_array_read_only(array)
#define aos_array_modify_only(array)
#define aos_array_read_modify(array)

#else
#include "semantic_operators/array_operators_def.h"

#define aos_array_decl(array, type, size)							\
		__aos_array_decl(__FILE__, __LINE__, array, type, size)

#define aos_array_delete(array)										\
		__aos_array_delete(__FILE__, __LINE__, array)

#define aos_array_modify(array, index, value)						\
		( 															\
		  	__aos_array_modify(__FILE__, __LINE__, 					\
				 array, index),										\
		  	array[index] = value 									\
		)

#define aos_array_read(array, index)								\
		(															\
		  	__aos_array_read(__FILE__, __LINE__, 					\
			  (void *)array, index),								\
			array[index]											\
		)

#define aos_array_stop(array)										\
		__aos_array_stop(__FILE__, __LINE__, array)

#define aos_array_resume(array)										\
		__aos_array_resume(__FILE__, __LINE__, array)

#define aos_array_read_only(array)									\
		__aos_array_read_only(__FILE__, __LINE__, array)

#define aos_array_modify_only(array)								\
		__aos_array_modify_only(__FILE__, __LINE__, array)

#define aos_array_read_modify(array)								\
		__aos_array_read_modify(__FILE__, __LINE__, array)

#endif // AOS_SEMANTICS

#endif // End of Include

