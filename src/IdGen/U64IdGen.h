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
// 10/04/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IdGen_U64IdGen_h
#define AOS_IdGen_U64IdGen_h

#include "alarm_c/alarm.h"
#include "Base64/Base64.h"
#include "IdGen/Ptrs.h"
#include "IdGen/IdGenDef.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/SyncServerReqCb.h"



class AosU64IdGen : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftBlocksize = 5000000
	};

	OmnMutexPtr		mLock;
	OmnString		mIdName;
	u64				mCrtId;
	u64				mMaxId;
	AosIdGenDefPtr	mDef;

public:
	AosU64IdGen(const AosXmlTagPtr &def);
	AosU64IdGen(const OmnString &idname);
	~AosU64IdGen();

	// AosSyncServerReqCb interface
	// virtual bool    syncServerCb(const AosSyncServerReqType, void *data);

	bool	start();
	u64		getNextId1();
	u64		getCrtid1() const {return mCrtId;}
	u64		getCrtMaxId() const {return mCrtId-1;}
	bool	setCrtid(const u64 &id);	
	bool	setBlocksize(const u32 &bsize) 
	{
		mLock->lock();
		bool rslt = mDef->setBlocksize(bsize);
		mLock->unlock();
		return rslt;
	}

	bool	getNextBlock()
	{
		mLock->lock();
		bool rslt = getNextBlockPriv();
		mLock->unlock();
		return rslt;
	}

	u64 loadId()
	{
		if (mCrtId > mMaxId) 
		{
			bool rslt = getNextBlock();
			aos_assert_r(rslt, 0);
		}
		return mCrtId;
	}

private:
	bool 	modifyField(const u32 offset, const OmnString &value);
	bool	getNextBlockPriv();
};

#endif
