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
#include "PigLatin/LatinJoin.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"


AosLatinJoin::AosLatinJoin()
:
AosSdocAction(AOSACTTYPE_LATINJOIN, AosActionType::eLatinJoin, true)
{
}


AosLatinJoin::AosLatinJoin(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AosActionType::eLatinJoin)
{
	configJoin(def, rdata);
}


bool
AosLatinJoin::configJoin(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


AosLatinJoin::~AosLatinJoin()
{
}


bool	
AosLatinJoin::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosLatinJoin::run(
		const AosTaskPtr &task, 
		const AosTaskDataPtr &def, 
		const AosRundataPtr &rundata)
{
	OmnNotImplementedYet;
	return false;
}
