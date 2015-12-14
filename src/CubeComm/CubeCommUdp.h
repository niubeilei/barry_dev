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
#ifndef Aos_CubeComm_CubeCommUdp_h
#define Aos_CubeComm_CubeCommUdp_h

#include "CubeComm/CubeComm.h"
#include "UtilComm/UdpComm.h"


class AosCubeCommUdp : public AosCubeComm
{
	enum 
	{
		eMaxSendBuffSize = 40000
	};

private:
	u64						mSendId;

	AosEndPointInfo			mEPInfo;
	OmnUdpCommPtr			mComm;

	struct MsgReceived
	{
		int num;
		map<int, OmnConnBuffPtr> connbuffs;
	};

	map<OmnString, MsgReceived>	mMsgReceived;

public:
	AosCubeCommUdp(const OmnCommListenerPtr &caller);
	AosCubeCommUdp(const AosEndPointInfo epinfo, const OmnCommListenerPtr &caller);
	~AosCubeCommUdp();

	virtual bool	connect(AosRundata *rdata);
	virtual bool	close();

	virtual bool	startReading(AosRundata *rdata, const OmnCommListenerPtr &caller);
	virtual bool	stopReading();
	virtual bool	sendTo(AosRundata *rdata, const AosEndPointInfo &remote_epinfo, AosBuff *buff);

	// CommListener interface
	virtual bool msgRead(const OmnConnBuffPtr &connbuff);

private:
	virtual bool	proc(const OmnConnBuffPtr &data);
	bool	rebuildConnBuff(const OmnConnBuffPtr &connbuff);
	bool	rebuildConnBuff(map<int, OmnConnBuffPtr> &connbuffs);

};

#endif

