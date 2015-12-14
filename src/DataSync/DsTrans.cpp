////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/12/2010	Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataSync/DsTrans.h"

#include "Porting/GetTime.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCommClt.h"
#include "UtilComm/TcpClient.h"
#include "DataSync/DataSyncClt.h"
#include "Debug/Except.h"


static u64 sgTransId = 100;
static OmnMutex sgLock;


AosDsTrans::AosDsTrans(
		const AosDataSyncCltPtr &data_sync,
		const OmnString &request)
:
mLock(OmnNew OmnMutex()),
mRequest(request),
mDataSync(data_sync)
{
	sgLock.lock();
	mTransId = sgTransId++;
	sgLock.unlock();
}


AosDsTrans::~AosDsTrans()
{
}


bool
AosDsTrans::sendRequest(u32 &logid)
{
	mLock->lock();
	AosXmlTagPtr child;
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(mRequest, "");
	if (!root || !(child = root->getFirstChild()))
	{
		OmnAlarm << "Failed to parse the request!" << mRequest << enderr;
		mLock->unlock();
		return false;
	}
	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	if (NULL == objdef)
	{
		OmnAlarm << "Failed to get objdef!" << enderr;
		mLock->unlock();
		return false;
	}
	AosXmlTagPtr log = objdef->getFirstChild();
	if (NULL == log)
	{
		OmnAlarm << "Failed to get log!" << enderr;
		mLock->unlock();
		return false;
	}
	logid = log->getAttrU32("logid", 0);
	root->setAttr(AOSMSG_TRANSID, mTransId);

	//Send
	OmnTcpClientPtr conn = mDataSync->getConn();
	if(NULL == conn)
	{
		OmnAlarm << "Failed to get conn!" << enderr;
		mLock->unlock();
		return false;
	}
	bool rest =  conn->smartSend(root->toString().data(), root->toString().length());
	mLock->unlock();
	return rest;
}

#endif
