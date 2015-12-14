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
// 2015/01/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_QueryNormalizer_h
#define Aos_Query_QueryNormalizer_h



class AosQueryNormalizer : public AosQueryNormalizerObj,
{
	OmnDefineRCObject;

private:
	AosQueryCallerPtr			mCaller;
	AosJqlSelectPtr				mOrigQuery;
	AosJqlSelectPtr				mParentQuery;
	AosJqlSelectPtr				mCrtQuery;
	vector<AosContainerObjPtr>	mTables;
	
public:
	AosQueryNormalizer();
	~AosQueryNormalizer();

	virtual bool setQuery(AosRundata *rdata, const AosQueryStmtObjPtr &query);
	virtual bool runQuery(AosRundata *rdata, const AosJimoCallerPtr &caller);
	virtual bool isNormalizeFinished(AosRundata *rdata);
};
#endif

