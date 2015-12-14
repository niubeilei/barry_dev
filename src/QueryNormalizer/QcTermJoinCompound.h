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
// 2015/02/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryNormalizer_QcTermJoinCompound_h
#define Aos_QueryNormalizer_QcTermJoinCompound_h

#include "QueryNormalizer/QcTerm.h"

class AosQcTermJoinCompound : public AosQcTerm
{
	OmnDefineRCObject;
public:
	enum Location
	{
		eInvalid,

		eUsedAsSelectedField,
		eUsedInFrom,
		eUsedInCond
	};

private:
	AosJqlStmtTablePtr	mTable;		// This is the table the field belongs to
	AosJqlSelectPtr 	mRootQuery;
	AosJqlSelectPtr		mParentQuery;
	AosJqlSelectPtr		mCrtQuery;
	AosJimoCallerPtr	mCaller;
	bool				mNormalized;
	int					mFieldIdx;
	Location			mHowBeingUsed;	

public:
	AosQcTermJoinCompound(const int version);
	~AosQcTermJoinCompound();
};

#endif

