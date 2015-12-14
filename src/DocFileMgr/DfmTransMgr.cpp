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
// 12/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocFileMgr/DfmTransMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"


OmnSingletonImpl(AosDfmTransMgrSingleton,
                 AosDfmTransMgr,
                 AosDfmTransMgrSelf,
                "AosDfmTransMgr");

AosDfmTransMgr::AosDfmTransMgr()
:
mLock(OmnNew OmnMutex()),
mIsStopping(false)
{
	AosDfmTransMgrObjPtr thisptr(this, false);
	AosDfmTransMgrObj::setDfmTransMgr(thisptr);

	bool rslt = initTransId();
	if (!rslt)
	{
		OmnThrowException("Failed initialize TransID");
		return;
	}
}


AosDfmTransMgr::~AosDfmTransMgr()
{
}


bool
AosDfmTransMgr::start()
{
	return true;
}


bool
AosDfmTransMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosDfmTransMgr::stop()
{
    return true;
}


bool
AosDfmTransMgr::initTransId()
{
	// This class manages the transaction ID. It saves the current
	// transaction ID in a file. The file is managed by DocFileMgr. 
	// This function is called when it starts. It retrieves the 
	// transaction ID from the file. If the file does not exist, 
	// it creates the file. Transaction IDs are used in blocks. 
	// It retrieves one block of IDs from the file.
	mLock->lock();
	OmnNotImplementedYet;
	mLock->unlock();
	return false;
}


bool
AosDfmTransMgr::cleanTrans()
{
	// This function is called when the class starts. It 
	// retrieves all the unfinished transactions, if any, 
	// and then cleans them. 
	OmnNotImplementedYet;
	return false;
}


AosDfmTransPtr
AosDfmTransMgr::createTrans(const AosRundataPtr &rdata)
{
	// This function creates a transaction.
	mLock->lock();
	u64 transId = getTransId();
	aos_assert_rl(transId > 0, mLock, 0);

	try
	{
		AosDfmTransPtr trans = OmnNew AosDfmTrans(transId);
		mLock->unlock();
		return trans;
	}

	catch (...)
	{
		OmnAlarm << "Failed creating transaction!" << enderr;
	}

	mLock->unlock();
	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDfmTransMgr::returnTrans(
		const AosDfmTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	// In the future, transactions are managed by a pool. In the
	// current implementations, it does nothing.
	return true;
}
#endif
