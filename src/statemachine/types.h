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
// 02/20/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_statemachine_types_h
#define aos_statemachine_types_h

struct aos_scm_msg;
struct aos_cond;

////////////////// aos_state_e //////////////////////
typedef enum
{
	eAosState_Invalid = 0, 

	eAosState_Atomic,

	eAosState_Last
} aos_state_e;

static inline int aos_state_check(aos_state_e type)
{
	return (type > eAosState_Invalid &&
			type < eAosState_Last)?1:0;
}

extern const char *aos_state_2str(aos_state_e type);
extern aos_state_e aos_state_2enum(const char * const type);

////////////////// aos_st_status_e //////////////////////
typedef enum
{
	eAosStStatus_Invalid = 0,
	
	eAosStStatus_Idle,
	eAosStStatus_InProgress,
	eAosStStatus_Finished,

	eAosStStatus_Last
} aos_st_status_e;

static inline int aos_st_status_check(aos_st_status_e s)
{
	return (s > eAosStStatus_Invalid &&
			s < eAosStStatus_Last)?1:0;
}

////////////////// aos_event_proc_t //////////////////////
typedef struct aos_event_proc
{
	struct aos_cond *		cond;
	struct aos_scm_msg *	msg_send;
	char					wait_for_resp;
	int 					next_state;
} aos_event_proc_t;


////////////////// aos_sm_status_e //////////////////////
typedef enum
{
	eAosSmStatus_Invalid = 0,
	
	eAosSmStatus_Idle,
	eAosSmStatus_InProgress,
	eAosSmStatus_InitActions,
	eAosSmStatus_ExitActions,
	eAosSmStatus_Finished,

	eAosSmStatus_Last
} aos_sm_status_e;

static inline int aos_sm_status_check(aos_sm_status_e s)
{
	return (s > eAosSmStatus_Invalid &&
			s < eAosSmStatus_Last)?1:0;
}
#endif

