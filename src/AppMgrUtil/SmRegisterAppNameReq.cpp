////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRegisterAppNameReq.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgrUtil/SmRegisterAppNameReq.h"

#include "Alarm/Alarm.h"
#include "AppMgrUtil/Ptrs.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "Message/Resp.h"
#include "Util/OmnNew.h"


OmnMsgPtr		
OmnSmRegisterAppNameReq::createInst(OmnSerialFrom &s)
{
	OmnSmRegisterAppNameReqPtr msg = OmnNew OmnSmRegisterAppNameReq();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmRegisterAppNameReq::toString() const
{
    OmnString str;
	str << "\nSmRegisterAppNameReq Message: "
		<< "\n---------------"
		<< "\n    AppName: " << mAppName
		<< OmnMsg::toString();
	return str;
}


OmnRslt			
OmnSmRegisterAppNameReq::serializeFrom(OmnSerialFrom &s)
{
	s >> mAppName;

	OmnMsg::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmRegisterAppNameReq::serializeTo(OmnSerialTo &s) const
{
	s << OmnMsgId::eSmRegisterAppNameReq;
	s << mAppName;

	OmnMsg::serializeTo(s);
	return true;
}


OmnMsgPtr		
OmnSmRegisterAppNameReq::clone() const
{
	OmnSmRegisterAppNameReqPtr msg = OmnNew OmnSmRegisterAppNameReq();
	msg.setDelFlag(false);

	OmnReq::clone(msg.getPtr());

	msg->mAppName = mAppName;
	return msg.getPtr();
}


OmnRespPtr
OmnSmRegisterAppNameReq::getResp()
{
	OmnNotImplementedYet;
	return 0;
}
