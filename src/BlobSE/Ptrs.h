////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014-12-19 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RawSE_Ptrs_H_
#define Aos_RawSE_Ptrs_H_

#include "Util/SPtr.h"

OmnPtrDecl(AosHeaderCache,			AosHeaderCachePtr)
//OmnPtrDecl(AosHeaderChangeLog,		AosHeaderChangeLogPtr)
OmnPtrDecl(AosBodyCache,			AosBodyCachePtr)
OmnPtrDecl(AosBlobSE,				AosBlobSEPtr)
OmnPtrDecl(AosHeaderFileMgr,		AosHeaderFileMgrPtr)
//OmnPtrDecl(AosDeletionLog,			AosDeletionLogPtr)
OmnPtrDecl(AosBlobHeader,			AosBlobHeaderPtr)
OmnPtrDecl(AosBlobSESnapshotMgr,	AosBlobSESnapshotMgrPtr)
OmnPtrDecl(AosBlobSESnapshot,		AosBlobSESnapshotPtr)
OmnPtrDecl(AosDirtyHeaderCache,		AosDirtyHeaderCachePtr)
OmnPtrDecl(AosOldHeaderCache,		AosOldHeaderCachePtr)
OmnPtrDecl(AosCompaction,			AosCompactionPtr)
OmnPtrDecl(AosOprIDCache,			AosOprIDCachePtr)
OmnPtrDecl(AosBlobSEAPI,			AosBlobSEAPIPtr)
OmnPtrDecl(AosBlobSEReqEntry,		AosBlobSEReqEntryPtr)
OmnPtrDecl(AosHeaderBuffAndFileMgr,	AosHeaderBuffAndFileMgrPtr)
//OmnPtrDecl(AosBucketMgr,			AosBucketMgrPtr)
//OmnPtrDecl(AosBucket,				AosBucketPtr)
OmnPtrDecl(AosDiskRecovery,			AosDiskRecoveryPtr)
OmnPtrDecl(AosMarkerMgr,			AosMarkerMgrPtr)
OmnPtrDecl(AosMarker,				AosMarkerPtr)

#endif /* Aos_RawSE_Ptrs_H_ */
