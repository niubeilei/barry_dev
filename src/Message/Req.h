////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Req.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_Req_h
#define Omn_Message_Req_h

#include "Debug/ErrId.h"
#include "Message/Msg.h"
#include "Message/Ptrs.h"


class OmnReq : public OmnMsg
{
protected:

public:
	OmnReq()
		:
	OmnMsg()
	{
	}

	OmnReq(const OmnIpAddr &senderIpAddr,
		   const int senderPort,
		   const OmnIpAddr &receiverIpAddr,
		   const int receiverPort)
		   :
	OmnMsg(senderIpAddr, senderPort, receiverIpAddr, receiverPort)
	{
	}

	virtual ~OmnReq() {}

	virtual bool		isReq() const {return true;}
	virtual bool		isResp() const {return false;}
	virtual OmnRespPtr	getResp() = 0;
};
#endif
