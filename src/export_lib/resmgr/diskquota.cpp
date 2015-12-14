////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cpumgr.cpp
// Description:
//   
//
// Modification History:
// 			Created by CHK 2007-03-21
////////////////////////////////////////////////////////////////////////////

#include "diskquota.h"
#include "common.h"

int aos_quota_set_status(char * partition, char* status)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "disk mgr quota status %s %s", status, partition);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_quota_get_status(char* partition, char *status)
{
	OmnString rslt = "";
	char* tmp;
	char show[1024];
	char mount_point[128];
	char *p;

	memset(show, 0, sizeof(show));
	memset(mount_point, 0, sizeof(mount_point));

	OmnCliProc::getSelf()->runCliAsClient("disk mgr quota status show", rslt);
	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

    strcpy(show, rslt);
	p = strtok(show, "\n");
	p = strtok(NULL, "\n");
	while (p)
	{
		sscanf(p, "%*s %s", mount_point);
		if (!strcmp(mount_point, partition))
		{
			strcpy(status, "on");
			return *((int*)tmp);
		}
		p = strtok(NULL, "\n");
	}
	strcpy(status, "off");

	return *((int*)tmp);
}

int aos_quota_list(char * result, int* len)
{
	OmnString rslt = "";
	char* tmp;

    if (*len <= (int)0 || *len < (int)((strlen(rslt)+1)))
        return -eAosRc_LenTooShort;

	OmnCliProc::getSelf()->runCliAsClient("disk mgr quota list", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);
}

int aos_quota_add_user(char * partition, char * user, char *size)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "disk mgr quota add user %s %s %s", partition, user, size);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_quota_delete_user(char * partition, char * user)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "disk mgr quota delete user %s %s", partition, user);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_quota_add_dir(char * partition, char * dir, char *size)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "disk mgr quota add dir %s %s %s", partition, dir, size);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_quota_delete_dir(char * partition, char * dir)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "disk mgr quota delete dir %s %s", partition, dir);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_quota_retrieve_user(char * user, char * result, int* len)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];

	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "disk mgr quota show user %s", user);
    if (*len <= (int)0 || *len < (int)((strlen(rslt)+1)))
        return -eAosRc_LenTooShort;

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);
}

int aos_quota_retrieve_dir(char * dir, char * result, int* len)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];

	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "disk mgr quota show dir %s", dir);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
    if (*len <= (int)0 || *len < (int)(rslt.length()+1))
        return -eAosRc_LenTooShort;
	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);
}

int aos_quota_clear_config()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("disk mgr quota clear config", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_quota_retrieve_config(char *result, int* len)
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("disk mgr quota show config", rslt);
    if (*len <= (int)0 || *len < (int)(rslt.length()+1))
        return -eAosRc_LenTooShort;
	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);
}


