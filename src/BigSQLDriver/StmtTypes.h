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
// 12/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BigSQLDriver_SqlStmtTypes_h
#define Aos_BigSQLDriver_SqlStmtTypes_h

#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosSqlStmtType
{
public:
	enum E
	{
		eMinInvalid, 

		eCreateTable,

		eInsert,

		eMaxInvalid
	};

	static E toEnum(const OmnString &name);
	static bool isValid(const E code) {return code>eMinInvalid && code<eMaxInvalid;}
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
	static OmnString toString(const E e);
	static bool check();
};
#endif

