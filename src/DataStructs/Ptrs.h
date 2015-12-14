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
// 04/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStructs_Ptrs_h
#define Aos_DataStructs_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosStructProc, AosStructProcPtr)
OmnPtrDecl(AosStatIdIDGen, AosStatIdIDGenPtr)
OmnPtrDecl(AosVector2D, AosVector2DPtr)
OmnPtrDecl(AosDistBlockMgr, AosDistBlockMgrPtr)
OmnPtrDecl(AosStatIdMgr, AosStatIdMgrPtr)
OmnPtrDecl(AosShuffleStatId, AosShuffleStatIdPtr)
OmnPtrDecl(AosRemoteShuffleStatId, AosRemoteShuffleStatIdPtr)
OmnPtrDecl(AosStatReaderRequest, AosStatReaderRequestPtr)
OmnPtrDecl(AosStatBatchReader, AosStatBatchReaderPtr)
OmnPtrDecl(AosVectorFile, AosVectorFilePtr)
OmnPtrDecl(AosVectorKeyFile, AosVectorKeyFilePtr)

#endif
