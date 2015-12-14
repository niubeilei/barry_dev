////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FactoryDefault.cpp
// Description:
//   
//
// Modification History:
// 12/07/2006: Created by jzz
// 
////////////////////////////////////////////////////////////////////////////
#include "FactoryDefault/FactoryDefault.h"

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "aosUtil/Tracer.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"

AosFactoryDefault::AosFactoryDefault()
{
}


AosFactoryDefault::~AosFactoryDefault()
{
}

bool
AosFactoryDefault::setToDefault(OmnString &rslt)
{
	
	return true;
}

bool 
AosFactoryDefault::connectToServer(const OmnTcpClientPtr &cliConn)
{
	// 
	// 1. Establish the connection
	//
	
	OmnString err;
	bool connected = false;
	int tries = 5;
	while(tries)
	{
		if(cliConn->connect(err))
		{
//			cout << "Connected!" << endl;
			connected = true;
			break;
		}
		tries--;
		sleep(1);
	}
	if(!connected)
	{
		return false;
	}
	return true;
}

bool 
AosFactoryDefault::login(OmnTcpClientPtr &cliConn,const OmnString &username, const OmnString &password,OmnString & sessionId)
{	//
	// login
	//
	int sec = 10 ;//eAosSinglePmi_CliTimeOut;
	bool timeout = false;
	bool connBroken = false;
	OmnString sendBuff;
	sendBuff << "00000000user login:" 
		<< username << ":" 
		<< password 
		<< (char)eSignature1 
		<< (char)eSignature2;
	int sendLen = sendBuff.length();
	cliConn->writeTo(sendBuff, sendLen);
	OmnConnBuffPtr revBuff = new OmnConnBuff();
	if (!cliConn->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		OmnAlarm << "cannot successfully get login resp from cli server" << enderr;
		return false;
	}
	if(revBuff && (! (*revBuff->getBuffer() == '\r' ||*revBuff->getBuffer() == '\n')))	
	{
		// get login response.
		if(revBuff->getDataLength() <= eSessionLength + 2)
		{
			OmnAlarm << "login resp error:" << revBuff->getDataLength() << enderr;
			return false;
		}		
		
		sessionId = OmnString(revBuff->getBuffer(),eSessionLength);
		OmnString response(revBuff->getBuffer()+eSessionLength,
						   revBuff->getDataLength()-eSessionLength -2);
		if(response == "user not exist or passwd error")
		{
			OmnAlarm << "login resp error" << enderr;
			return false;
		}		
	}
	
	OmnString changeLevelCmd = sessionId;
	changeLevelCmd << "user level set config"
		<< (char)eSignature1 
		<< (char)eSignature2;
	sendLen = changeLevelCmd.length();
	cliConn->writeTo(changeLevelCmd, sendLen);
	revBuff = new OmnConnBuff();
	if (!cliConn->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		OmnAlarm << "cannot successfully get change level resp from cli server" << enderr;
		return false;
	}
	if(revBuff && (! (*revBuff->getBuffer() == '\r' ||*revBuff->getBuffer() == '\n')))	
	{
		// get login response.
		if(revBuff->getDataLength() < eSessionLength + 2)
		{
			OmnAlarm << "change level error:"  << revBuff->getDataLength() << enderr;
			return false;
		}		
		
		OmnString response(revBuff->getBuffer()+eSessionLength,
						   revBuff->getDataLength()-eSessionLength -2);

		if(response == "can not set as configer, another one occupies")
		{
			OmnAlarm << "another config level is online."  << enderr;
			return false;
		}		
		
	}
	return true;
}

bool 
AosFactoryDefault::sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer,const OmnString &sessionId, const OmnString &username)
{
	OmnString errmsg;
	OmnRslt rslt;
	int sec = 10 ;//eAosSinglePmi_CliTimeOut;
	bool timeout = false;
	bool connBroken;
	
	if (!dstServer->isConnGood())
	{
		dstServer->closeConn();
		dstServer->connect(errmsg);
		if (!dstServer->isConnGood())
		{
			//alarm and return;
			OmnAlarm << "Failed to process connection" << rslt.getErrmsg() << enderr;
			return false;
		}
	}
	OmnString sendBuff = sessionId;
	sendBuff << cmd
				<< (char)eSignature1 
				<< (char)eSignature2;
	int sendLen = sendBuff.length();

	dstServer->writeTo(sendBuff, sendLen);

	OmnConnBuffPtr revBuff = new OmnConnBuff();
	if (!dstServer->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		//alarm and return
		showCmd(cmd,revBuff,sessionId,dstServer);
		OmnAlarm << "cannot login to cli server" << rslt.getErrmsg() << enderr;
		return false;
	}
	if(revBuff)	
	{
		showCmd(cmd,revBuff,sessionId,dstServer);
	}
	
	return true;
}

void
AosFactoryDefault::showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff,const OmnString &sessionId ,const OmnTcpClientPtr &dstServer)
{
	if(revBuff)
	{
		if(revBuff->getDataLength() < eSessionLength + 2)
		{
			cout << "response length not right:";
			cout << OmnString(revBuff->getBuffer(),revBuff->getDataLength()) << endl;
		}
		else
		{
			cout << OmnString(revBuff->getBuffer() + eSessionLength,revBuff->getDataLength() - eSessionLength - 2) << endl;
		}		
	}
	else
	{
		if(!connectToServer(dstServer))
		{
			cout << "Connection lost!" << endl;
			exit(-1);
		}
	}
}
