////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2014/02/26 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmSnapShot_DfmSnapshotCreator_h
#define AOS_DfmSnapShot_DfmSnapshotCreator_h

#include "DfmUtil/Ptrs.h"
#include "UtilData/DfmDocType.h"
#include "DfmSnapshot/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DfmSnapshotObj.h"
#include "SEInterfaces/DfmSnapshotType.h"
#include "Util/String.h"

class AosDfmSnapshotCreator
{
	
public:
	static AosDfmSnapshotObjPtr createSnapshot(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotType::E snap_tp,
				const AosDfmSnapshotMgrPtr &snapshot_mgr, 
				const u64 snap_id,
				const AosDfmDocType::E doc_tp);

	static AosDfmSnapshotObjPtr createSnapshot(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotMgrPtr &snapshot_mgr, 
				const AosBuffPtr &buff,
				const AosDfmDocType::E doc_tp);

};

#endif
