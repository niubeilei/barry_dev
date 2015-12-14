//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "JimoAgent/JimoAgentReport.h"
#include "Porting/Sleep.h"
AosJimoAgentReport::AosJimoAgentReport(
		const OmnString &addr,
		const int port,
		const int heartbeat_time)
:
mHeartBeatTime(heartbeat_time)
{
	mClient = OmnNew OmnTcpClient("report_client", addr, port, 1, eAosTLT_FirstFourHigh);
}


AosJimoAgentReport::~AosJimoAgentReport()
{
}

bool
AosJimoAgentReport::start()
{
	bool rslt = sendReport();
	if (!rslt)
	{
		return true;
	}
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "JimoAgentReportThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}

bool
AosJimoAgentReport::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnString str;
	str << "<report opr=\"heartbeat\">"
		<< "</report>";
	OmnString errmsg;
	while (state == OmnThrdStatus::eActive)
	{
		while(!mClient->connect(errmsg))
		{
			OmnSleep(1);
		}
printf("jozhi: send %s\n", str.data());
		mClient->smartSend(str.data(), str.length());
		OmnSleep(mHeartBeatTime);
	}
	return true;
}

bool
AosJimoAgentReport::signal(const int threadLogicId)
{
	return true;
}


bool
AosJimoAgentReport::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosJimoAgentReport::sendReport()
{
	OmnString report = getReport();
	aos_assert_r(mClient, false);
	OmnString errmsg;
	while(true)
	{
		while(!mClient->connect(errmsg))
		{
			OmnSleep(1);
		}
printf("jozhi: send %s\n", report.data());
		bool rslt = mClient->smartSend(report.data(), report.length());
		if (rslt)
		{
			break;
		}
		OmnSleep(1);
	}
	return true;
}

OmnString
AosJimoAgentReport::getReport()
{
	OmnString str;
	str << "<report opr=\"report\" >"
		<< "</report>";
	return str;
}
