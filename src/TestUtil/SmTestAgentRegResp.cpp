////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmTestAgentRegResp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestUtil/SmTestAgentRegResp.h"

#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "TestUtil/Ptrs.h"
#include "Util/OmnNew.h"


OmnSmTestAgentRegResp::OmnSmTestAgentRegResp()
{
}



OmnMsgPtr		
OmnSmTestAgentRegResp::createInst(OmnSerialFrom &s)
{
	OmnSmTestAgentRegRespPtr msg = OmnNew OmnSmTestAgentRegResp();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmTestAgentRegResp::toString() const
{
    OmnString str;
	str << "\nTestAgentRegResp Message: "
		<< "\n---------------"
		<< OmnResp::toString();
	return str;
}


OmnRslt			
OmnSmTestAgentRegResp::serializeFrom(OmnSerialFrom &s)
{
	OmnResp::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmTestAgentRegResp::serializeTo(OmnSerialTo &s) const
{
	OmnResp::serializeTo(s);
	s.setDataLength();
	return true;
}


OmnMsgPtr		
OmnSmTestAgentRegResp::clone() const
{
	OmnSmTestAgentRegRespPtr msg = OmnNew OmnSmTestAgentRegResp();
	msg.setDelFlag(false);
	OmnMsg::clone(msg.getPtr());

	return msg.getPtr();
}

