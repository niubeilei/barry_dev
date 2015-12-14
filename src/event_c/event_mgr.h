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
// 03/03/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_event_event_mgr_h
#define Omn_event_event_mgr_h

#include "event_c/types.h"
#include "util_c/list.h"
#include "util_c/types.h"

struct aos_event;


typedef struct aos_pending_rcd
{
	aos_list_head_t 	link;
	struct aos_event *	event;
	int					ref_count;
} aos_pending_rcd_t;


typedef struct aos_event_rcd
{
	void *					user_data;		// Do not change these
	aos_event_e				type;			// three fields' order
	int						match_data;		// !!!!

	u32						reg_id;
	aos_list_head_t			link;
	aos_event_callback_t	callback;
	int						ref_count;
} aos_event_rcd_t;

typedef struct aos_event_mgr
{
	aos_list_head_t 		events[eAosEvent_Last];
} aos_event_mgr_t;


extern int aos_register_event(
		const aos_event_e event,
		aos_event_callback_t callback,
		const int match_data, 
		void *user_data, 
		u32 *reg_id);

extern int aos_unregister_event(
		const aos_event_e event,
		aos_event_callback_t callback, 
		const int match_data, 
		const u32 reg_id); 

extern int aos_add_event(struct aos_event *event);

#endif




