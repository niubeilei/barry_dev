////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpProcTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProc/http_proc/Tester/HttpProcTester.h"
#include "AppProc/http_proc/HttpParser.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"

#define MAX_BUFF_SIZE 5000
extern char HTTP_DATA[MAX_BUFF_SIZE];

bool HttpProcTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool HttpProcTester::basicTest()
{
	// aos_trace_hex("ClientHello", buffer, msglen);
	bool ret = false;
	unsigned int find_header_ret = find_header(HTTP_DATA, strlen(HTTP_DATA));
	char out_buff[200];
	memset(out_buff, 0x00, 200);
	sprintf(out_buff, "%d\n", find_header_ret);
	OmnTest(find_header_ret == NULL) << out_buff << endtc;
				
	return true;
}	


