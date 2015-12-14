////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmGetAppInfoReq.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgrUtil_SmGetAppInfoReq_h
#define Omn_AppMgrUtil_SmGetAppInfoReq_h

#include "Message/Req.h"


class OmnSmGetAppInfoReq : public OmnReq
{
	OmnDefineRCObject;

public:
	enum InfoType
	{
		eStartTime,
		eThreadInfo,
		eAppInfo
	};

private:
	InfoType		mInfoType;

public:
	OmnSmGetAppInfoReq() {}
	virtual ~OmnSmGetAppInfoReq() {}

	virtual OmnMsgPtr		createInst(OmnSerialFrom &s);
	virtual OmnMsgId::E		getMsgId() const {return OmnMsgId::eSmGetAppInfoReq;}
	virtual OmnMsgId::E		getSubtype() const {return OmnMsgId::eSmGetAppInfoReq;}
	virtual OmnString		toString() const;
	virtual OmnRslt			serializeFrom(OmnSerialFrom &s);
	virtual OmnRslt			serializeTo(OmnSerialTo &s) const;
	virtual OmnMsgPtr		clone() const;
	virtual OmnRespPtr		getResp();

	InfoType	getInfoType() const {return mInfoType;}
	void		setInfoType(const InfoType t) {mInfoType = t;}
};


#endif

