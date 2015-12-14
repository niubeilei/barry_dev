////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: resmgr.cpp
// Description:
//   
//
// Modification History:
// 			Created by CHK 2007-03-21
////////////////////////////////////////////////////////////////////////////

#include "resmgr.h"
#include "common.h"

// 1. 
int aos_resmgr_start()
{
	OmnString rslt = "";
	char* tmp;

//	OmnCliProc::getSelf()->runCliAsClient("on", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return 0;	
}

// 2. 
int aos_resmgr_stop()
{
	OmnString rslt = "";
	char* tmp;

//	OmnCliProc::getSelf()->runCliAsClient("off", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

//	return *((int*)tmp);
	return 0;
}

// 3. 
int aos_resmgr_clear_config()
{
	OmnString rslt = "";
	char* tmp;
	OmnCliProc::getSelf()->runCliAsClient("res mgr clear config", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 4.	Retrieve Resource Manager Configuration
int aos_resmgr_retrieve_config(char * const config, const u32 bufsize)
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("res mgr show config", rslt);
	strncpy(config, rslt.data(), bufsize);
	config[bufsize-1] = 0;

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 5. 
int aos_resmgr_save_config(const char * fname)
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("res mgr save config", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 6. 
int aos_resmgr_load_config(const char * fname)
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("res mgr load config", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 7. 
int aos_resmgr_nice(const int pid, const int inc)
{
	OmnString rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "system nice set %d %d", pid, inc);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}
