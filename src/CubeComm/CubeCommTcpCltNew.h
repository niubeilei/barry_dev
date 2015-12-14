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
// 2015/08/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_CubeCommTcpCltNew_h
#define Aos_CubeComm_CubeCommTcpCltNew_h

#include "CubeComm/CubeComm.h"
#include "JimoCall/Ptrs.h"

#include <map>
#include <queue> 

using namespace std;


class AosCubeCommTcpCltNew : public AosCubeComm 
{
	typedef map<int, queue<OmnTcpCommCltPtr> > map_t;
	typedef map<int, queue<OmnTcpCommCltPtr> >::iterator itr_t;

private:
	map_t	mConns;

public:
	AosCubeCommTcpCltNew(const OmnCommListenerPtr &caller);
	~AosCubeCommTcpCltNew();

	virtual bool	connect(AosRundata *rdata);
	virtual bool	close();

	virtual bool	sendTo(AosRundata *rdata, 
							const AosEndPointInfo &remote_epinfo, 
							AosBuff *buff);
	virtual bool	msgRead(const OmnConnBuffPtr &connbuff);

private:
	virtual bool	proc(const OmnConnBuffPtr &data);

	OmnTcpCommCltPtr getConn(const AosEndPointInfo remote_epinfo);
	void returnConn(const AosEndPointInfo remote_epinfo, const OmnTcpCommCltPtr &conn);

};

#endif

