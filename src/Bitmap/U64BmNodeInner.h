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
#ifndef Aos_Bitmap_U64BitmapInnerNode_h
#define Aos_Bitmap_U64BitmapInnerNode_h

#include "Bitmap/BitmapUtil.h"
#include "Bitmap/U64BmNode.h"
#include "Thread/Ptrs.h"
#include "Thread/SimpleMutex.h"
#include "Util/OmnNew.h"
#include <vector>
using namespace std;

class AosSimpleMutex;

class AosU64BmNodeInner : public AosU64BmNode
{
private:
	enum
	{
		eLinearThreshold = 30,
		eNumSubnodes = 1024
	};

	AosSimpleMutex*			mLock;
	AosBitmapUtil::Mode		mMode;
	vector<AosU64BmNode*>	mSubnodes;
	vector<u8>				mIndex;

	static u32 smTooManyForPacked;

public:
	AosU64BmNodeInner()
	:
	AosU64BmNode(false),
	mLock(OmnNew AosSimpleMutex()),
	mMode(AosBitmapUtil::ePacked),
	mSubnodes(0)
	{
	}
	
	~AosU64BmNodeInner();

	AosU64BmNode* setDocid(const u8 docid, const bool insert_leaf);

	static AosU64BmNodeInner *getNode();
	static bool returnNode(AosU64BmNodeInner *node);

private:
	AosU64BmNode* insertBeforeLocked(const u16 idx, const u8 value, const bool leaf);
	AosU64BmNode* insertAfterLocked(const u16 idx, const u8 value, const bool leaf);
};
#endif

