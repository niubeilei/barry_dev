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
#include "QueryAnalyzer/QcTermConstant.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQcTermConstant::AosQcTermConstant()
{
}


AosQcTermConstant::~AosQcTermConstant()
{
}


bool 
AosQcTermConstant::normalizeSubquery(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	return true;
}


bool 
AosQcTermConstant::isNormalizeSubqueryFinished()
{
	return true;
}


bool 
AosQcTermConstant::normalizeSubqueryFinished()
{
	OmnShouldNevetComeHere;
	return false;
}


bool 
AosQcTermConstant::normalizeSubqueryFailed()
{
	OmnShouldNeverComeHere;
	return false;
}

