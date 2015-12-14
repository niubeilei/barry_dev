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
// 2015/02/03 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_EndPointInfo_h
#define Aos_CubeComm_EndPointInfo_h

#include "Rundata/Rundata.h"
#include "Util/IpAddr.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"


class AosEndPointInfo
{
public:
	u32			mEpId;
	OmnString	mEpName;
	OmnIpAddr	mIpAddr;
	int			mPort;

public:
	AosEndPointInfo();
	AosEndPointInfo(
		const u32 epid,
		const OmnString &name,
		const OmnIpAddr &ip_addr,
		const int port);
	~AosEndPointInfo();

	bool init(AosRundata *rdata, const AosXmlTagPtr &cfg);
	inline bool isValid() const
	{
		return mIpAddr.isValid() && mPort > 0;
	}

	//getters and setters
	u32 getEndpointId() { return mEpId; }

	const char* toStr() const;	//by white, 2015-08-14 17:30
};

#endif

