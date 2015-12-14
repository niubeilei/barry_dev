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
// 11/02/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DocServer_Ptrs_h
#define Omn_DocServer_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosDocSvrProc, AosDocSvrProcPtr)
OmnPtrDecl(AosDocBatchReader, AosDocBatchReaderPtr)
OmnPtrDecl(AosDocReader, AosDocReaderPtr)
OmnPtrDecl(AosDocDelete, AosDocDeletePtr)
OmnPtrDecl(AosDocBatchReaderReq, AosDocBatchReaderReqPtr)
OmnPtrDecl(AosReadWriteLock, AosReadWriteLockPtr)
OmnPtrDecl(AosRWLock, AosRWLockPtr)
OmnPtrDecl(AosRWLockReq, AosRWLockReqPtr)

#endif
