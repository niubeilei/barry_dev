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
// 2013/03/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Message_QueryMsg_h
#define Omn_Message_QueryMsg_h

#include "Message/BuffMsg.h"
#include "Util/Buff.h"


class AosQueryMsg : public AosBuffMsg
{
protected:
	u64			mQueryId;

public:
	AosQueryMsg(
		const AosRundataPtr &rdata, 
		const OmnMsgId::E msgid,
		const int send_phyid, 
		const int recv_phyid, 
		const bool is_request, 
		const u64 &query_id,
		const int version = smVersion);
	AosQueryMsg(
		const AosRundataPtr &rdata, 
		const OmnMsgId::E msgid,
		const u32 send_cubeid, 
		const u32 recv_cubeid, 
		const bool is_request, 
		const u64 &query_id,
		const int version = smVersion);
	virtual ~AosQueryMsg();

	u64 getQueryId() const {return mQueryId;}
	void setQueryId(const u64 &query_id) {mQueryId = query_id;}
};
#endif
