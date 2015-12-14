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
// 03/16/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/IdMgr/SobjIdMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/ExitHandler.h"
#include "DocClient/DocClient.h"
#include "DocServer/DocSvr.h"
#include "IILUtil/IILSystemKeys.h"
#include "IILUtil/IILId.h"
#include "SEUtil/IILName.h"
#include "SEUtil/Docid.h"
#include "SEUtil/DocFileMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"


AosSobjIdMgr::AosSobjIdMgr()
:
mLock(OmnNew OmnMutex()),
mInitId(0),
mNumVirtuals(0),
mIdIncValue(eDftDocidIncValue)
{
}


AosSobjIdMgr::~AosSobjIdMgr()
{
}


bool
AosSobjIdMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	
	mName = config->getAttrStr("name");
	if (mName == "")
	{
		OmnAlarm << "Missing ID Name: " << config->toString() << enderr;
		return false;
	}

	mNumVirtuals = config->getAttrInt(AOSTAG_NUM_VIRTUALS, -1);
	if (mNumVirtuals < 1)
	{
		OmnAlarm << "Num of Virtuals Incorrect: " << mNumVirtuals 
			<< ": " << config->toString() << enderr;
		return false;
	}

	mInitId = config->getAttrU64("init_id", 0);

	mIdIncValue = config->getAttrInt("inc_value", eDftDocidIncValue);
	if (mIdIncValue < 1)
	{
		OmnAlarm << "Invalid Docid Inc Value: " << mIdIncValue 
			<< ": " << config->toString() << enderr;
		mIdIncValue = eDftDocidIncValue;
	}
	return true;
}


bool
AosSobjIdMgr::stop()
{
    return true;
}


bool
AosSobjIdMgr::start()
{
	return true;
}


u64
AosSobjIdMgr::nextId(const int virtual_id, const AosRundataPtr &rdata)
{
	// If 'objid' is empty, it will create an objid that maps to the
	// same virtual as the docid. Otherwise, it allocates a docid on the
	// same virtual to which 'objid' maps.
	aos_assert_rr(mNumVirtuals > 0, rdata, false);
	aos_assert_rr(virtual_id > 0 && virtual_id < mNumVirtuals, rdata, false);

	// 1. Determine the virtual
	mLock->lock();
	if (mNumIds[virtual_id] <= 0)
	{
		bool rslt = retrieveIdsLocked(virtual_id, rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	u64 id = mCrtIds[virtual_id]++;
	mNumIds[virtual_id]--;
	mLock->unlock();
	return id;
}


bool
AosSobjIdMgr::retrieveIdsLocked(const int vid, const AosRundataPtr &rdata)
{
	aos_assert_rr(mName != "", rdata, false);
	OmnString iilname = AosIILName::composeSobjIdMgrIILName(mName);
	u64 id = 0;
	bool rslt = AosIncrementKeyedValue(iilname, mName, id, true, 
			mIdIncValue, mInitId, true, rdata);
	aos_assert_rr(rslt, rdata, false);

	mCrtIds[vid] = id;
	mNumIds[vid] = mIdIncValue;
	return true;
}


void
AosSobjIdMgr::returnId(const u64 &id, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
}

