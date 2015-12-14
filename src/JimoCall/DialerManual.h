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
// 2014/12/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_DialerManual_h
#define Aos_JimoCall_DialerManual_h

#include "CubeComm/CubeComm.h"
#include "JimoCall/JimoCallDialer.h"
#include "SEInterfaces/CubeMapObj.h"

#include <vector>
using namespace std;


class AosDialerManual : public AosJimoCallDialer
{
	OmnDefineRCObject;

private:
	enum
	{
		eDefaultNumTries = 3
	};

	u32						mEndPointIdx;
	u64						mJimoCallID;
	AosCubeCommPtr			mCubeComm;
	AosCubeComm *			mCubeCommRaw;
	vector<AosEndPointInfo>	mEndPoints;
	int						mNumTries;

public:
	AosDialerManual(AosRundata *rdata, const AosXmlTagPtr &cfg, const u32 dialer_id);
	~AosDialerManual();

	virtual u64		nextJimoCallID(AosRundata *rdata);
	virtual bool	makeJimoCall(AosRundata *rdata,
						const AosEndPointInfo &endpoint,
						const AosJimoCallPtr &jimo_call,
						AosBuff *buff);
	virtual bool	getTargetCubes(AosRundata *rdata, vector<AosCubeMapObj::CubeInfo> &cube_infos);

private:
	bool	config(AosRundata *rdata, const AosXmlTagPtr &cfg);

};

#endif

