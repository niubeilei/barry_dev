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
#if 0
#ifndef Aos_IILCache_MsgRetrvQueryBlockResp_h
#define Aos_IILCache_MsgRetrvQueryBlockResp_h

#include "Message/BuffMsg.h"


class AosMsgRetrvQueryBlockResp : public AosBuffMsg
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

	u64 getQueryid() const 					{return getFieldU64(0, 0);}
	OmnString getIILName() const 			{return getFieldStr(1, "");}
	AosBitmapObjPtr getBitmap() const 	{return getFieldBitmap(2);}
	AosBuffPtr getNodeList() const			{return getFieldBuff(3);}
};
#endif
#endif
