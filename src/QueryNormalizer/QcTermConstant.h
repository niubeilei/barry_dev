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
#ifndef Aos_QueryNormalizer_QcTermConstant_h
#define Aos_QueryNormalizer_QcTermConstant_h

#include "QueryAnalyzer/QcTerm.h"


class AosQcTermConstant : public AosQcTerm
{
private:
	
public:
	AosQcTermConstant();
	~AosQcTermConstant();

	virtual bool normalizeSubquery(
						AosRundata *rdata, 
						const AosQueryCallerPtr &caller);
	virtual bool isNormalizeSubqueryFinished();
	virtual bool normalizeSubqueryFinished();
	virtual bool normalizeSubqueryFailed();
};
#endif

