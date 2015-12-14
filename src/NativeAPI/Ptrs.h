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
#ifndef Aos_NativeAPI_Ptrs_h
#define Aos_NativeAPI_Ptrs_h

#include "NativeAPI/Util/SPtr.h"

OmnPtrDecl(OmnGroupComm, OmnGroupCommPtr);
OmnPtrDecl(OmnTcpCltGrp, OmnTcpCltGrpPtr);
OmnPtrDecl(OmnTcpListener, OmnTcpListenerPtr);
OmnPtrDecl(OmnTcpMsgReader, OmnTcpMsgReaderPtr);
OmnPtrDecl(OmnCommGroup, OmnCommGroupPtr);
OmnPtrDecl(OmnConnBuff, OmnConnBuffPtr);
OmnPtrDecl(OmnGroupRRobin, OmnGroupRRobinPtr);
OmnPtrDecl(OmnCommListener, OmnCommListenerPtr);
OmnPtrDecl(OmnCommGroupMgr, OmnCommGroupMgrPtr);
OmnPtrDecl(OmnCommGroup, OmnCommGroupPtr);
OmnPtrDecl(OmnCommMgr, OmnCommMgrPtr);
OmnPtrDecl(OmnTcp, OmnTcpPtr);
OmnPtrDecl(OmnTcpServer, OmnTcpServerPtr);
OmnPtrDecl(OmnTcpServerEpoll, OmnTcpServerEpollPtr);
OmnPtrDecl(OmnTcpCommSvr, OmnTcpCommSvrPtr);
OmnPtrDecl(OmnTcpCommClt, OmnTcpCommCltPtr);
OmnPtrDecl(OmnStreamRequester, OmnStreamRequesterPtr);
OmnPtrDecl(OmnStreamSendTrans, OmnStreamSendTransPtr);
OmnPtrDecl(OmnStreamRecvTrans, OmnStreamRecvTransPtr);
OmnPtrDecl(OmnStreamer, OmnStreamerPtr);
OmnPtrDecl(OmnSmStreamAck, OmnSmStreamAckPtr);
OmnPtrDecl(OmnSmStreamAckResp, OmnSmStreamAckRespPtr);
OmnPtrDecl(OmnSmStreamInvite, OmnSmStreamInvitePtr);
OmnPtrDecl(OmnSmStreamInviteResp, OmnSmStreamInviteRespPtr);
OmnPtrDecl(OmnSoStreamLog, OmnSoStreamLogPtr);
OmnPtrDecl(OmnComm, OmnCommPtr);
OmnPtrDecl(OmnTcpClient, OmnTcpClientPtr);
OmnPtrDecl(OmnUdp, OmnUdpPtr);
OmnPtrDecl(OmnUdpComm, OmnUdpCommPtr);
OmnPtrDecl(OmnProxyComm, OmnProxyCommPtr);
OmnPtrDecl(OmnStreamReader, OmnStreamReaderPtr);
OmnPtrDecl(OmnStreamReaderObj, OmnStreamReaderObjPtr);
OmnPtrDecl(OmnStreamLogEntry, OmnStreamLogEntryPtr);
OmnPtrDecl(OmnStreamItr, OmnStreamItrPtr);
OmnPtrDecl(OmnConnBuffItr, OmnConnBuffItrPtr);
OmnPtrDecl(OmnConnMsgProc, OmnConnMsgProcPtr);
OmnPtrDecl(OmnTcpCltGrpListener, OmnTcpCltGrpListenerPtr);
OmnPtrDecl(OmnTcpCltGrp, OmnTcpCltGrpPtr);
OmnPtrDecl(OmnTcpServerGrp, OmnTcpServerGrpPtr);
OmnPtrDecl(AosTcpTrafficGenClient, AosTcpTrafficGenClientPtr);
OmnPtrDecl(AosTcpTrafficGen, AosTcpTrafficGenPtr);
OmnPtrDecl(AosTcpTrafficGenThread, AosTcpTrafficGenThreadPtr);
OmnPtrDecl(AosTcpTrafficGenListener, AosTcpTrafficGenListenerPtr);
OmnPtrDecl(AosTcpBouncerClient, AosTcpBouncerClientPtr);
OmnPtrDecl(AosTcpBouncerListener, AosTcpBouncerListenerPtr);
OmnPtrDecl(AosTcpBouncer, AosTcpBouncerPtr);
OmnPtrDecl(AosCommOwner, AosCommOwnerPtr);
OmnPtrDecl(AosTcpEpollReader, AosTcpEpollReaderPtr);
OmnPtrDecl(AosUdpComm, AosUdpCommPtr);

#endif
