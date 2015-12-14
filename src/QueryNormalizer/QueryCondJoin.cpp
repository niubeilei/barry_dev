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
// A Join term is in the form:
// 	expr operator expr
// where both expressions are simple expressions (that is, they involve
// only one field) but they are from different tables.
//
// Modification History:
// 2015/02/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/QcTermJoin.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQcTermJoin::AosQcTermJoin()
{
}


AosQcTermJoin::~AosQcTermJoin()
{
}


bool 
AosQcTermJoin::normalizeSubqueries(
		AosRundata *rdata, 
		const AosQueryProcCallerPtr &caller)
{
	return true;
}

