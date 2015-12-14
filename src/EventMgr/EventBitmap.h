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
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_EventMgr_EventBitmap_h
#define Omn_EventMgr_EventBitmap_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "EventMgr/EventIds.h"
#include "Util/String.h"

class AosEventBitmap
{
	u64			mBitmaps;
	static u64	smBitmap[64];

public:
	AosEventBitmap() 
	:
	mBitmaps(0)
	{
	}

	AosEventBitmap(const AosEventId::E event)
	:
	mBitmaps(0)
	{
		setEvent(event);
	}

	bool setEvent(const AosEventId::E event)
	{
		aos_assert_r(event >= 0 && event < AosEventId::eMax, false);
		mBitmaps |= smBitmap[event];
		return true;
	}

	bool checkEvent(const AosEventId::E event, bool reset = true)
	{
		aos_assert_r(event >= 0 && event < AosEventId::eMax, false);
		bool rslt = (mBitmaps & smBitmap[event])?true:false;
		if (reset)
		{
			mBitmaps &= ~smBitmap[event];
		}
		return rslt;
	}

	AosEventId::E nextEvent()
	{
		// It retrieves the next event in mBitmaps. After that, it resets 
		// the corresponding bit. 
		OmnNotImplementedYet;
		return AosEventId::eInvalid;
	}
};
#endif

#endif
