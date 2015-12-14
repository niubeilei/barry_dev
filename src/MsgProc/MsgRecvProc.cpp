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
// Modification History:
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MsgProc/MsgRecvProc.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosMsgRecvProc::AosMsgRecvProc(const bool flag)
:
AosMsgProc(AOSMSGPROCTYPE_RECEIVER_PROC, AosMsgProcType::eReceiverProc, flag)
{
}


AosMsgRecvProc::~AosMsgRecvProc()
{
}


bool
AosMsgRecvProc::msgReceived(
		const OmnString &appid,
		const AosXmlTagPtr &msg, 
		const AosRundataPtr &rdata)
{
	// msg should be this format:
	// <Request>
	//   <header>
	//   	<version>1.0</version>
	//   	<deviceId>{2013E03B7065,  这个是变量}</deviceId>
	//   	<command>1</command>
	//   </header>
	//   <body>
	//     【具体的参数字符串，这个字符串的内容根据Command不同而不同，所以其解析也依赖于Command】
	//   </body>
	// </Request>
	//
	aos_assert_r(msg, false);
	return false;
}


