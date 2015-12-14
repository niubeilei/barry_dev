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
#ifndef Aos_Compress_CompressType_h
#define Aos_Compress_CompressType_h

#include "Util/String.h"

#define AOSCOMPRESS_INVALID					"invalid"
#define AOSCOMPRESS_GZIP					"gzip"

class AosCompressType 
{
public:
	enum E
	{
		eInvalid,
		eGZip,

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
		if (code == AOSCOMPRESS_GZIP) return eGZip;
		return eInvalid;
	}
	
	static OmnString toString(const E code)
	{
		switch(code)
		{
		case eGZip: 		return AOSCOMPRESS_GZIP;
		default: break;
		}
		return AOSCOMPRESS_INVALID;
	}
	static bool check();
	static bool addName(const OmnString &name, const E code);
};

#endif
