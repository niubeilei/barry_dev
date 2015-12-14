////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Comm.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
//o

#include "UtilComm/Comm.h"

#include "Alarm/Alarm.h"
#include "Debug/Error.h"
#include "Debug/ErrId.h"
#include "Network/NetIf.h"
#include "Util/OmnNew.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/TcpCommClt.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/CommGroup.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"



OmnComm::OmnComm(const OmnCommProt::E protocol,
				 const bool proxySenderAddr,
				 const OmnCommGroupPtr &comm,
				 const int targetId,
				 const OmnNetEtyType::E targetType)
:
mProtocol(protocol),
mCommGroup(comm),
mTargetId(targetId),
mTargetType(targetType),
mProxySenderAddr(proxySenderAddr)
{
}


OmnComm::OmnComm(const OmnCommProt::E prot,
				 const OmnString &name)
				 :
mName(name),
mProtocol(prot),
mProxySenderAddr(false)
{
}


OmnComm::~OmnComm()
{
}


OmnCommPtr	
OmnComm::createComm(
		const OmnCommProt::E protocol, 
		const bool proxySenderAddr,
		const OmnXmlItemPtr &def,
		const int targetId,
		const OmnNetEtyType::E targetType,
		const OmnCommGroupPtr &comm, 
		const OmnString &fname, 
		const int line)
{
	//
	// This function assumes 'def' is an xml item defining a comm. 
	// It retrieves the tag <CommType> to determine the type of 	
	// group and create an instance of it.
	//

	try
	{
		OmnString type = def->getStr(OmnXmlItemName::eCommType, "");
		switch (strToEnum(type))
		{
		case eUdpComm:
			 //
			 // It is UDP Communicator
			 //
			 return OmnNew OmnUdpComm(protocol, proxySenderAddr, 
							 def, targetId, targetType, comm);

		//case eTcpServer:
			 //
			 // It is TCP Server 
			 //
		//	 return new OmnTcpCommSvr(protocol, def, targetId, targetType, comm);
		
		case eTcpClient:
			 return OmnNew OmnTcpCommClt(protocol, def, targetId, targetType, comm, fname, line);

		default:
			 OmnAlarm << "OmnErrId::eInvalidCommType"
				<< def->toString() << enderr;
			 return 0;
		}		
	}

	catch (const OmnExcept &)
	{
		return 0;
	}
}


/*
OmnCommPtr	
OmnComm::createComm(const OmnIpAddr &addr, 
					const int port,
					const OmnCommProt::E prot, 
					const OmnString &name)
{
	try
	{
		switch (prot)
		{
		case OmnCommProt::eUDP:
			 //
			 // It is UDP Communicator
			 //
			 return OmnNew OmnUdpComm(addr, port, name);

		case OmnCommProt::eTcpServer:
			 //
			 // It is TCP Server 
			 //
			 // return OmnNew OmnTcpCommSvr(addr, port, name);
			 OmnAlarm << "Not implemented yet" << enderr;
			 return 0;
		
		case OmnCommProt::eTcpClient:
			 return OmnNew OmnTcpCommClt(addr, port, name);

		default:
			 OmnAlarm << OmnErrId::eAlarmProgramError
				<< "Unrecognized protocol: " << prot 
				<< ":" << addr.toString() << ":" << port
				<< ":" << name << enderr;
			 return 0;
		}		
	}

	catch (const OmnExcept &)
	{
		return 0;
	}
}
*/


OmnComm::CommType
OmnComm::strToEnum(OmnString &name)
{
	name.toLower();
	if (name == OmnString("udpcomm"))
	{
		return eUdpComm;
	}

	if (name == OmnString("tcpserver"))
	{
		return eTcpServer;
	}

	if (name == OmnString("tcpclient"))
	{
		return eTcpClient;
	}

	return eInvalidComm;
}

