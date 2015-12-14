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
// 06/28/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILUtil_IILLogType_h
#define Aos_IILUtil_IILLogType_h

#include "Util/String.h"

struct AosIILLogType
{
	enum E 
	{
		eInvalid = 0,
		eHitAddDoc,
		eHitRemoveDoc,

		eStrAddValueDoc,
		eStrModifyValueDoc,
		eStrRemoveValueDoc,

		eU64AddValueDoc,
		eU64ModifyValueDoc,
		eU64RemoveValueDoc,
		eU64IncrementDocid,
		eU64ModifyDocid,
		eU64IncrementCounter,
		eStrIncrementCounter,
		eU64AppendManualOrder,
		eU64MoveManualOrder,
		eU64SwapManualOrder,
		eU64RemoveManualOrder,
		eU64UpdateDoc,
		eU64UpdateDoc1,
		eU64UpdateDoc3,
		eU64UpdateDoc4,
		eU64UpdateKeyedValue,
		eU64SetDocid,
		eU64SetValueDoc,

		eStrIncrementDocid,

		eMax
	};

	static E toEnum(const OmnString &name)
	{
		return eInvalid;
	}

	static bool isValid(const E code) {return code>eInvalid && code<eMax;}
};

#endif

