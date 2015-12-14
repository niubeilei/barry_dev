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
#ifndef Aos_QueryNormalizer_QcTerm_h
#define Aos_QueryNormalizer_QcTerm_h



class AosQcTerm : public OmnRCObject
{
private:
	
public:
	AosQcTerm();
	~AosQcTerm();

	virtual bool normalizeSubquery(
						AosRundata *rdata, 
						const AosQueryCallerPtr &caller) = 0;
	virtual bool isNormalizeSubqueryFinished() = 0;
	virtual bool normalizeSubqueryFinished() = 0;
	virtual bool normalizeSubqueryFailed() = 0;
};
#endif

