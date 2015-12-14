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
// 10/13/2010: Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_DataSync_DataSyncSvr_h
#define Aos_DataSync_DataSyncSvr_h

#include "Database/Ptrs.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "SEServer/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"



class AosDataSyncSvr : public AosNetReqProc
{
	OmnDefineRCObject;

public:

private:

public:
	AosDataSyncSvr();
	~AosDataSyncSvr();

	static bool		config(const AosXmlTagPtr &config);
	static bool		stop();
	virtual bool	procRequest(const OmnConnBuffPtr &req);
	virtual AosNetReqProcPtr	clone();

	void 	sendResp(const AosWebRequestPtr &req, 
				const AosXmlRc errcode,
				const OmnString &errmsg, 
				const OmnString &contents); 
};
#endif
#endif
