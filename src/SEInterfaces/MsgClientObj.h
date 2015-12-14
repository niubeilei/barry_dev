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
#ifndef Aos_SEInterfaces_MsgClientObj_h
#define Aos_SEInterfaces_MsgClientObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosMsgClientObj : virtual public OmnRCObject
{
	static AosMsgClientObjPtr	smMsgClient;

public:
	virtual bool startProcCb(const u32 logic_pid) = 0;
	virtual bool stopProcCb(const u32 logic_pid) = 0;
	virtual bool procMsgSvrRequest(
					const OmnString &request,
					const AosRundataPtr &rdata) = 0;
	virtual OmnString getMsgSvrUrl() = 0;
public:
	static void setMsgClient(const AosMsgClientObjPtr &d) {smMsgClient= d;}
	static AosMsgClientObjPtr getMsgClient() {return smMsgClient;}

	static bool startProcCbStatic(const u32 logic_pid);
	static bool sotpPRocCbStatic(const u32 logic_pid);
	static bool procMsgSvrRequestStatic(
			const OmnString &request,
			const AosRundataPtr &rdata);
	static OmnString getMsgSvrUrlStatic();
};

#endif
