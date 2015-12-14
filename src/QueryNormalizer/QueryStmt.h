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
// 2015/02/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryNormalizer_QueryStmt_h
#define Aos_QueryNormalizer_QueryStmt_h

#include "SEInterfaces/QueryStmtObj.h"

class AosQueryStmt : public AosQueryStmtObj,
{
	OmnDefineRCObject;

private:
	AosJqlSelectPtr				mOrigQuery;

	vector<AosContainerObjPtr>	mTables;
	vector<AosContainerObjPtr>	mOuterTables;
	vector<AosExprObjPtr>		mSelectedFields;
	vector<AosQueryCondObjPtr>	mJoinConds;
	vector<AosQueryCondObjPtr>	mConds;
	vector<AosExprObjPtr>		mOrderByFields;		
	vector<QueryCondObjPtr>		mHavingConds;		
	
public:
	AosQueryStmt();
	~AosQueryStmt();
};
#endif

