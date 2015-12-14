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
// 11/30/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Event_EventEntry_h
#define Aos_Event_EventEntry_h

#include "aosUtil/Memory.h"
#include "Event/Ptrs.h"
#include "Event/EventId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


struct AosEventEntry : virtual public OmnRCObject
{
	OmnDefineRCObject;

	AosEventListenerPtr	mListener;
	AosEventId::E  		mEventId;
	void			   *mData;
	u32					mLength;

	AosEventEntry()
		:
	mData(0),
	mLength(0)
	{
	}

	AosEventEntry(
			  const AosEventListenerPtr &listener,
			  const AosEventId::E eventId, 
			  void *data, 
			  const u32 length
			  )
		:
	mListener(listener),
	mEventId(eventId),
	mLength(length)
	{
		// 
		// If length <= 4, the data is assigned directly. 
		// Otherwise, it will allocate a piece of memory for it.
		//
		if (length <= 4)
		{
			mData = data;
		}
		else
		{
			mData = aos_malloc(length);
			memcpy(mData, data, length);
		}
	}

	~AosEventEntry()
	{
		if (mLength > 4) aos_free(mData);
	}

	u32		getHashKey() const {return (u32)mData + (u32)mEventId;}
	bool	isSameObj(const AosEventEntryPtr &rhs)
		    {return mEventId == rhs->mEventId &&
					mLength == rhs->mLength &&
					(mLength <= 4 && mData == rhs->mData ||
					 mLength > 4 && memcmp(mData, rhs->mData, mLength));}
};

#endif

