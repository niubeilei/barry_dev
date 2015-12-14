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
#ifndef Aos_QueryNormalizer_QueryCondOR_h
#ifndef Aos_QueryNormalizer_QueryCondOR_h

#include "QueryNormalizer/QueryCond.h"



class AosQueryCondOR : public AosQueryCond 
{
private:
	
public:
	AosQueryCondOR();
	~AosQueryCondOR();

	virtual bool normalizeSubqueries(
						AosRundata *rdata, 
						const AosQueryProcCallerPtr &caller);
	virtual bool isNormalizeSubqueryFinished() const;
	virtual bool normalizeSubqueryFinished(AosRundata *rdata);
	virtual bool normalizeSubqueryFailed(AosRundata *rdata);
};
#endif

