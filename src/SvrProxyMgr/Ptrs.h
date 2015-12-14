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
#ifndef Aos_SvrProxyMgr_Ptrs_h
#define Aos_SvrProxyMgr_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosSvrProxy, AosSvrProxyPtr)
OmnPtrDecl(AosTransHandler, AosTransHandlerPtr)
OmnPtrDecl(AosResendEndHandler, AosResendEndHandlerPtr)
OmnPtrDecl(AosSvrInfo, AosSvrInfoPtr)
OmnPtrDecl(AosIpcSvrInfo, AosIpcSvrInfoPtr)
OmnPtrDecl(AosNetSvrInfo, AosNetSvrInfoPtr)
OmnPtrDecl(AosIpcCltInfo, AosIpcCltInfoPtr)
OmnPtrDecl(AosProcessMgr, AosProcessMgrPtr)
OmnPtrDecl(AosProcess, AosProcessPtr)
OmnPtrDecl(AosCubeProcess, AosCubeProcessPtr)
OmnPtrDecl(AosBkpCubeProcess, AosBkpCubeProcessPtr)
OmnPtrDecl(AosSockConn, AosSockConnPtr)
OmnPtrDecl(AosIpcSvr, AosIpcSvrPtr)
OmnPtrDecl(AosNetSvr, AosNetSvrPtr)
OmnPtrDecl(AosSvrMonitor, AosSvrMonitorPtr)
OmnPtrDecl(AosNetSvrInfo, AosNetSvrInfoPtr)
OmnPtrDecl(AosRecvEventHandler, AosRecvEventHandlerPtr)
OmnPtrDecl(AosResendCheckThrd, AosResendCheckThrdPtr)

#endif
