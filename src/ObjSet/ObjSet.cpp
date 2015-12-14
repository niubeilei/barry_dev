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
// 01/11/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "ObjSet/ObjSet.h"

#include "Event/Event.h"

AosObjSet::AosObjSet()
{
}


AosObjSet::~AosObjSet()
{
}


bool	
AosObjSet::start()
{
	AosEventListenerPtr thisPtr(this);
	std::list<AosEventEntry>::iterator itr;
	for (itr = mEventData.begin(); itr != mEventData.end(); itr++)
	{
		aos_register_event(thisPtr, itr->mEventId, itr->mData, itr->mLength);
	}

	return true;
}


bool	
AosObjSet::stop()
{
	AosEventListenerPtr thisPtr(this);
	std::list<AosEventEntry>::iterator itr;
	for (itr = mEventData.begin(); itr != mEventData.end(); itr++)
	{
		aos_unregister_event(thisPtr, itr->mEventId, itr->mData, itr->mLength);
	}

	return true;
}

