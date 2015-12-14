////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmGetAppInfoReq.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgrUtil/SmGetAppInfoReq.h"

#include "Alarm/Alarm.h"
#include "AppMgrUtil/Ptrs.h"
#include "Message/Resp.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"


OmnMsgPtr		
OmnSmGetAppInfoReq::createInst(OmnSerialFrom &s)
{
	OmnSmGetAppInfoReqPtr msg = OmnNew OmnSmGetAppInfoReq();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmGetAppInfoReq::toString() const
{
    OmnString str;
	str << "\nSmGetAppInfoReq Message: "
		<< "\n---------------"
		<< "\n    InfoType: " << mInfoType
		<< OmnReq::toString();
	return str;
}


OmnRslt			
OmnSmGetAppInfoReq::serializeFrom(OmnSerialFrom &s)
{
	s >> (int &)mInfoType;

	OmnMsg::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmGetAppInfoReq::serializeTo(OmnSerialTo &s) const
{
	s << OmnMsgId::eSmGetAppInfoReq;
	s << mInfoType;

	OmnMsg::serializeTo(s);
	return true;
}


OmnMsgPtr		
OmnSmGetAppInfoReq::clone() const
{
	OmnSmGetAppInfoReqPtr msg = OmnNew OmnSmGetAppInfoReq();
	msg.setDelFlag(false);

	OmnReq::clone(msg.getPtr());

	msg->mInfoType = mInfoType;
	return msg.getPtr();
}


OmnRespPtr
OmnSmGetAppInfoReq::getResp()
{
	OmnNotImplementedYet;
	return 0;
}
