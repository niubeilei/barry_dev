////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbObjMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/DbObjMgr.h"

#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Obj/MgdDbObj.h"
#include "Obj/NotInDbObj.h"
#include "Statistics/Statistics.h"
#include "Util1/Time.h"



OmnDbObjMgr::OmnDbObjMgr()
:
mInvalidObjTTL(eDefaultInvalidObjTTL)
{
}


OmnDbObjMgr::~OmnDbObjMgr()
{
}


OmnMgdDbObjPtr 
OmnDbObjMgr::getObj(const OmnMgdDbObjPtr &objLookFor, const bool removeFlag)
{
	// 
	// Retrieve the object from the hash table
	//
	OmnExceptionBegin
	try
	{
		mLock.lock();
		OmnMgdDbObjPtr obj = mHash.get(objLookFor, removeFlag);
		if (obj)
		{
			if (obj->getClassId() != OmnClassId::eOmnNotInDbObj)
			{
				// 
				// It is a valid object. 
				// 
				obj->objAccessed();
				mLock.unlock();
				return obj;
			}

			// 
			// It is an unknown object. Check whether it has expired. 
			//
			if (OmnTime::getSecTick() - obj->getCreationTime() < (u32)mInvalidObjTTL)
			{
				// 
				// The object has not expired yet. Return 0.
				// Chen Ding, 2015/02/06
				// OmnStatDbUndefinedObjAccessed;
				mLock.unlock();
				return 0;
			}

			// 
			// The unknown object has expired. 
			// 
			mHash.get(objLookFor, true);
		}

		// 
		// The object is not in memory. Check whether it is in the database.
		//
		OmnRslt rslt = objLookFor->serializeFromDb();
		if (!rslt)
		{
			if (rslt.getErrId() == OmnErrId::eObjectNotFound)
			{
				// 
				// The object is not in the database. Add a NotInDbObj
				// for this object. 
				// 
				OmnMgdDbObjPtr notFound = objLookFor->getNotInDbObj();
				// OmnStatistics::mDbObjectNotFound++;
				mHash.add(notFound, true);
				mLock.unlock();
				return 0;
			}

			// 
			// Failed to access the database. This is a serious problem
			//
			mLock.unlock();
			OmnAlarm << rslt.getErrId()
				<< "Failed to access database: " << rslt.getErrmsg() << enderr;
			return 0;
		}

		// 
		// Found the object. Need to add the object into the hash table.
		//
		obj = objLookFor->clone();
		// OmnStatistics::mDbObjectCached++;
		mHash.add(obj, true);
		mLock.unlock();
		return obj;
	}

	catch (const OmnExcept &e)
	{
		if (e.procException() != OmnExcept::eContinueTry)
		{
			// 
			// Failed to retrieve the object and the exception does not know
			// what to do next. Simply return 0.
			//
			mLock.unlock();
			return 0;
		}

		OmnStatExceptionRetry;
	}
	OmnExceptionEnd;

	return 0;
}


bool 
OmnDbObjMgr::objCreated(const OmnMgdDbObjPtr &obj)
{
	// 
	// It adds the object to the hash table. If the object already exist
	// in the hash table, it is overriden. This is important that the object
	// could have been a non-exist object. 
	//
	mLock.lock();
	bool rslt = mHash.add(obj, true);
	mLock.unlock();
	return rslt;
}


bool 
OmnDbObjMgr::objDeleted(const OmnMgdDbObjPtr &obj)
{
	// 
	// The object has been deleted. It removes the object from the 
	// hash table.
	//
	OmnAssert(obj, "");
	mLock.lock();
	bool rslt = mHash.get(obj, true);
	mLock.unlock();
	return rslt;
}


bool 
OmnDbObjMgr::housekeeping()
{
	// 
	// This function will remove those objects that are too old.
	// This function is normally called by the housekeeping thread,
	// but it can also be called by anyone who wants to make more
	// memory.
	//
	mLock.lock();
	mHash.purge();
	mLock.unlock();
	return true;
}


bool
OmnDbObjMgr::clear()
{
	OmnTrace << "DbObjMgr is cleared: " << mName << endl;
	mLock.lock();
	mHash.clear();
	mLock.unlock();
	return true;
}
