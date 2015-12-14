////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAgentMonitor_RecveJimoAgentProc_h
#define Aos_JimoAgentMonitor_RecveJimoAgentProc_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"

class AosRecveJimoAgentProc : public AosNetReqProc
{
	OmnDefineRCObject;
public:
	AosRecveJimoAgentProc();
	~AosRecveJimoAgentProc();

	AosNetReqProcPtr	clone();
	virtual bool procRequest(const OmnConnBuffPtr &buff);
	static bool config(const AosXmlTagPtr &config);
	bool procCommand(const OmnString &command, const OmnTcpClientPtr &client);
};
#endif

