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
// 	Created: 01/05/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Query_Ptrs_h
#define Omn_Query_Ptrs_h

#include "Util/SPtr.h"


//OmnPtrDecl(AosQueryTerm, AosQueryTermPtr)
OmnPtrDecl(AosTermOr, AosTermOrPtr)
OmnPtrDecl(AosTermAnd, AosTermAndPtr)
OmnPtrDecl(AosTermArith, AosTermArithPtr)
//OmnPtrDecl(AosTermTag, AosTermTagPtr)
//OmnPtrDecl(AosQueryReq, AosQueryReqPtr)
//OmnPtrDecl(AosTermLog, AosTermLogPtr)
//OmnPtrDecl(AosCounterSingle, AosCounterSinglePtr)
//OmnPtrDecl(AosTermKeywords, AosTermKeywordsPtr);
OmnPtrDecl(AosUpdate, AosUpdatePtr)

#endif
