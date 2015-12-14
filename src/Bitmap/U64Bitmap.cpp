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
#include "Bitmap/U64Bitmap.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Bitmap/U64BmNodeRoot.h"
#include "Bitmap/U64BmNodeInner.h"
#include "Bitmap/U64BmNodeLeaf.h"
#include "Thread/SimpleMutex.h"
#include "Util/OmnNew.h"


AosU64Bitmap::AosU64Bitmap()
:
mLock(OmnNew AosSimpleMutex()),
mRootNode(AosU64BmNodeRoot::getNode())
{
}

AosU64Bitmap::~AosU64Bitmap()
{
	AosU64BmNodeRoot::returnNode(mRootNode);
	OmnDelete mLock;
	mRootNode = 0;
	mLock = 0;
}


bool
AosU64Bitmap::setDocid(const u64 &docid)
{
	AosU64BmNodeInner *crt_node = mRootNode->setDocid(docid);
	aos_assert_r(crt_node, false);

	AosU64BmNode* node = crt_node->setDocid(((u32)docid) >> 24);
	aos_assert_r(node, false);

	node = ((AosU64BmNodeInner*)node)->setDocid(((u32)docid) >> 16);
	aos_assert_r(node, false);

	aos_assert_r(node && node->isLeafNode(), false);
	return ((AosU64BmNodeLeaf*)(node))->setDocid(docid);
}


