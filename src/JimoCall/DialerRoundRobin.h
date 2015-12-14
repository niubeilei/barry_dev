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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_DialerRoundRobin_h
#define Aos_JimoCall_DialerRoundRobin_h

#include "CubeComm/CubeComm.h"
#include "CubeComm/EndPointInfo.h"
#include "JimoCall/JimoCallDialer.h"

#include <vector>
using namespace std;


class AosDialerRoundRobin : public AosJimoCallDialer
{
	OmnDefineRCObject;

private:
	u32				mEndPointIdx;
	u64				mJimoCallID;
	AosCubeComm *	mCubeComm;
	vector<AosEndPointInfo>	mEndPoints;

public:
	AosDialerRoundRobin(AosRundata *rdata, const AosXmlTagPtr &conf, const u32 dialer_id);
	~AosDialerRoundRobin();

	u64 makeJimoCall(	AosRundata *rdata, 
						const AosJimoCallPtr &call, 
						AosBuff *data, 
						u32 &from_epid,
						u32 &to_epid);

	virtual u64		nextJimoCallID(AosRundata *rdata) { return 0; }
	virtual bool	makeJimoCall(AosRundata *rdata,
						const AosEndPointInfo &endpoint,
						const AosJimoCallPtr &jimo_call,
						AosBuff *buff) { return false; }

	virtual bool	getTargetCubes(AosRundata *rdata, vector<AosCubeMapObj::CubeInfo> &cube_infos) { return false; }

};

#endif
