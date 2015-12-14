////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRegisterAppReq.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgrUtil/SmRegisterAppReq.h"

#include "Alarm/Alarm.h"
#include "AppMgrUtil/Ptrs.h"
#include "Message/Resp.h"
#include "Util/OmnNew.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"


OmnMsgPtr		
OmnSmRegisterAppReq::createInst(OmnSerialFrom &s)
{
	OmnSmRegisterAppReqPtr msg = OmnNew OmnSmRegisterAppReq();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmRegisterAppReq::toString() const
{
    OmnString str;
	str << "\nSmRegisterAppReq Message: "
		<< "\n---------------"
		<< "\n    App: " << mApp
		<< OmnMsg::toString();
	return str;
}


OmnRslt			
OmnSmRegisterAppReq::serializeFrom(OmnSerialFrom &s)
{
	s >> mApp;

	OmnMsg::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmRegisterAppReq::serializeTo(OmnSerialTo &s) const
{
	s << OmnMsgId::eSmRegisterAppReq;
	s << mApp;

	OmnMsg::serializeTo(s);
	return true;
}


OmnMsgPtr		
OmnSmRegisterAppReq::clone() const
{
	OmnSmRegisterAppReqPtr msg = OmnNew OmnSmRegisterAppReq();
	msg.setDelFlag(false);

	OmnReq::clone(msg.getPtr());

	msg->mApp = mApp;
	return msg.getPtr();
}


OmnRespPtr
OmnSmRegisterAppReq::getResp()
{
	OmnNotImplementedYet;
	return 0;
}
