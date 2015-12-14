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
// Modification History:
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_CubeCommTcpSvr_h
#define Aos_CubeComm_CubeCommTcpSvr_h

#include "CubeComm/CubeComm.h"
#include "Proggie/ReqDistr/ReqDistr.h"


class AosCubeCommTcpSvr : public AosCubeComm
{
	AosEndPointInfo			mEPInfo;
	AosReqDistrPtr			mReqDistr;

public:
	AosCubeCommTcpSvr(const AosEndPointInfo epinfo);
	~AosCubeCommTcpSvr();

	virtual bool	connect(AosRundata *rdata);
	virtual bool	close();

	virtual bool	startReading(AosRundata *rdata, const OmnCommListenerPtr &caller);
	virtual bool	stopReading();
	virtual bool	sendTo(AosRundata *rdata, const AosEndPointInfo &remote_epinfo, AosBuff *buff);

private:
	virtual bool	proc(const OmnConnBuffPtr &data);

};

#endif

