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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSource_DataSourceType_h
#define Aos_DataSource_DataSourceType_h

#include "Util/String.h"

#define AOSDATASOURCE_LOCAL_CACHE_FILE		"loccachefile"
#define AOSDATASOURCE_INVALID				"invalid"


class AosDataSourceType
{
public:
	enum E
	{
		eInvalid, 

		eLocalCacheFile,

		eMax
	};

private:
	static OmnString		smNames[eMax];

public:
	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &id);
	static OmnString toStr(const E code);
	static bool check();
	static bool addName(const E code, const OmnString &name);
};
#endif

