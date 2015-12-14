////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NmsServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "NMS/NmsServer.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Network/Network.h"
#include "NMS/Nms.h"
#include "NMS/NmsSvrObj.h"
#include "Util/IpAddr.h"
#include "Util/SerialFrom.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"



OmnNmsServer::OmnNmsServer()
:
mFinished(false)
{
}


OmnNmsServer::~OmnNmsServer()
{
}


bool
OmnNmsServer::config(const OmnXmlItemPtr &conf)
{
	// 
	// Start the NMS server, which is defined by
	// 		<NmsServer>
	// 			<LocalNiid>
	// 			<LocalPort>
	// 			<LocalNumPorts>
	// 			<LengthType>
	// 		</NmsServer>
	//
	/* Chen Ding, 06/02/2011
	int niid = conf->getInt(OmnXmlItemName::eLocalNiid, -1);
	OmnIpAddr ipaddr = OmnNetworkSelf->getIpAddr(niid);
	if (!ipaddr.isValid())
	{
		OmnAlarm << "Invalid NMS NIID: "
			<< conf->toString()
			<< enderr;
		return false;
	}

	int localPort = conf->getInt(OmnXmlItemName::eLocalPort, -1);
	if (localPort < 0)
	{
		OmnAlarm << "Invalid NMS local port: " << localPort 
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
				localPort, numPorts, "NMS", lenType);

	OmnString err;
	if (!mServer->connect(err))
	{
		OmnAlarm << "Failed to start NMS TCP Server" 
			<< ". " << conf->toString() << enderr;
		return false;
	}
	*/
	return true;
}


bool
OmnNmsServer::start()
{
	OmnTcpClientPtr conn;
	OmnConnBuffPtr buf;
	while (!mFinished)
	{
		mServer->readFrom(buf, conn);
		procMsg(buf, conn);
	}
	return true;
} 


bool
OmnNmsServer::stop()
{
	mFinished = true;
	return true;
}


void 
OmnNmsServer::procMsg(const OmnConnBuffPtr &buf, const OmnTcpClientPtr &conn)
{
/*    if (buf.isNull())
	{
		//
		// Did not read anything. This should not happen.
		//
		OmnCommWarn << OmnErrId::eWarnCommError
			<< "Did not read anything from NMS: "
			<< enderr;
		return;
	}
	
	switch (OmnSysObj::getMsgId(buf))
	{	
	case OmnMsgId::eSmNmsCmd:
		 procNmsCommand(buf, conn);
		 break;

	case OmnMsgId::eSmEpCmd:
		 procEpCommand(buf, conn);
		 break;

	case OmnMsgId::eSmChgState:
		 procChangeState(buf, conn);
		 break;

	default:
		OmnWarn << OmnErrId::eWarnHeartbeatError
			<< "Received is not a SmNmsCmd: " 
			<< OmnSysObj::getMsgId(buf) << enderr;
		return;
	}
*/
}

	
void
OmnNmsServer::procChangeState(const OmnConnBuffPtr &buf,
							  const OmnTcpClientPtr &conn)
{
/*
	OmnSerialFrom s(buf);
	s.popMsgId();
	try
	{
		OmnSmChgState msg(s);

		OmnTraceHB << "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
			<< msg.toString()
			<< "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

		OmnNmsSelf->procStateHb(msg);
	}

	catch (const OmnExcept &e)
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Failed to process message: " 
			<< e.toString() << enderr;
	}
*/
}


void
OmnNmsServer::procNmsCommand(const OmnConnBuffPtr &buf, 
						   const OmnTcpClientPtr &conn)
{
/*
	//
	// Construct the message
	//
	OmnSerialFrom s(buf);
	s.popMsgId();
	OmnString rslt;
	bool status;
	try
	{
		OmnSmNmsCmd msg(s);

		OmnTrace << "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"
			<< msg.toString()
			<< "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

		if (!mCallback.isNull() && 
			!mCallback->procNmsCommand(msg, conn, status, rslt))
		{
			switch (msg.getCmdId())
			{
			case OmnSmNmsCmd::eCheckMemory:
			 	 status = checkMemory(conn, rslt);
			 	 break;

			case OmnSmNmsCmd::eStopProg:
			 	 mFinished = true;
			 	 OmnNmsSelf->stopProg();
			 	 rslt = "Stream Router stopped";
			 	 status = true; 
			 	 break;

			case OmnSmNmsCmd::eCheckProgStatus:
				 status = true;
			 	 rslt << "Program:    " << OmnNmsSelf->getProgName() << "\n"
				  	<< "Start Time: " << OmnNmsSelf->getStartTime() << "\n"
				  	<< OmnNmsSelf->getVersion() << "\n"
				  	<< OmnNmsSelf->getBuildNo() << "\n"
				  	<< OmnNmsSelf->getBuildTime() << "\n";
				  	//<< "Number of calls processed: " 
				  	//<< OmnMgcpConn::getObjCounts();
			  	 break;

			//case OmnSmNmsCmd::eDownloadAlgData:
			//	 status = true;
			//	 rslt << "To download ALG Data";
			//	 OmnSpnrNmsSelf->downloadAlgData();
			//	 break;

			default:
			 	 rslt << "Command not defined: " << msg.getCmdId();
				 status = false;
			 	 break;
			}
		}

		OmnSmNmsRespPtr resp = OmnNew OmnSmNmsResp(status, rslt);
		OmnTrace << "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"
			<< resp->toString()
			<< "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << endl;
		conn->sendMsgTo(resp);
		return;
	}

	catch (const OmnExcept &e)
	{
		//
		// This means the message wasn't constructed right.
		//
		OmnAlarm << OmnErrId::eAlarmHeartbeatError
			<< "Read an incorrect message from a heartbeat server"
			<< enderr;
		return;
	}
*/
}
	

bool
OmnNmsServer::checkMemory(const OmnTcpClientPtr &conn, OmnString &rslt)
{
	rslt << "Object creation: \n"
		 << OmnNmsSelf->getObjTotals();
	return true;
}


void
OmnNmsServer::setNmsSvrObj(const OmnNmsSvrObjPtr &obj)
{
	mCallback = obj;
}


OmnIpAddr
OmnNmsServer::getLocalIpAddr() const
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
OmnNmsServer::getLocalPort() const
{
	if (mServer.isNull())
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Server is null" << enderr;
		return 0;
	}

	return mServer->getLocalPort();
}


