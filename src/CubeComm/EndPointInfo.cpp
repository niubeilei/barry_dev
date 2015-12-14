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
#include "CubeComm/EndPointInfo.h"


AosEndPointInfo::AosEndPointInfo()
:
mEpId(0),
mIpAddr(0),
mPort(0)
{
}


AosEndPointInfo::AosEndPointInfo(
		const u32 epid,
		const OmnString &name,
		const OmnIpAddr &ip_addr,
		const int port)
:
mEpId(epid),
mEpName(name),
mIpAddr(ip_addr),
mPort(port)
{
}


AosEndPointInfo::~AosEndPointInfo()
{
}


bool
AosEndPointInfo::init(AosRundata *rdata, const AosXmlTagPtr &cfg)
{
	aos_assert_r(cfg, false);

	int epid = cfg->getAttrInt("id", -1);
	OmnString name = cfg->getAttrStr("name");
	OmnString addr = cfg->getAttrStr("addr");
	int port = cfg->getAttrInt("port", -1);

	if (epid < 0)
	{
		AosLogError(rdata, true, AosErrmsgId::eInvalidConfig)
			<< AosFN::eErrorMsg << "Invalid endpoint id" 
			<< AosFN::eValue << epid << enderr;
		return false;
	}

	OmnIpAddr ip_addr(addr);
	if (!ip_addr.isValid())
	{
		AosLogError(rdata, true, AosErrmsgId::eInvalidConfig)
			<< AosFN::eErrorMsg << "Invalid IP address" 
			<< AosFN::eValue << addr << enderr;
		return false;
	}

	if (port < 0)
	{
		AosLogError(rdata, true, AosErrmsgId::eInvalidConfig)
			<< AosFN::eErrorMsg << "Invalid port" 
			<< AosFN::eValue << port << enderr;
		return false;
	}

	mEpId = epid;
	mEpName = name;
	mIpAddr = ip_addr;
	mPort = port;

	return true;
}


const char*
AosEndPointInfo::toStr() const	//by white, 2015-08-14 17:30
{
	std::stringstream s;
	s.str("");
	s.clear();
	s << "epid:" << mEpId << " epname:" << mEpName << " ip:" << mIpAddr.toString() << " port:" << mPort;
	return s.str().c_str();
}
