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
// 01/18/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semantics_c/semantics.h"

#include "alarm_c/alarm.h"
#include "semanobj/array_so.h"
#include "semantics_c/objdict.h"
#include <stdlib.h>

#define AOS_SEMANTICS_LOGDIR_MAX_LEN 100
static char sg_log_dir[AOS_SEMANTICS_LOGDIR_MAX_LEN] = "";

int aos_semantics_init()
{
	int ret;

	ret = aos_objdict_init();

	return ret;
}


void aos_semantics_set_logdir(const char * const logdir)
{
	aos_assert(logdir);
	if (strlen(logdir) >= AOS_SEMANTICS_LOGDIR_MAX_LEN)
	{
		strncpy(sg_log_dir, logdir, AOS_SEMANTICS_LOGDIR_MAX_LEN-1);
	}
	else
	{
		strcpy(sg_log_dir, logdir);
	}
}


// 
// It retrieves the semantics log directory, which is determined as:
// 1. If sg_log_dir is not null, return it.
// 2. Otherwise, it retrieves the environemnt variable LOGDIR. 
// 3. If LOGDIR is not defined, the current directory is assumed.
// Note that 'sg_log_dir' can be set through command line.
//
char * aos_semantics_get_logdir()
{
	if (strlen(sg_log_dir) <= 0)
	{
		strcpy(sg_log_dir, getenv("LOGDIR"));
		if (strlen(sg_log_dir) <= 0)
		{
			strcpy(sg_log_dir, ".");
		}
	}

	return sg_log_dir;
}


// 
// Processes the command line arguments. If an argument is consumed
// by this function, it is removed from the list. 
//
// -log <logdir>
//
void aos_semantics_proc_cmdline(int *argc, char **argv)
{
	int index = 1;
	while (index < *argc)
	{
		if (strcmp(argv[index], "-log") == 0)
		{
			aos_semantics_set_logdir(argv[index+1]);
			index -= 2;
			continue;
		}

		index++;
	}
}

