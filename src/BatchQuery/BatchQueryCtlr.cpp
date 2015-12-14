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
//
// Modification History:
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BatchQuery/BatchQueryCtlr.h"

AosBatchQueryCtlr::AosBatchQueryCtlr()
:
mStatus(eActive)
{
}


AosBatchQueryCtlr::~AosBatchQueryCtlr()
{
}


bool 
AosBatchQueryCtlr::noResults()
{
	mStatus = eNoResults;
	return true;
}


void 
AosBatchQueryCtlr::setError(const char *file, const int line, const OmnString &errmsg)
{
	mErrmsg = errmsg;
	mStatus = eError;
}


