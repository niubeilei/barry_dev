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
// 02/21/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_event_types_h
#define aos_event_types_h

struct aos_event;

typedef int (*aos_event_callback_t)(
		struct aos_event *event, 
		void *user_data);

typedef enum
{
	eAosEvent_Invalid = 0,

	eAosEvent_ActionFinished,
	eAosEvent_DataRcvd,
	eAosEvent_MsgRcvd,
	eAosEvent_StateFinished,
	eAosEvent_ThrdFinished,

	eAosEvent_Last
} aos_event_e;

static inline int aos_event_check(const aos_event_e type)
{
	return (type > eAosEvent_Invalid &&
			type < eAosEvent_Last)?1:0;
}

extern const char *aos_event_2str(const aos_event_e type);
extern aos_event_e aos_event_2enum(const char * const type);

#endif

