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
// 04/01/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_AmUtil_Ptrs_h
#define Omn_AmUtil_Ptrs_h

#include "Util/SPtr.h"

//#include "AmUtil/AmMsg.h"
//#include "AmUtil/AmBuff.h"

OmnPtrDecl(AosAmServer, AosAmServerPtr)
OmnPtrDecl(AosAmMsg, AosAmMsgPtr)
OmnPtrDecl(AosAmRcvBuff, AosAmRcvBuffPtr)
OmnPtrDecl(AosAmTrans, AosAmTransPtr)
OmnPtrDecl(AosAmExternalSvr, AosAmExternalSvrPtr)
OmnPtrDecl(AosAmExternalSvrMgr, AosAmExternalSvrMgrPtr)

#endif
