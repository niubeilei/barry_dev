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
#ifndef AOS_Util_LinkNode_h
#define AOS_Util_LinkNode_h

#include "Util/Ptrs.h"


class AosLinkNode : virtual public OmnRCObject 
{
private:
	AosLinkNodePtr mPrev;
	AosLinkNodePtr mNext;

public:
	AosLinkNode(){ }
	~AosLinkNode() {}

	AosLinkNodePtr getPrev() const {return mPrev;}
	AosLinkNodePtr getNext() const {return mNext;}
	void setPrev(const AosLinkNodePtr &p) {mPrev = p;}
	void setNext(const AosLinkNodePtr &n) {mNext = n;}

	inline void removeFromList()
	{
		// It assumes this tag is in a linked list. It removes itself
		// from the linked list.
		if (mNext) mNext->mPrev = mPrev;
		if (mPrev) mPrev->mNext = mNext;
	}
	
	inline bool insertToList(const AosLinkNodePtr &node)
	{
		// It inserts itself to the front of 'node'
		aos_assert_r(node, false);
		mPrev = node->mPrev;
		if (mPrev) mPrev->mNext = this;
		mNext = node;
		node->mPrev = this;
		return true;
	}

	inline void resetLinkNode()
	{
		mPrev = 0;
		mNext = 0;
	}
};
#endif
