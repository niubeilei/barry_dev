////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmTestAgentRegResp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_TestAgentRegResp_h
#define Omn_TestUtil_TestAgentRegResp_h

#include "Message/Resp.h"


class OmnSmTestAgentRegResp : public OmnResp
{
	OmnDefineRCObject;

private:

public:
	OmnSmTestAgentRegResp();
	virtual ~OmnSmTestAgentRegResp() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmTestAgentRegResp;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmTestAgentRegResp;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;
};


#endif

