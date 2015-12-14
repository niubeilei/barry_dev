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
// 09/04/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_UtilHash_HashedObjType_h
#define AOS_UtilHash_HashedObjType_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilHash/Ptrs.h"

class AosHashedObjType
{
public:
	enum E
	{
		eInvalid,

		eU64,

		eMax
	};
};
#endif
