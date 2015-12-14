////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRPC.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestUtil/SmRPC.h"

#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "TestUtil/Ptrs.h"
#include "TestUtil/SmRPCResp.h"
#include "Util/OmnNew.h"


OmnSmRPC::OmnSmRPC(const OmnConnBuffPtr &parms)
				   :
mParms(parms)
{
}


OmnMsgPtr		
OmnSmRPC::createInst(OmnSerialFrom &s)
{
	OmnSmRPCPtr msg = OmnNew OmnSmRPC();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmRPC::toString() const
{
    OmnString str;
	str << "\nSmRPC Message: "
		<< "\n---------------"
		<< OmnReq::toString();
	return str;
}


OmnRslt			
OmnSmRPC::serializeFrom(OmnSerialFrom &s)
{
	int hasParms;
	s.pop(eHasParms, hasParms);

	if (hasParms)
	{
		s.pop(eParms, mParms);
	}

	OmnReq::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmRPC::serializeTo(OmnSerialTo &s) const
{
	s.push(OmnMsgId::eSmRPC);

	if (mParms)
	{
		s.push(eHasParms, 1);
		s.push(eParms, mParms);
	}
	else
	{
		s.push(eHasParms, 0);
	}

	OmnReq::serializeTo(s);
	s.setDataLength();
	return true;
}


OmnMsgPtr		
OmnSmRPC::clone() const
{
	OmnSmRPCPtr msg = OmnNew OmnSmRPC();
	msg.setDelFlag(false);
	OmnMsg::clone(msg.getPtr());

	msg->mParms = mParms;
	return msg.getPtr();
}


OmnRespPtr	
OmnSmRPC::getResp()
{
	OmnSmRPCRespPtr resp = OmnNew OmnSmRPCResp();
	resp.setDelFlag(false);
	resp->setTransId(mTransId);
	return (OmnResp*)resp.getPtr();
}

