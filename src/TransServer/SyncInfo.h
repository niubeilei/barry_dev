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
// 06/03/2011: Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransServer_SyncInfo_h
#define Aos_TransServer_SyncInfo_h

#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosSyncInfo: public OmnRCObject 
{
	OmnDefineRCObject;
	
private:
	u64		mSyncBeg;
	u64		mSyncEnd;
	u64		mCrtSyncSeq;
	bool	mSyncFinish;

public:
	AosSyncInfo()
	:
	mSyncBeg(0),
	mSyncEnd(0),
	mCrtSyncSeq(0),
	mSyncFinish(false)
	{
	};
	
	void	setSyncBeg(const u64 seq){ mSyncBeg = seq; };
	u64		getSyncBeg(){ return mSyncBeg; };
	
	void	setSyncEnd(const u64 seq){ mSyncEnd = seq; };
	u64		getSyncEnd(){ return mSyncEnd; };
	bool	isNoSyncEnd(){ return mSyncEnd == 0; };

	void	setCrtSyncSeq(const u64 seq){ mCrtSyncSeq = seq; };
	u64		getCrtSyncSeq(){ return mCrtSyncSeq; };

	void	setSyncFinish(){ mSyncFinish = true; };
	bool	isSyncFinish(){ return mSyncFinish; };

};
#endif
