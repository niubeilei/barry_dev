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
// 03/02/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_gen_ptr_h
#define aos_util_gen_ptr_h

#include "alarm_c/alarm.h"
#include "util_c/memory.h"

typedef enum
{
	eAosGenPtrTag_Invalid, 

	eAosGenPtrTag_RecvMsgAction,

	eAosGenPtrTag_Last
} aos_gen_ptr_tag_e; 

static inline int aos_gen_ptr_tag_check(aos_gen_ptr_tag_e type)
{
	return (type > eAosGenPtrTag_Invalid &&
			type < eAosGenPtrTag_Last);
}


typedef struct aos_gen_ptr
{
	aos_gen_ptr_tag_e	tag;
	void *				ptr;
} aos_gen_ptr_t;

static inline aos_gen_ptr_t *aos_gen_ptr_create(
		aos_gen_ptr_tag_e tag, 
		void *ptr)
{
	aos_gen_ptr_t *p = (aos_gen_ptr_t *)aos_malloc(sizeof(aos_gen_ptr_t));
	aos_assert_r(p, 0);
	p->tag = tag;
	p->ptr = ptr;
	return p;
}

#endif

