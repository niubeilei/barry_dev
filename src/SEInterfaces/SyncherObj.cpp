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
// 2015/01/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/SyncherObj.h"


AosSyncherObj::AosSyncherObj(const int version)
:
AosJimo(AosJimoType::eSyncher, version)
{
}


//AosSyncherObjPtr 
//AosSyncherObj::createSyncher(
//	AosRundata *rdata, 
//	const OmnString &syncher_name)
//{
//	OmnNotImplementedYet;
//	return 0;
//}
//

AosJimoPtr 
AosSyncherObj::cloneJimo() const
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosSyncherObj::serializeFromBuff(const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}
