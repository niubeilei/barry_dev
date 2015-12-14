////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmTestAgentReg.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_TestAgentReg_h
#define Omn_TestUtil_TestAgentReg_h

#include "Message/Req.h"


class OmnSmTestAgentReg : public OmnReq
{
	OmnDefineRCObject;

private:
	enum
	{
		eAgentName
	};

	OmnString		mAgentName;

public:
	OmnSmTestAgentReg() {}
	OmnSmTestAgentReg(const OmnString &name);
	OmnSmTestAgentReg(const OmnConnBuffPtr &returnValue);
	virtual ~OmnSmTestAgentReg() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmTestAgentReg;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmTestAgentReg;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;
	virtual OmnRespPtr		getResp();

	OmnString		getAgentName() const {return mAgentName;}
};


#endif

