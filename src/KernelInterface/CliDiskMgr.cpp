////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Cli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#include "KernelInterface/Cli.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/CliCmd.h"
#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>
#include <stdlib.h>
#include <error.h>

#define	HMD	"/root/AOS/src/DiskMgr/diskmgr"//
#define TYPEFILE "/root/type"

#include "KernelInterface_exe/version.h"

int
AosCli::diskMgrSetRecordTime(OmnString &rslt)
{
	return 0;
}

int
AosCli::diskMgrSetRecordStop(OmnString &rslt)
{
	return 0;
}

int
AosCli::diskMgrShowStatistics(OmnString &rslt)
{
	return 0;
}

int
AosCli::diskMgrFileTypeAdd(OmnString &rslt)
{
	int curPos = 0;
	FILE *fp;
	int i;
	char readbuf[16];
	OmnString type;
	curPos = mFileName.getWord(curPos, type);

	if ((fp = fopen(TYPEFILE, "a+")) == NULL)
		return -1;

	memset(readbuf, 0, sizeof(readbuf));
	while (fgets(readbuf, sizeof(readbuf), fp) != NULL) {
		for (i = 0; i < sizeof(readbuf); i++) {
			if (readbuf[i] == '\n')
				readbuf[i] = '\0';
		}
		if (!strcmp(readbuf, type)) {
			printf("this file type already exists\n");
			return 0;
		}
	}

	if (fputs(type, fp) < 0)
		printf("add type error");
	return 0;
}

int
AosCli::diskMgrFileTypeRemove(OmnString &rslt)
{
	int curPos = 0;
	FILE *fp;
	int i;
	char readbuf[16];
	OmnString type;
	curPos = mFileName.getWord(curPos, type);

	if ((fp = fopen(TYPEFILE, "r+")) == NULL)
		return -1;

	memset(readbuf, 0, sizeof(readbuf));

	while (fgets(readbuf, sizeof(readbuf), fp) != NULL) {
		for (i = 0; i < sizeof(readbuf); i++) {
			if (readbuf[i] == '\n')
				readbuf[i] = '\0';
		}
		if (!strcmp(readbuf, type)) {

		}
	}
	return 0;
}

int
AosCli::diskMgrFileTypeClear(OmnString &rslt)
{
	if (unlink(TYPEFILE) < 0) {
			printf("the type is areadly be cleared\n");
			return -1;
	}
	return 0;
}

int
AosCli::diskMgrFileTypeShow(OmnString &rslt)
{
	FILE *fp;
	char readbuf[128];
	memset(readbuf, 0, sizeof(readbuf));

	if ((fp = fopen(TYPEFILE, "r")) == NULL) 
	{
		printf("the file is areadly be cleared");
		return -1;
	}

	while (fgets(readbuf, sizeof(readbuf), fp) != NULL)
		printf("%s", readbuf);

	return 0;
}

#endif

