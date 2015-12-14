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
// 08/14/2015 Created by Phil
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/StreamSyncer.h"
#include "Thread/Mutex.h"

static vector<AosStreamSyncer *> sgSyncerList;
static OmnMutex	sgLock;

AosStreamSyncer::AosStreamSyncer()
:mLock(OmnNew OmnMutex())
{
}

AosStreamSyncer::~AosStreamSyncer()
{
}

///////////////////////////////////////////
//  control method
///////////////////////////////////////////

//
//hold the syncer
//The caller needs to hold the global lock
//
bool
AosStreamSyncer::hold(
		OmnString stream,
		u64 rddId)
{
	if (mStream == "")
	{
		mStream = stream;
		aos_assert_r(mRDDList.size() == 0, false);
	}

	//already been held by another stream
	if (mStream != stream)
		return false;

	mRDDList.push_back(rddId);
	return true;
}

//
//release the stream syncer
//the caller need to hold the global lock
//
bool
AosStreamSyncer::release(
		OmnString stream,
		u64 rddId)
{
	aos_assert_r(mStream == stream, false);
	aos_assert_r(mRDDList.size() > 0, false);

	vector<u64>::iterator itr;

	itr = mRDDList.begin();
	//find the rddId entry in the list
	while (itr != mRDDList.end())
	{
		if (*itr == rddId)
		{
			mRDDList.erase(itr);
			//if last RDD hold the syncer, reset
			//mStream for other stream to hold
			//the syncer
			if (mRDDList.size() == 0)
				mStream = "";

			return true;
		}

		itr++;
	}

	//failed to find the entry
	return false;
}

//
//check if the stream can hold the syncer
//the caller need to hold the global lock
//
bool
AosStreamSyncer::canHold(OmnString stream)
{
	if (mStream == "")
		return true;

	if (mStream == stream)
		return true;

	return false;
}

//
//Find all the syncers relevant with the stream and
//hold them all
//
bool 
AosStreamSyncer::holdByStream(
		OmnString stream,
		u64 rddId)
{
	bool holdFlag = true;

	sgLock.lock();

	//check if the stream can get all the syncers
	//it needs
	for (u32 i = 0; i < sgSyncerList.size(); i++)
	{
		if (sgSyncerList[i]->hasStream(stream))
		{
			if (!sgSyncerList[i]->canHold(stream))
			{
				holdFlag = false;
				break;
			}
		}
	}

	//hold all the needed syncers
	if (holdFlag)
	{
		for (u32 i = 0; i < sgSyncerList.size(); i++)
		{
			if (sgSyncerList[i]->hasStream(stream))
				sgSyncerList[i]->hold(stream, rddId);
		}
	}

	sgLock.unlock();
	return holdFlag;
}

//
//Find all the syncers relevant with the stream and
//hold them all
//
bool 
AosStreamSyncer::releaseByStream(
		OmnString stream,
		u64 rddId)
{
	sgLock.lock();
	for (u32 i = 0; i < sgSyncerList.size(); i++)
	{
		if (sgSyncerList[i]->hasStream(stream))
			sgSyncerList[i]->release(stream, rddId);
	}
	sgLock.unlock();

	return true;
}

///////////////////////////////////////////
//  helper method
///////////////////////////////////////////
bool
AosStreamSyncer::hasStream(OmnString stream)
{
	for (u32 i = 0; i < mStreamList.size(); i++)
	{
		if (mStreamList[i] == stream)
			return true;
	}

	return false;
}

bool
AosStreamSyncer::addToSyncerList(OmnString syncer_name)
{
	setName(syncer_name);
	sgSyncerList.push_back(this);
	return true;
}

bool
AosStreamSyncer::addToStreamList(OmnString name)
{
	mStreamList.push_back(name);
	return true;
}
	
bool
AosStreamSyncer::dropSyncer(OmnString name)
{
	vector<AosStreamSyncer *>::iterator itr;
	itr = sgSyncerList.begin();
	while(itr != sgSyncerList.end())
	{
		if((*itr)->getName() == name)
		{
			sgSyncerList.erase(itr);
			break;
		}
		itr++;
	}
	return true;
}

bool
AosStreamSyncer::setName(OmnString name)
{
	mName = name;
	return true;
}

OmnString
AosStreamSyncer::getName()
{
	return mName;
}
