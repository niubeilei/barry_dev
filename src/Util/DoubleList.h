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
// 12/13/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_DoubleList_h
#define Aos_Util_DoubleList_h

#include "Util/Ptrs.h"
#include "Util/RCObject.h"


class AosDoubleList : public OmnRCObject
{
protected:
	AosDoubleListPtr	mDblListPrevEntry;
	AosDoubleListPtr	mDblListNextEntry;

public:
	AosDoubleList();
	virtual ~AosDoubleList();

	virtual AosDoubleListPtr nextEntry() const {return mDblListNextEntry;}
	virtual AosDoubleListPtr prevEntry() const {return mDblListPrevEntry;}
	virtual void resetPointers() {mDblListPrevEntry = 0; mDblListNextEntry = 0;}
	virtual void setPointers() {mDblListPrevEntry = this; mDblListNextEntry = this;}
	virtual bool moveToFront(const AosDoubleListPtr &header);
	virtual bool insertAt(const AosDoubleListPtr &header);
	virtual bool removeFromList();
};

#endif

