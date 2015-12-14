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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
//	All documents are stored in 'mDocFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mDocFilename' + 
//		'Idx_' + seqno
//
// Modification History:
// 12/12/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Snapshot/SnapshotMgr.h"


AosSnapshotMgr::AosSnapshotMgr()
{
}


AosSnapshotMgr::~AosSnapshotMgr()
{
}


bool
AosSnapshotMgr::createSnapshot(
		const u64 &dfmid, 
		const OmnString &name, 
		const bool sync, 
		const AosRundataPtr &rdata)
{
	// This function creates a snapshot. Snapshots can be created in sync mode 
	// and async mode. If it is sync mode, it will suspend the system briefly. 
	// After the system is suspended, it issues a request to all virtuals. 
	// If it is async mode, it simply sends a request to all virtuals. 
	OmnNotImplementedYet;
	return false;
}


bool
AosSnapshotMgr::cubeSuspended(
		const int &cubeid, 
		const AosRundataPtr &rdata)
{
	// A cube has suspended itself. It updates its status and then check whether
	// all cubes have responded. If not, it needs to wait. Otherwise, it will
	// issue a request to all cubes to create a snapshot.
	OmnNotImplementedYet;
	return false;
}


bool
AosSnapshotMgr::snapshotCreated(
		const int &cubeid, 
		const AosRundataPtr &rdata)
{
	// This function is called when a cube has successfully created a snapshot.
	// It updates the status. When all cubes created their snapshots, it finishes
	// the operation.
	OmnNotImplementedYet;
	return false;
}


bool
AosSnapshotMgr::removeSnapshot(
		const u64 &dfmid, 
		const OmnString &name, 
		const AosRundataPtr &rdata)
{
	// This function removes a snapshot. 
}
#endif
