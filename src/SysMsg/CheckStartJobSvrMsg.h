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
#ifndef Aos_SysMsg_CheckStartJobSvrMsg_h
#define Aos_SysMsg_CheckStartJobSvrMsg_h

#include "TransBasic/AppMsg.h"
#include "API/AosApi.h"
#include "SEInterfaces/ProcessType.h"

class AosCheckStartJobSvrMsg : public AosAppMsg
{
public:
	AosCheckStartJobSvrMsg(const bool regflag);
	AosCheckStartJobSvrMsg();
	~AosCheckStartJobSvrMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eCheckStartJobSvrMsg"; };
	virtual bool proc();
	
};
#endif

