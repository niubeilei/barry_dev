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
// 2013/03/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_IILCache_IILQueryReq_h
#define AOS_IILCache_IILQueryReq_h

#include "CloudCube/CubeMsgProc.h"
#include "Util/RCObject.h"
#include "Util/RCOBjImp.h"


class AosIILQueryReq : public AosCubeMsgProc
{
	OmnDefineRCObject;

public:
	struct Node
	{
		int 			level;
		int 			iil_level;
		u64				bitmap_id;
		vector<u64>		children;
	};

private:
	u64				mIILID;
	u64				mStartIILID;
	u64				mEndIILID;
	u64				mQueryId;
	AosIILIdx		mOrigStartPos;
	AosIILIdx		mCrtStartPos;
	AosIILIdx		mEndPos;
	vector<Node>	mNodes;
	u64				mTotalDocs;				// The sorting IIL size
	u64				mSortingIILTotalDocs;	// Total docs picked by sorting IIL
	bool			mQueryFinished;			// True if no more blocks for the query
	u64				mBlockStartPos;
	AosBuffPtr		mBitmapBuff;			// Bitmap for crt block
	AosBuffPtr		mNodelistBuff;			// Nodelist for crt block
	AosBuffPtr		mContextBuff;

	AosBitmapObjPtr	mFrontBitmap;
	AosBitmapObjPtr	mEndBitmap;

public:
	AosIILQueryReq(const AosRundataPtr &rdata, 
			const AosBuffPtr &buff);
	~AosIILQueryReq();

	// CubeMsgProc interface
	virtual bool procMsg(
					const AosRundataPtr &rdata,
					const AosBuffMsgPtr &msg);
	virtual AosCubeMsgProcPtr cloneCubeMsgProc();

	virtual bool isFirstBlock() const;
	virtual void setOrigStartPos(const AosIILIdx &idx);
	virtual void setEndPos(const AosIILIdx &idx);
	virtual bool isEmpty() const;
	virtual AosIILIdx getStartPos() const;
	virtual AosIILIdx getEndPos() const;
	virtual bool setPartialBitmap(const AosBitmapObjPtr &bitmap);
	virtual bool appendNode(const AosRundataPtr &rdata, 
							const int level, 
							const u64 &node_id);
	virtual bool addNumEntries(
							const AosRundataPtr &rdata, 
							const AosIILIdx &start_pos, 
							const AosIILIdx &end_pos);
	virtual bool addNumEntries(
							const AosRundataPtr &rdata, 
							const AosIILIdx &start_pos);
	virtual bool addNumEntries(const int num);
	virtual bool isReverse() const;
	virtual bool isFinished() const;
	virtual bool nextSubiil(const AosRundataPtr &rdata, 
							const int level);
	virtual void setTotalDocs(const u64 &d) {mTotalDocs = d;}
	virtual void setSortingIILTotalDocs(const u64 &d) {mSortingIILTotalDocs = d;}
	virtual void queryFinished() {mQueryFinished = true;}

private:

};
#endif
#endif

