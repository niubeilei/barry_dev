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
// 10/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataTypes_Value_h
#define Aos_DataTypes_Value_h

class AosValue
{
public:
	enum E
	{
		eInvalid,

		eSignedInt,
		eUnsignedInt,
		eString,
		eBoolean,

		eMax
	};

	static E toEnum(const OmnString &name);
	static OmnString toStr(const E code);
	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

public:

};
#endif
