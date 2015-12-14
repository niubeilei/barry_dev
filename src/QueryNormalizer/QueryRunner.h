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
#ifndef Aos_Query_QueryRunner_h
#define Aos_Query_QueryRunner_h



class AosQueryRunner : public AosQueryRunnerObj,
{
	OmnDefineRCObject;

private:
	AosJimoCallerPtr			mCaller;
	AosJqlSelectPtr				mOrigQuery;
	AosJqlSelectPtr				mParentQuery;
	AosJqlSelectPtr				mCrtQuery;
	vector<AosContainerObjPtr>	mTables;
	vector<AosContainerObjPtr>	mTablesAfterNormalize;
	
public:
	AosQueryRunner();
	~AosQueryRunner();

	virtual OmnString getQueryID() const;
};
#endif

