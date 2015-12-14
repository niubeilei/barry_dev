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
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataCacherType_h
#define Aos_SEInterfaces_DataCacherType_h

#include "Util/String.h"

#define AOSDATACACHER_SCANCACHER			"scancacher"
#define AOSDATACACHER_SIMPLE				"simple"
#define AOSDATACACHER_MULTI					"multi"
#define AOSDATACACHER_FILE					"cacherfile"
#define AOSDATACACHER_VIRTUAL_FILE			"cachervirtfile"
#define AOSDATACACHER_INVALID				"invalid"


class AosDataCacherType
{
public:
	enum E
	{
		eInvalid, 

		eSimple,
		eMulti,
		eScanCacher,
		eCasherFile,
		eCasherVirtualFile,

		eMax
	};


public:
	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &id);
	static OmnString toStr(const E code);
	static bool check();

private:
	// static bool addName(const OmnString &name, const E code);
};
#endif

