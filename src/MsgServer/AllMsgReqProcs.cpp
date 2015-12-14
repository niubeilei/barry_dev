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
// 06/17/2011	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "MsgServer/AllMsgReqProcs.h"

#include "XmlUtil/XmlTag.h"
#include "MsgServer/Connect.h"
#include "MsgServer/DisConnect.h"
#include "MsgServer/GetMessage.h"
#include "MsgServer/SendMsg.h"

AosMsgRequestProcPtr         sgMsgRequestProcs[AosMsgReqid::eMax];
AosStr2U32_t                 sgReqidMap;
AosAllMsgReqProcs 			 sgAosAllMsgReqProcs;

AosAllMsgReqProcs::AosAllMsgReqProcs()
{
OmnScreen << "jozhi===========================================init msg proc" << endl;
	static AosConnect			sgConnect(true);
	static AosDisConnect		sgDisConnect(true);
	static AosGetMessage		sgGetMessage(true);
	static AosSendMsg			sgSendMsg(true);
}
