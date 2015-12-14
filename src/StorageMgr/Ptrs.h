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
// 	Created: 10/08/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_StorageMgr_Ptrs_h
#define Omn_StorageMgr_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosVirtualFileSys, AosVirtualFileSysPtr)
OmnPtrDecl(AosDiskAlloc, AosDiskAllocPtr)
OmnPtrDecl(AosStorageDirInfo, AosStorageDirInfoPtr)
OmnPtrDecl(AosVirtualDirInfo, AosVirtualDirInfoPtr)
OmnPtrDecl(AosConfigMgr, AosConfigMgrPtr)
OmnPtrDecl(AosStorageMgr, AosStorageMgrPtr)
OmnPtrDecl(AosStorageMgrConn, AosStorageMgrConnPtr)
OmnPtrDecl(AosStoragePolicy, AosStoragePolicyPtr)
OmnPtrDecl(AosSpaceAlloc, AosSpaceAllocPtr)
OmnPtrDecl(AosAioRequest, AosAioRequestPtr)
OmnPtrDecl(DeleteFileReq, DeleteFileReqPtr)

#endif
