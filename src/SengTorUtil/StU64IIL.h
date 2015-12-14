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
// 2014/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StU64IIL_h
#define AosSengTorUtil_StU64IIL_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SengTorUtil/Ptrs.h"
#include "SengTorUtil/StIIL.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosStU64IIL : public AosStIIL
{
	OmnDefineRCObject;

private:
	int					mMinLen;
	int					mMaxLen;
	//int					mIdx;
	AosBuffArrayVarPtr	mCacheKey;
	AosBuffArrayVarPtr	mCacheDocid;

	enum
	{
		eDftMinLen = 100,
		eDftMaxLen = 1000
	};

public:
	AosStU64IIL(const OmnString &name,
				const u64 iilid,
				const bool key_unique,
				const bool docid_unique,
				const bool stable);

	~AosStU64IIL();

	virtual bool pickEntry(
						u64 &key,
						u64 &docid,
						bool &key_unique,
						bool &docid_unique, 
						bool &added,
						AosRundata *rdata);

	virtual i64 getIILSize();
	virtual bool nextValue(i64 &idx, OmnString &key, u64 &docid);
};
#endif

