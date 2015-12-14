////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRPC.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_SmRPC_h
#define Omn_TestUtil_SmRPC_h

#include "Message/Req.h"


class OmnSmRPC : public OmnReq
{
	OmnDefineRCObject;

private:
	enum
	{
		eHasParms,
		eParms
	};

	OmnConnBuffPtr	mParms;

public:
	OmnSmRPC()
	{
	}

	OmnSmRPC(const OmnConnBuffPtr &parms);
	virtual ~OmnSmRPC() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmRPC;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmRPC;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;
	virtual OmnRespPtr		getResp();

	OmnConnBuffPtr			getParms() const;
};


#endif

