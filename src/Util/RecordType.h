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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataRecord directory
#ifndef Aos_DataFormat_RecordType_h
#define Aos_DataFormat_RecordType_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#define AOSRECORDTYPE_FIXBIN			"fixbin"
#define AOSRECORDTYPE_FIXSTR			"fixstr"
#define AOSRECORDTYPE_VARIABLE			"variable"

class AosRecordType
{
public:
	enum E
	{
		eInvalid,

		eFixedBinary,
		eFixedString,
		eVariable,

		eMax
	};

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	inline static E toEnum(const OmnString &name)
	{
		if (name == AOSRECORDTYPE_FIXBIN) return eFixedBinary;
		if (name == AOSRECORDTYPE_FIXSTR) return eFixedString;
		if (name == AOSRECORDTYPE_VARIABLE) return eVariable;
		return eInvalid;
	}
};
#endif
#endif
