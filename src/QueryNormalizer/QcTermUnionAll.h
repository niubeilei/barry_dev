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
// 2015/02/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryAnalyzer_QcTermUnionAll_h
#define Aos_QueryAnalyzer_QcTermUnionAll_h

#include "QueryNormalizer/QcTerm.h"



class AosQcTermUnionAll : public AosQcTerm
{
	OmnDefineRCObject;

private:
	
public:
	AosQcTermUnionAll();
	~AosQcTermUnionAll();

	virtual bool normalize(AosRundata *rdata);
	virtual bool isNormalizeFinished(AosRundata *rdata);
};
#endif

