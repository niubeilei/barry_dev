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
#ifndef AOS_Util_XmlDocCache_h
#define AOS_Util_XmlDocCache_h

#include "Thread/Mutex.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"
#include <map>
using namespace std;

class AosXmlDocCache : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxNumDocs2Cache = 3
	};

	struct Node
	{
		OmnString		mKey;
		AosXmlTagPtr	mData;
		Node *			mPrev;
		Node *			mNext;

		Node()
		:
		mPrev(0),
		mNext(0)
		{
		}

		Node(const OmnString &key, const AosXmlTagPtr &data)
		:
		mKey(key),
		mData(data),
		mPrev(0),
		mNext(0)
		{
		}
	};

private:
	OmnMutexPtr					mLock;
	std::map<OmnString, Node*>	mMap;
	Node*						mHead;
	u32							mNumNodes;

public:
	AosXmlDocCache()
	:
	mLock(OmnNew OmnMutex()),
	mHead(0),
	mNumNodes(0)
	{
	}
	
	~AosXmlDocCache()
	{
		std::map<OmnString, Node*>::iterator itr = mMap.begin();
		for(; itr!=mMap.end(); ++itr)
		{
			delete itr->second;
		}
	}

	u32		getNumNodes() const {return mMap.size();};

	bool getData(const OmnString &key, AosXmlTagPtr &data);
	bool addData(const OmnString &key, const AosXmlTagPtr &data);
	bool deleteData(const OmnString &key);

	bool returnNode(Node *node)
	{
		OmnDelete node;
		node = 0;
		return true;
	}

	bool moveNode(Node* node)
	{
		aos_assert_r(sanityCheck(), false);
		if (mHead == node) return true;

		node->mPrev->mNext = node->mNext;
		node->mNext->mPrev = node->mPrev;
	
		node->mPrev = mHead->mPrev;
		mHead->mPrev->mNext = node;
	
		node->mNext = mHead;
		mHead->mPrev = node;
	
		mHead = node;
		aos_assert_r(sanityCheck(), false);
		return true;
	}

	bool addNode(Node* node)
	{
		// It adds the node 'node' to the front of the list. 
		// aos_assert_r(sanityCheck(), false);
		aos_assert_r(node, false);
		if(mNumNodes == 0)
		{
			// This is the first node to add:
			aos_assert_r(!mHead, false);
			mHead = node;
			mHead->mNext = node;
			mHead->mPrev = node;
		}
		else
		{
			// Insert it to the front:
			// node next to mHead next
			// node prev to mHead
			// mHead next's prev to node
			// mHead next to node

			node->mPrev = mHead->mPrev;
			node->mNext = mHead;
			node->mPrev->mNext = node;
			node->mNext->mPrev = node;
			mHead = node;
		}
		// aos_assert_r(sanityCheck(), false);
		return true;
	}

	Node *getNode(const OmnString &key, const AosXmlTagPtr &data)
	{
		return OmnNew Node(key, data);
	}

	bool sanityCheck();
};
#endif
