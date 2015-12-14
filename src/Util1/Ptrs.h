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

#ifndef Omn_Util1_Ptrs_h
#define Omn_Util1_Ptrs_h

#include "Util/SPtr.h"

class OmnMemMgr;
class OmnMemPool;
class OmnTime;
class OmnTimer;
class OmnTimerObj;
class AosBandWidthMgr;

typedef OmnSPtr<OmnMemPool>		OmnMemPoolPtr;
typedef OmnSPtr<OmnTimerObj>	OmnTimerObjPtr;
typedef OmnSPtr<AosBandWidthMgr>	AosBandWidthMgrPtr;

OmnPtrDecl(AosMemory, AosMemoryPtr)
OmnPtrDecl(AosMemPool, AosMemPoolPtr)

#endif
