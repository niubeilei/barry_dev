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
// 05/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/MsgServerObj.h"

AosMsgServerObjPtr AosMsgServerObj::smMsgServer;

bool
AosMsgServerObj::procRequestStatic(
		const AosXmlTagPtr &root,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smMsgServer, false);
	return smMsgServer->procRequest(root, rdata);
}
