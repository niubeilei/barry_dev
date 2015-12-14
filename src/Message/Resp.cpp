////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Resp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Message/Resp.h"

#include "Alarm/Alarm.h"
#include "Message/Req.h"
#include "Util/OmnNew.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"



OmnResp::OmnResp()
:
mRespCode(OmnErrId::eUnknown)
{
}


OmnResp::OmnResp(const OmnReqPtr &req)
:
OmnMsg(req->getRecverAddr(), req->getRecverPort(), 
	   req->getSenderAddr(), req->getSenderPort()),
mRespCode(OmnErrId::eUnknown)
{
}


OmnResp::OmnResp(const OmnIpAddr &senderAddr,
		    	 const int senderPort,
		    	 const OmnIpAddr &receiverAddr,
		    	 const int receiverPort)
:
OmnMsg(senderAddr, senderPort, receiverAddr, receiverPort),
mRespCode(OmnErrId::eUnknown)
{
}


OmnResp::~OmnResp()
{
}


OmnString       
OmnResp::toString() const
{
	OmnString str1 = OmnMsg::toString();
	OmnString emp("");
	str1 << emp << "test";

    OmnString str;
    str << "\nResp Message: "
        << "\n---------------"
        << "\n    RespCode: " << OmnErrId::toStr(mRespCode)
		<< "\n    Errmsg:   " << mErrmsg
		<< OmnMsg::toString();
    return str;
}


OmnRslt         
OmnResp::serializeFrom(OmnSerialFrom &s)
{
	try
	{
		int code;
		s >> code;
		if (OmnErrId::isValid((OmnErrId::E) code))
		{
			mRespCode = (OmnErrId::E)code;
		}
		else
		{
			OmnAlarm << "Incorrect response code: " << code << enderr;
			mRespCode = OmnErrId::eInvalid;
		}

		s >> mErrmsg;

		OmnMsg::serializeFrom(s);
	}
	
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to serialize from: " << e.toString() << enderr;
		return e.getRslt();
	}
	return true;
}


OmnRslt         
OmnResp::serializeTo(OmnSerialTo &s) const
{
	try
	{
		s << mRespCode;
		s << mErrmsg;

		OmnMsg::serializeTo(s);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to serialize from: " << e.toString() << enderr;
		return e.getRslt();
	}

    return true;
}

