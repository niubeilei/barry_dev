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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/Tester/CubeCommTester.h"

#include "CubeComm/CubeComm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Porting/Sleep.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>

using namespace std;

extern int mEndpointId;
extern int mRemoteEndpointId;
extern bool gIsServer;
extern int tries;
AosCubeCommTester::AosCubeCommTester()
{
	mTries=tries;
	mName = "CubeCommTester";
	mCubeComm = new AosCubeComm(mEndpointId);
	mRundata = new AosRundata();
	mMsgReceived = 0;

	OmnCommListenerPtr thisptr(this, false);
	mCubeComm->startReading(mRundata, thisptr);
}


bool 
AosCubeCommTester::start()
{
	return basicTest();
}


bool
AosCubeCommTester::basicTest()
{
	OmnConnBuffPtr connBuff;
	AosBuffPtr buff = OmnNew AosBuff();

	if (gIsServer)
	{
		while (1)
		{
			OmnSleep(1);
		}
	}
	else
	{
		buff->setU32(mEndpointId);
		buff->setU32(mRemoteEndpointId);
		mValue = "01234567890123456789012345678901234567890123456789\0";
		buff->setOmnStr(mValue);

		for (int i=0; i<mTries; i++)
		{
			mCubeComm->sendTo(mRundata, mRemoteEndpointId, buff.getPtr());
			OmnScreen << "Send data. buff length is: " << buff->dataLen() << endl;
		}

		while (mMsgReceived != mTries)
		{
			OmnSleep(1);
			//msgRead(connBuff);
			//sleep(1);
		}
	}

	return true;
}


bool		
AosCubeCommTester::msgRead(const OmnConnBuffPtr &buff)
{
	//mCubeComm->msgRead(buff);
	if (!buff)
		return false;

	OmnScreen << "test Received buff length: " << buff->getDataLength() << endl;

	AosBuffPtr b = OmnNew AosBuff(buff->getData(),buff->getDataLength(),buff->getDataLength());
	u32 from_endpoint = b->getU32(0);
	u32 to_endpoint = b->getU32(0);
	OmnString value = b->getOmnStr("");
	OmnScreen << "test Received data: " << value << endl;

	aos_assert_r(from_endpoint == mRemoteEndpointId, false);
	aos_assert_r(to_endpoint == mEndpointId, false);
	mMsgReceived++;
	if (gIsServer)
	{
		AosBuffPtr bb = OmnNew AosBuff();
		bb->setU32(to_endpoint);
		bb->setU32(from_endpoint);
		bb->setOmnStr(value);
		mCubeComm->sendTo(mRundata, mRemoteEndpointId, bb.getPtr());
		OmnScreen << "Send data. buff length is: " << bb->dataLen() << endl;

		return true;
	}

	//aos_assert_r(value == mValue, false); 
	return true;
}


OmnString	
AosCubeCommTester::getCommListenerName() const
{
	return "CubeCommTester";
}


void 		
AosCubeCommTester::readingFailed()
{
}
