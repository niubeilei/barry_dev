////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Alarm_AlarmId_h
#define Omn_Alarm_AlarmId_h

//#include "Util/String.h"

class OmnAlarmId
{
public:
	enum E
	{
		eUnknown,

		eAddSocketError,
		eInvalidSocketError,
		eMsgSerializationError,

		eOmnConnBuffItr_next1,
		eOmnConnBuffItr_next2,
		eOmnConnBuffItr_getSeg1,
		eOmnConnBuffItr_getSeg2,

		eOmnCreateDataTrans_procCreateLdeResp1,

		eOmnDbLogger_addEntry1,
		eOmnDbLogger_addEntry2,

		eOmnDnMgr_getDn,
		eOmnEventMgr_procEvent,

		eOmnFileItr_Constructor,
		eOmnFileItr_next1,
		eOmnFileItr_next2,
		eOmnFileItr_next3,
		eOmnFileItr_getSeg1,
		eOmnFileItr_getSeg2,

		eOmnIfbTrans_procMsg1,
		eOmnIfbTrans_procMsg2,

		eOmnInfobusCltSendMsg_queryDns,
		eOmnInfobusClt_sendMsg2,
		eOmnInfobusClt_procMsg,
		eOmnInfobusClt_procMsg_1,
		eOmnInfobusClt_procMsg_2,
		eOmnInfobus_sendMsg1,
		eOmnInfobusClt_sendResp1,

		eOmnLoggerMgr_addEntry1,
		eOmnLoggerMgr_addEntry2,

		eOmnParser_printerr,
		eOmnParser_nextHost,
		eOmnParser_nextIpAddr,

		eOmnRealmImage_createPde1,
		eOmnRealmImage_createLde1,
		eOmnRealmImage_createLde2,
		eOmnRealmImage_createLde3,
		eOmnRealmImage_createLde4,
		eOmnRealmImage_prepareToRecvStream,

		eOmnTcpCommSvr_readFrom2,

		eRealmMgr_proc1,
		eRealmMgr_proc2,
		eRealmMgr_proc3,
		eRealmMgr_proc4,

		eShouldNeverComeToThisPoint,
		eSocketAcceptError,

		eOmnRetransMgr_addTrans1,
		eOmnRetransMgr_addTrans2,

		eOmnStrLogEntry_log,

		eOmnSSendReq_procStreamAck1,
		eOmnSSendReq_procStreamAck2,
		eOmnSendEntry_start,

		eOmnStreamer_sendMsg_1,
		eOmnStreamer_sendMsg_2,
		eOmnStreamer_procBuff,
		eOmnStreamer_msgRecved,
		eOmnStreamer_procStreamAck,

		eOmnStreamSendTrans_procStreamAck1,
		eOmnStreamSendTrans_procStreamAck2,

		eOmnSysObj_getMsgId1,
		eOmnSysObj_getMsgId2,
		eOmnSysObj_getMsgId3,

		eOmnRetransMgr_addTrans,
		eOmnTcpCommSvr_threadFunc,
		eOmnTcpCommSvr_startReading1,
		eOmnTcpCommSvr_stopReading,

		eUnclaimedTcpPacket,

		eMaxAlarmId
	};

	static bool isValidAlarmId(const E c) {return c > eUnknown && c < eMaxAlarmId;}
};

#endif
