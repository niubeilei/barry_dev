////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpAppServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "TcpAppServer/TcpAppServer.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Util/IpAddr.h"
#include "Util/SerialFrom.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"



AosTcpAppServer::AosTcpAppServer()
:
mFinished(false),
mUsePrivateLenthType(false)
{
}


AosTcpAppServer::~AosTcpAppServer()
{
}

OmnString	
AosTcpAppServer::getTcpListenerName() const
{
	return "TcpAppServer";
}

bool
AosTcpAppServer::config(const OmnXmlItemPtr &conf,const OmnIpAddr &localIP)
{
	// 
	// Start the tcp app server, which is defined by
	// 		<TcpAppServer>
	// 			<LocalAddr>
	// 			<LocalPort>
	// 			<LocalNumPorts>
	// 			<LengthType>
	// 		</TcpAppServer>
	//
	OmnIpAddr ipaddr = conf->getIpAddr(OmnXmlItemName::eLocalAddr, OmnIpAddr::eInvalidIpAddr);
	if (!ipaddr.isValid())
	{
		if(!localIP.isValid())
		{
			OmnAlarm << "Invalid NIID: "
				<< conf->toString()
				<< enderr;
			return false;
		}else
		{
			ipaddr = localIP;
		}
	}

	int localPort = conf->getInt(OmnXmlItemName::eLocalPort, -1);
	if (localPort < 0)
	{
		OmnAlarm << "Invalid local port: " << localPort 
			<< ". " << conf->toString() << enderr;
		return false;
	}

	int numPorts = conf->getInt(OmnXmlItemName::eLocalNumPorts, 1);

	OmnString str = conf->getStr(OmnXmlItemName::eLengthType, 
			"LengthIndicator");
	AosTcpLengthType lenType = AosConvertTcpLenType(str);
	if (lenType == eAosTLT_InvalidLenType)
	{
		OmnAlarm << "Incorrect length type: " 
			<< conf->toString() << enderr;
		return false;
	}

	OmnTrace << "To create NmsServer: " << ipaddr.toString()
		<< ":" << localPort << ":" << numPorts << endl;

	mServer = OmnNew OmnTcpServer(ipaddr, 
				localPort, numPorts, getTcpListenerName(), lenType);

	return true;
}


bool
AosTcpAppServer::start()
{
	OmnString err;
	if (!mServer->connect(err))
	{
		OmnAlarm << "Failed to bind the tcp app server" 
			<< enderr;
		return false;
	}

	if(mListener)
	{
		OmnTcpListenerPtr thisPtr(this, false);
		mServer->setListener(thisPtr);
	}
	else if(mReader)
	{
		OmnTcpMsgReaderPtr thisPtr(this, false);
		mServer->setReader(thisPtr);
	}
	mServer->startReading();
	return true;
} 


bool
AosTcpAppServer::stop()
{
	OmnString err;
/*	if (!mServer->closeConn(err))
	{
		OmnAlarm << "Failed to disconnect the tcp app server" 
			<< enderr;
		return false;
	}
*/	mFinished = true;
	return true;
}



	
OmnIpAddr
AosTcpAppServer::getLocalIpAddr() const
{
	if (mServer.isNull())
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Server is null" << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	return mServer->getLocalIpAddr();
}

int
AosTcpAppServer::getLocalPort() const
{
	if (mServer.isNull())
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Server is null" << enderr;
		return 0;
	}

	return mServer->getLocalPort();
}

void		
AosTcpAppServer::setLocalIpAddr(const OmnIpAddr addr)
{
	if (mServer.isNull())
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Server is null" << enderr;
	}

	mServer->setLocalIpAddr(addr);
	return;
}


void		
AosTcpAppServer::setLocalPort(const int port)
{
	if (mServer.isNull())
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Server is null" << enderr;
	}
	mServer->setLocalPort(port);
	return;
}



void		
AosTcpAppServer::msgRecved(const OmnConnBuffPtr &buff,
		  					   const OmnTcpClientPtr &conn)
{
	// for Tcp Application Server (parent class),just return the package;
	if(mListener.notNull())
	{
		return mListener->msgRecved(buff, conn);
	}
}



OmnRslt		
AosTcpAppServer::writeTo(const char *data, const int length, const OmnTcpClientPtr client)
{
	return mServer->writeTo(data,length,client);
}

OmnRslt		
AosTcpAppServer::writeTo(const OmnString &msg, const OmnTcpClientPtr client)
{
	return mServer->writeTo(msg,client);
}


void
AosTcpAppServer::setListener(const OmnTcpListenerPtr &listener)
{
	if (mListener)
	{
		OmnAlarm << "To set TCP Server listener, but there is already one!" 
			<< enderr;
	}

	mListener = listener;
}


void
AosTcpAppServer::setReader(const OmnTcpMsgReaderPtr &reader)
{
	if (mReader)
	{
		OmnAlarm << "To set TCP Server listener, but there is already one!" 
			<< enderr;
	}

	mReader = reader;
}

OmnString	
AosTcpAppServer::getTcpMsgReaderName() const
{
	return "TcpAppServer";
}

int			
AosTcpAppServer::nextMsg(const OmnConnBuffPtr &buff,
		   const OmnTcpClientPtr &conn)
{
	// for Tcp Application Server (parent class),just return the package;
	if(mReader.notNull())
	{
		return mReader->nextMsg(buff,conn);
	}
	return 0;
}

void		
AosTcpAppServer::connClosed(const OmnTcpClientPtr &conn)
{
	if(mReader.notNull())
	{
		return mReader->connClosed(conn);
	}
	return;
}


void    	
AosTcpAppServer::connAccepted(const OmnTcpClientPtr &conn)
{
	if(mReader.notNull())
	{
		return mReader->connAccepted(conn);
	}
	return;
}


bool		
AosTcpAppServer::getClientByRemoteAddr(const OmnIpAddr addr, const int port, OmnTcpClientPtr & client)
{
	return mServer->getClientByRemoteAddr(addr,port,client);
}

void				
AosTcpAppServer::setConnFlag(const bool connFlag)
{
	mServer->setConnFlag(connFlag);
	return;
}


