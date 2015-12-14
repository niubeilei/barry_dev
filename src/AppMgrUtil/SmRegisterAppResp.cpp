////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRegisterAppResp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgrUtil/SmRegisterAppResp.h"

#include "Alarm/Alarm.h"
#include "AppMgrUtil/Ptrs.h"
#include "Message/Resp.h"
#include "Util/OmnNew.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"


OmnMsgPtr		
OmnSmRegisterAppResp::createInst(OmnSerialFrom &s)
{
	OmnSmRegisterAppRespPtr msg = OmnNew OmnSmRegisterAppResp();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmRegisterAppResp::toString() const
{
    OmnString str;
	str << "\nSmRegisterAppResp Message: "
		<< "\n---------------"
		<< "\n    App: " << mApp
		<< OmnMsg::toString();
	return str;
}


OmnRslt			
OmnSmRegisterAppResp::serializeFrom(OmnSerialFrom &s)
{
	s >> mApp;

	OmnMsg::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmRegisterAppResp::serializeTo(OmnSerialTo &s) const
{
	s << OmnMsgId::eSmRegisterAppResp;
	s << mApp;

	OmnMsg::serializeTo(s);
	return true;
}


OmnMsgPtr		
OmnSmRegisterAppResp::clone() const
{
	OmnSmRegisterAppRespPtr msg = OmnNew OmnSmRegisterAppResp();
	msg.setDelFlag(false);

	OmnReq::clone(msg.getPtr());

	msg->mApp = mApp;
	return msg.getPtr();
}


OmnRespPtr
OmnSmRegisterAppResp::getResp()
{
	OmnNotImplementedYet;
	return 0;
}
