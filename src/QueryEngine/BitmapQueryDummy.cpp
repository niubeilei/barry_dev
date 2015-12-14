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
// 2014/04/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryEngine/BitmapQueryDummy.h"

AosBitmapQueryDummy::AosBitmapQueryDummy(const int ver)
:
AosBitmapQueryTermObj(ver)
{
}


AosBitmapQueryDummy::~AosBitmapQueryDummy()
{
}


AosQueryReqObjPtr 
AosBitmapQueryDummy::getQueryReq() const
{
	return 0;
}


bool 		
AosBitmapQueryDummy::runQuery(const AosRundataPtr &rdata)
{
	return false;
}


void 		
AosBitmapQueryDummy::clear()
{
}


bool		
AosBitmapQueryDummy::reset()
{
	return false;
}


bool		
AosBitmapQueryDummy::moveTo(const u64 &startidx, const AosRundataPtr &rdata)
{
	return false;
}


OmnString 	
AosBitmapQueryDummy::getBitmapQueryTermType() const
{
	return "";
}


OmnString 	
AosBitmapQueryDummy::getBitmapQueryTermInfo() const
{
	return "";
}


u64		  	
AosBitmapQueryDummy::getBitmapQueryTermId() const
{
	return 0;
}

