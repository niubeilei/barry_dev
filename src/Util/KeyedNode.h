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
#ifndef AOS_Util_KeyedNode_h
#define AOS_Util_KeyedNode_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Ptrs.h"


class AosKeyedNode : virtual public OmnRCObject 
{
protected:
	OmnString		mNodeKey;
	AosKeyedNodePtr mPrev;
	AosKeyedNodePtr mNext;

public:
	AosKeyedNode(){ }
	~AosKeyedNode() {}

	AosKeyedNodePtr getPrev() const {return mPrev;}
	AosKeyedNodePtr getNext() const {return mNext;}
	OmnString getNodeKey() const {return mNodeKey;}
	void setNodeKey(const OmnString &k) {mNodeKey = k;}
	void setPrev(const AosKeyedNodePtr &p) {mPrev = p;}
	void setNext(const AosKeyedNodePtr &n) {mNext = n;}

	inline void removeFromList()
	{
		// It assumes this tag is in a linked list. It removes itself
		// from the linked list.
		if (mNext) mNext->mPrev = mPrev;
		if (mPrev) mPrev->mNext = mNext;
		mPrev = 0;
		mNext = 0;
	}
	
	inline bool insertToList(const AosKeyedNodePtr &node)
	{
		// It inserts itself to the front of 'node'
		aos_assert_r(node, false);
		mPrev = node->mPrev;
		if (mPrev) mPrev->mNext = this;
		mNext = node;
		node->mPrev = this;
		return true;
	}

	inline void resetKeyedNode()
	{
		mPrev = 0;
		mNext = 0;
	}
};
#endif
