////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

// Our RHC System
#include "../rhcUtil/aosMemMgrApp.h"
#include "../rhcUtil/aosResMgrToolkit.h"

#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/utsname.h>

// Linux programmer
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	char *zErrmsg = NULL;

	// 1. Validate the input
	if(2 == argc && (strcasecmp(argv[1], "--help") == 0))
	{
		printMemMgrHelp(argv[0]);
		return 0;
	}

	if(0 != getuid())
	{
		zErrmsg = "Error: Authority not enough.";
		printf (USAGE_INFO_FMT,zErrmsg,argv[0]);
		return -1;
	}
	else if(3 != argc)
	{
		zErrmsg = "Error: Illegal input.";
		printf (USAGE_INFO_FMT,zErrmsg,argv[0]);
		return -1;
	}

	// 2. Get the input value
	g_theMemMgrApp.m_interval = getDuration(argv[1]);
	g_theMemMgrApp.m_duration = getDuration(argv[2]);
	if(g_theMemMgrApp.m_duration <= 0 || g_theMemMgrApp.m_interval <= 0)
	{
		zErrmsg = "Error: Illegal input.";
		printf (USAGE_INFO_FMT,zErrmsg,argv[0]);
		return -1;
	}
	// 3. Get the key value 
	g_theMemMgrApp.m_maxr = g_theMemMgrApp.m_duration/g_theMemMgrApp.m_interval;
	if(g_theMemMgrApp.m_maxr > MAX_TIME_RATIO)
	{
		// illegal input!
		zErrmsg = (char *) malloc(sizeof(char)*100);  // malloc(40);
		sprintf(zErrmsg, "Error: Records number [%d] out of bound %ld.", g_theMemMgrApp.m_maxr, MAX_TIME_RATIO);
		printf (USAGE_INFO_FMT,zErrmsg,argv[0]);
		if(zErrmsg)
		{
			free(zErrmsg);
			zErrmsg = NULL;
		}
		return -1;
	}

	// 4. Init daemon Part
	// 4.1 kill all of the same name process
	if(!killSameProc(argv[0]))
	{
		return -1;
	}
	// 4.2 make the process being daemon
#ifndef _DEBUG_
	daemon(0,0);
#endif
	// 4.3 register the real time time interrupt recall function!
	aos_init_real_timer(aos_mem_mgr_repeat, g_theMemMgrApp.m_interval);

	// 5. Always hold the recent maxr number records
	while (1)
	{
		// Delaying g_theMemMgrApp.m_interval
		sleep(g_theMemMgrApp.m_interval);
	}

	return 0;
}