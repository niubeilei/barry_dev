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
#ifndef Aos_DataBlob_DataBlobType_h
#define Aos_DataBlob_DataBlobType_h

#include "Util/String.h"

#define AOSDATABLOB_INVALID					"invalid"
#define AOSDATABLOB_RECORD					"record"
#define AOSDATABLOB_VARIABLE				"variable"

class AosDataBlobType 
{
public:
	enum E
	{
		eInvalid,

		eRecord,
		eVariable,

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
		if (code == AOSDATABLOB_RECORD) return eRecord;
		return eInvalid;
	}
	
	static OmnString toString(const E code)
	{
		switch(code)
		{
		case eRecord:		return AOSDATABLOB_RECORD;
		default: break;
		}
		return AOSDATABLOB_INVALID;
	}
	static bool check();
	static bool addName(const OmnString &name, const E code);
};

#endif
