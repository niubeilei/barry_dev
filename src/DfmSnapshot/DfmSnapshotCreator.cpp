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
// 2014/02/26 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
//
#include "DfmSnapshot/DfmSnapshotCreator.h"

#include "DfmSnapshot/BigDataDfmSnapshot.h"


AosDfmSnapshotObjPtr
AosDfmSnapshotCreator::createSnapshot(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotType::E snap_tp,
		const AosDfmSnapshotMgrPtr &snapshot_mgr, 
		const u64 snap_id,
		const AosDfmDocType::E doc_tp)
{
	switch(snap_tp)
	{
	case AosDfmSnapshotType::eBigData:
		return AosBigDataDfmSnapshot::createSnapshotStatic(
				rdata, snapshot_mgr, snap_id, doc_tp);
	
	default:
		break;
	}
	
	OmnShouldNeverComeHere;
	return 0;
}


AosDfmSnapshotObjPtr
AosDfmSnapshotCreator::createSnapshot(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotMgrPtr &snapshot_mgr, 
		const AosBuffPtr &buff,
		const AosDfmDocType::E doc_tp)
{
	AosDfmSnapshotType::E snap_tp = (AosDfmSnapshotType::E)buff->getU32(0); 
	switch(snap_tp)
	{
	case AosDfmSnapshotType::eBigData:
		return AosBigDataDfmSnapshot::createSnapshotStatic(
				rdata, snapshot_mgr, buff, doc_tp);
	
	default:
		break;
	}
	
	OmnShouldNeverComeHere;
	return 0;
}



