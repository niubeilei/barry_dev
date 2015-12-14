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
#include "TransServer/Tester/TransSvrProc.h"

#include "Porting/Sleep.h"
#include "TransBasic/Trans.h"
#include "TransUtil/XmlTrans.h"
#include "TransServer/TransServer.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"


AosTransSvrProc::AosTransSvrProc()
{
}


bool
AosTransSvrProc::proc(
		const AosXmlTransPtr &trans, 
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans_doc, false);
	//OmnScreen << "proc_trans: ID:" << trans->getTransId() << endl;	
	static int i=0;
	OmnString respMsg = "<msg>";
	respMsg << i++ << "</msg>";
	rdata->setContents(respMsg);
	//trans->sendResponse(respMsg, rdata);

	static u32 total = 0;
	static u64 t1 = 0, t2 = 0;
	if(total % 50000 == 0)
	{
		u64 t2 = OmnGetSecond();
		if(total) OmnScreen << "each 50000 trans times:" << t2-t1 << endl;
		t1 = t2;
	}
	total++;
	
	return true;
}
