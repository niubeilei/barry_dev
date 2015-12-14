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
// 02/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GicCodeGen_GicGenType_h
#define Aos_GicCodeGen_GicGenType_h

#include "Util/String.h"



class AosGicGenType
{
public:
	enum E
	{
		eInvalid, 

		eDataCol,

		eMax
	};

	static bool addName(const OmnString &name, const E code);
	static E toEnum(const OmnString &name);
	static OmnString toStr(const E code);
	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static bool init();
};
#endif

