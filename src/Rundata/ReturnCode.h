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
// 2013/11/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ReturnCode_ReturnCode_h
#define Aos_ReturnCode_ReturnCode_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"

struct AosReturnCode
{
public:
	enum E
	{
		eInvalid,

		eSuccess,
		eFailed,
		eMissingUserName,
		eMissingPassword,
		eMissingDatabase,
		eMissingApiKey,
		eUserNotDefined,
		eDatabaseNotDefined,
		ePasswordMismatch,
		eInvalidSession,
		eAccessDenied,
		eUnrecogDocType,
		eSyntaxError,
		eQuotaExceeded,
		eNotImplement,
		eInvalidDocid,
		eDocNotFound,
		eInternalError,
		eMissingDocids,
		eInvalidObjid,
		eMissingObjids,
		eInvalidIndexType,
		eNotDeleTable,
		eDocLocked,
		eDatabaseNotSpecied,
		eTooManyTables,
		eMissingAttr,
		eEmptyTable,
		eTableNameDuplicate,

		eFinished,
		eMax
	};
	
	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

};
#endif

