////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AttackMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "AttackMgr/AttackMgr.h"

#include "AttackMgr/Attacker.h"
#include "AttackMgr/TcpAttacker.h"
#include "AttackMgr/UdpAttacker.h"
#include "AttackMgr/IcmpAttacker.h"
#include "aos/aosKernelApi.h"
#include "CliClient/CliClient.h"
#include "CliClient/ModuleOprId.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "TcpAppServer/TcpAppServer.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/Ptrs.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include <netdb.h>

OmnSingletonImpl(AosAttackMgrSingleton,
				 AosAttackMgr,
				 AosAttackMgrSelf,
				 "AosAttackMgr");

AosAttackMgr::AosAttackMgr()
{
}


AosAttackMgr::~AosAttackMgr()
{
}



bool
AosAttackMgr::config(const OmnXmlParserPtr &conf)
{
	return true;
}

bool
AosAttackMgr::start()
{
	return true;	
}

bool
AosAttackMgr::stop()
{
	return true;
}



bool			
AosAttackMgr::setCliClient(const AosCliClientPtr cliClient)
{
	mCliClient = cliClient;
	return true;
}

int 
AosAttackMgr::addAttacker(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	OmnString protocol = parms->mStrings[0];
	OmnString strsip = parms->mStrings[0];
	int sport = parms->mIntegers[0];
	OmnString strdip = parms->mStrings[0];
	int dport = parms->mIntegers[1];
	int packetNum = parms->mIntegers[2];
	int interval = parms->mIntegers[3];
	
	OmnIpAddr sip(strsip);
	OmnIpAddr dip(strdip);
	
	OmnString resp;

	if(protocol == "SYN")
	{
		AosTcpAttackerPtr tcpatt = OmnNew AosTcpAttacker(sip,sport,dip,dport,packetNum,interval);
		tcpatt->setStyle(AosAttacker::eSyn);
		AosAttackerPtr att = tcpatt;
		att->start();
		AosAttackMgr::getSelf()->mAttackList.append(att);
	}
	else if(protocol == "UDP")
	{
		AosAttackerPtr att = OmnNew AosUdpAttacker(sip,sport,dip,dport,packetNum,interval);
		att->start();
		AosAttackMgr::getSelf()->mAttackList.append(att);
	}
	else if(protocol == "ICMP")
	{
		AosAttackerPtr att = OmnNew AosTcpAttacker(sip,sport,dip,dport,packetNum,interval);
		att->start();
		AosAttackMgr::getSelf()->mAttackList.append(att);
	}
	
	
	if(*optlen < 50)
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}


int 
AosAttackMgr::removeAttacker(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	OmnString protocol = parms->mStrings[0];
	OmnString strsip = parms->mStrings[0];
	int sport = parms->mIntegers[0];
	OmnString strdip = parms->mStrings[0];
	int dport = parms->mIntegers[1];
	int packetNum = parms->mIntegers[2];
	int interval = parms->mIntegers[3];
	
	
	OmnIpAddr sip(strsip);
	OmnIpAddr dip(strdip);

	OmnVList<AosAttackerPtr> *list = &(AosAttackMgr::getSelf()->mAttackList);
	
	OmnString resp = "not found";
	list->reset();
	while(list->hasMore())
	{
		if(list->crtValue()->isSame(protocol,sip,sport,dip,dport,packetNum,interval))
		{
			list->crtValue()->stop();
			list->eraseCrt();
			resp = "ok";
			break;
		}		
		list->next();
	}
	
	//
	// set response
	//
	if(*optlen < 50)
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}


bool				
AosAttackMgr::registerCliCallbackFunc()
{
	if(!mCliClient)
	{
		return false;
	}
	mCliClient->addCliFunc(addAttacker,AosModuleOprId::eAttackMgrAddAttacker);
	mCliClient->addCliFunc(removeAttacker,AosModuleOprId::eAttackMgrRemoveAttacker);
	return true;
}

