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
// 	Created: 10/15/2015 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_WordIDMgr_SmartArray_h
#define Omn_WordIDMgr_SmartArray_h 

#include "Funcs/GenFunc.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DfmUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/OmnNew.h"
#include "aosUtil/Types.h"

#include <stdio.h>


class AosSmartArray: public OmnRCObject
{
	enum
	{
		eDownExtendSize = 50,
	};
	OmnDefineRCObject;

private:
	u64*		mData;
	i64			mStart;
	i64			mLen;
	i64			mUpdateTime;
public:
	AosSmartArray();
	~AosSmartArray();
	AosSmartArray(const AosSmartArray &rhs);
	u64& operator [](const i64 &idx);
};

#endif

