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
// 2015/02/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryAnalyzer_QcTermUnion_h
#define Aos_QueryAnalyzer_QcTermUnion_h

#include "QueryNormalizer/QcTerm.h"



class AosQcTermUnion : public AosQcTerm
{
private:
	
public:
	AosQcTermUnion();
	~AosQcTermUnion();

	virtual bool normalize(AosRundata *rdata);
	virtual bool isNormalizeFinished(AosRundata *rdata);
};
#endif

