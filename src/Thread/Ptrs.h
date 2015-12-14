////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_Ptrs_h
#define Omn_Thread_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosLockMonitor, AosLockMonitorPtr);
OmnPtrDecl(AosThreadRunner, AosThreadRunnerPtr);
OmnPtrDecl(AosThreadRunnerObj, AosThreadRunnerObjPtr);
OmnPtrDecl(AosThrdMsgQueue, AosThrdMsgQueuePtr);
OmnPtrDecl(OmnCondVar, OmnCondVarPtr);
OmnPtrDecl(OmnSem, OmnSemPtr);
OmnPtrDecl(OmnRwLock, OmnRwLockPtr);
OmnPtrDecl(OmnMutex, OmnMutexPtr);
OmnPtrDecl(OmnThread, OmnThreadPtr);
OmnPtrDecl(OmnThreadedObj, OmnThreadedObjPtr);
OmnPtrDecl(OmnThrdShellProc, OmnThrdShellProcPtr);
OmnPtrDecl(OmnThreadShell, OmnThreadShellPtr);
OmnPtrDecl(OmnCntlVar, OmnCntlVarPtr);
OmnPtrDecl(OmnThrdTrans, OmnThrdTransPtr);
OmnPtrDecl(AosThreadFunc, AosThreadFuncPtr);

// Ken Lee, 2013/04/12
OmnPtrDecl(OmnThreadPool, OmnThreadPoolPtr);
OmnPtrDecl(OmnThreadShellCaller, OmnThreadShellCallerPtr);

#endif
