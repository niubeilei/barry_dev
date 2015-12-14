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
// 12/14/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semantics_rco_h
#define aos_semantics_rco_h

#include "aosUtil/Types.h"
#include "aosUtil/Memory.h"
#include "porting_c/mutex.h"
#include <stdio.h>

extern aos_lock_t aos_rco_lock;
extern int __aos_rco_hold(char * filename, 
						  const int lineno, 
						  void *holder, 
						  void *obj);

extern int __aos_rco_put(char * filename, 
						  const int lineno, 
						  void *holder, 
						  void *obj);
typedef void (*aos_rco_destructor)(void *obj);

typedef struct
{
	u32					count;
	aos_rco_destructor	destructor;
} aos_rco_t;

#define aos_rco_define aos_rco_t __aos_rco_4321632
#define aos_rco_init(obj, dest)							\
			obj->__aos_rco_4321632.count = 0;			\
			obj->__aos_rco_4321632.destructor = dest;	


#define aos_rco_hold(holder, obj)						\
	aos_lock(&aos_rco_lock);							\
	((obj)->__aos_rco_4321632.count)++;					\
	aos_unlock(&aos_rco_lock);							\
	__aos_rco_hold(__FILE__, __LINE__, holder, obj)

#define aos_rco_put(holder, obj)						\
	__aos_rco_put(__FILE__, __LINE__, holder, obj);		\
	aos_lock(&aos_rco_lock);							\
	if (--((obj)->__aos_rco_4321632.count) == 0)		\
	{													\
		if ((obj->__aos_rco_4321632).destructor)		\
			(obj->__aos_rco_4321632.destructor)(obj);	\
		aos_free(obj);									\
	}													\
	aos_unlock(&aos_rco_lock);							

#define aos_rco_reset(ptr)								\
	if ((*ptr)) __aos_rco_put(__FILE__, __LINE__, ptr, *(ptr));			\
	
#define aos_rco_assign(ptr, obj)							\
	if ((*ptr)) __aos_rco_put(__FILE__, __LINE__, ptr, *(ptr));			\
	aos_lock(&aos_rco_lock);								\
	if ((*ptr) && --((*(ptr))->__aos_rco_4321632.count) == 0)			\
	{														\
		if ((*(ptr))->__aos_rco_4321632.destructor)			\
			(*(ptr))->__aos_rco_4321632.destructor(*(ptr));	\
		aos_free(*(ptr));									\
	}														\
	*(ptr) = obj;											\
	((obj)->__aos_rco_4321632.count)++;						\
	aos_unlock(&aos_rco_lock);								\
	__aos_rco_hold(__FILE__, __LINE__, ptr, obj);


	
#define aos_rco_dump(obj)								\
	printf("Ref count: %d, Destructor: %x\n", 			\
		(obj)->__aos_rco_4321632.count, 				\
		(unsigned int)(obj)->__aos_rco_4321632.destructor) 

#endif // End of Include

