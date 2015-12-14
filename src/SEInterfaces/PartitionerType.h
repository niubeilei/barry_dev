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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_PartitionerType_h
#define Aos_SEInterfaces_PartitionerType_h

#include "Util/String.h" 

#define AOSPARTITIONER_DOCID					"docid"

class AosPartitionerType
{
private:
	static bool		smInited;

public:
	enum E
	{
		eInvalid, 

		eDocid,

		eMax
	};

	static E toEnum(const OmnString &name);
	static OmnString  toString(const E type);
	static bool isValid(const E code) 
	{
		return code > eInvalid && code < eMax;
	}
	static bool addName(const OmnString &name, const E value, OmnString &errmsg);
	static void setInited() {smInited = true;}
	static bool check();
};

#endif

