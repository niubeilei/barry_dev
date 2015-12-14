////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRPCResp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_SmRPCResp_h
#define Omn_TestUtil_SmRPCResp_h

#include "Message/Resp.h"


class OmnSmRPCResp : public OmnResp
{
	OmnDefineRCObject;

private:
	enum
	{
		eHasReturnValue,
		eReturnValue
	};

	OmnConnBuffPtr	mReturnValue;

public:
	OmnSmRPCResp();
	OmnSmRPCResp(const OmnConnBuffPtr &returnValue);
	virtual ~OmnSmRPCResp() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmRPCResp;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmRPCResp;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;

	OmnConnBuffPtr			getReturnValue() const;
};


#endif

