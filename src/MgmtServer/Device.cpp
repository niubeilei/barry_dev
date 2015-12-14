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
// 12/05/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "MgmtServer/Device.h"



AosDevice::AosDevice(const int device_id)
:
mDeviceId(device_id)
{
}


AosDevice::~AosDevice()
{
}


bool
AosDevice::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	// The configuration should be: 
	// 	<device device_id="xxx"
	// 		addr1="xxx" port1="xxx" num_ports1="xxx
	// 		addr1="xxx" port1="xxx" num_ports1="xxx
	// 		.../>
	aos_assert_rr(conf, rdata, false);
	int device_id = conf->getAttrInt("device_id", -1);
	if (device_id != mDeviceid)
	{
		AosSetError(rdata, AOSLT_DEVICE_ID_MISMATCHING);
		OmnAlarm << rdata->getError() << ". " << mDeviceId << ":" << device_id << enderr;
		return false;
	}

	int guard = eMaxAddresses;
	int idx = 1;
	while (guard--)
	{
		OmnString addrname = "addr";
		addrname << idx;
		OmnIpAddr addr(conf->getAttrStr(addrname));
		if (!addr.isValid()) break;

		OmnString portname = "port";
		portname << idx;
		int port = conf->getAttrInt(portname, -1);
		if (port <= 0)
		{
			AosSetError(rdata, AOSLT_INVALID_PORT);
			OmnAlarm << rdata->getErrmsg() << ". " << conf->toString() << enderr;
			return false;
		}

		OmnString num_portsname = "num_ports";
		num_portsname << idx;
		int num_ports = conf->getAttrInt(num_portsname, -1);
		if (num_ports <= 0) num_ports = 1;

		mAddrs.push_back(addr);
		mPorts.push_back(port);
		mNumPorts.push_back(num_ports);
		idx++;
	}

	if (mAddrs.size() <= 0)
	{
		AosSetError(rdata, AOSLT_MISSING_ADDRESSES);
		OmnAlarm << rdata->getErrmsg() << ". " << conf->toString() << enderr;
		return false;
	}
	return true;
}


bool
AosDevice::start(const AosRundataPtr &rdata)
{
	// It starts listen to the well known port. It supports two interfaces.
	// The second one is optional. If it is valid, it will listen to the
	// second address.
	aos_assert_rr(mAddrs.size() > 0, rdata, false);
	for (u32 i=0; i<mAddrs.size(); i++)
	{
		if (!mAddrs[i].isValid())
		{
			AosSetError(rdata, AOSLT_MISSING_PRIMARY_ADDR);
			OmnAlarm << rdata->getErrmsg() << ". " << mDeviceId << enderr;
			return false;
		}

		try
		{
			OmnTcpServerPtr server = OmnNew OmnTcpServer(mAddrs[i], 
				mPorts[i], mNumPorts[i], "ManagementServer", eAosTLT_FirstFourHigh);
			mServers.push_back(server);
		}

		catch (...)
		{
			AosSetError(rdata, AOSLT_FAILED_START_TCP_SERVER);
			OmnAlarm << rdata->getErrmsg() << ". " << mAddrs[i].toString()
				<< ": " << mPorts[i] << enderr;
		}
	}
	return true;
}


bool
AosDevice::sendHeartbeat()
{
	aos_assert_r(mConn, false);
	OmnString msg = createHeartbeatMsg();
	aos_assert_r(msg != "", false);
	bool rslt = mConn->smartSend(msg);
	aos_assert_r(rslt, false);
	return true;
}


OmnString
AosDevice::createMsg()
{
	// It creates a heartbeat message. 
	OmnString str = "<heartbeat/>";
	return str;
}

