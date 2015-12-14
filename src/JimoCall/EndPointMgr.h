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
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_EndpointMgr_h
#define Aos_JimoCall_EndpointMgr_h

#include "CubeComm/EndPointInfo.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"



class AosEndPointMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	static vector<AosEndPointInfo>	smEndpoints;
	static AosEndPointInfo  		smInvalidEndpoint;

public:
	AosEndPointMgr(AosRundata *rdata);
	~AosEndPointMgr();

	inline static AosEndPointInfo &getEndpoint(AosRundata *rdata, const int endpoint_id)
	{
		if (endpoint_id < 0 || (u32)endpoint_id >= smEndpoints.size())
		{
			return smInvalidEndpoint;
		}
		return smEndpoints[endpoint_id];
	}

private:
	bool init(AosRundata *rdata);
};
#endif
