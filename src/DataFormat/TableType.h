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
#ifndef Aos_DataFormat_TableType_h
#define Aos_DataFormat_TableType_h

#include "DataFormat/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#define AOSTABLETYPE_FIXED			"fixed"
#define AOSTABLETYPE_VARIABLE		"variable"

class AosTableType
{
public:
	enum E
	{
		eInvalid,

		eFixed,
		eVariable,

		eMax
	};

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	inline static E toEnum(const OmnString &name)
	{
		if (name == AOSTABLETYPE_FIXED) return eFixed;
		if (name == AOSTABLETYPE_VARIABLE) return eVariable;
		return eInvalid;
	}
};
#endif

