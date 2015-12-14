////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 09/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/CappedHash.h"


AosKeyedNodePtr 
AosCappedHash::find(const OmnString &key)
{
	mLock->lock();
	aos_assert_rl(sanityCheck(), mLock, NULL);
	MapTypeItr_t itr = mMap.find(key);
	if (itr != mMap.end())
	{
		// This means that there are entries that match 'key'. 
		AosKeyedNodePtr node = itr->second;
		moveNode(node);
		mLock->unlock();
		aos_assert_rl(sanityCheck(), mLock, NULL);
		return node;
	}

	mLock->unlock();
	return 0;
} 


bool 
AosCappedHash::addData(const OmnString &key, const AosKeyedNodePtr &data)
{
	// This function adds 'data' into the map. 
	// If the data already exists, it replaces it.
	mLock->lock();
	aos_assert_rl(sanityCheck(), mLock, false);
	MapTypeItr_t itr = mMap.find(key);
	if (itr != mMap.end())
	{
		// This means that the data exists. Replace it.
		AosKeyedNodePtr node = itr->second; 
		moveNode(node, data);
		itr->second = data;
		aos_assert_rl(sanityCheck(), mLock, false);
		mLock->unlock();
		return true;
	}

	// The data does not exist. Add it.
	if(mNumNodes < eMaxNumDocs2Cache)
	{
		addNode(data);
		mNumNodes++;
	}
	else
	{
		AosKeyedNodePtr node = mHead->getPrev();
		mMap.erase(node->getNodeKey());
		moveNode(node, data);
	}
	mMap[key] = data;
	aos_assert_rl(sanityCheck(), mLock, false);
	mLock->unlock();
	return true;
}


bool 
AosCappedHash::erase(const OmnString &key)
{
	mLock->lock();
	aos_assert_rl(sanityCheck(), mLock, false);
	if (mNumNodes == 0)
	{
		aos_assert_rl(!mHead, mLock, false);
		mLock->unlock();
		return false;
	}
	
	aos_assert_rl(mHead, mLock, false);
	MapTypeItr_t itr = mMap.find(key);
	if (itr == mMap.end())
	{
		// Not found
		aos_assert_rl(sanityCheck(), mLock, false);
		mLock->unlock();
		return false;
	}
	
	AosKeyedNodePtr node = itr->second;
	if (mNumNodes == 1)
	{
		aos_assert_rl(mHead == node, mLock, false);
		mHead = 0;
		mNumNodes = 0;
		mMap.erase(key);
		aos_assert_rl(sanityCheck(), mLock, false);
		mLock->unlock();
		return true;
	}

	if (mHead == node)
	{
		mHead = node->getNext();
		aos_assert_rl(mHead, mLock, false);
		aos_assert_rl(mHead != node, mLock, false);
	}

	// Remove the node from the list
	node->removeFromList();
	mMap.erase(key);
	mNumNodes--;
	aos_assert_rl(sanityCheck(), mLock, false);
	mLock->unlock();
	return true;
}


bool
AosCappedHash::sanityCheck()
{
	aos_assert_r(mNumNodes >= 0, false);
	if (mNumNodes == 0)
	{
		aos_assert_r(!mHead, false);
		return true;
	}

	AosKeyedNodePtr crt = mHead;
	for (u32 i=0; i<mNumNodes; i++)
	{
		if (i == mNumNodes-1)
		{
			aos_assert_r(crt->getNext() == mHead, false);
			continue;
		}
		aos_assert_r(crt->getNext() != crt, false);
		aos_assert_r(crt->getNext() != mHead, false);
		crt = crt->getNext();
	}

	crt = mHead;
	for (u32 i=0; i<mNumNodes; i++)
	{
		if (i == mNumNodes-1)
		{
			aos_assert_r(crt->getPrev() == mHead, false);
			continue;
		}
		aos_assert_r(crt->getPrev() != crt, false);
		aos_assert_r(crt->getPrev() != mHead, false);
		crt = crt->getPrev();
	}

	return true;
}

