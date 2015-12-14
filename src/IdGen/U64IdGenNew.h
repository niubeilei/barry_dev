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
// 09/16/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IdGen_U64IdGenNew_h
#define AOS_IdGen_U64IdGenNew_h

#include "alarm_c/alarm.h"
#include "Base64/Base64.h"
#include "IdGen/Ptrs.h"
#include "IdGen/IdGenDef.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"



class AosU64IdGenNew : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftBlocksize = 5000000
	};

	OmnMutexPtr		mLock;
	u64				mDocid;
	u64				mCrtId;
	u64				mMaxId;
	u64				mNextId;
	u32				mBlockSize;
	AosXmlTagPtr	mDoc;

public:
	AosU64IdGenNew(const u64 &docid, const AosRundataPtr &rdata);
	~AosU64IdGenNew();

	bool	start(const AosRundataPtr &rdata);
	u64		getNextId(const AosRundataPtr &rdata);
	u64		getCrtId() const {return mCrtId;}
	u64		getMaxId() const {return mMaxId;}
	//bool	setCrtid(const u64 &id, const AosRundataPtr &rdata);	
	static AosU64IdGenNewPtr createNewIdGen(
				const u64 &docid, 
				const u64 &crtid,
				const u64 &maxid, 
				const u32 &blocksize, 
				const AosRundataPtr &rdata);
};

#endif
