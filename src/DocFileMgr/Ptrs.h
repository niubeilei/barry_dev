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
#ifndef Omn_DocFileMgr_Ptrs_h
#define Omn_DocFileMgr_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosDocFileMgr, AosDocFileMgrPtr)
OmnPtrDecl(AosDiskBlock2, AosDiskBlock2Ptr)
OmnPtrDecl(AosDiskBlockMgr, AosDiskBlockMgrPtr)
OmnPtrDecl(AosFileGroupMgr, AosFileGroupMgrPtr)
OmnPtrDecl(AosStorageApp, AosStorageAppPtr)
OmnPtrDecl(AosBodyFile, AosBodyFilePtr)
OmnPtrDecl(AosSlabBlockFile, AosSlabBlockFilePtr)
OmnPtrDecl(AosSlabBlockMgr, AosSlabBlockMgrPtr)
OmnPtrDecl(AosReadOnlyBlock, AosReadOnlyBlockPtr)
OmnPtrDecl(AosDfmSaveCacher, AosDfmSaveCacherPtr)
//OmnPtrDecl(AosDfmDoc, AosDfmDocPtr)
//OmnPtrDecl(AosDfmDocNorm, AosDfmDocNormPtr)
OmnPtrDecl(AosDfmHeader, AosDfmHeaderPtr)
OmnPtrDecl(AosDfmBody, AosDfmBodyPtr)

OmnPtrDecl(AosDfmFile, AosDfmFilePtr)
OmnPtrDecl(AosDfmHeaderFile, AosDfmHeaderFilePtr)
OmnPtrDecl(AosDfmSlabFile, AosDfmSlabFilePtr)
OmnPtrDecl(AosDfmDiskFile, AosDfmDiskFilePtr)
OmnPtrDecl(AosDfmReadOnlyFile, AosDfmReadOnlyFilePtr)

OmnPtrDecl(AosDfmProc, AosDfmProcPtr)

OmnPtrDecl(AosRaidFileReq, AosRaidFileReqPtr)
OmnPtrDecl(AosRaidFile, AosRaidFilePtr)
OmnPtrDecl(AosRawFiles, AosRawFilesPtr)

OmnPtrDecl(AosDfmLog, AosDfmLogPtr)
OmnPtrDecl(AosDfmLogFile, AosDfmLogFilePtr)
OmnPtrDecl(AosDfmCaller, AosDfmCallerPtr)
OmnPtrDecl(AosDfmLogProcThrd, AosDfmLogProcThrdPtr)

#endif
