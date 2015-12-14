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
// 06/14/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ShortMsgClt_ShortMsgClt_h
#define AOS_ShortMsgClt_ShortMsgClt_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"
#include "TransClient/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"

OmnDefineSingletonClass(AosShortMsgCltSingleton,
						AosShortMsgClt,
						AosShortMsgCltSelf,
						OmnSingletonObjId::eShortMsgClt,
						"AosShortMsgClt");

class AosShortMsgClt : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	bool 					mIsLocal;

public:
	AosShortMsgClt();
	~AosShortMsgClt();

    //
    // Singleton class interface
    //
    static AosShortMsgClt *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def);
	
	bool    sendShortMsg(
					const OmnString &receivers,
					const OmnString &msg,
					AosXmlTagPtr &resp,
					const u32 timer,
					const AosRundataPtr &rdata);
	bool    sendShortMsg(
					const OmnString &receivers,
					const OmnString &msg,
					const AosRundataPtr &rdata);
	bool	confirmThrShortmsg(
					const OmnString &receiver, 
					const OmnString &msg, 
					const OmnString &confirm_code,
					const OmnString &sdoc_objid, 
					const AosRundataPtr &rdata);
};
#endif
