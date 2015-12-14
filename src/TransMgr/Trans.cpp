////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Trans.cpp
// Description:
//	This is the super class for transactions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TransMgr/Trans.h"

#include "Debug/Debug.h"
#include "Porting/LongTypes.h"
#include "Thread/Mutex.h"
#include "Util1/Time.h"


static OmnMutex		sgTransIdLock;
static int64_t		sgTransId = 1000;

unsigned int OmnTrans::mMaxActivity = OmnTrans::eDefaultMaxActivity;

OmnTrans::OmnTrans()
	:
mTransId(0),
mFinished(false),
mRespCode(OmnErrId::e200Ok),
mLastActivity(OmnTime::getSecTick()),
mTTL(eDefaultTTL),
mTooManyActivity(false)
{
	sgTransIdLock.lock();
	mTransId = sgTransId++;
	sgTransIdLock.unlock();
	OmnTrace << "Create trans: " << (int)mTransId << endl;
}


OmnString
OmnTrans::toString() const
{
	OmnString str;
	str << "\n----------------------"
		<< "\n    TransId:        " << (int)mTransId
		<< "\n    Finished:       " << mFinished
		<< "\n    RespCode:       " << OmnErrId::toStr(mRespCode)
		<< "\n    Errmsg:         " << mErrmsg
		<< "\n    Sender Addr:    " << mForeignTransId.mAddr.toString()
		<< "\n    Sender Port:    " << mForeignTransId.mPort
		<< "\n    Sender TransId: " << (int)mForeignTransId.mTransId
		<< "\n----------------------";
	return str;
}


bool
OmnTrans::ttlExpired() const
{
	return OmnTime::getSecTick() - mLastActivity > mTTL;
}
