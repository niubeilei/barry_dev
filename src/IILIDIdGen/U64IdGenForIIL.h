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
// 11/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IdGen_U64IdGenForIIL_h
#define AOS_IdGen_U64IdGenForIIL_h

#include "alarm_c/alarm.h"
#include "Base64/Base64.h"
#include "IdGen/Ptrs.h"
#include "IdGen/IdGenDef.h"
#include "IILIDIdGen/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosU64IdGenForIIL : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftBlocksize = 5000000
	};

	OmnMutexPtr		mLock;
	u32				mVirtualId;
	u64				mDocid;
	u64				mCrtId;
	u64				mNextId;
	int				mBlockSize;
	vector<bool>	mStatus;
	AosXmlTagPtr	mDoc;
	u64				mStartId;

public:
	AosU64IdGenForIIL(const u32 virtual_id, const u64 &docid, const AosRundataPtr &rdata);
	~AosU64IdGenForIIL();

	bool	start(const AosRundataPtr &rdata);
	u64		getNextId(const AosRundataPtr &rdata);
	u64		getCrtId() const {return mCrtId;}
	static AosU64IdGenForIILPtr createNewIdGen(
				const u32 virtual_id,	// Ketty 2012/07/30
				const u64 &docid, 
				const u64 &crtid,
				const u64 &maxid, 
				const u32 &blocksize, 
				const AosRundataPtr &rdata);
};

#endif
