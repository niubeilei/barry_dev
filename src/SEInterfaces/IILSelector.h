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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_IILSelector_h
#define Aos_SEInterfaces_IILSelector_h

#include "IILUtil/IILEntrySelType.h"
#include "IILScanner/IILValueType.h"
#include "SEInterfaces/ValueSelObj.h"
#include "Util/ValueRslt.h"

struct AosIILSelector
{
	AosIILEntrySelType::E	mSelectorType;
	AosIILValueType::E		mValueType;
	int						mFieldIdx;
	int						mMemberIdx;
	bool					mCreateFlag;
	AosValueRslt			mDftValue;
	AosValueSelObjPtr		mValueSel;
	bool					mSetDocid;		// Chen Ding, MMMMM
	bool					mSetRundata;
	int						mSourceFieldIdx;
	OmnString				mSourceSeparator;

	bool isValid() const
	{
		if (!AosIILEntrySelType::isValid(mSelectorType)) return false;
		if (!AosIILValueType::isValid(mValueType)) return false;
		if (mFieldIdx < 0) return false;
		return true;
	}
};
#endif
