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
#ifndef Aos_MySqlAdapter_MySqlFieldType_h
#define Aos_MySqlAdapter_MySqlFieldType_h 

#include "Util/String.h"

#define AOSMYSQLFIELD_INVALID			"invalid"
#define AOSMYSQLFIELD_TINYINT			"tinyint"
	

class AosMySqlFieldType
{
public:
	enum E
	{
		eInvalid,

		eFieldTinyInt,

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
	
	static E toEnum(const OmnString &code);
	static OmnString toString(const E code);
	static bool check();
	static bool addName(const OmnString &name, const E code);
};

#endif
