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
#ifndef AOS_Job_JobLogKeys_h
#define AOS_Job_JobLogKeys_h


#define AOSJOBLOG_JOB_ABORTED				"job_aborted"
#define AOSJOBLOG_JOB_DENIED				"job_denied"
#define AOSJOBLOG_JOB_FAILED				"job_failed"
#define AOSJOBLOG_JOB_FAILED_GET_TASKDOC	"job_failed_get_taskdoc"
#define AOSJOBLOG_JOB_FAILED_START_TASK		"job_failed_start_task"
#define AOSJOBLOG_JOB_FINISHED				"job_finished"
#define AOSJOBLOG_JOB_MISSING_JOBDOC		"job_missing_jobdoc"
#define AOSJOBLOG_JOB_NO_TASKS				"job_no_tasks"
#define AOSJOBLOG_JOB_NOT_IDLE				"job_not_idle"
#define AOSJOBLOG_JOB_RUNNING				"job_running"
#define AOSJOBLOG_JOB_STOP					"job_stop"

#define AOSJOBLOG_TASK_ABORTED				"task_aborted"
#define AOSJOBLOG_TASK_ALL_FINISHED			"task_all_finished"
#define AOSJOBLOG_TASK_FAILED				"task_failed"
#define AOSJOBLOG_TASK_FINISHED				"task_finished"

#define AOSJOBLOG_DATACOL_MERGE_STARTED		"datacol_merge_started"
#define AOSJOBLOG_DATACOL_MERGE_FINISHED	"datacol_merge_finished"

#define AOSJOBLOG_CLEANFIXEDDOC_STARTED		"cleanfixeddoc_started"
#define AOSJOBLOG_CLEANFIXEDDOC_FINISHED	"cleanfixeddoc_finished"

#define AOSJOBLOG_INTERNAL_ERROR			"internal_error"

#endif

