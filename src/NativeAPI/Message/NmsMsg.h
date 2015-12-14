////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NmsMsg.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_NmsMsg_h
#define Omn_Message_NmsMsg_h

#include "Message/MsgComm.h"
#include "Message/Ptrs.h"



class OmnRawMsg : public OmnMsgComm
{
private:

public:
	OmnRawMsg(const OmnConnBuffPtr &buff);
	~OmnRawMsg();
};
#endif

