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
// 01/24/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IdGen/U64IdGen.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "IdGen/IdGenDef.h"
#include "IdGen/IdGenMgr.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "SiteMgr/SyncServer.h"
#include "XmlUtil/XmlTag.h"



AosU64IdGen::AosU64IdGen(const OmnString &idName)
:
mLock(OmnNew OmnMutex()),
mIdName(idName),
mCrtId(0),
mMaxId(0)
{
	bool rslt = start();
	if (!rslt)
	{
		OmnString errmsg = "Failed to create IdGen: ";
		errmsg << idName;
		OmnAlarm << errmsg << enderr;
		OmnExcept e(OmnFileLine, errmsg);
		throw e;
	}
}


AosU64IdGen::AosU64IdGen(const AosXmlTagPtr &def)
:
mLock(OmnNew OmnMutex()),
mCrtId(0),
mMaxId(0)
{
	if (!def)
	{
		OmnString errmsg = "Missing definition: ";
		OmnAlarm << errmsg << enderr;
		OmnExcept e(OmnFileLine, errmsg);
		throw e;
	}

	mIdName = def->getTagname();
	mDef = AosIdGenMgr::getSelf()->getIdDef(def);
	aos_assert(mDef);
	
	bool rslt = getNextBlock();
	aos_assert(rslt);
}


AosU64IdGen::~AosU64IdGen()
{
}


bool
AosU64IdGen::start()
{
	// It retrieves the definition from the database and
	// initializes itself.
	mDef = AosIdGenMgr::getSelf()->getIdDef(mIdName);
	aos_assert_r(mDef, false);
	
	bool rslt = getNextBlock();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosU64IdGen::getNextBlockPriv()
{
	aos_assert_r(mDef, false);
	bool rslt = mDef->getNextBlock(mCrtId, mMaxId);
	aos_assert_r(rslt, false);
	return true;
}


u64
AosU64IdGen::getNextId1()
{
	// The caller should use lock to make sure it is thread
	// safe. This class is not thread safe.
	mLock->lock();
	if (mCrtId > mMaxId)
	{
		mLock->unlock();
		OmnAlarm << "ID incorrect: " << mCrtId << ":" << mMaxId << enderr;
		return 0;
	}

	u64 id = mCrtId++;
	if (mCrtId > mMaxId)
	{
		bool rslt = getNextBlockPriv();
		aos_assert_rl(rslt, mLock, 0);
		aos_assert_rl(mCrtId <= mMaxId, mLock, 0);
	}
	mLock->unlock();
	return id;
}


bool
AosU64IdGen::setCrtid(const u64 &id)
{
	aos_assert_r(mDef, false);
	mLock->lock();
	if (!mDef->setCrtid(id))
	{
		mLock->unlock();
		OmnAlarm << "Failed set crtid: " << id << enderr;
		return false;
	}
	mCrtId = id;
	mMaxId = id;
	mLock->unlock();
	return true;
}

