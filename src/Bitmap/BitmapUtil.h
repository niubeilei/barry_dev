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
#ifndef Aos_Bitmap_BitmapUtil_h
#define Aos_Bitmap_BitmapUtil_h


#define AosU64BmCheckNode(idx, node, vv)	\
	if (mSubnodes[idx]->mValue == vv) 		\
	{										\
		node = mSubnodes[idx]->mNode;		\
		mLock->unlock();					\
		return node;						\
	}

#define AosU64BmSetLeaf(idx, value)			\
	if (mIndex[idx]
class AosBitmapUtil
{
public:
	enum Mode
	{
		ePacked,
		eFull
	};
};
#endif

