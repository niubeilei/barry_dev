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
#ifndef Aos_CubeComm_CubeCommTcpSvrProc_h
#define Aos_CubeComm_CubeCommTcpSvrProc_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Util/RCObject.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"


class AosCubeCommTcpSvrProc : virtual public AosNetReqProc
{
	OmnDefineRCObject;

	OmnCommListenerPtr	mCaller;

public:
	AosCubeCommTcpSvrProc(const OmnCommListenerPtr &caller);
	AosCubeCommTcpSvrProc(AosCubeCommTcpSvrProc &proc);
	~AosCubeCommTcpSvrProc();

	virtual bool				procRequest(const OmnConnBuffPtr &connbuff);
	virtual AosNetReqProcPtr	clone();
};
#endif

