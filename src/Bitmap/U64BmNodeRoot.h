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
#ifndef Aos_Bitmap_U64BmNodeRoot_h
#define Aos_Bitmap_U64BmNodeRoot_h

#include "Bitmap/U64BmNode.h"
#include "Bitmap/BitmapUtil.h"
#include "Thread/Ptrs.h"
#include "Thread/SimpleMutex.h"
#include "Util/OmnNew.h"
#include <vector>
using namespace std;

#define AosU64BmRoot_Check(idx, node, vv) 	\
	if (mSubnodes[idx]->mValue == vv) 		\
	{										\
		node = mSubnodes[idx]->mNode;		\
		mLock->unlock();					\
		return node;						\
	}

class AosSimpleMutex;
class AosU64BmNode;
class AosU64BmNodeInner;

class AosU64BmNodeRoot : public AosU64BmNode
{
private:
	AosSimpleMutex *		mLock;
	AosBitmapUtil::Mode		mMode;
	vector<AosU64BmNode*>	mSubnodes;
	vector<u8>				mIndex;

public:
	AosU64BmNodeRoot()
	:
	AosU64BmNode(false),
	mLock(OmnNew AosSimpleMutex()),
	mMode(AosBitmapUtil::ePacked)
	{
	}
	
	~AosU64BmNodeRoot()
	{
		mLock->lock();
		for (u32 i=0; i<mSubnodes.size(); i++)
		{
			OmnDelete mSubnodes[i];
		}
		mSubnodes.clear();
		mLock->unlock();

		OmnDelete mLock;
		mLock = 0;
	}

	AosU64BmNodeInner *setDocid(const u32 docid);

	static AosU64BmNodeRoot *getNode();
	static bool returnNode(const AosU64BmNodeRoot *node);
};
#endif

