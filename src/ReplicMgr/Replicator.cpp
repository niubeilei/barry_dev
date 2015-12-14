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
// This is a module used in the frontend.
//	
// Modification History:
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Replicator/Replicator.h"

#include "API/AosApiG.h"

static u32 sgModuleId = 0;

AosReplicatorMgr::replicateData(
		AosRundata *rdata, 
		const u64 docid,
		const u64 snapshot,
		const int replication_id,
		const int compressoin_id,
		const int encryption_id,
		AosBuff *data, 
		AosReplicCaller *caller)
:
mDocid(docid),
mSnapshot(snapshot),
mReplicationID(replication_id),
mCompressionID(compression_id),
mEncryptionID(encryption_id),
mCaller(caller)
{
	mReplicatorID = getReplicatorID();
}


AosReplicator::~AosReplicator()
{
}


bool
AosReplicator::start(AosRundata *rdata)
{
	// 1. Send requests to all the involved cubes.
	mCubes = getCubes(rdata);
	AosBuffPtr request = composeRequest(rdata);
	AosBuff *request_raw = request.getPtr();
	for (u32 i=0; i<mCubes.size(); i++)
	{
		sendRequest(rdata, mCubes[i], request_raw);
	}

	// 2. Check whether it finishes the call
	if (callFinished(rdata))
	{
		mCaller->replicationFinished(rdata, mReplicatorID);
	}
	return true;
}

#endif
