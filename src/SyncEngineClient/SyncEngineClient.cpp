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
//	// 1. Find the table: 
//	// 2. Check whether a syncher has been registered with the table
//	//    If not, do nothing. Otherwise, get the syncher. If the 
//	//    syncher has not been created yet, create it.
//	//    Append the doc to the syncher.
//
//
// Modification History:
// 2015/10/15 Created by Young
////////////////////////////////////////////////////////////////////////////

#include "SyncEngineClient/SyncEngineClient.h"

#include "SEUtil/DeltaBeanOpr.h"
#include "Synchers/SyncherTable.h"
#include "SingletonClass/SingletonImpl.cpp"


OmnSingletonImpl(AosSyncEngineClientSingleton,
				AosSyncEngineClient,
				AosSyncEngineClientSelf,
				"AosSyncEngineClient");


AosSyncEngineClient::AosSyncEngineClient()
:
mLock(OmnNew OmnMutex())
{
}


AosSyncEngineClient::~AosSyncEngineClient()
{
}


bool
AosSyncEngineClient::config(const AosXmlTagPtr &def)
{
	return true;
}


bool        		
AosSyncEngineClient::start()
{
	return true;
}


bool
AosSyncEngineClient::stop()
{
	return true;
}


AosJimoPtr 
AosSyncEngineClient::cloneJimo() const
{
	try 
	{
		return OmnNew AosSyncEngineClient(*this);
	}
	catch (...)
	{
		OmnAlarm << "clone Jimo failed" << enderr;		
	}

	return NULL;
}

bool 
AosSyncEngineClient::isTableSynced(
		AosRundata *rdata,
		const OmnString &tablename)
{
	OmnNotImplementedYet;
	return true;
}


AosSyncherObjPtr
AosSyncEngineClient::getSyncher(
		AosRundata *rdata, 
		const OmnString &sync_name,
		const AosSyncherType::E type)
{
	mLock->lock();
	aos_assert_rl(AosSyncherType::isValid(type), mLock, 0);
	OmnString repairedname = repairSyncherName(rdata, sync_name, type);

	AosSyncherObjPtr sync;
	if (mSynchersMap.count(repairedname))
	{
		sync = mSynchersMap[repairedname];
		aos_assert_rl(sync, mLock, NULL);
	}
	else
	{
		sync = OmnNew AosSyncherTable(0);
		aos_assert_rl(sync, mLock, NULL);
		sync->setRundata(rdata);
		AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		buff->setOmnStr(sync_name);
		buff->reset();
		sync->serializeFromBuff(buff);

		mSynchersMap[repairedname] = sync;
	}
	mLock->unlock(); 

	return sync;
}


OmnString 
AosSyncEngineClient::repairSyncherName(
		AosRundata *rdata,
		const OmnString &sync_name,
		const AosSyncherType::E type)
{
	OmnString repairedName = "";
	repairedName << AosSyncherType::getTypeStr(type) << "_" << sync_name;
	return repairedName;
}
