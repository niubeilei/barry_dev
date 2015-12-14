////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 02/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IdGen_U64IdGenWithIIL_h
#define AOS_IdGen_U64IdGenWithIIL_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosU64IdGenWithIIL : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString	mIdName;
	u64			mIILID;
	u64			mInitValue;
	int			mBlockSize;
	u64			mStartId;
	u64			mEndId;

public:
	AosU64IdGenWithIIL(
		const AosXmlTagPtr &def,
		const OmnString &tagname,
		const OmnString &idName,
		const u64 &iilid_prefix);
	~AosU64IdGenWithIIL();

	u64 nextId();
	u64 nextId(const u32 siteid, const AosRundataPtr &rdata);
};

#endif
