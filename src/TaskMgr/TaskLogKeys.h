////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 04/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_TaskLogKeys_h
#define AOS_TaskMgr_TaskLogKeys_h

#define AOSTASKLOG_CREATE_TASK_FAILED		"create_task_failed"
#define AOSTASKLOG_RUNNING					"task_running"
#define AOSTASKLOG_TASK_NOT_RUNNING 		"task_not_running"
#define AOSTASKLOG_PAUSE					"task_paused"
#define AOSTASKLOG_RESUME					"task_resumed"
#define AOSTASKLOG_TASK_ALREADY_FINISHED	"task_already_finished"
#define AOSTASKLOG_ABORTED					"task_aborted"
#define AOSTASKLOG_FINISHED					"task_finished"
#define AOSTASKLOG_FAILED					"task_failed"
#define AOSTASKLOG_TASK_NOT_LOCAL			"task_not_local"
#define AOSTASKLOG_INTERNAL_ERROR			"internal_error"
#define AOSTASKLOG_MISSING_SDOC_OBJID		"missing_sdoc_objid"

#define AOSTASKLOG_ACTION_RUNNING			"action_running"
#define AOSTASKLOG_ACTION_SUCCESS			"action_success"
#define AOSTASKLOG_ACTION_PROCESS			"action_process"
#define AOSTASKLOG_ACTION_FAILED			"action_failed"
#define AOSTASKLOG_ACTION_ABORTED			"action_aborted"

#endif
