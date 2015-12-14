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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/22/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MsgProc_MsgRecvProc_h
#define AOS_MsgProc_MsgRecvProc_h

#include "MsgProc/MsgProc.h"

class AosMsgRecvProc : public AosMsgProc
{
	OmnDefineRCObject;

public:
	AosMsgRecvProc(const bool flag);
	~AosMsgRecvProc();

	virtual bool msgReceived(const OmnString &appid, 
							 const AosXmlTagPtr &msg, 
							 const AosRundataPtr &rdata); 
};
#endif

