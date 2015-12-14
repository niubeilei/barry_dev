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
#ifndef Aos_CubeComm_CubeCommTcpClt_h
#define Aos_CubeComm_CubeCommTcpClt_h

#include "CubeComm/CubeComm.h"

#include <map>
#include <queue> 

using namespace std;


class AosCubeCommTcpClt : public AosCubeComm 
{
	map<int, queue<OmnTcpClientPtr> >	mConns;

public:
	AosCubeCommTcpClt(const OmnCommListenerPtr &caller);
	~AosCubeCommTcpClt();

	virtual bool	connect(AosRundata *rdata);
	virtual bool	close();

	virtual bool	startReading(AosRundata *rdata, const OmnCommListenerPtr &caller);
	virtual bool	stopReading();
	virtual bool	sendTo(AosRundata *rdata, 
							const AosEndPointInfo &remote_epinfo, 
							AosBuff *buff);

private:
	virtual bool	proc(const OmnConnBuffPtr &data);

	OmnTcpClientPtr getConn(const AosEndPointInfo remote_epinfo);
	void returnConn(const AosEndPointInfo remote_epinfo, const OmnTcpClientPtr &conn);

};

#endif

