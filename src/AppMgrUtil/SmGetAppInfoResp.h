////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmGetAppInfoResp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgrUtil_SmGetAppInfoResp_h
#define Omn_AppMgrUtil_SmGetAppInfoResp_h

#include "Message/Resp.h"


class OmnSmGetAppInfoResp : public OmnResp
{
	OmnDefineRCObject;

private:
	OmnString		mApp;

public:
	OmnSmGetAppInfoResp() {}
	OmnSmGetAppInfoResp(const OmnString &name)
		:
	mApp(name)
	{
	}

	virtual ~OmnSmGetAppInfoResp() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmGetAppInfoResp;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmGetAppInfoResp;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;
	virtual OmnRespPtr		getResp();
};


#endif

