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
// 2015/02/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryProcJimos_QueryFieldSubquery_h
#define Aos_QueryProcJimos_QueryFieldSubquery_h

#include "QueryProcJimos/QueryField.h"


class AosQueryFieldSubquery : virtual public AosQueryField
{
	OmnDefineRCObject;

private:
	AosJqlStmtTablePtr		mTable;		// This is the table the field belongs to
	AosJqlSelectPtr 		mRootQuery;
	AosJqlSelectPtr			mParentQuery;
	AosJqlSelectPtr			mCrtQuery;
	AosJimoCallerPtr		mCaller;
	bool					mNormalized;
	int						mFieldIdx;

public:
	AosQueryFieldSubquery(const int version);
	~AosQueryFieldSubquery();
};

#endif

