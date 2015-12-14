////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HbServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Heartbeat_HbServer_h
#define Omn_Heartbeat_HbServer_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Thread/Ptrs.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class OmnHbServer : public AosNetReqProc
{
	OmnDefineRCObject;

public:
	OmnHbServer();
	~OmnHbServer();

	bool	start();
	bool	stop();
	bool	config(const AosXmlTagPtr &conf);

	// AosNetReqProc Interface
	virtual bool				procRequest(const OmnConnBuffPtr &req);
	virtual AosNetReqProcPtr	clone();
	virtual bool				connCreated(const OmnTcpClientPtr &conn) {return true;}
	virtual bool				connClosed(const OmnTcpClientPtr &conn) {return true;}
};

#endif
