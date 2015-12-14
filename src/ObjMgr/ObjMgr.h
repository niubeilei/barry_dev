////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ObjMgr_ObjMgr_h
#define Omn_ObjMgr_ObjMgr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


template <int size>
class OmnObjMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnHashTable<OmnObjPtr, size>	mHash; 

public:
	OmnObjMgr();
	virtual ~OmnObjMgr();

	bool	start();
	bool	stop();

	int		getObjCount() const {return mHash.getObjCount();}
	void	removeAll() {mHash.removeAll();}

	OmnObjPtr getObj(const OmnObjId &objId)
	{
		// 
		// Retrieve the object from the hash table
		//
		OmnObjPtr obj = mHash.get(objId);
		if (obj->getObjId() == objId)
		{
			return obj;
		}

		// 
		// The object is not in memory. Check whether 
		//
		obj->setObjId(objId);
		obj->serializeFromDb();
		if (obj->isGood())
		{
			// 
			// Retrieved the object. Add it to the hash table.
			//
			mLock.lock();
			mHash.add(obj);
			mLock.unlock();
		}

		// 
		// Not a good one. This is a non-exist object. Need to add the non-exist
		// object into the hash table. 
		//
		obj = OmnObj::getNonExistObj(objId);
		mHash.add(obj);

		return 0;
	}

	bool objCreated(const OmnObjPtr &obj)
	{
		// 
		// It adds the object to the hash table. If the object already exist
		// in the hash table, it is overriden. This is important that the object
		// could have been a non-exist object. 
		//
		mLock.lock();
		bool rslt = mHash.add(obj);
		mLock.unlock();
		return rslt;
	}

	bool objDeleted(const OmnObjPtr &obj)
	{
		// 
		// The object has been deleted. It removes the object from the 
		// hash table.
		//
		return mHash.remove(obj);
	}

	bool housekeeping()
	{
		// 
		// This function will remove those objects that are too old.
		// This function is normally called by the housekeeping thread,
		// but it can also be called by anyone who wants to clean the
		// manager. 
		//
		mLock.lock();
		mHash.purge();
		mLock.unlock();
		return true;
	}
};

#endif
