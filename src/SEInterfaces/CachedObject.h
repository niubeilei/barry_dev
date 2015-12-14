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
// 2013/10/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_CachedObject_h
#define Aos_SEInterfaces_CachedObject_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/Ptrs.h"

class AosCachedObjectNode;

class AosCachedObject : public OmnRCObject
{
private:
	u64					mCachedObjDocidDNO;
	u32					mCachedObjSiteidDNO;
	AosCachedObjectPtr 	mNextCachedObjDNO;
	AosCachedObjectPtr 	mPrevCachedObjDNO;

public:
	AosCachedObject();
	virtual ~AosCachedObject();

	AosCachedObjectPtr getPrevCachedObjDNO() const {return mPrevCachedObjDNO;}
	AosCachedObjectPtr getNextCachedObjDNO() const {return mNextCachedObjDNO;}
	void setPrevCachedObjDNO(const AosCachedObjectPtr &obj) {mPrevCachedObjDNO = obj;}
	void setNextCachedObjDNO(const AosCachedObjectPtr &obj) {mNextCachedObjDNO = obj;}
	u32 getCachedObjSiteid() const {return mCachedObjSiteidDNO;}
	u32 getCachedObjDocid() const {return mCachedObjDocidDNO;}
	void resetCachedObjLinks() 
	{
		mNextCachedObjDNO = 0;
		mPrevCachedObjDNO = 0;
	}
};
#endif



