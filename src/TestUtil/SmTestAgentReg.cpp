////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmTestAgentReg.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestUtil/SmTestAgentReg.h"

#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "TestUtil/Ptrs.h"
#include "TestUtil/SmTestAgentRegResp.h"
#include "Util/OmnNew.h"


OmnSmTestAgentReg::OmnSmTestAgentReg(const OmnString &agentName)
				   :
mAgentName(agentName)
{
}


OmnMsgPtr		
OmnSmTestAgentReg::createInst(OmnSerialFrom &s)
{
	OmnSmTestAgentRegPtr msg = OmnNew OmnSmTestAgentReg();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmTestAgentReg::toString() const
{
    OmnString str;
	str << "\nTestAgentReg Message: "
		<< "\n---------------"
		<< "\n    AgentName:   " << mAgentName
		<< OmnReq::toString();
	return str;
}


OmnRslt			
OmnSmTestAgentReg::serializeFrom(OmnSerialFrom &s)
{
	s.pop(eAgentName, mAgentName);
	OmnReq::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmTestAgentReg::serializeTo(OmnSerialTo &s) const
{
	s.push(OmnMsgId::eSmTestAgentReg);
	s.push(eAgentName, mAgentName);
	OmnReq::serializeTo(s);
	s.setDataLength();
	return true;
}


OmnMsgPtr		
OmnSmTestAgentReg::clone() const
{
	OmnSmTestAgentRegPtr msg = OmnNew OmnSmTestAgentReg();
	msg.setDelFlag(false);
	OmnMsg::clone(msg.getPtr());

	msg->mAgentName = mAgentName;
	return msg.getPtr();
}


OmnRespPtr	
OmnSmTestAgentReg::getResp()
{
	OmnSmTestAgentRegRespPtr resp = OmnNew OmnSmTestAgentRegResp();
	resp.setDelFlag(false);
	resp->setTransId(mTransId);
	return (OmnResp*)resp.getPtr();
}

