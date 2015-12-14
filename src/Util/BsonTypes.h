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
// Modification History:
// 2014/01/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_BSONTypes_h
#define Snt_Util_BSONTypes_h

class AosBsonTypes
{
public:
	enum E
	{
		eInvalid 		= 0x00, 

		eDouble 		= 0x01, 
		eUTF8Str 		= 0x02,
		eEmbeddedDoc 	= 0x03,
		eArray			= 0x04,
		eBinary			= 0x05,
		eUndefined		= 0x06,
		eObjectId		= 0x07,
		eBool			= 0x08,
		eUTCDatetime	= 0x09,
		eNull			= 0x0a,
		eRegularExpr	= 0x0b,
		eDBPointer		= 0x0c,
		eJavaScript		= 0x0d,
		eSymbol			= 0x0e,
		eJavaScriptW	= 0x0f,
		eInt32			= 0x10,
		eTimestamp		= 0x11,
		eInt64			= 0x12,

		eMaxEntry
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMaxEntry;
	}
};

#endif
