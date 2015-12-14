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
//   
//
// Modification History:
// 05/18/2010	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransUtil_Ptrs_h
#define Aos_TransUtil_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosTrans, AosTransPtr)
OmnPtrDecl(AosCubicTrans, AosCubicTransPtr)
OmnPtrDecl(AosTaskTrans, AosTaskTransPtr)
OmnPtrDecl(AosIILTrans, AosIILTransPtr)

OmnPtrDecl(AosTransBitmap, AosTransBitmapPtr)
OmnPtrDecl(AosRoundRobin, AosRoundRobinPtr)
OmnPtrDecl(AosIdRobin, AosIdRobinPtr)
OmnPtrDecl(AosIILIdRobin, AosIILIdRobinPtr)
OmnPtrDecl(AosTransDistributor, AosTransDistributorPtr)
//OmnPtrDecl(AosAsyncReqTrans, AosAsyncReqTransPtr)
OmnPtrDecl(AosAsyncRespCaller, AosAsyncRespCallerPtr)
OmnPtrDecl(AosBinaryDocScanner, AosBinaryDocScannerPtr)
OmnPtrDecl(AosBinaryDocTrans, AosBinaryDocTransPtr)
//OmnPtrDecl(AosAsyncReqCaller, AosAsyncReqCallerPtr)
OmnPtrDecl(AosTransProcThrd, AosTransProcThrdPtr)
OmnPtrDecl(AosAsyncRespCaller, AosAsyncRespCallerPtr)
#endif
