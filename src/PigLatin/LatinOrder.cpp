////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/02/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "PigLatin/LatinOrder.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"


AosLatinOrder::AosLatinOrder()
:
AosSdocAction(AOSACTTYPE_LATINORDER, AosActionType::eLatinOrder, true)
{
}


AosLatinOrder::AosLatinOrder(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AosActionType::eLatinOrder)
{
	configOrder(def, rdata);
}


bool
AosLatinOrder::configOrder(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


AosLatinOrder::~AosLatinOrder()
{
}


bool	
AosLatinOrder::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosLatinOrder::run(
		const AosTaskPtr &task, 
		const AosTaskDataPtr &def, 
		const AosRundataPtr &rundata)
{
	OmnNotImplementedYet;
	return false;
}

