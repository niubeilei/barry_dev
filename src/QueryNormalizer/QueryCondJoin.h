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
#ifndef Aos_QueryProc_QcTermJoin_h
#define Aos_QueryProc_QcTermJoin_h



class AosQcTermJoin : public OmnRCObject
{
private:
	
public:
	AosQcTermJoin();
	~AosQcTermJoin();

	virtual bool normalizeSubqueries(
			AosRundata *rdata, 
			const AosQueryProcCallerPtr &caller);
};
#endif

