////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Nms.cpp
// Description:
//	This class implements the default HASS Application Network Management
//  functions. If an application requires some special network management
//  functions, it can derive from this class.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "NMS/Nms.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "NMS/NmsServer.h"
#include "NMS/NmsClient.h"
#include "NMS/VersionInfo.h"
#include "NMS/HouseKp.h"
#include "Porting/Sleep.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util1/Time.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServer.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include "XmlParser/XmlParser.h"

OmnSingletonImpl(OmnNmsSingleton,
				 OmnNms,
				 OmnNmsSelf,
				 "OmnNms");

OmnAppType::E	OmnNms::mAppType = OmnAppType::eUnknown;
OmnString		OmnNms::mProgName;
OmnString		OmnNms::mStartTime;
OmnString		OmnNms::mVersion;
OmnString		OmnNms::mPatch;
int				OmnNms::mBuildNo = 0;
OmnString		OmnNms::mBuildTime;
OmnString		OmnNms::mProcId;
OmnString		OmnNms::mFailReason;
OmnString		OmnNms::mMachine;
OmnString		OmnNms::mInstId;
OmnString		OmnNms::mLocation;
bool 			OmnNms::mIsPrimary = true;

OmnNms::OmnNms()
:
mLastLogTick(0),
mProgLogFreq(eDefaultProgLogFreq)
{
}


OmnNms::~OmnNms()
{
}


bool
OmnNms::start()
{
	// 
	// Store the application information into the database
	//
	mLastLogTick = 0;
	mHouseKp->start();
/*
	mProgLog.setProgName(mProgName);
	mProgLog.setLocation(mLocation);
	mProgLog.setMachine(mMachine);
	mProgLog.setVersion(mVersion);
	mProgLog.setBuildNo(mBuildNo);
	mProgLog.setBuildTime(mBuildTime);

	if (mIsPrimary)
	{
		mProgLog.setMode("Primary");
	}
	else
	{
		mProgLog.setMode("Standby");
	}
*/
	return true;
}


void
OmnNms::storeAppInfo() const
{
/*	OmnSoHbStatus so;
	so.setAddId(mProgName);
	so.setInstId(mInstId);
	so.setProcId(mProcId);
	so.setVersion(mVersion);
	so.setBuildNum(mBuildNo);
	so.setFailReason(mFailReason);
	so.setMachineId(mMachine);
	so.setNmsAddr(getNmsAddr(), getNmsPort());
	so.setStartTime(OmnGetTime());
	so.storeAppInfo();
*/
}


bool
OmnNms::stop()
{
	mHouseKp->stop();
	return true;
}


void
OmnNms::runNmsServer()
{
	// 
	// IMPORTANT:
	// This is a infinite loop, called from main.cpp. It should
	// never return unless the program exits. 
	//
	
	if (mAppType == OmnAppType::eUnknown)
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Application type is unknown!" << enderr;
	}

	if (mNmsServer.isNull())
	{
		// 
		// There is no NmsServer. This program can only be stopped
		// by cntl-c.
		//
		OmnWarn << OmnErrId::eWarnProgramError
			<< "No NMS Server defined. Program can only be stopped "
			<< "by cntl-c" << enderr;
		while (1)
		{
			OmnSleep(1000);
		}
	}
	else
	{
		mNmsServer->start();
	}
}


OmnRslt
OmnNms::config(const OmnXmlParserPtr &conf)
{   
    // 
    // Configure this class. If fails, use default
    //  
	//	<NmsConfig>
	//		<InstanceId>
	//		<MachineId>
    //  	<HouseKeep>
    //     	 	<Frequency>
    //  	</HouseKeep>
	//  	<NmsServer>
	//  		<LocalNiid>
	//  		<LocalPort>
	//  		<LocalNumPorts>
	//  	</NmsServer>
	//  	<HeartbeatConfig>
	//  	...
	//  	</HeartbeatConfig>
	//  	<NmsClients>
	// 			<NmsClient>
	//				<Name> 	
	//				<RemoteIpAddr>
	//				<RemotePort
	//				<RemoteNumPorts>
	//				<LocalNiid>
	//			</NmsClient>
	//			...
	//  	</NmsClients>
	//	</NmsConfig>
    //

	if (!conf)
	{
		mHouseKp = OmnNew OmnHouseKp(0);
		return true;
	}

	OmnXmlItemPtr def;
   	def = conf->tryItem(OmnXmlItemName::eNmsConfig);
   	if (def.isNull())
   	{
		mHouseKp = OmnNew OmnHouseKp(0);
		return true;
	}
	
	mInstId = def->getStr(OmnXmlItemName::eInstId, "1000");
	mMachine = def->getStr(OmnXmlItemName::eMachineId, "NoMachineId");

   	OmnXmlItemPtr item = def->tryItem(OmnXmlItemName::eHouseKeep);
	mHouseKp = OmnNew OmnHouseKp(item);

	OmnXmlItemPtr serverConfig = def->tryItem(OmnXmlItemName::eNmsServer);
	if (!serverConfig.isNull())
	{
		mNmsServer = OmnNew OmnNmsServer();
		if (!mNmsServer->config(serverConfig))
		{
			return false;
		}
	}

	//
	// Chen Ding, 09/21/2003, 2003-0265
	//
	// OmnXmlItemPtr clientConf = def->tryItem(OmnXmlItemName::eNmsClients);
	// if (!clientConf.isNull())
	// {
	// 	mNmsClient = OmnNew OmnNmsClient();
	// 	return mNmsClient->config(clientConf);
	// }

	return true;
}


void
OmnNms::addHouseKeepObj(OmnHouseKpObj *obj)
{
	if (!mHouseKp.isNull())
	{
		mHouseKp->addObj(obj);
	}
}


void
OmnNms::stopProg()
{
}


//
// Chen Ding, 09/21/2003, 2003-0265
//
// OmnTcpClientPtr
// OmnNms::getNmsConn(const OmnString &name)
// {
// 	if (mNmsClient.isNull())
// 	{
// 		return 0;
// 	}
// 
// 	return mNmsClient->getClientConn(name);
// }
// 
// 
//bool
//OmnNms::isProgDefined(const OmnString &name)
//{
//	if (mNmsClient.isNull())
//	{
//		return false;
//	}
//
//	return mNmsClient->isProgDefined(name);
//}
//
//
//bool
//OmnNms::stopProg(const OmnString &name, OmnString &err)
//{
//	if (mNmsClient.isNull())
//	{
//		OmnWarn << OmnErrId::eWarnConfigError
//			<< "No NMS Client defined!" << enderr;
//		return false;
//	}
//
//	return mNmsClient->stopProg(name, err);
//}
//
//
//bool
//OmnNms::checkProgStatus(const OmnString &name, OmnString &rslt)
//{
//	if (mNmsClient.isNull())
//	{
//		rslt = "No NMS Client defined!";
//		return false;
//	}
//
//	return mNmsClient->checkProgStatus(name, rslt);
//}


OmnString
OmnNms::getObjTotals() const
{
	// return OmnObjMgr::getObjTotals();
	OmnNotImplementedYet;
	return "NotImplementedYet";
}


//
// Chen Ding, 09/21/2003, 2003-0265
//
// OmnNmsClientPtr	
// OmnNms::getNmsClient() const 
// {
// 	return mNmsClient;
// }


void		
OmnNms::setNmsSvrObj(const OmnNmsSvrObjPtr &obj)
{
	if (mNmsServer.isNull())
	{
		OmnWarn << OmnErrId::eWarnProgramError
			<< "Server is null" << enderr;
		return;
	}

	mNmsServer->setNmsSvrObj(obj);
}


OmnIpAddr
OmnNms::getNmsAddr() const 
{
	if (mNmsServer.isNull())
	{
		return OmnIpAddr::eInvalidIpAddr;
	}

	return mNmsServer->getLocalIpAddr();
}


int
OmnNms::getNmsPort() const
{
	if (mNmsServer.isNull())
	{
		return 0;
	}

	return mNmsServer->getLocalPort();
}	


OmnString
OmnNms::getAlgConfig(const OmnString &remoteAddr) const
{
	OmnString str;
	str << 
		"<NmsConfig>"
			"<InstId>1000</InstId>"
			"<MachineId>AlgMachine</MachineId>"

			"<NmsServer>"
				"<LocalNiid>6</LocalNiid>"
				"<LocalPort>5010</LocalPort>"
				"<NumPorts>5</NumPorts>"
				"<LengthType>LengthIndicator</LengthType>"
			"</NmsServer>"

			"<NmsClients>"
				"<NmsClient>"
					"<Name>StreamRouterA</Name>"
					"<Type>StreamRouter</Type>"
					"<RemoteIpAddr>" << remoteAddr << "</RemoteIpAddr>" <<
					"<RemotePort>15010</RemotePort>"
					"<RemoteNumPorts>5</RemoteNumPorts>"
					"<LocalNiid>6</LocalNiid>"
				"</NmsClient>"
			"</NmsClients>"
		"</NmsConfig>";
	
	return str;
}


/*
void
OmnNms::procStateHb(const OmnSmChgState &msg)
{
	// 
	// The Cntler sent a 'SmChgState' message to this SAG. This message
	// serves two purposes: heartbeating and enforcing the mode. 
	//
	bool stateChanged = false;
	if (mIsPrimary != msg.getState())
	{
		// 
		// Not necessarily an error, but need attention
		//
		OmnWarn << OmnErrId::eWarnRuntimeError
			<< "Execution mode changed to: " 
			<< msg.getState() << enderr;
		mIsPrimary = msg.getState();

		if (mIsPrimary)
		{
			mProgLog.setMode("Primary");
		}
		else
		{
			mProgLog.setMode("Standby");
		}

		stateChanged = true;
	}

	int tick = OmnTimeSelf->getSecTick();
	if (stateChanged || tick - mLastLogTick > mProgLogFreq)
	{
		// 
		// It is the time to generate a log.
		//
		mProgLog.addLog();
		mLastLogTick = tick;
	}
}
*/


bool
OmnNms::setAppInfo(int argc, char **argv)
{
	// 
	// argv[0] is the program name. This function looks for the 
	// following: 
	//
	// -loc 	location
	// -machine machine
	//
	mProgName = argv[0];

	int index = 1;
	while (index < argc)
	{
		if (OmnString(argv[index]) == "-loc")
		{
			// 
			// It is the location flag
			//
			index++;
			if (index >= argc)
			{
				cout << "Command line error: Missing location after"
					 << " the -loc flag" << endl;
				return false;
			}

			mLocation = argv[index];
			index++;
			continue;
		}

		if (OmnString(argv[index]) == "-machine")
		{
			index++;
			if (index > argc)
			{
				cout << "Command line error: Missing machine name after"
					 << " the -machine flag" << endl;
				return false;
			}

			mMachine = argv[index];
			index++;
			continue;
		}

		index++;
	}

	return true;
}


void
OmnNms::setVersionInfo()
{
	mVersion = OmnVersionInfo::getVersion();
	mBuildNo = OmnVersionInfo::getBuildNo();
	mBuildTime = OmnVersionInfo::getBuildTime();
	mPatch = OmnVersionInfo::getPatch();
}
