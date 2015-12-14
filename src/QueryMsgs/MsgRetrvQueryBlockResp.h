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
// 2013/03/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryMsgs_MsgRetrvQueryBlockResp_h
#define Aos_QueryMsgs_MsgRetrvQueryBlockResp_h

#include "Message/QueryMsg.h"


class AosMsgRetrvQueryBlockResp : public AosQueryMsg
{
public:
	AosMsgRetrvQueryBlockResp(
		const AosRundataPtr &rdata, 
		const int send_phyid, 
		const int recv_phyid, 
		const u64 &query_id,
		const OmnString &iilname, 
		const AosBitmapObjPtr &bitmap,
		const AosBuffPtr &nodelist);
	~AosMsgRetrvQueryBlockResp(); 

	u64 getQueryid()  			{return getFieldU64(0, 0);}
	OmnString getIILName()  	{return getFieldStr(1, "");}
	AosBuffPtr getBitmap(const bool copy)  	{return getFieldBuff(2, copy);}
	AosBuffPtr getNodeList(const bool copy)	{return getFieldBuff(3, copy);}
};
#endif

