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
#ifndef Aos_QueryNormalizer_QueryCondAnd_h
#define Aos_QueryNormalizer_QueryCondAnd_h

#include "QueryNormalizer/QueryCond.h"



class AosQueryCondAnd : public AosQueryCond
{
private:
	vector<AosQueryCondPtr>	mConds;
	
public:
	AosQueryCondAnd();
	~AosQueryCondAnd();

	virtual bool normalizeSubqueries(
						AosRundata *rdata, 
						const AosQueryCallerPtr &caller);
	virtual bool isNormalizeSubqueryFinished() const;
	virtual bool normalizeSubqueryFinished(AosRundata *rdata);
	virtual bool normalizeSubqueryFailed(AosRundata *rdata);
};
#endif

