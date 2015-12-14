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
// 2015/01/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/SyncJobObj.h"


AosSyncJobObj::AosSyncJobObj(const int version)
:
AosJimo(AosJimoType::eSyncJob, version)
{
}


/*
AosSyncJobObjPtr 
AosSyncJobObj::createSyncJob(
	AosRundata *rdata, 
	const AosDatasetObjPtr &dataset)
{
	OmnNotImplementedYet;
	return 0;
}
*/
