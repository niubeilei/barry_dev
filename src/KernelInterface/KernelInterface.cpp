////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelInterface.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelInterface/KernelInterface.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/TcpCommSvr.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"


OmnSingletonImpl(OmnKernelInterfaceSingleton,
				 OmnKernelInterface,
				 OmnKernelInterfaceSelf,
				 "OmnKernelInterface");


OmnKernelInterface::OmnKernelInterface()
:
mUdpIndex(0),
mTcpIndex(0)
{
}


void testSend()
{
//	char data[1000];
//	memset(data, 'c', 1000);

//	OmnUdpComm comm(OmnIpAddr("192.168.1.80").toInt(), 5000, "Test");
//	comm.sendTo(data, 1000, OmnIpAddr("192.168.1.4"), 6000);

//	OmnTrace << "Sent 1000" << endl;
}


bool
OmnKernelInterface::start()
{
	int i;

	testSend();

	OmnCommListenerPtr thisPtr(this, false);
	OmnTcpListenerPtr tcpListener(this, false);

	for (i=0; i<mUdpIndex; i++)
	{
		mUdpComm[i] = OmnNew OmnUdpComm(mUdpAddr[i], 
			mUdpPort[i], "KernelInterface");
		mUdpComm[i]->startReading(thisPtr);
	}

	for (i=0; i<mTcpIndex; i++)
	{
		mTcpComm[i] = OmnNew OmnTcpCommSvr(mTcpAddr[i], 
			mTcpPort[i], "KernelInterface");
		mTcpComm[i]->startReading(tcpListener);
	}

	return true;
}


bool
OmnKernelInterface::stop()
{
	return true;
}


OmnKernelInterface::~OmnKernelInterface()
{
}


OmnRslt
OmnKernelInterface::config(const OmnXmlParserPtr &conf)
{
	if (!conf)
	{
		return true;
	}

    OmnXmlItemPtr def = conf->tryItem("KernelApiConfig");
    if (!def)
    {
		return true;
    }

	OmnXmlItemPtr udpConfig = def->tryItem("UdpConfig");
	if (udpConfig)
	{
		// 
		// Need to start the UDP connection
		//
		udpConfig->reset();
		mUdpIndex = 0;
		while (udpConfig->hasMore() && mUdpIndex < eMaxUdpConns)
		{
			OmnXmlItemPtr udpInfo = udpConfig->next();
			OmnIpAddr addr = udpInfo->getIpAddr("Addr", OmnIpAddr::eInvalidIpAddr);
			int port = udpInfo->getInt("Port", -1);
			if (!addr.isValid() || port <= 0)
			{
				OmnAlarm << "Invalid UDP connection: " 
					<< udpInfo->toString() << enderr;
			}
			else
			{
				mUdpAddr[mUdpIndex] = addr;
				mUdpPort[mUdpIndex] = port;
				mUdpIndex++;
			}
		}
    }

	OmnXmlItemPtr tcpConfig = def->tryItem("TcpConfig");
	if (tcpConfig)
	{
		// 
		// Need to start the UDP connection
		//
		tcpConfig->reset();
		mTcpIndex = 0;
		while (tcpConfig->hasMore() && mTcpIndex < eMaxTcpConns)
		{
			OmnXmlItemPtr tcpInfo = tcpConfig->next();
			OmnIpAddr addr = tcpInfo->getIpAddr("Addr", OmnIpAddr::eInvalidIpAddr);
			int port = tcpInfo->getInt("Port", -1);
			if (!addr.isValid() || port <= 0)
			{
				OmnAlarm << "Invalid TCP connection: " 
					<< tcpInfo->toString() << enderr;
			}
			else
			{
				mTcpAddr[mTcpIndex] = addr;
				mTcpPort[mTcpIndex] = port;
				mTcpIndex++;
			}
		}
    }

	return true;
}


bool		
OmnKernelInterface::msgRead(const OmnConnBuffPtr &buff)
{
	return true;
}


OmnString	
OmnKernelInterface::getCommListenerName() const
{
	return "KernelApiComm";
}


OmnString	
OmnKernelInterface::getTcpListenerName() const
{
	return "KernelApiComm";
}


void
OmnKernelInterface::msgRecved(const OmnConnBuffPtr &buff,
							  const OmnTcpClientPtr &conn)
{
	return;
}


