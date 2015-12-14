////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamSimuMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "StreamSimu/StreamSimuMgr.h"

#include "aos/aosKernelApi.h"
#include "StreamSimu/TcpStreamSimuEntry.h"
#include "StreamSimu/UdpStreamSimuEntry.h"
#include "StreamSimu/StreamSimuEntry.h"
#include "StreamSimu/StreamSimuConn.h"
//#include "StreamSimuMgr/TcpStreamSimuSpeedLimitListener.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/Ptrs.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include "Porting/Sleep.h"

OmnSingletonImpl(AosStreamSimuMgrSingleton,
				 AosStreamSimuMgr,
				 AosStreamSimuMgrSelf,
				 "AosStreamSimuMgr");


AosStreamSimuMgr::AosStreamSimuMgr()
:
mLock(OmnNew OmnMutex()),
mCallback(0)
{
//	mBandWidthMgr = OmnNew AosBandWidthMgr();
}


AosStreamSimuMgr::~AosStreamSimuMgr()
{
}



bool
AosStreamSimuMgr::config(const OmnXmlParserPtr &conf)
{
	AosStreamSimuEntry::initContent();
	return true;
}

bool
AosStreamSimuMgr::start()
{
//	mBandWidthMgr->start();
	return true;	
}

bool
AosStreamSimuMgr::stop()
{
	return true;
}



bool 			
AosStreamSimuMgr::addStream(const OmnString 					&protocol,
						  	const OmnIpAddr 					&senderIp,
						  	const int		  					senderPort,
						  	const OmnIpAddr 					&recverIp,
						  	const int		  					recverPort,
						  	const int		  					bandwidth,
						  	const int		  					packetSize,
                          	const AosStreamType::E 				streamType,
                     	  	const int                          	maxBw,
                          	const int                          	minBw)

{						  
	// searching from entry list
	AosStreamSimuEntryPtr entry;
	bool found = false;
	mLock->lock();
	mStreamSimuEntryList.reset();
OmnTrace << "To add stream: " << senderIp.toString() << ":" << senderPort << " -> " << recverIp.toString() << ":" << recverPort << endl;
	while(mStreamSimuEntryList.hasMore())
	{
		entry = mStreamSimuEntryList.crtValue();
		if(entry->entryExist(senderIp,senderPort,protocol))
		{
			found = true;
			break;
		}
		mStreamSimuEntryList.next();
	}
	
	if(!found)
	{
OmnTrace << "Not found" << endl;
		if(protocol == "tcp")
		{
OmnTrace << "1" << endl;
			entry = new AosTcpStreamSimuEntry(senderIp,senderPort);
			entry->addConn(recverIp,
						   recverPort,
						   bandwidth,
						   packetSize,
						   streamType,
					   	   maxBw,
					       minBw);
		}
		else // udp
		{
OmnTrace << "1" << endl;
			entry = new AosUdpStreamSimuEntry(senderIp,senderPort);		
		}
OmnTrace << "1" << endl;
		mStreamSimuEntryList.append(entry);
OmnTrace << "1" << endl;
		entry->start();
OmnTrace << "1" << endl;
	}	
	
OmnTrace << "1" << endl;
	if(protocol == "udp")
	{
OmnTrace << "1" << endl;
		entry->addConn(recverIp,
					   recverPort,
					   bandwidth,
					   packetSize,
					   streamType,
					   maxBw,
					   minBw);
	}
OmnTrace << "1" << endl;
	mLock->unlock();
OmnTrace << "1" << endl;
	return 0;
}
		


bool 			
AosStreamSimuMgr::removeStream(const OmnString &protocol,
					  const OmnIpAddr &senderIp,
					  const int		  senderPort,
					  const OmnIpAddr &recverIp,
					  const int		  recverPort)
{
	// searching from entry list
	AosStreamSimuEntryPtr entry;
	bool found = false;
	mLock->lock();
	mStreamSimuEntryList.reset();
	while(mStreamSimuEntryList.hasMore())
	{
		entry = mStreamSimuEntryList.crtValue();
		if(entry->entryExist(senderIp,senderPort,protocol))
		{
			if(!entry->removeConn(recverIp,recverPort))
			{
				// not found , found == false;
				break;
			}
			// remove current entry if it contains 0 conns.
			if(entry->connNum() <= 0)
			{
				mStreamSimuEntryList.eraseCrt();
			}
			entry->stop();
			found = true;
			break;
		}
		mStreamSimuEntryList.next();
	}
	mLock->unlock();
	

	return 0;
}

bool
AosStreamSimuMgr::getBandwidth(const OmnString 	&protocol,
							   const OmnIpAddr 	&senderIp,
							   const int		senderPort,
							   const OmnIpAddr 	&recverIp,
							   const int		recverPort,
							   int			  	&bw,
							   int			  	&packetNum)
{
	// searching from entry list
	AosStreamSimuEntryPtr entry;
	mLock->lock();
	mStreamSimuEntryList.reset();
	while(mStreamSimuEntryList.hasMore())
	{
		entry = mStreamSimuEntryList.crtValue();
		if(entry->entryExist(senderIp,senderPort,protocol))
		{
			mLock->unlock();
			return entry->getBandwidth(recverIp,recverPort,bw,packetNum);
		}
		mStreamSimuEntryList.next();
	}
	mLock->unlock();
	return 0;
}

bool 			
AosStreamSimuMgr::modifyStream(const OmnString 						&protocol,
							   const OmnIpAddr 						&senderIp,
							   const int							senderPort,
							   const OmnIpAddr 						&recverIp,
							   const int		  					recverPort,
							   const int		  					bandwidth,
							   const int		  					packetSize,
	                           const AosStreamType::E 				streamType,
	                     	   const int                          	maxBw,
	                           const int                          	minBw)
{
	// searching from entry list
	AosStreamSimuEntryPtr entry;
	bool found = false;
	mLock->lock();
	mStreamSimuEntryList.reset();
	while(mStreamSimuEntryList.hasMore())
	{
		entry = mStreamSimuEntryList.crtValue();
		if(entry->entryExist(senderIp,senderPort,protocol))
		{
			if(!entry->modifyStream(recverIp,
									recverPort,
									bandwidth,
									packetSize,
									streamType,
									maxBw,
									minBw))
			{
				// not found , found == false;
				break;
			}
			
			found = true;
			break;
		}
		mStreamSimuEntryList.next();
	}
	mLock->unlock();
	
	return 0;
}


bool	
AosStreamSimuMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter AosStreamSimuMgr::threadFunc." << endl;

	AosStreamSimuEntryPtr entry;
    while (state == OmnThrdStatus::eActive)
    {
		OmnSleep(1);

		mLock->lock();
		mStreamSimuEntryList.reset();
		while(mStreamSimuEntryList.hasMore())
		{
			entry = mStreamSimuEntryList.next();
			if (entry->isFinished())
			{
				// 
				// Remove this entry and close the connection
				//
				if (mCallback)
				{
					mCallback(entry);
				}
			}
		}

		mLock->unlock();
	}

	return true;
}


bool
AosStreamSimuMgr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosStreamSimuMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool 			
AosStreamSimuMgr::addStream(const OmnIpAddr 	    &senderIp,
						  	const int		  		senderPort,
						  	const OmnIpAddr 		&recverIp,
						  	const int		  		recverPort,
						  	const AosStreamType::E	streamType)
{
	OmnString proto;
	switch (streamType)
	{
	case AosStreamType::eTelnet:
	case AosStreamType::eFtp:
	case AosStreamType::eSSH:
		 proto = "tcp";
		 break;

	default:
		 OmnAlarm << "Invalid call: " << streamType << enderr;
		 return false;
	}

	return addStream(proto, senderIp, senderPort, recverIp, recverPort, 
				0, 0, streamType, 0, 0);
}

