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
#ifndef Aos_SysMsg_GetClusterConfigMsg_h
#define Aos_SysMsg_GetClusterConfigMsg_h

#include "TransBasic/AppMsg.h"

class AosGetClusterConfigMsg : public AosAppMsg
{

private:	

public:
	AosGetClusterConfigMsg(const bool regflag);
	AosGetClusterConfigMsg(const int to_svr_id);
	~AosGetClusterConfigMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eGetClusterConfigMsg"; };
	virtual bool proc();
	
};
#endif

