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
// 03/03/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Proggie_ReqDistr_NetReqProc_h
#define Aos_Proggie_ReqDistr_NetReqProc_h

#include "Proggie/ProggieUtil/Ptrs.h"
#include "Proggie/ReqDistr/Ptrs.h"
#include "Util/RCObject.h"
#include "UtilComm/Ptrs.h"
#include "Util/String.h"
#include <vector>


class AosNetReqProc : virtual public OmnRCObject
{
public:
	virtual bool				procRequest(const OmnConnBuffPtr &req) = 0;
	virtual bool				connCreated(const OmnTcpClientPtr &conn) {return true;}
	virtual bool				connClosed(const OmnTcpClientPtr &conn) {return true;}
	virtual AosNetReqProcPtr	clone() = 0;
};
#endif

