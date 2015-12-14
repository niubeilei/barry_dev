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
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_Ptrs_h
#define AOS_SeLogSvr_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosSeLog, AosSeLogPtr)
OmnPtrDecl(AosSeLogEntry, AosSeLogEntryPtr)
OmnPtrDecl(AosLogAging, AosLogAgingPtr)
OmnPtrDecl(AosLogCapAging, AosLogCapAgingPtr)
OmnPtrDecl(AosLogRcdAging, AosLogRcdAgingPtr)
OmnPtrDecl(AosSeqnoFnameEntry, AosSeqnoFnameEntryPtr)
OmnPtrDecl(AosSeLogEntryNorm, AosSeLogEntryNormPtr)
OmnPtrDecl(AosSeLogEntryVersion, AosSeLogEntryVersionPtr)
OmnPtrDecl(AosMdlLogSvr, AosMdlLogSvrPtr)
OmnPtrDecl(AosPhyLogSvr, AosPhyLogSvrPtr)

#endif
