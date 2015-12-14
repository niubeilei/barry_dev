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
// 04/29/2012: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SEClient/TcpClientTester/TcpClientThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include "XmlUtil/SeXmlParser.h"

AosTcpClientThrd::AosTcpClientThrd(
		const OmnString &name,
		const OmnString &addr, 
		const int port)
{
	/*
	OmnScreen << "connect to server address: " << addr << " port: " << port << endl;
	mClient = OmnNew OmnTcpClient(name, addr, port, 1, eAosTLT_FirstFourHigh);
	OmnString errmsg;
	if (!mClient->connect(errmsg))
	{
		errmsg = "Faild to connect the server";
		OmnAlarm << errmsg << enderr;
		return;
	}
	*/
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "Thrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosTcpClientThrd::~AosTcpClientThrd()
{
	OmnScreen << "AosTcpClientThrd deleted" << endl;
}

bool
AosTcpClientThrd::sendData()
{
	OmnAlarm << "AosTcpClientThrd deleted" << enderr;

	/*
	// It randomly generates a content, send it, and then read the response.
	u32 len = ((u32)rand()) % 10000000;
	char *data = new char[len];
	char dd = rand() % 256;
	memset(data, dd, len);
	bool rslt = mClient->smartSend(data, len);
	aos_assert_r(rslt, false);

	OmnConnBuffPtr buff;
	int nn = mClient->smartRead(buff);
	aos_assert_r(nn > 0 && (u32)nn == len, false);
	aos_assert_r(buff, false);
	nn = buff->getDataLength();
	aos_assert_r(nn > 0 && (u32)nn == len, false);
	char *recv_data = buff->getData();
	aos_assert_r(recv_data, false);
	aos_assert_r(strncmp(recv_data, data, nn) == 0, false);
	delete data;
	return true;
	*/
}

bool
AosTcpClientThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		sendData();
		//OmnSleep(1);
	}
	state = OmnThrdStatus::eExit;
	return true;
}


bool    
AosTcpClientThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


void
AosTcpClientThrd::heartbeat(const int tid)
{
}


bool    
AosTcpClientThrd::signal(const int threadLogicId)
{
	return true;
}

