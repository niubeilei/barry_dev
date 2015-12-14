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
// 01/20/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_UtilHash_AgedHashMapStr_h
#define AOS_UtilHash_AgedHashMapStr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include <hash_map>
using namespace std;

template<T, u32 max_entries>
class AosAgedHashMap : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	struct Entry
	{
		T			mObj;
		OmnString	mKey;
		Entry  	   *mPrev;
		Entry  	   *mNext;
		Entry(const OmnString &key, const T &obj)
		:
		mObj(obj),
		mKey(key),
		mPrev(0),
		mNext(0)
		{
		}
	};

	typedef hasp_map<OmnString, Entry*, Omn_Str_hash, compare_str>	HashMap_t;
	typedef hasp_map<OmnString, Entry*, Omn_Str_hash, compare_str>::iterator HashMapItr_t;

	HashMap_t	mMap;
	T			mHead;
	u32			mMaxEntries;

public:
	AosAgedHashMapStr()
	:
	mMaxEntries(max_entries)
	{
	}

	~AosAgedHashMapStr()
	{
		HashMapItr_t itr = mMap.begin();
		for (; itr != mMap.end(); itr++)
		{
			OmnDelete itr->second;
		}
	}

	T getObj(const OmnString &key, const AosRundataPtr &rdata)
	{
		aos_assert_r(key != "", 0);
		mLock->lock();
		HashMapItr_t itr = mMap.find(key);
		if (itr != mMap.end())
		{
			// Entry found. Move it to the head
			Entry *node = itr->second;
			if (mMap.size() != 1)
			{
				moveNode(node);
			}
			mLock->unlock();
			return node->mObj;
		}
		mLock->unlock();
		return 0;
	}

	bool addDoc(const OmnString &key, const T &obj)
	{
		aos_assert_r(key != "", false);
		mLock->lock();
		HashMapItr_t itr = mMap.find(key);
		if (itr != mMap.end())
		{
			// This means that the doc exists. Replace it.
			Entry *node = itr->second; 
			if (mMap.size() != 1)
			{
				moveNode(node);
			}
			node->mObj = obj;
			mLock->unlock();
			return true;
		}

		// The doc does not exist. Add it.
		if (mMap.size() < mMaxEntries)
		{
			Entry *node = OmnNew Entry(key, obj);
			addNode(node);
		}
		else
		{
			node = mHead->mPrev;
			mMap.erase(node->mKey);
			moveNode(node);
			node->mObj = obj;
			node->mKey = key;
		}
		mMap[key] = node;
		mLock->unlock();
		return true;
	}

	bool deleteDoc(const OmnString &key)
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

		Entry *node = itr->second;
		if (mMap.size() == 1)
		{
			aos_assert_rl(mHead == node, mLock, false);
			mHead = 0;
			OmnDelete node;
			node = 0;
			mMap.erase(key);
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
		OmnDelete node;
		node = 0;
		mLock->unlock();
		return true;
	}

	bool moveNode(Entry *node)
	{
		if (mHead == node) return true;	
		node->mPrev->mNext = node->mNext;
		node->mNext->mPrev = node->mPrev;
	
		node->mPrev = mHead->mPrev;
		mHead->mPrev->mNext = node;
	
		node->mNext = mHead;
		mHead->mPrev = node;
	
		mHead = node;
		return true;
	}

	bool addNode(Entry *node)
	{
		// It adds the node 'node' to the front of the list. 
		if (mMap.size() == 0)
		{
			aos_assert_r(!mHead, false);
			mHead = node;
			mHead->mNext = node;
			mHead->mPrev = node;
		}
		else
		{
			node->mPrev = mHead->mPrev;
			node->mNext = mHead;
			node->mPrev->mNext = node;
			node->mNext->mPrev = node;
			mHead = node;
		}
		return true;
	}
};
#endif

