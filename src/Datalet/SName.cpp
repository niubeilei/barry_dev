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
//
// Modification History:
// 2013/03/28 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Datalet/SName.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/File.h"


AosSName::AosSName()
:
mLock(OmnNew OmnMutex()), 
mCrtId(AosSName::eUserDefinedId)
{
}


AosSName::AosSName(
		const AosRundataPtr &rdata, 
		const OmnString &objid, 
		const bool is_local)
:
mLock(OmnNew OmnMutex()), 
mCrtId(AosSName::eUserDefinedId),
mObjid(objid),
mIsLocal(is_local)
{
	if (!loadNames(rdata))
	{
		OmnThrowException("Failed loading names");
		return;
	}
}


AosSName::~AosSName()
{
}


bool
AosSName::loadNames(const AosRundataPtr &rdata)
{
	if (mObjid == "")
	{
		OmnAlarm << "Objid is empty" << enderr;
		return false;
	}

	mLock->lock();
	bool rslt;
	if (mIsLocal)
	{
		rslt = loadLocalNamesLocked(rdata);
	}
	else
	{
		rslt = loadDbNamesLocked(rdata);
	}
	mLock->unlock();
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosSName::loadDbNamesLocked(const AosRundataPtr &rdata)
{
	// This function loads the names that are defined locally.
	OmnNotImplementedYet;
	return false;
}


bool
AosSName::loadLocalNamesLocked(const AosRundataPtr &rdata)
{
	// This function loads the names that are defined locally.
	OmnString fname = AosGetBaseDirname();
	aos_assert_rr(fname != "", rdata, false);
	fname << "/Data/Semantics/" << mObjid;
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (file.isGood())
	{
		AosSetErrorU(rdata, "file_not_found") << ":" << mObjid;
		return false;
	}
	
	AosBuffPtr buff;
	bool rslt = file.readToBuff(buff, eMaxFileLength);
	aos_assert_rr(rslt, rdata, false);

	// The contents are defined as [name, index] pairs.
	buff->reset();
	OmnString name;
	while ((name = buff->getOmnStr("")) != "")
	{
		u32 idx = buff->getU32(eInvalidNameId);
		aos_assert_rr(idx != eInvalidNameId, rdata, false);
		mMap[name] = idx;
	}

	return true;
}


u32
AosSName::addName(const OmnString &name)
{
	mLock->lock();
	itr_t itr = mMap.find(name);
	if (itr == mMap.end())
	{
		u32 idx = mCrtId++;
		mMap[name] = idx;
		mLock->unlock();
		return idx;
	}

	mLock->unlock();
	OmnAlarm << "Name already registered: " << name << enderr;
	return itr->second;
}


u32
AosSName::getNameIdx(const OmnString &name)
{
	mLock->lock();
	itr_t itr = mMap.find(name);
	if (itr == mMap.end())
	{
		mLock->unlock();
		return 0;
	}

	mLock->unlock();
	return itr->second;
}

