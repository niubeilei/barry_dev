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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiS.h"

#include "API/AosApiG.h"
#include "SEInterfaces/TransSvrObj.h"

bool AosFinishTrans(const AosTransPtr &trans)
{
	AosTransSvrObjPtr trans_svr = AosTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);
	
	return trans_svr->finishTrans(trans);
}

bool AosFinishTrans(vector<AosTransId> &trans_ids)
{
	AosTransSvrObjPtr trans_svr = AosTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);
	
	return trans_svr->finishTrans(trans_ids);
}



