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
// 12/11/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"


AosDfmTrans::AosDfmTrans(const u64 &transid)
:
mTransId(transid),
mReadFlag(false),
mStatus(eInvalid),
mCanBeCleaned(true)
{
}


AosDfmTrans::~AosDfmTrans()
{
}


bool 		
AosDfmTrans::start()
{
	aos_assert_r(mStatus == eInvalid, false);
	mStatus = eActive;
	return true;
}


bool 
AosDfmTrans::commit(const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_rl(mStatus == eActive, mLock, false);
	mStatus = eCommitting;
	mReadFlag = true;
	mLock->unlock();
	return true;
}


bool 
AosDfmTrans::rollback(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDfmTrans::cleanTrans(const AosRundataPtr &rdata)
{
	// A transaction may fail due to various reasons, such as system
	// crashes, software core dumps, etc. When the system restarts, 
	// it may need to clean transactions. 
	// Whether a transaction can be cleaned or not is determined
	// by the transaction. A transaction collects a number of headers. 
	// Headers point to bodies. Cleaning a header means to remove 
	// the body it points to. In the current implementations, headers
	// are either all cleanable, or none can be cleaned. 
	OmnNotImplementedYet;
	return false;
}


bool
AosDfmTrans::saveHeader(
		const u32 seqno,
		const u64 &offset, 
		const char *header, 
		const int len, 
		const AosRundataPtr &rdata)
{
	// This function saves the header in memory. If 'flush' is true, 
	// or if 'mFlush' is true, it should save the changes to files.
	//
	// Headers are saved in blocks. Each block holds a number of headers.
	// Headers in a block are sorted based on docids. Header blocks
	// are sorted. The transaction keeps a distribution map. 
	OmnNotImplementedYet;
	return false;
}


bool
AosDfmTrans::readDoc(
		const u64 &docid,
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	// This function checks whether its reading flag is on. 
	// If not, it sets 'buff' to null. Otherwise, it checks whether
	// the doc is in this transaction. If not, it sets 'buff' to null.
	// If the doc is in the transaction, it reads in the doc and sets 
	// it to 'buff'.
	//
	// If any error occured, it returns false. Otherwise, it returns true.
	mLock->lock();
	OmnNotImplementedYet;
	mLock->unlock();
	return false;
}


bool        
AosDfmTrans::run()
{
	// This is the OmnThrdShellProc function. It is called in a thread shell.
	// It does the following:
	// 1. Modify all the headers contained in this transaction to the
	//    real headers. 
	// 2. Delete all the bodies pointed to by the old headers. 
	// 3. Clean the transaction.
	// 4. Return the transaction to the transaction manager

	if (!modifyHeaders(mRundata)) return false;
	if (!deleteOldBodies(mRundata)) return false;
	if (!cleanTrans(mRundata)) return false;
	if (!returnTrans(mRundata)) return false;
	return true;
}


bool
AosDfmTrans::modifyHeaders(const AosRundataPtr &rdata)
{
	// The transaction has a collection of headers. This function
	// modifies the real headers by these temporary headers. 
	// During that process, it collects all the old headers. 
	OmnNotImplementedYet;
	return false;
}


bool
AosDfmTrans::deleteOldBodies(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDfmTrans::returnTrans(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool        
AosDfmTrans::procFinished()
{
	OmnNotImplementedYet;
	return false;
}
