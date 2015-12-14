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
// 06/20/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SvrProxyUtil_Ptrs_h
#define Aos_SvrProxyUtil_Ptrs_h

#include "Util/SPtr.h"

OmnPtrDecl(AosConnMsg, AosConnMsgPtr)
OmnPtrDecl(AosAppMsg, AosAppMsgPtr)
OmnPtrDecl(AosConnMsgAck, AosConnMsgAckPtr)
OmnPtrDecl(AosConnMsgWaitSeq, AosConnMsgWaitSeqPtr)
OmnPtrDecl(AosConnMsgGetWaitSeq, AosConnMsgGetWaitSeqPtr)

#endif
