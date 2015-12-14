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
// This hash map has a linked list that can age entries out. The caller 
// can set the maximum number of entries. Whenever an entry is accessed,
// it is moved to the head. When adding a new entry, if there are too
// many entries, the oldest one is removed.
//
// Modification History:
// 02/11/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Security_SessionMap_h
#define AOS_Security_SessionMap_h

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

class AosSessionMap
{
private:
	typedef hash_map<OmnString, AosSessionPtr, Omn_Str_hash, compare_str> HashMap_t;
	typedef hash_map<OmnString, AosSessionPtr, Omn_Str_hash, compare_str>::iterator HashMapItr_t;

	enum
	{
		eDftMaxEntries = 100000
	};

	OmnMutexPtr		mLock;
	HashMap_t		mMap;
	AosSessionPtr	mHead;
	u32				mMaxEntries;

public:
	AosSessionMap()
	:
	mLock(OmnNew OmnMutex()),
	mMaxEntries(eDftMaxEntries)
	{
	}

	~AosSessionMap()
	{
	}

	void setMaxEntries(const u32 m) {mMaxEntries = m;}

	AosSessionPtr find(const OmnString &key)
	{
		aos_assert_r(key != "", 0);
		mLock->lock();
		HashMapItr_t itr = mMap.find(key);	
		if (itr != mMap.end())
		{
			// Entry found. Move it to the head
			AosSessionPtr session = itr->second;
			if (mMap.size() != 1)
			{
				moveNode(session);
			}
			mLock->unlock();
			return session;
		}
		mLock->unlock();
		return 0;
	}

	bool insert(const OmnString &key, const AosSessionPtr &session)
	{
		aos_assert_r(key != "", false);
		mLock->lock();
		HashMapItr_t itr = mMap.find(key);
		aos_assert_rl(itr == mMap.end(), mLock, false);

		// The doc does not exist. Add it.
		mMap[key] = session;
		if (mMap.size() > mMaxEntries)
		{
			AosSessionPtr ss = mHead->getMapPrev();
			mMap.erase(ss->getSsid());
			removeNode(ss);
		}
		addNode(session);
		mLock->unlock();
		return true;
	}

	bool erase(const OmnString &key)
	{
		aos_assert_r(key != "", false);
		mLock->lock();
		if (mMap.size() == 0)
		{
			aos_assert_rl(!mHead, mLock, false);
			mLock->unlock();
			return true;
		}

		aos_assert_rl(mHead, mLock, false);
		HashMapItr_t itr = mMap.find(key);
		if (itr == mMap.end())
		{
			mLock->unlock();
			return true;
		}

		AosSessionPtr session = itr->second;
		if (mMap.size() == 1)
		{
			aos_assert_rl(mHead == session, mLock, false);
			mHead = 0;
			mMap.erase(key);
			mLock->unlock();
			return true;
		}

		if (mHead == session)
		{
			mHead = session->getMapNext();
			aos_assert_rl(mHead, mLock, false);
			aos_assert_rl(mHead != session, mLock, false);
		}

		// Remove the node from the list
		removeNode(session);
		mMap.erase(key);
		mLock->unlock();
		return true;
	}

private:
	bool moveNode(const AosSessionPtr &node)
	{
		if (mHead == node) return true;	
		node->getMapPrev()->setMapNext(node->getMapNext());
		node->getMapNext()->setMapPrev(node->getMapPrev());
	
		node->setMapPrev(mHead->getMapPrev());
		mHead->getMapPrev()->setMapNext(node);
	
		node->setMapNext(mHead);
		mHead->setMapPrev(node);
	
		mHead = node;
		return true;
	}

	bool addNode(const AosSessionPtr &node)
	{
		// It adds the node 'node' to the front of the list. 
		if (!mHead)
		{
			aos_assert_r(mMap.size() == 1, false);
			mHead = node;
			mHead->setMapNext(node);
			mHead->setMapPrev(node);
			return true;
		}

		node->setMapPrev(mHead->getMapPrev());
		node->setMapNext(mHead);
		node->getMapPrev()->setMapNext(node);
		node->getMapNext()->setMapPrev(node);
		mHead = node;
		return true;
	}

	bool removeNode(const AosSessionPtr &node)
	{
		aos_assert_r(mMap.size() > 0, false);
		aos_assert_r(mHead, false);
		aos_assert_r(mHead != node, false);

		aos_assert_r(node->getMapPrev(), false);
		aos_assert_r(node->getMapNext(), false);
		node->getMapPrev()->setMapNext(node->getMapNext());
		node->getMapNext()->setMapPrev(node->getMapPrev());
		node->setMapPrev(0);
		node->setMapNext(0);
		return true;
	}
};
#endif

