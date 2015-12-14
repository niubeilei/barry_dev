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
#ifndef Aos_Bitmap_U64BmNodeLeaf_h
#define Aos_Bitmap_U64BmNodeLeaf_h

#include "Bitmap/U64BmNode.h"
#include "Bitmap/BitmapUtil.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <vector>
using namespace std;

class AosSimpleMutex;

class AosU64BmNodeLeaf : public AosU64BmNode
{
	OmnDefineRCObject;

private:
	AosSimpleMutex*		mLock;
	AosBitmapUtil::Mode	mMode;
	vector<u64>			mSubnodes;
	vector<u8>			mIndex;

public:
	AosU64BmNodeLeaf()
	:
	AosU64BmNode(true),
	mLock(OmnNew AosSimpleMutex()),
	mMode(AosBitmapUtil::ePacked),
	mSubnodes(0)
	{
	}
	
	~AosU64BmNodeLeaf()
	{
		mSubnodes.clear();
		mIndex.clear();
		OmnDelete mLock;
	}

	bool setDocid(const u16 docid);
	static AosU64BmNodeLeaf* getNode();
	static bool returnNode(AosU64BmNodeLeaf*);
};
#endif

