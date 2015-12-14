////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmRegisterAppNameResp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgrUtil_SmRegisterAppNameResp_h
#define Omn_AppMgrUtil_SmRegisterAppNameResp_h

#include "Message/Resp.h"


class OmnSmRegisterAppNameResp : public OmnResp
{
	OmnDefineRCObject;

private:
	OmnString		mAppName;

public:
	OmnSmRegisterAppNameResp() {}
	OmnSmRegisterAppNameResp(const OmnString &name)
		:
	mAppName(name)
	{
	}

	virtual ~OmnSmRegisterAppNameResp() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmRegisterAppNameResp;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmRegisterAppNameResp;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;
	virtual OmnRespPtr		getResp();
};


#endif

