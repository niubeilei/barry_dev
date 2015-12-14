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
// 05/31/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransClient/Tester/TransSvrProcLocal.h"

#include "Porting/Sleep.h"
#include "TransServer/TransServer.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"


AosTransSvrProcLocal::AosTransSvrProcLocal()
{
}


bool
AosTransSvrProcLocal::proc(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans_doc, false);
	
	static int i=0;
	OmnString respMsg = "<msg>";
	respMsg << i++ << "</msg>";
	rdata->setContents(respMsg);
	//trans->sendResponse(respMsg, rdata);
	return true;
}
