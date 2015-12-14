////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SysMsg_RebootProcMsg_h
#define AOS_SysMsg_RebootProcMsg_h

#include "Util/Buff.h"
#include "TransBasic/AppMsg.h"

class AosRebootProcMsg: public AosAppMsg
{

public:
	AosRebootProcMsg(const bool reg_flag);
	AosRebootProcMsg(
		const int to_svrid,
		const u32 to_proc_id);
	~AosRebootProcMsg();
	
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eRebootProc"; };
	virtual AosConnMsgPtr clone2();
	virtual bool	proc();
	
};

#endif
