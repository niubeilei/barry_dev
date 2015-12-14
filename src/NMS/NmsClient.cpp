////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NmsClient.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "NMS/NmsClient.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Network/Network.h"
#include "Util/SerialFrom.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"


extern OmnNetwork *		OmnNetworkSelf;


OmnNmsClient::OmnNmsClient()
{
}


OmnNmsClient::~OmnNmsClient()
{
}


bool
OmnNmsClient::config(const OmnXmlItemPtr &conf)
{
	//
	// Create the TcpClient connection that is used to 
	// communicate with SPNR
	// It assumes the following configuration
	//
	// 	<NmsClients>
	// 		<NmsClient>
	//			<Name> 	
	// 			<Type> 
	//			<MonitoredInstId>
	//			<RemoteIpAddr>
	//			<RemotePort>
	// 			<RemoteNumPorts>
	//			<LocalNiid>
	//		</NmsClient>
	//		...	
	// 	</NmsClients>
	//
	// If <MonitoredInstId> is present, retrieve the remote IP address
	// and port from the database. Otherwise, retrieve the IP 
	// address and port from <RemoteIpAddr> and <RemotePort>
	//
/*	
	conf->reset();
	OmnString err;

	mTcpClts = OmnNew OmnTcpCltGrp();
	OmnConnMsgProcPtr thisPtr(this, false);
	mTcpClts->setMsgProc(thisPtr);

	while (conf->hasMore())
	{
		OmnXmlItemPtr connconf = conf->next();

		//
		// Retrieve the type
		//
		OmnString tstr = connconf->getStr(OmnXmlItemName::eType, "NoType");
		OmnNetEtyType::E type = OmnNetEtyType::nameToEnum(tstr);
		if (type == OmnNetEtyType::eInvalidNetEntity)
		{
			OmnWarn << OmnErrId::eWarnConfigError
				<< "Invalid NMS Client type: " 
				<< connconf->toString()
				<< enderr;
			return false;
		}

		// Retrieve LocalNiid
		int localNiid = connconf->getInt(OmnXmlItemName::eLocalNiid, -1);
		OmnIpAddr localAddr = OmnNetworkSelf->getIpAddr(localNiid);
		if (localAddr == OmnIpAddr::eInvalidIpAddr)
		{
			OmnWarn << OmnErrId::eWarnProgramError
				<< "Incorrect IP address: " 
				<< localAddr.toString() << enderr;
			return false;
		}

		// Retrieve MonitoredInstId
		OmnString instId = connconf->getStr(
			OmnXmlItemName::eMonitoredInstId, "NoId");
		
		OmnIpAddr addr;
		int port;
		int remoteNumPorts = 1;
		if (!(instId == "NoId"))
		{
			// Retrieve the object from the database
			OmnSoHbStatus so;
			so.setInstId(instId);
			if (!so.serializeFromDb())
			{
				OmnWarn << OmnErrId::eWarnProgError
					<< "Failed to retrieve app info from db: "
					<< instId
					<< ". NmsClient not enabled!"
					<< enderr;
				continue;
			}

			OmnTrace << "To create NmsClient: " << tstr << endl;

			addr = so.getNmsIpAddr();
			port = so.getNmsPort();
		}
		else
		{
			// 
			// <MonitoredInstId> is not present. Use <RemoteIpAddr>
			// and <RemotePort>.
			//
			OmnString str = connconf->getStr(
				OmnXmlItemName::eRemoteIpAddr, "0.0.0.0");
			addr.set(str);
			if (!addr.isValid())
			{
				OmnWarn << OmnErrId::eWarnProgError
					<< "Incorrect remote IP address: " 
					<< connconf->toString() << enderr;
				return false;
			}

			port = connconf->getInt(OmnXmlItemName::eRemotePort, -1);
			remoteNumPorts = connconf->getInt(
				OmnXmlItemName::eRemoteNumPorts, 1);
			if (port <= 0)
			{
				OmnWarn << OmnErrId::eWarnProgError
					<< "Incorrect remote port: " 
					<< connconf->toString() << enderr;
				return false;
			}
		}

		OmnString name = connconf->getStr(OmnXmlItemName::eName, "NoName");
		OmnTcpClientPtr conn = OmnNew OmnTcpClient(
			addr,
			port,
			remoteNumPorts,
			localAddr,
			0,
			1,
			name);

		if (!conn->connect(err))
		{
			OmnWarn << OmnErrId::eWarnCommError
				<< "Failed to connect" << enderr;
		}
		else
		{
			mClients.append(NmsClient(type, conn));
			mTcpClts->addConn(conn);
		}
	}
*/
	return true;
}


OmnTcpClientPtr
OmnNmsClient::getClientConn(const OmnString &name)
{
	mClients.reset();
	while (mClients.hasMore())
	{
		if ((mClients.crtValue().mConn)->getName() == name)
		{
			return mClients.crtValue().mConn;
		}

		mClients.next();
	}

	return 0;
}


bool
OmnNmsClient::isProgDefined(const OmnString &name)
{
	OmnTcpClientPtr client = getClientConn(name);
	if (client.isNull())
	{
		return false;
	}

	//if (client->isConnGood())
	//{
	//	return true;
	//}

	//return client->connect();
	return true;
}


bool
OmnNmsClient::stopProg(const OmnString &name, OmnString &err)
{
/*
	OmnTcpClientPtr client = getClientConn(name);
	if (client.isNull())
	{
		err = "Program not configured";
		return false;
	}

	if (!client->isConnGood() && !client->connect(err))
	{
		return false;
	}

	OmnSmNmsCmdPtr msg = OmnNew OmnSmNmsCmd(OmnSmNmsCmd::eStopProg);
	if (!client->sendMsgTo(msg))
	{
		client->closeConn();
		if (!client->connect(err))
		{
			return false;
		}

		if (!client->sendMsgTo(msg))
		{
			err = "Failed to send";
			return false;
		}
	}

	readResp(name, client, err);
*/
	return true;
}


bool
OmnNmsClient::readResp(const OmnString &name, 
					   const OmnTcpClientPtr &conn,
					   OmnString &err)
{
/*
	OmnConnBuffPtr buff = OmnNew OmnConnBuff();
	bool timeout;
	bool connBroken;
	if (!conn->readFrom(buff, 5, timeout, connBroken))
	{
		// 
		// Failed to read anything the program
		//
		err = "Failed to read anything from: ";
		err << name;
		return false;
	}

	if (OmnSysObj::getMsgId(buff) != OmnMsgId::eSmNmsResp)
	{
		err = "Read is not NMS response";
		return false;
	}

	OmnSerialFrom s(buff);
	s.popMsgId();
	OmnSmNmsResp msg;
	msg.serializeFrom(s);
	cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	cout << msg.toString() << endl;
	cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
*/
	return true;
}


bool
OmnNmsClient::checkProgStatus(const OmnString &name, OmnString &rslt)
{
/*
	OmnTcpClientPtr client = getClientConn(name);
	if (client.isNull())
	{
		rslt = "Program not configured";
		return false;
	}

	if (!client->isConnGood() && !client->connect(rslt))
	{
		return false;
	}

	OmnSmNmsCmdPtr msg = OmnNew OmnSmNmsCmd(OmnSmNmsCmd::eCheckProgStatus);
	if (!client->sendMsgTo(msg))
	{
		client->closeConn();
		if (!client->connect(rslt))
		{
			return false;
		}

		if (!client->sendMsgTo(msg))
		{
			rslt = "Failed to send";
			return false;
		}
	}

	readResp(name, client, rslt);
*/
	return true;
}	



void    
OmnNmsClient::recvMsg(const OmnConnBuffPtr &buf,
					  const OmnTcpClientPtr &conn)
{
/*
	switch (OmnSysObj::getMsgId(buf))
	{
	case OmnMsgId::eSmNmsCmd:
		 procNmsCmd(buf, conn);
		 break;

	case OmnMsgId::eSmEpCmd:
		 procEpCmd(buf, conn);
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
OmnNmsClient::procNmsCmd(const OmnConnBuffPtr &, const OmnTcpClientPtr &)
{
}


void		
OmnNmsClient::procEpCmd(const OmnConnBuffPtr &, const OmnTcpClientPtr &)
{
}


