////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRegisterAppReq.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgrUtil_SmRegisterAppReq_h
#define Omn_AppMgrUtil_SmRegisterAppReq_h

#include "Message/Req.h"


class OmnSmRegisterAppReq : public OmnReq
{
	OmnDefineRCObject;

private:
	OmnString		mApp;

public:
	OmnSmRegisterAppReq() {}
	OmnSmRegisterAppReq(const OmnString &name)
		:
	mApp(name)
	{
	}

	virtual ~OmnSmRegisterAppReq() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmRegisterAppReq;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmRegisterAppReq;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;
	virtual OmnRespPtr		getResp();
};


#endif

