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
#ifndef aos_semanobj_so_h
#define aos_semanobj_so_h

#include "aosUtil/Types.h"
#include "porting_c/mutex.h"
#include "semanobj/so_type.h"

struct aos_so;

// 
// Member Function Definitions
//
extern int aos_so_lock(struct aos_so *obj);
extern int aos_so_unlock(struct aos_so *obj);
extern enum aos_so_type_e aos_so_get_type(struct aos_so *obj);

#define AOS_SO_MEMBER_FUNC_INIT					\
			aos_so_lock, 						\
			aos_so_unlock, 						\
			aos_so_get_type

struct aos_so;
extern struct aos_so * aos_so_constructor(void *inst, enum aos_so_type_e type);

#define AOS_SO_MEMBER_FUNC_DECL								\
	int (*lock)(struct aos_so *obj);						\
	int (*unlock)(struct aos_so *obj);						\
	enum aos_so_type_e (*get_type)(struct aos_so *obj)

struct aos_so_class
{
	AOS_SO_MEMBER_FUNC_DECL;
};

struct aos_so
{
	struct aos_so_class *member_funcs;

	void 			   *inst;
	aos_lock_t			lock;
	u16					ref;
	enum aos_so_type_e	type;

};
#endif // End of Include

