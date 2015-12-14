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
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ThreadShellRunners_Ptrs_h
#define Aos_ThreadShellRunners_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosDistrBlobToBucketsRunner, AosDistrBlobToBucketsRunnerPtr)
OmnPtrDecl(AosActionRunnerListener, AosActionRunnerListenerPtr)
OmnPtrDecl(AosCacherActionsRunner, AosCacherActionsRunnerPtr)
OmnPtrDecl(AosActionsRunner, AosActionsRunnerPtr);

#endif
