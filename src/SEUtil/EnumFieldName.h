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
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_EnumFieldName_h
#define AOS_SEUtil_EnumFieldName_h

#include "Util/String.h"


struct AosEnumFieldName
{
	enum E
	{
		eInvalidFieldName,

		eErrorType,
		eDocid,
		eSnapshot,
		eUserid,
		eForturerName,			// Levi, 2014/11/13
		eRawFileId,				// White, 2014-11-25
		eMediaType,				// White, 2014-11-26

		eMaxFieldName
	};

	static OmnString toStr(const E code);
};
#endif
