////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmGetAppInfoResp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgrUtil/SmGetAppInfoResp.h"

#include "Alarm/Alarm.h"
#include "AppMgrUtil/Ptrs.h"
#include "Message/Resp.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"


OmnMsgPtr		
OmnSmGetAppInfoResp::createInst(OmnSerialFrom &s)
{
	OmnSmGetAppInfoRespPtr msg = OmnNew OmnSmGetAppInfoResp();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmGetAppInfoResp::toString() const
{
    OmnString str;
	str << "\nSmGetAppInfoResp Message: "
		<< "\n---------------"
		<< "\n    App: " << mApp
		<< OmnMsg::toString();
	return str;
}


OmnRslt			
OmnSmGetAppInfoResp::serializeFrom(OmnSerialFrom &s)
{
	s >> mApp;

	OmnMsg::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmGetAppInfoResp::serializeTo(OmnSerialTo &s) const
{
	s << OmnMsgId::eSmGetAppInfoResp;
	s << mApp;

	OmnMsg::serializeTo(s);
	return true;
}


OmnMsgPtr		
OmnSmGetAppInfoResp::clone() const
{
	OmnSmGetAppInfoRespPtr msg = OmnNew OmnSmGetAppInfoResp();
	msg.setDelFlag(false);

	OmnResp::clone(msg.getPtr());

	msg->mApp = mApp;
	return msg.getPtr();
}


OmnRespPtr
OmnSmGetAppInfoResp::getResp()
{
	OmnNotImplementedYet;
	return 0;
}
