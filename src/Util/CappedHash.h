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
#ifndef AOS_Util_CappedHash_h
#define AOS_Util_CappedHash_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/KeyedNode.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <map>
using namespace std;

class AosCappedHash : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxNumDocs2Cache = 3
	};

private:
	typedef std::map<OmnString, AosKeyedNodePtr> MapType_t;
	typedef std::map<OmnString, AosKeyedNodePtr>::iterator MapTypeItr_t;
	OmnMutexPtr		mLock;
	MapType_t		mMap;
	AosKeyedNodePtr	mHead;
	u32				mNumNodes;

public:
	AosCappedHash()
	:
	mLock(OmnNew OmnMutex()),
	mHead(0),
	mNumNodes(0)
	{
	}
	
	~AosCappedHash()
	{
	}

	u32		getNumNodes() const {return mMap.size();};

	AosKeyedNodePtr find(const OmnString &key);
	bool addData(const OmnString &key, const AosKeyedNodePtr &data);
	bool erase(const OmnString &key);

	bool moveNode(const AosKeyedNodePtr &node)
	{
		aos_assert_r(sanityCheck(), false);
		aos_assert_r(mHead, false);

		if (mNumNodes == 1)
		{
			aos_assert_r(mHead == node, false);
			return true;
		}

		if (mHead == node) 
		{
			return true;
		}
		
		node->removeFromList();
		node->insertToList(mHead);	
	
		mHead = node;
		aos_assert_r(sanityCheck(), false);
		return true;
	}

	bool moveNode(const AosKeyedNodePtr &oldnode, const AosKeyedNodePtr &newnode)
	{
		aos_assert_r(sanityCheck(), false);
		aos_assert_r(mHead, false);

		if (mNumNodes == 1)
		{
			aos_assert_r(mHead == oldnode, false);
			oldnode->resetKeyedNode();
			mHead = newnode;
			mHead->setPrev(newnode);
			mHead->setNext(newnode);
			return true;
		}

		if (mHead == oldnode) 
		{
			newnode->setPrev(oldnode->getPrev());
			newnode->setNext(oldnode->getNext());
			oldnode->resetKeyedNode();
			mHead = newnode;
			return true;
		}
		
		oldnode->removeFromList();
		newnode->insertToList(mHead);	
	
		mHead = newnode;
		aos_assert_r(sanityCheck(), false);
		return true;
	}

	bool addNode(const AosKeyedNodePtr &node)
	{
		// It adds the node 'node' to the front of the list. 
		// aos_assert_r(sanityCheck(), false);
		aos_assert_r(node, false);
		if(mNumNodes == 0)
		{
			// This is the first node to add:
			aos_assert_r(!mHead, false);
			mHead = node;
			mHead->setNext(node);
			mHead->setPrev(node);
		}
		else
		{
			node->insertToList(mHead);
			mHead = node;
		}
		// aos_assert_r(sanityCheck(), false);
		return true;
	}

	bool sanityCheck();
};
#endif
