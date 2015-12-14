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
// 05/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogSvr/LogReq.h"

AosLogReq::AosLogReq(
		const AosLogReq::Reqid reqid, 
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
	:
mReqid(reqid),
mDoc(doc)
{
	mRundata.copyFrom(rdata);
}



