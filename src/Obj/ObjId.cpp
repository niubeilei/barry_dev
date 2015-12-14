////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjId.cpp
// Description:
//	An object ID consists of 8 bytes:
//		Byte 7 (highest):	Major
//		Byte 6:				Minor
//		Byte 5-0:			Sequence Number   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/ObjId.h"

#include "Alarm/Alarm.h"


OmnObjId
OmnObjId:: operator ++()
{
	// 
	// IMPORTANT: The caller should make sure the call is thread safe
	//
	if (mLow == 0xffffffff)
	{
		//
		// mLow overflows. 
		//
		mLow = 0;
		if ((mHigh & 0xffff) == 0xffff)
		{
			//
			// High also overflows
			//
			OmnWarn << "Object ID overfolw. High: " << mHigh << enderr;
			mHigh &= 0xffff0000;
			mLow = eStart;
		}
		else
		{
			// 
			// High not overflowed
			//
			mHigh++;
		}
	}
	else
	{
		// 
		// Low is not overflowed;
		//
		mLow++;
	}

	return *this;
}
