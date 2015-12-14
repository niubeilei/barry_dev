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
// 2013/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryEngine_Ptrs_h
#define Aos_QueryEngine_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosBitmapQueryIILInfo,			AosBitmapQueryIILInfoPtr);
OmnPtrDecl(AosBitmapQueryReq,				AosBitmapQueryReqPtr);
OmnPtrDecl(AosBitmapQueryEngine,			AosQueryEnginePtr);
OmnPtrDecl(AosBitmapQueryTerm,				AosBitmapQueryTermPtr);
OmnPtrDecl(AosBitmapQueryTermOr,			AosBitmapQueryTermOrPtr);
OmnPtrDecl(AosBitmapQueryTermAnd,			AosBitmapQueryTermAndPtr);
OmnPtrDecl(AosTransBitmapQuery,				AosTransBitmapQueryPtr);
OmnPtrDecl(AosTransBitmapQueryResp,			AosTransBitmapQueryRespPtr);
OmnPtrDecl(AosTransCalcQueryResults,		AosAosTransCalcQueryResultsPtr);
OmnPtrDecl(AosTransCalcQueryResultsResp,	AosTransCalcQueryResultsRespPtr);
OmnPtrDecl(AosTransBitmapQueryThrd,			AosTransBitmapQueryThrdPtr);
OmnPtrDecl(AosTransBitmapQueryReadThrd,		AosTransBitmapQueryReadThrdPtr);

#endif
