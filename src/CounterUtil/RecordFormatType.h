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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_RecordFormatType_h
#define Aos_CounterUtil_RecordFormatType_h

#include "Util/String.h"

class AosRecordFormatType 
{
public:
	enum E
	{
		eInvalid,
		eOneDime,
		eTwoDime,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	
	static bool isValid(const OmnString &code)
	{
		return isValid(toEnum(code));
	}
	
	static E toEnum(const OmnString &code)
	{
		if (code == "onedime") return eOneDime;
		return eInvalid;
	}
	
	static OmnString toString(const E code)
	{
		switch(code)
		{
		case eOneDime: 		return "onedime";
		default: break;
		}
		return "";
	}
};

#endif
