////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BouncerMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "BouncerMgr/BouncerMgr.h"

#include "aos/aosKernelApi.h"
#include "BouncerMgr/TcpBouncerEntry.h"
#include "BouncerMgr/UdpBouncerEntry.h"
#include "BouncerMgr/BouncerEntry.h"
#include "BouncerMgr/BouncerConn.h"
//#include "BouncerMgr/TcpBouncerSpeedLimitListener.h"
#include "CliClient/CliClient.h"
#include "CliClient/ModuleOprId.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "TestDrivers/TcpBouncer/TcpBouncer.h"
#include "TestDrivers/TcpBouncer/Ptrs.h"
#include "TcpAppServer/TcpAppServer.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/BandWidthMgr.h"
#include "UtilComm/TcpBouncer.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/Ptrs.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include <netdb.h>

OmnSingletonImpl(AosBouncerMgrSingleton,
				 AosBouncerMgr,
				 AosBouncerMgrSelf,
				 "AosBouncerMgr");


AosBouncerMgr::AosBouncerMgr()
:
mLock(OmnNew OmnMutex())
{
//	mBandWidthMgr = OmnNew AosBandWidthMgr();
}


AosBouncerMgr::~AosBouncerMgr()
{
}



bool
AosBouncerMgr::config(const OmnXmlParserPtr &conf)
{
	return true;
}

bool
AosBouncerMgr::start()
{
//	mBandWidthMgr->start();
	return true;	
}

bool
AosBouncerMgr::stop()
{
	return true;
}



bool			
AosBouncerMgr::setCliClient(const AosCliClientPtr cliClient)
{
	mCliClient = cliClient;
	return true;
}

int 
AosBouncerMgr::addBouncer(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	OmnString protocol = parms->mStrings[0];
	OmnIpAddr senderIp(parms->mIntegers[0]);
	int senderPort = parms->mIntegers[1];
	OmnIpAddr recverIp(parms->mIntegers[2]);
	int recverPort = parms->mIntegers[3];

cout<< "add bouncer: " << protocol << ":" 
			<< 	senderIp.toString() << ":" << senderPort << ":"	
			<<  recverIp.toString() << ":" << recverPort << endl;
	OmnTrace << "add bouncer: " << protocol << ":" 
			<< 	senderIp.toString() << ":" << senderPort << ":"	
			<<  recverIp.toString() << ":" << recverPort << endl;
	OmnString resp;
	int rtNum = AosBouncerMgr::getSelf()->addBouncer(protocol,
													 senderIp,
													 senderPort,
													 recverIp,
													 recverPort,
													 resp);
	if(*optlen < 50)
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();

	return rtNum;
}

int 
AosBouncerMgr::addBouncer(const OmnString &protocol,
						  const OmnIpAddr &senderIp,
						  const int		  &senderPort,
						  const OmnIpAddr &recverIp,
						  const int		  &recverPort,
						  OmnString		  &resp)
{						  
	// searching from entry list
	AosBouncerEntryPtr entry;
	bool found = false;
	mLock->lock();
	mBouncerEntryList.reset();
	while(mBouncerEntryList.hasMore())
	{
		entry = mBouncerEntryList.crtValue();
		if(entry->entryExist(recverIp,recverPort,protocol))
		{
			found = true;
			break;
		}
		mBouncerEntryList.next();
	}
	
	if(!found)
	{
		if(protocol == "tcp")
		{
			entry = OmnNew AosTcpBouncerEntry(recverIp,recverPort);
		}
		else // udp
		{
			entry = OmnNew AosUdpBouncerEntry(recverIp,recverPort);		
		}
		mBouncerEntryList.append(entry);
		entry->start();
	}	
	entry->addConn(senderIp,senderPort);
	mLock->unlock();
	return 0;
}
		


int 
AosBouncerMgr::removeBouncer(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	OmnString protocol = parms->mStrings[0];
	OmnIpAddr senderIp(parms->mIntegers[0]);
	int senderPort = parms->mIntegers[1];
	OmnIpAddr recverIp(parms->mIntegers[2]);
	int recverPort = parms->mIntegers[3];
	
	OmnString resp;
	int rtNum = AosBouncerMgr::getSelf()->removeBouncer(protocol,
													 senderIp,
													 senderPort,
													 recverIp,
													 recverPort,
													 resp);
	if(*optlen < 50)
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();

	return rtNum;
}

int 			
AosBouncerMgr::removeBouncer(const OmnString &protocol,
							 const OmnIpAddr &senderIp,
							 const int		  &senderPort,
							 const OmnIpAddr &recverIp,
							 const int		  &recverPort,
							 OmnString		  &resp)
{
	// searching from entry list
	AosBouncerEntryPtr entry;
	bool found = false;
	mLock->lock();
	mBouncerEntryList.reset();
	while(mBouncerEntryList.hasMore())
	{
		entry = mBouncerEntryList.crtValue();
		if(entry->entryExist(recverIp,recverPort,protocol))
		{
			if(!entry->removeConn(senderIp,senderPort))
			{
				OmnAlarm << "Failed to remove the connection: " 
					<< senderIp.toString() << ":" << senderPort << enderr;
				// not found , found == false;
				break;
			}
			// remove current entry if it contains 0 conns.
			if(entry->connNum() <= 0)
			{
				mBouncerEntryList.eraseCrt();
			}
			entry->stop();
			found = true;
			break;
		}
		mBouncerEntryList.next();
	}
	mLock->unlock();
	
	if(!found)
	{
		resp = "can not find the entry";
	}

	return 0;
}

int 
AosBouncerMgr::setPinholeAttack(char *data, 
								unsigned int *optlen, 
								struct aosKernelApiParms *parms, 
								char *errmsg, 
								const int errlen)
{
	return 0;
}
							
int 
AosBouncerMgr::setBandWidth(char *data, 
							unsigned int *optlen, 
							struct aosKernelApiParms *parms, 
							char *errmsg, 
							const int errlen)
{
	return 0;
}

int 
AosBouncerMgr::getBandWidth(char *data, 
							unsigned int *optlen, 
							struct aosKernelApiParms *parms, 
							char *errmsg, 
							const int errlen)
{
	OmnString protocol = parms->mStrings[0];
	OmnIpAddr senderIp(parms->mIntegers[0]);
	int senderPort = parms->mIntegers[1];
	OmnIpAddr recverIp(parms->mIntegers[2]);
	int recverPort = parms->mIntegers[3];

	OmnString resp;
	int rtNum = AosBouncerMgr::getSelf()->getBandWidth(protocol,
													 senderIp,
													 senderPort,
													 recverIp,
													 recverPort,
													 resp);
	if(*optlen < 50)
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();

	return rtNum;
}


int 			
AosBouncerMgr::getBandWidth(const OmnString &protocol,
							const OmnIpAddr &senderIp,
							const int		  &senderPort,
							const OmnIpAddr &recverIp,
							const int		  &recverPort,
							OmnString		  &resp)
{
	int crtBandwidth = -1;	
	int avgBandwidth = -1;	
	// searching from entry list
	AosBouncerEntryPtr entry;
	bool found = false;
	mLock->lock();
	mBouncerEntryList.reset();
	while(mBouncerEntryList.hasMore())
	{
		entry = mBouncerEntryList.crtValue();
		if(entry->entryExist(recverIp,recverPort,protocol))
		{
			if(!entry->getBandwidth(senderIp,
									senderPort,
									crtBandwidth,
									avgBandwidth))
			{
				// not found , found == false;
				break;
			}
			resp << "current bandwidth " << crtBandwidth 
				<< " average bandwidth " << avgBandwidth;
			
			found = true;
			break;
		}
		mBouncerEntryList.next();
	}
	mLock->unlock();
	
	if(!found)
	{
		resp = "can not find the entry";
	}
	return 0;
}

int 
AosBouncerMgr::resetStat(char *data, 
							unsigned int *optlen, 
							struct aosKernelApiParms *parms, 
							char *errmsg, 
							const int errlen)
{
	OmnString protocol = parms->mStrings[0];
	OmnIpAddr senderIp(parms->mIntegers[0]);
	int senderPort = parms->mIntegers[1];
	OmnIpAddr recverIp(parms->mIntegers[2]);
	int recverPort = parms->mIntegers[3];
	
	OmnString resp;
	int rtNum = AosBouncerMgr::getSelf()->resetStat(protocol,
													 senderIp,
													 senderPort,
													 recverIp,
													 recverPort,
													 resp);
	if(*optlen < 50)
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();

	return rtNum;
}


int 			
AosBouncerMgr::resetStat(const OmnString &protocol,
						 const OmnIpAddr &senderIp,
						 const int		  &senderPort,
						 const OmnIpAddr &recverIp,
						 const int		  &recverPort,
						 OmnString		  &resp)
{
	// searching from entry list
	AosBouncerEntryPtr entry;
	bool found = false;
	mLock->lock();
	mBouncerEntryList.reset();
	while(mBouncerEntryList.hasMore())
	{
		entry = mBouncerEntryList.crtValue();
		if(entry->entryExist(recverIp,recverPort,protocol))
		{
			if(!entry->resetConnStat(senderIp,senderPort))
			{
				// not found , found == false;
				break;
			}
			// reset ok
			found = true;
			break;
		}
		mBouncerEntryList.next();
	}
	mLock->unlock();
	
	if(!found)
	{
		resp = "can not find the entry";
	}
	return 0;
}

bool				
AosBouncerMgr::registerCliCallbackFunc()
{
	if(!mCliClient)
	{
		return false;
	}
	mCliClient->addCliFunc(addBouncer,AosModuleOprId::eBouncerMgrAddBouncer);
	mCliClient->addCliFunc(removeBouncer,AosModuleOprId::eBouncerMgrRemoveBouncer);

	mCliClient->addCliFunc(getBandWidth,AosModuleOprId::eBouncerMgrGetBandwidth);
	mCliClient->addCliFunc(resetStat,AosModuleOprId::eBouncerMgrResetStat);

//	mCliClient->addCliFunc(setPinholeAttack,AosModuleOprId::eBouncerMgrSetPinHoleAttack);
//	mCliClient->addCliFunc(removeBouncer,AosModuleOprId::eBouncerMgrSetBandWidth);
	return true;
}

                                                                     
