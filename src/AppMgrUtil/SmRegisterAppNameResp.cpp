////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRegisterAppNameResp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgrUtil/SmRegisterAppNameResp.h"

#include "Alarm/Alarm.h"
#include "AppMgrUtil/Ptrs.h"
#include "Message/Resp.h"
#include "Util/OmnNew.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"


OmnMsgPtr		
OmnSmRegisterAppNameResp::createInst(OmnSerialFrom &s)
{
	OmnSmRegisterAppNameRespPtr msg = OmnNew OmnSmRegisterAppNameResp();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmRegisterAppNameResp::toString() const
{
    OmnString str;
	str << "\nSmRegisterAppNameResp Message: "
		<< "\n---------------"
		<< "\n    AppName: " << mAppName
		<< OmnMsg::toString();
	return str;
}


OmnRslt			
OmnSmRegisterAppNameResp::serializeFrom(OmnSerialFrom &s)
{
	s >> mAppName;

	OmnMsg::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmRegisterAppNameResp::serializeTo(OmnSerialTo &s) const
{
	s << OmnMsgId::eSmRegisterAppNameResp;
	s << mAppName;

	OmnMsg::serializeTo(s);
	return true;
}


OmnMsgPtr		
OmnSmRegisterAppNameResp::clone() const
{
	OmnSmRegisterAppNameRespPtr msg = OmnNew OmnSmRegisterAppNameResp();
	msg.setDelFlag(false);

	OmnResp::clone(msg.getPtr());

	msg->mAppName = mAppName;
	return msg.getPtr();
}


OmnRespPtr
OmnSmRegisterAppNameResp::getResp()
{
	OmnNotImplementedYet;
	return 0;
}
