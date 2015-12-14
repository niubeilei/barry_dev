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
#ifndef Aos_SysMsg_AddClusterMsg_h
#define Aos_SysMsg_AddClusterMsg_h

#include "TransBasic/AppMsg.h"
#include "API/AosApi.h"

class AosAddClusterMsg : public AosAppMsg
{

private:	
	OmnString		mArgs;
	AosXmlTagPtr 	mClusterConfig;
	AosXmlTagPtr 	mNormConfig;

public:
	AosAddClusterMsg(const bool regflag);
	AosAddClusterMsg(
			const OmnString &args,
			const AosXmlTagPtr &cluster_config,
			const AosXmlTagPtr &norm_config,
			const int to_svr_id,
			const u32 to_proc_id);
	~AosAddClusterMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eAddClusterMsg"; };
	virtual bool proc();
	
};
#endif

