////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 05/26/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SQLInterface_SQLInterface_h
#define Aos_SQLInterface_SQLInterface_h

#include "Database/Ptrs.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Rundata/Ptrs.h"



class AosSQLInterface : public AosNetReqProc
{
	OmnDefineRCObject;

public:

private:

public:
	AosSQLInterface();
	~AosSQLInterface();

	static bool		config(const AosXmlTagPtr &config);
	static bool		stop();
	virtual bool	procRequest(const OmnConnBuffPtr &req);
	virtual AosNetReqProcPtr	clone();

	void 	sendResp(const AosWebRequestPtr &req, 
				const AosXmlRc errcode,
				const OmnString &errmsg, 
				const OmnString &contents); 

private:
};
#endif
