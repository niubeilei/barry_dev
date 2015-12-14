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
#ifndef AOS_MsgProc_MsgReceiver_h
#define AOS_MsgProc_MsgReceiver_h

#include "MsgProc/MsgReceiver.h"

class AosMsgReceiver : public AosMsgProc
{
public:
	virtual bool msgReceived(const OmnString &appid, 
							 const AosXmlTagPtr &msg); 
};
#endif

