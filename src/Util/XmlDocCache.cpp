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
// 09/03/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/XmlDocCache.h"



bool 
AosXmlDocCache::getData(const OmnString &key, AosXmlTagPtr &data)
{
	mLock->lock();
	aos_assert_rl(sanityCheck(), mLock, false);
	std::map<OmnString, Node *>::iterator itr = mMap.find(key);
	if (itr != mMap.end())
	{
		// This means that there are entries that match 'key'. 
		Node* node = itr->second;
		if (mNumNodes != 1)
		{
			// Move the doc to the front of the list. This will 
			// ensure that the doc will not be removed from 
			// the cache in the near future.
			moveNode(node);
		}

		mLock->unlock();
		data = node->mData;
		aos_assert_rl(sanityCheck(), mLock, false);
		return true;
	}

	mLock->unlock();
	return false;
} 


bool 
AosXmlDocCache::addData(const OmnString &key, const AosXmlTagPtr &data)
{
	// This function adds 'data' into the map. 
	// If the data already exists, it replaces it.
	mLock->lock();
	aos_assert_rl(sanityCheck(), mLock, false);
	std::map<OmnString, Node*>::iterator itr = mMap.find(key);
	if (itr != mMap.end())
	{
		// This means that the data exists. Replace it.
		Node* node = itr->second; 
		if (mNumNodes != 1)
		{
			// Move the node to the front of the list. This will 
			// ensure that the node will not be removed from 
			// the cache in the near future.
			moveNode(node);
		}
		node->mData = data;
		aos_assert_rl(sanityCheck(), mLock, false);
		mLock->unlock();
		return true;
	}

	// The data does not exist. Add it.
	Node* node;
	if(mNumNodes < eMaxNumDocs2Cache)
	{
		node = getNode(key, data);
		addNode(node);
		mNumNodes++;
	}
	else
	{
		node = mHead->mPrev;
		mMap.erase(node->mKey);
		moveNode(node);
		node->mKey = key;
		node->mData = data;
	}
	mMap[key] = node;
	aos_assert_rl(sanityCheck(), mLock, false);
	mLock->unlock();
	return true;
}


bool 
AosXmlDocCache::deleteData(const OmnString &key)
{
	mLock->lock();
	aos_assert_rl(sanityCheck(), mLock, false);
	if (mNumNodes == 0)
	{
		aos_assert_rl(!mHead, mLock, false);
		mLock->unlock();
//		OmnAlarm << "Failed deleting: " << key << enderr;
		return false;
	}
	
	aos_assert_rl(mHead, mLock, false);
	std::map<OmnString, Node*>::iterator itr = mMap.find(key);
	if (itr == mMap.end())
	{
		// Not found
		aos_assert_rl(sanityCheck(), mLock, false);
		mLock->unlock();
		return false;
	}
	
	Node* node = itr->second;
	if (mNumNodes == 1)
	{
		aos_assert_rl(mHead == node, mLock, false);
		mHead = 0;
		mNumNodes = 0;
		returnNode(node);
		mMap.erase(key);
		aos_assert_rl(sanityCheck(), mLock, false);
		mLock->unlock();
		return true;
	}

	if (mHead == node)
	{
		mHead = node->mNext;
		aos_assert_rl(mHead, mLock, false);
		aos_assert_rl(mHead != node, mLock, false);
	}

	// Remove the node from the list
	node->mPrev->mNext = node->mNext;
	node->mNext->mPrev = node->mPrev;
	mMap.erase(key);
	returnNode(node);
	mNumNodes--;
	aos_assert_rl(sanityCheck(), mLock, false);
	mLock->unlock();
	return true;
}


bool
AosXmlDocCache::sanityCheck()
{
	aos_assert_r(mNumNodes >= 0, false);
	if (mNumNodes == 0)
	{
		aos_assert_r(!mHead, false);
		return true;
	}

	Node *crt = mHead;
	for (u32 i=0; i<mNumNodes; i++)
	{
		if (i == mNumNodes-1)
		{
			aos_assert_r(crt->mNext == mHead, false);
			continue;
		}
		aos_assert_r(crt->mNext != crt, false);
		aos_assert_r(crt->mNext != mHead, false);
		crt = crt->mNext;
	}

	crt = mHead;
	for (u32 i=0; i<mNumNodes; i++)
	{
		if (i == mNumNodes-1)
		{
			aos_assert_r(crt->mPrev == mHead, false);
			continue;
		}
		aos_assert_r(crt->mPrev != crt, false);
		aos_assert_r(crt->mPrev != mHead, false);
		crt = crt->mPrev;
	}

	return true;
}

