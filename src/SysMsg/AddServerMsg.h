// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysMsg_AddServerMsg_h
#define Aos_SysMsg_AddServerMsg_h

#include "TransBasic/AppMsg.h"
#include "API/AosApi.h"

class AosAddServerMsg : public AosAppMsg
{

private:	
	int				mCrtJudgerSid;
	AosXmlTagPtr 	mServersConfig;
	AosXmlTagPtr	mAdminConfig;

public:
	AosAddServerMsg(const bool regflag);
	AosAddServerMsg(
			const int crt_judger_sid,
			const AosXmlTagPtr &servers_config,
			const AosXmlTagPtr &admin_config,
			const int to_svr_id,
			const u32 to_proc_id);
	AosAddServerMsg(
			const AosXmlTagPtr &servers_config,
			const AosXmlTagPtr &admin_config,
			const int to_svr_id,
			const u32 to_proc_id);
	~AosAddServerMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eAddServer"; };
	virtual bool proc();
	
};
#endif

