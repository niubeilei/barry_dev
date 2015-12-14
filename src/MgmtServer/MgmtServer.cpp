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
#include "MgmtServer/MgmtServer.h"

#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"



AosMgmtServer::AosMgmtServer(
		const int device_id)
:
mDeviceId(device_id),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mDevices(0)
{
}


AosMgmtServer::~AosMgmtServer()
{
}


bool
AosMgmtServer::config(const AosXmlTagPtr &conf)
{
	// The configuration should be:
	// 	<config ...>
	//		<ManagedServers
	//			heartbeat_freq="xxx">
	//			<server .../>
	//			<server .../>
	//			...
	//		</ManagedServers>
	//		...
	//	</config>
	if (!conf)
	{
		OmnAlarm << "Missing configuration" << enderr;
		exit(0);
	}

	AosXmlTagPtr servers = conf->getFirstChild("ManagedServers");
	if (!servers)
	{
		OmnAlarm << "Configuration invalid!" << enderr;
		exit(0);
	}

	mLock->lock();
	if (!mDeviceMap.config(servers))
	{
		mLock->unlock();
		OmnAlarm << "Failed creating the device map" << enderr;
		exit(0);
	}

	mDevice = mDeviceMap.getDevice(mDeviceId);
	if (!mDevice)
	{
		mLock->unlock();
		OmnAlarm << "Device not configured: " << mDeviceId << enderr;
		exit(0);
	}

	mHeartbeatFreq = servers->getAttrInt("heartbeat_freq", -1);
	if (mHeartbeatFreq <= 0)
	{
		mHeartbeatFreq = eDftHeartbeatFreq;
	}
	mLock->unlock();
	return true;
}


bool
AosMgmtServer::start(const AosRundataPtr &rdata)
{
	aos_assert_rr(mDevice, rdata, false);
	bool rslt = mDevice->start(rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed starting the server: " << rdata->getErrmsg() << enderr;
		return false;
	}

	// Start the device check thread
	OmnThreadedObjPtr thisPtr(this, false);
	mDeviceCheckThread = OmnNew OmnThread(thisPtr, "DeviceCheck", 0, true, true, __FILE__, __LINE__);
	mDeviceCheckThread->start();

	// Start the heartbeat thread
	mHeartbeatThread = OmnNew OmnThread(thisPtr, "MgmtSvrHb", 1, true, true, __FILE__, __LINE__);
	mHeartbeatThread->start();
	
	return true;
}


bool
AosMgmtServer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == 0)
	{
		return deviceCheckThreadFunc(state, thread);
	}

	if (thread->getLogicId() == 1)
	{
		return heartbeatThreadFunc(state, thread);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosMgmtServer::deviceCheckThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		mCheckLock->lock();
		if (mDevicesToCheck.size() <= 0)
		{
			mCheckCond->wait(mCheckLock);
			mCheckLock->unlock();
			continue;
		}

		AosDevicePtr device = mDevicesToCheck.pop_front();
		mCheckLock->unlock();
		device->checkDevice();
	}
	return true;
}


bool
AosMgmtServer::heartbeatThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		OmnSleep(mHeartbeatFreq);

		// Check whether all devices have received responses or not
		for (int i=0; i<mMaxDeviceId; i++)
		{
			if (mDevices[i])
			{
				if (!mDevices[i]->heartbeatRespReceived())
				{
					// The device did not receive heartbeat response. 
					// This is a good indication that the device is down.
					confirmDeviceStatus(mDevices[i]);
				}
				else
				{
					bool rslt = mDevices[i]->sendHeartbeat();
				}
			}
		}
	}
	return true;
}


OmnString
AosMgmtServer::createMsg()
{
	// It creates a heartbeat message. 
	OmnString str = "<heartbeat/>";
	return str;
}


OmnString	
AosMgmtServer::getTcpListenerName() const
{
	return mName;
}


void		
AosMgmtServer::msgRecved(
		const OmnConnBuffPtr &buff,
		const OmnTcpClientPtr &conn)
{
	AosXmlTagPtr msg = AosXmlParser::parse(buff);
	aos_assert(msg);
	AosMsgId::E msgid = AosMsgId::toEnum(msg->getAttrStr("msgid"));
	switch (msgid)
	{
	case AosMsgId::eHeartbeat:
		 // Someone sent a heartbeat message. Just return a response.
		 return handleHeartbeat(msg, conn);
		 
	case AosMsgId::eHeartbeatResp:
		 // Received a heartbeat response
		 return handleHeartbeatResp(msg, conn);

	default:
		 break;
	}
	OmnShouldNeverComeHere;
}


void		
AosMgmtServer::connCreated(const OmnTcpClientPtr &conn)
{
}

	
void		
AosMgmtServer::connClosed(const OmnTcpClientPtr &client)
{
}


bool
AosMgmtServer::handleHeartbeat(
		const AosXmltagPtr &msg, 
		const OmnTcpClientPtr &conn)
{
	// A heartbeat is received. In the current implementations, 
	// just return a response.
	aos_assert_r(conn, false);
	OmnString resp = "<heartbeat_resp device_id=\"";
	resp << mDeviceId << "\" status=\"200\"/>";
	bool rslt = conn->smartSend(resp);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosMgmtServer::handleHeartbeatResp(
		const AosXmltagPtr &msg, 
		const OmnTcpClientPtr &conn)
{
	// A Heartbeat response was received. 
	int device_id = msg->getAttrInt("device_id", -1);
	aos_assert_r(device_id >= 0 && device_id < eMaxDevices, false);
	aos_assert_r(mDevices[device_id], false);
	return mDevices[device_id]->handleHeartbeatResp(msg, conn);
}


