////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRPCResp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestUtil/SmRPCResp.h"

#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"
#include "TestUtil/Ptrs.h"
#include "Util/OmnNew.h"


OmnSmRPCResp::OmnSmRPCResp()
{
}


OmnSmRPCResp::OmnSmRPCResp(const OmnConnBuffPtr &returnValue)
:
mReturnValue(returnValue)
{
}


OmnMsgPtr		
OmnSmRPCResp::createInst(OmnSerialFrom &s)
{
	OmnSmRPCRespPtr msg = OmnNew OmnSmRPCResp();
	msg.setDelFlag(false);

	msg->serializeFrom(s);
	return msg.getPtr();
}


OmnString		
OmnSmRPCResp::toString() const
{
    OmnString str;
	str << "\nSmRPCResp Message: "
		<< "\n---------------"
		<< OmnResp::toString();
	return str;
}


OmnRslt			
OmnSmRPCResp::serializeFrom(OmnSerialFrom &s)
{
	int hasReturnValue;
	s.pop(eHasReturnValue, hasReturnValue);
	if (hasReturnValue)
	{
		s.pop(eReturnValue, mReturnValue);
	}
	else
	{
		mReturnValue = 0;
	}

	OmnResp::serializeFrom(s);
	return true;
}


OmnRslt			
OmnSmRPCResp::serializeTo(OmnSerialTo &s) const
{
	s.push(OmnMsgId::eSmRPCResp);
	if (mReturnValue)
	{
		s.push(eHasReturnValue, 1);
		s.push(eReturnValue, mReturnValue);
	}
	else
	{
		s.push(eHasReturnValue, 0);
	}

	OmnResp::serializeTo(s);
	s.setDataLength();
	return true;
}


OmnMsgPtr		
OmnSmRPCResp::clone() const
{
	OmnSmRPCRespPtr msg = OmnNew OmnSmRPCResp();
	msg.setDelFlag(false);
	OmnMsg::clone(msg.getPtr());

	msg->mReturnValue = mReturnValue;
	return msg.getPtr();
}


OmnConnBuffPtr
OmnSmRPCResp::getReturnValue() const
{
	return mReturnValue;
}
