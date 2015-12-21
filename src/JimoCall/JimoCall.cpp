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
// A JimoCall defines a Jimo Functoin Call. There are following types
// of Jimo Calls:
// 	1. Reading Sync Calls
// 	2. Reading Async Calls
// 	3. Writing Sync Calls
// 	4. Writing Async Calls
// 	5. Normal Sync Calls
// 	6. Normal Async Calls
//
//
// Modification History:
// 2014/11/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/JimoCall.h"

#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallWaiter.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/ClusterObj.h"
#include "Thread/Sem.h"
#include "UtilData/FN.h"
#include "Util1/Time.h"


AosJimoCall::AosJimoCall()
:
mFunc(0),
mMsgID(0),
mJimoCallID(0),
mStatus(eCallIdle),
mSem(0),
mCallServer(0),
mStartTime(0),
mCluster(0),
mCrtEndpointIdx(0),
mTimerMs(eDftTimerMs)
{
}


AosJimoCall::AosJimoCall(
		const OmnConnBuffPtr &buff,
		AosJimoCallServer *server)
:
mFunc(0),
mMsgID(0),
mJimoCallID(0),
mStatus(eCallIdle),
mSem(0),
mStartTime(0),
mCluster(0),
mCrtEndpointIdx(0),
mTimerMs(eDftTimerMs)
{
	mConnBuff = buff;
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert(conn);
	mWebReq = OmnNew AosWebRequest(conn, buff);
	mCallServer = server;
	AosBuffPtr buffptr = OmnNew AosBuff(buff AosMemoryCheckerArgs);
	setBSON(buffptr);
	setFromEPInfo(mConnBuff->getRemoteAddr(), mConnBuff->getRemotePort());
	mStartTime = OmnTime::getTimestamp();
}


AosJimoCall::AosJimoCall(
		AosRundata *rdata,
		const OmnString &package,
		const int func,
		AosClusterObj *cluster)
:
mPackage(package),
mFunc(func),
mMsgID(0),
mJimoCallID(0),
mStatus(eCallIdle),
mSem(0),
mCallServer(0),
mStartTime(0),
mCluster(cluster),
mCrtEndpointIdx(0),
mTimerMs(eDftTimerMs)
{
}


AosJimoCall::~AosJimoCall()
{
	if(mSem)
	{
		OmnDelete mSem;
	}
}


/*
void
AosJimoCall::set(
		const int package_id,
		const int func_id,
		AosJimoCallDialer *dialer)
{
	reset();

	mPackage = package_id;
	mFunc = func_id;
	mCallDialer = dialer;
}
*/


void
AosJimoCall::setConnBuff(const OmnConnBuffPtr &conn_buff)
{
    mConnBuff = conn_buff;
}


void
AosJimoCall::setBSON(const AosBuffPtr &buff)
{
	mBSON.setBuff(buff);
}


bool
AosJimoCall::msgRead(
		AosRundata *rdata,
		AosBuff *buff)
{
	// If it is a request message, it pops off the attributes:
	// 	message ID		(u64)
	// 	jimo call ID	(u64)
	// 	package			(OmnString)
	// 	func			(int)
	//	cube id			(int)
	//	dupid			(u8)
	aos_assert_r(buff, false);

	mMsgID = (OmnMsgId::E)buff->getU64(0);
	mJimoCallID = buff->getU64(0);
	if (mMsgID == OmnMsgId::eJimoCallReq)
	{
		mPackage = buff->getOmnStr("");
		mFunc = buff->getInt(-1);
		mCubeID = buff->getInt(-1);
		mDupid = buff->getU8(0);
		aos_assert_r(mCubeID >= 0, false);
	}
	else if (mMsgID == OmnMsgId::eJimoCallResp)
	{
		mStatus = (Status)buff->getU8(0);
	}
	return true;
}


bool
AosJimoCall::getFieldValue(
		AosRundata *rdata,
		const AosFN::E message,
		AosValueRslt &value)
{
	//return mBSON.getFieldValue(rdata, message, value);
	OmnAlarm << enderr;
	return false;
}


bool
AosJimoCall::sendResp(AosRundata *rdata)
{
	// This function sends the response back to the caller.
	// The response is encoded as:
	// BSON				variable
	// <rdata>         	variable
	// MessageID       	u64
	// JimoCallID      	u64
	// Status          	char
	//
	aos_assert_r(mCallServer, false);

	AosBuff *buff = mBSON.getBuffRaw();
	aos_assert_r(buff, false);

	bool rslt = rdata->serializeToBuffForJimoCall(buff);
	aos_assert_r(rslt, false);

	buff->appendU64((u64)OmnMsgId::eJimoCallResp);
	buff->appendU64(mJimoCallID);
	buff->append((char)mStatus);
	rdata->setRequest(mWebReq);
	mCallServer->JimoCallBack(rdata, *this, buff);
	aos_assert_r(rslt, false);

	return true;
}


void
AosJimoCall::setSem(OmnSem * sem)
{
	OmnScreen << "set wait sem:" << this << "," << mJimoCallID << endl;
	mSem = sem;
}


void
AosJimoCall::logCall(AosRundata *rdata)
{
	AosBSON bson;
	bson.setValue(AosFN::ePackage, mPackage);
	bson.setValue(AosFN::eFunc, mFunc);
	bson.setValue(AosFN::eStatus, mStatus);
	bson.setValue(AosFN::eJimoCallID, mJimoCallID);
	bson.setValue(AosFN::eStartTime, mStartTime);
	bson.setValue(AosFN::eLength, OmnTime::getTimestamp() - mStartTime);

	// To be implemented;
}


void
AosJimoCall::logFailedTry(AosRundata *rdata)
{
	// To be implemented;
}


void
AosJimoCall::reset()
{
	mBSON.clear();
	mStatus = eCallIdle;
}


bool
AosJimoCall::makeCall(AosRundata *rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosJimoCall::callResponded(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnShouldNeverComeHere;
	return false;
}


void
AosJimoCall::checkTimer(AosRundata *rdata)
{
	OmnShouldNeverComeHere;
}


void
AosJimoCall::logFailedCall(AosRundata *rdata)
{
	OmnAlarm << "Jimo call failed: " << rdata->getErrmsg() << enderr;
}


void
AosJimoCall::logTimeoutCall(AosRundata *rdata)
{
	OmnNotImplementedYet;
}


void
AosJimoCall::dump() const
{
	OmnScreen << "dumping jimocall..." << endl
			<< "mPackage:" << mPackage.data() << endl
			<< "mFunc:" << mFunc << endl
			<< "mFromEPInfo:" << mFromEPInfo.toStr() << endl
			<< "mToEPInfo:" << mToEPInfo.toStr() << endl
			<< "mStatus:" << mStatus << endl;
}
