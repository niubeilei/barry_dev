////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SpnrCtlr.h
// Description:
//	Use this class to interface with SPNR to get information from it, 
//  to send information to it, to change its internal data, etc. 
//  This is used by Network Management to manage an SPNR.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef NMS_SpnrController_h
#define NMS_SpnrController_h

#include "Util/Obj.h"

class OmnSpnrController OmnDeriveFromObj
{
private:

	//
	// Do not use the two
	//
	OmnSpnrController(const OmnSpnrController &rhs);
	OmnSpnrController & operator = (const OmnSpnrController &rhs);

public:
	OmnSpnrController();
	~OmnSpnrController();

	bool		procMsg(const OmnNmsMsgPtr &msg);

private:
	bool		startSpnr(const OmnNmsMsgPtr &msg);
	bool		stopSpnr(const OmnNmsMsgPtr &msg);

	bool		addRoutingEntry(const OmnNmsMsgPtr &msg);
	bool		deleteRoutingEntry(const OmnNmsMsgPtr &msg);
	bool		modifyRoutingEntry(const OmnNmsMsgPtr &msg);
	bool		retrieveRoutingTable(const OmnNmsMsgPtr &msg);
	
	bool		createConn(const OmnNmsMsgPtr &msg);
	bool		modifyConn(const OmnNmsMsgPtr &msg);
	bool		deleteConn(const OmnNmsMsgPtr &msg);
	bool		queryConn(const OmnNmsMsgPtr &msg);

	bool		addEndpoint(const OmnNmsMsgPtr &msg);
	bool		deleteEndpoint(const OmnNmsMsgPtr &msg);
	bool		modifyEndpoint(const OmnNmsMsgPtr &msg);
	bool		queryEndpoint(const OmnNmsMsgPtr &msg);

	bool		checkRedundancyMode(const OmnNmsMsgPtr &msg);
};
#endif
