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
// 2014/04/02	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatServer_Ptrs_h
#define Aos_StatServer_Ptrs_h

#include "Util/SPtr.h"

//OmnPtrDecl(AosStatRcdCtnr, AosStatRcdCtnrPtr)
//OmnPtrDecl(AosStatistic, AosStatisticPtr)
OmnPtrDecl(AosStatCube, AosStatCubePtr)
OmnPtrDecl(AosStatKeyConn, AosStatKeyConnPtr)
OmnPtrDecl(AosStatKeyFile, AosStatKeyFilePtr)
//yang
OmnPtrDecl(AosStatKeyDistFile, AosStatKeyDistFilePtr)
#endif
