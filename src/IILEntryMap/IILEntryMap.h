////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILEntryMap_IILEntryMap_h
#define Aos_IILEntryMap_IILEntryMap_h 

#include "IILEntryMap/Ptrs.h"
#include "Thread/RwLock.h"
#include "Thread/Sem.h"
#include "Rundata/Rundata.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <map>


class AosIILEntryMap : virtual public OmnRCObject, 
					   public map<OmnString, OmnString>
{
	OmnDefineRCObject;

	OmnRwLockPtr	mLock;
	OmnSemPtr		mSem;
	OmnString		mIILName;
	bool			mNeedSplit;
	OmnString		mSep;
	bool			mNeedSwap;
	bool			mUseKeyAsValue;
	
	friend class AosIILEntryMapMgr;

public:
	AosIILEntryMap(
			const OmnString &iilname,
			const bool need_split,
			const OmnString &sep,
			const bool need_swap,
			const bool use_key_as_value)
	:
	mLock(OmnNew OmnRwLock()),
	mSem(OmnNew OmnSem(0)),
	mIILName(iilname),
	mNeedSplit(need_split),
	mSep(sep),
	mNeedSwap(need_swap),
	mUseKeyAsValue(use_key_as_value)
	{
	}
	~AosIILEntryMap()
	{
	}

	void readlock(){mLock->readlock();}
	void writelock(){mLock->writelock();}
	void unlock(){mLock->unlock();}

	void wait(){mSem->wait();}
	void post(){mSem->post();}

	OmnString	getIILName() const {return mIILName;}
};

typedef map<OmnString, OmnString>::iterator AosIILEntryMapItr;

#endif
