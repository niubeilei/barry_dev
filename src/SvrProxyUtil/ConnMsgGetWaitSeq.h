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
#ifndef AOS_SvrProxyUtil_ConnMsgGetWaitSeq_h
#define AOS_SvrProxyUtil_ConnMsgGetWaitSeq_h

#include "TransBasic/ConnMsg.h"
#include "Util/Buff.h"

class AosConnMsgGetWaitSeq: public AosConnMsg
{

public:
	AosConnMsgGetWaitSeq(const bool reg_flag);
	AosConnMsgGetWaitSeq();
	~AosConnMsgGetWaitSeq();

	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual AosConnMsgPtr clone2();
	virtual OmnString getStrType(){ return "eGetWaitSeq"; };

};

#endif
