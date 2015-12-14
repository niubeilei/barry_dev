// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 04/15/2015	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysMsg_NotifyProcIsUp_h
#define Aos_SysMsg_NotifyProcIsUp_h

#include "TransBasic/AppMsg.h"
#include "API/AosApi.h"
#include "SEInterfaces/ProcessType.h"

class AosNotifyProcIsUp : public AosAppMsg
{
private:
	int 		mChildPid;
public:
	AosNotifyProcIsUp(const bool regflag);
	AosNotifyProcIsUp(const int child_pid);
	~AosNotifyProcIsUp();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eNotifyProcIsUp"; };
	virtual bool proc();
	
};
#endif

