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
//  This class is used to create docs. It will:
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/ReservedMgr.h"

#include "SingletonClass/SingletonImpl.cpp"
#include "Util1/Timer.h"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(AosReservedMgrSingleton,
                 AosReservedMgr,
                 AosReservedMgrMgrSelf,
                "AosReversedMgr");


AosReservedMgr::AosReservedMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosReservedMgr::~AosReservedMgr()
{
}


bool
AosReservedMgr::start()
{
	return true;
}


bool
AosReservedMgr::stop()
{
	return true;
}


bool
AosReservedMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


void
AosReservedMgr::cancelTimer(const u64 &docid)
{
	// This function cancels the timer
	int timerid;
	mLock->lock();
	map<u64, int>::iterator it = mTimers.find(docid);
	if(it != mTimers.end())
	{
		timerid = it->second;
		mTimers.erase(it);
	}
	mLock->unlock();

	if(timerid != 0)
	{
		OmnTimer::getSelf()->cancelTimer(timerid);	
	}

}


void
AosReservedMgr::addTimer(const int timerid, const u64 &docid)
{
	//mTimers[docid] = timerid;
	mLock->lock();
	mTimers.insert(make_pair(docid, timerid));
	mLock->unlock();
}


void
AosReservedMgr::removeTimer(const u64 &docid)
{
	mLock->lock();
	
	map<u64, int>::iterator it = mTimers.find(docid);
	if(it != mTimers.end())
	{
		mTimers.erase(it);
	}

	mLock->unlock();
}

