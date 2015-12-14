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
// 11/24/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Torturer_Ptrs_h
#define Aos_Torturer_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosTorturer, AosTorturerPtr);
OmnPtrDecl(AosCommand, AosCommandPtr);
OmnPtrDecl(AosFuncCmd, AosFuncCmdPtr);
OmnPtrDecl(AosCliCmd, AosCliCmdPtr);
OmnPtrDecl(AosModule, AosModulePtr);
OmnPtrDecl(AosFuncModule, AosFuncModulePtr);
OmnPtrDecl(AosCliModule, AosCliModulePtr);
OmnPtrDecl(AosProduct, AosProductPtr);

#endif
