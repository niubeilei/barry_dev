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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStructs_StatImRslt_h
#define AOS_DataStructs_StatImRslt_h

#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "StatUtil/StatRecord.h"

class AosStatImRslt : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	AosVector2DRecord * 	mRecord;
	u64						mRootStatId;
	u64						mExtDistBlockDocid;
	u64						mStartIdx;
	u32						mTimeCondIndex;
public:
	AosStatImRslt(AosVector2DRecord * record,
			const u64 root_stat_id,
			const u64 &ext_dist_block_docid,
			const u64 &start_idx,
			const u32 &idx)
	:
	mRecord(record),
	mRootStatId(root_stat_id),
	mExtDistBlockDocid(ext_dist_block_docid),
	mStartIdx(start_idx),
	mTimeCondIndex(idx)
	{
	}

	AosStatImRslt(AosVector2DRecord * record,
			const u64 root_stat_id,
			const u64 &ext_dist_block_docid,
			const u64 &start_idx)
	:
	mRecord(record),
	mRootStatId(root_stat_id),
	mExtDistBlockDocid(ext_dist_block_docid),
	mStartIdx(start_idx),
	mTimeCondIndex(0)
	{
	}

	AosStatImRslt(){};
	~AosStatImRslt(){};

	inline bool operator < (const AosStatImRslt &lhs) const
	{
		return lhs.getExtDistBlockDocid() > mExtDistBlockDocid;
	}

	u64 getExtDistBlockDocid() const {return mExtDistBlockDocid;}
	u64 getExtStartIdx() const {return mStartIdx;}
	u32 getTimeCondIndex() const {return mTimeCondIndex;}
	AosVector2DRecord * getRecord() {return mRecord;}
	u64 getRootStatId() {return mRootStatId;}
};
#endif
