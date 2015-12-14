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
#ifndef Aos_QueryNormalizer_QcTermSubquery_h
#define Aos_QueryNormalizer_QcTermSubquery_h



class AosQcTermSubquery : public OmnRCObject
{
private:
	
public:
	AosQcTermSubquery();
	~AosQcTermSubquery();
};
#endif

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
#ifndef Aos_QueryNormalizer_QcTermSubquery_h
#define Aos_QueryNormalizer_QcTermSubquery_h

#include "QueryNormalizerJimos/QcTerm.h"


class AosQcTermSubquery : virtual public AosQcTerm
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
	AosQcTermSubquery(const int version);
	~AosQcTermSubquery();
};

#endif

