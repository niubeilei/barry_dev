////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_MsgServerObj_h
#define Aos_SEInterfaces_MsgServerObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosMsgServerObj : virtual public OmnRCObject
{
	static AosMsgServerObjPtr	smMsgServer;

public:
	virtual bool procRequest(
			const AosXmlTagPtr &root,
			const AosRundataPtr &rdata) = 0;
public:
	static void setMsgServer(const AosMsgServerObjPtr &d) {smMsgServer = d;}
	static AosMsgServerObjPtr getMsgServer() {return smMsgServer;}

	static bool procRequestStatic(
			const AosXmlTagPtr &root,
			const AosRundataPtr &rdata);
};

#endif
