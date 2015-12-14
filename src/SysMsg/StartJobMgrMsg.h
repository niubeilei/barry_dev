// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/03/07	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysMsg_StartJobMgrMsg_h
#define Aos_SysMsg_StartJobMgrMsg_h

#include "TransBasic/AppMsg.h"

class AosStartJobMgrMsg : public AosAppMsg
{
public:
	AosStartJobMgrMsg(const bool regflag);
	AosStartJobMgrMsg();
	~AosStartJobMgrMsg();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eStartJobMgrMsg"; };
	virtual bool proc();
	
};
#endif

