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
// Modification History:
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEClient/SEClient.h"
#include "SEClient/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int gAosLogLevel = 0;
bool gAosShowNetTraffic = false;

int 
main(int argc, char **argv)
{
	int index = 1;
	OmnApp::appStart(argc, argv);             
	//OmnAppPtr theApp = OmnNew OmnApp(argc, argv);

	OmnString ip = "192.168.99.97";
	u64 port = 5556;
	OmnString fname = "request.txt";

	if (argc > 1)
		ip = argv[1];
	if (argc > 2)
		port  = atoi(argv[2]);
	if (argc > 3)
		fname = argv[3];

	AosSEClientPtr conn = OmnNew AosSEClient(ip, port);


	OmnFile f(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!f.isGood())
	{
		OmnAlarm << "Failed to open the req file: " << fname << enderr;
		exit(-1);
	}
	OmnString req, resp, err;                                                       
	if (!f.readToString(req))
	{
		OmnAlarm << "Failed to read req: " << fname << enderr;
		exit(-1);
	}

	cout << "Request:\n" << req.data() << endl;
	conn->procRequest(100, "", "", req, resp, err);
	cout << "Response:\n" << resp.data() << endl;	

	return 0;
} 
