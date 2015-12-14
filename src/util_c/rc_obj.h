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
// 02/11/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_rc_obj_h
#define aos_util_rc_obj_h

#include "porting_c/mutex.h"
#include <stdio.h>

#define aos_rc_obj_assign(lhs, rhs)		\
{										\
	if (lhs) (lhs)->mf->put(lhs);		\
	if (rhs) (rhs)->mf->hold(rhs);		\
	lhs = rhs;							\
}

#define AOS_DECLARE_RCOBJ_LOCK static aos_lock_t *sg_rcobj_lock = 0
// 
// This macro implements the 'hold(...)' function. It assumes
// there is a lock named 'sg_rcobj_lock'. 
//
#define AOS_RC_OBJ_HOLD(obj)									\
	aos_assert_r(obj, -1);										\
	if (!sg_rcobj_lock)											\
	{															\
		aos_assert_r(!aos_create_lock_exclusively(&sg_rcobj_lock), -1);	\
	}															\
	aos_lock(sg_rcobj_lock);									\
	obj->ref_count++;											\
	aos_unlock(sg_rcobj_lock);									\
	aos_assert_r(obj->is_destroying == 0, -1);					\
	return 0;


// 
// This macro implements the 'put(...)' function. It assumes
// there is a lock named 'sg_rcobj_lock' and the object
// has a member function 'destroy(...)'. 
//
#define AOS_RC_OBJ_PUT(obj)										\
	aos_assert_r(obj, -1);										\
	aos_assert_r(obj->is_destroying == 0, -1);					\
	aos_assert_r(sg_rcobj_lock, -1);							\
	aos_lock(sg_rcobj_lock);									\
	obj->ref_count--;											\
	if (obj->ref_count == 0)									\
	{															\
		obj->is_destroying = 1;									\
		aos_unlock(sg_rcobj_lock);								\
		obj->mf->destroy(obj);									\
		return 0;												\
	}															\
	aos_unlock(sg_rcobj_lock);									\
	return 0;

// 
// This macro implements the 'hold(...)' function. It assumes
// there is a lock named 'sg_rcobj_lock'. 
//
#define AOS_RCM_OBJ_HOLD(obj)									\
	aos_assert_r(obj, -1);										\
	if (!sg_rcobj_lock)											\
	{															\
		aos_assert_r(!aos_create_lock_exclusively(&sg_rcobj_lock), -1);	\
	}															\
	aos_lock(sg_rcobj_lock);									\
	obj->ref_count++;											\
	aos_unlock(sg_rcobj_lock);									\
	aos_assert_r(obj->is_destroying == 0, -1);					\
	return 0;


// 
// This macro implements the 'put(...)' function. It assumes
// there is a lock named 'sg_rcobj_lock' and the object
// has a member function 'destroy(...)'. 
//
#define AOS_RCM_OBJ_PUT(obj)									\
	if (!sg_rcobj_lock)											\
	{															\
		aos_assert_r(!aos_create_lock_exclusively(&sg_rcobj_lock), -1);	\
	}															\
	aos_lock(sg_rcobj_lock);								\
	obj->ref_count--;										\
	if (obj->ref_count == obj->min_ref_count)				\
	{														\
		aos_unlock(sg_rcobj_lock);							\
		obj->mf->release_paired_obj(obj);					\
		aos_lock(sg_rcobj_lock);							\
		aos_assert_r(obj->ref_count == 0, -1);				\
		obj->is_destroying = 1;								\
		aos_unlock(sg_rcobj_lock);							\
		obj->mf->destroy(obj);								\
		return 0;											\
	}														\
	aos_unlock(sg_rcobj_lock);								\
	return 0;

#define AOS_DESTROY_OBJ(obj)								\
	aos_assert_r((obj), -1);								\
	aos_assert_r(!(obj)->mf->release_memory(obj), -1);		\
	aos_free(obj);											\
	return 0;

#endif // End of Include

