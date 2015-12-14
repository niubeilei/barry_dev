////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 11/06/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_ShortMsgCltObj_h
#define AOS_SEInterfaces_ShortMsgCltObj_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"


class AosShortMsgCltObj : virtual public OmnRCObject
{
private:
	static AosShortMsgCltObjPtr	smClient;

public:
	virtual bool sendShortMsg(
					const OmnString &receivers,
					const OmnString &msg,
					AosXmlTagPtr &resp,
					const u32 timer,
					const AosRundataPtr &rdata) = 0;

	virtual bool sendShortMsg(
					const OmnString &receivers,
					const OmnString &msg,
					const AosRundataPtr &rdata) = 0;

	virtual bool confirmThrShortmsg(
					const OmnString &receiver, 
					const OmnString &msg, 
					const OmnString &confirm_code,
					const OmnString &sdoc_objid, 
					const AosRundataPtr &rdata) = 0;

	static AosShortMsgCltObjPtr getClient() {return smClient;}
	static void setClient(const AosShortMsgCltObjPtr &d) {smClient = d;}
};
#endif
