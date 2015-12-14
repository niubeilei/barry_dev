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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BatchQuery_BatchQuery_h
#define Aos_BatchQuery_BatchQuery_h

#include "BatchQuery/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosBatchQuery : public OmnRCObject
{
private:
	AosBatchQueryCtlrPtr	mCtlr;
	AosBitmapObjPtr	mBitmap;
	vector<u64>				mIILIDs;
	u32 					mSize;
	int						mStartIdx;
	int						mEndIdx;

public:
	AosBatchQuery(
			const AosBatchQueryCtlrPtr &ctlr, 
			const AosBitmapObjPtr &bitmap,
			const u32 size,
			const AosRundataPtr &rdata);
	~AosBatchQuery();

	inline bool addIILIDUnlocked(const u64 &iilid)
	{
		// This is not thread safe!!!
		mIILIDs.push_back(iilid);
		return (mIILIDs.size() < mSize);
	}	

	inline void setStartIdx(const int startidx)
	{
		mStartIdx = startidx;
	}

	inline void setEndIdx(const int endidx)
	{
		mEndIdx = endidx;
	}

	bool process(const AosRundataPtr &rdata);
};
#endif

