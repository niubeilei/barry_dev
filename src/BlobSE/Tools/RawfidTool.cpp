////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 06 Jul 2015 created by White
////////////////////////////////////////////////////////////////////////////

#include <getopt.h>
#include <sstream>

#include "aosUtil/Types.h"
#include "AppMgr/App.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/Entry.h"
#include "Porting/Mutex.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

u32 	g_ulBuffLen = 1000000;		//1M
int 	gAosShowNetTraffic = 1;		//for the aos framework
bool	gSanityCheck = true;

void
usage(char* p)
{
	printf("this tool is designed to convert an old header file's rawfid to the date it belongs to.\n");
	printf("Usage: %s\n", p);
	printf("\t-r rawfid\n");
}


int
main(int argc, char **argv)
{
	int opt = 0;
	u64 ullRawfid = 0;
	char* pEndPtr;
	while((opt = getopt(argc, argv, "r:")) != -1)
	{
		switch (opt)
		{
		case 'r':
			ullRawfid = strtoll(optarg, &pEndPtr, 10);
			break;
		default:
			std::cout << "Invalid argument." << endl;
			usage(argv[0]);
			return -99;
			break;
		}
	}
	if (0 == ullRawfid)
	{
		usage(argv[0]);
		return -1;
	}
	std::cout << "rawfid:" << ullRawfid << endl;
	u32 ulTimestamp = (u32)((ullRawfid - eOldHeaderFileID_start) >> 16) * 86400;
	OmnString sCmd = "date -d @";
	sCmd << ulTimestamp;
	system(sCmd.data());
	return 0;
}
