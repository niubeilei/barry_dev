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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataFormat_DataFieldType_h
#define Aos_DataFormat_DataFieldType_h


class AosDataFieldType 
{
public:
	enum E
	{
		eInvalid,

		eAttr,
		eText,
		eCDATA,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	
	static E toEnum(const OmnString &name)
	{
		if (name == "attr") return eAttr;
		if (name == "text") return eText;
		if (name == "cdata") return eCDATA;
		return eInvalid;
	}
};

#endif
