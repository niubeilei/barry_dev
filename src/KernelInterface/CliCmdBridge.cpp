////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCmdBridge.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#include "KernelInterface/CliCmdBridge.h"

#include "Alarm/Alarm.h"
#include "aos/KernelApiStruct.h"
#include "aos/aosKernelAlarm.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "Util/StrParser.h"



/*
bool
OmnCliBridgeAdd::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// bridge add <bridgeId> <method> <dev1> <weight1> <dev2> <weight2>
	//
	OmnStrParser parser(parms);
	int bridgeId = parser.nextInt();
	rslt = "";
	if (bridgeId <= 0 || bridgeId >= eAosMaxDev)
	{
		rslt << " Invalid bridge ID: " << parms;
		return false;
	}

	OmnString method = parser.nextWord();
	method.toLower();
	int intMethod;
	if (method == "wrr")
	{
		intMethod = eAosBridgeMethod_WRR;
	}
	else if (method == "ipflow_wrr")
	{
		intMethod = eAosBridgeMethod_IPFLOW_WRR;
	}
	else
	{
		rslt << "Invalid method: " << method;
		return false;
	}
	
	OmnString dev1 = parser.nextWord();
	if (dev1.length() <= 0 || dev1.length() >= eAosMaxDevNameLen)
	{
		rslt = "Missing/Incorrect <dev1>. "
			"Usage: bridge add <bridgeId> <dev1> <weight1> <dev2> <weight2>";
		return false;
	}

	int weight1 = parser.nextInt();
	if (weight1 < 0)
	{
		rslt = "Missing/Incorrect <weight1>. "
			"Usage: bridge add <bridgeId> <dev1> <weight1> <dev2> <weight2>";
		return false;
	}

	OmnString dev2 = parser.nextWord();
	if (dev2.length() <= 0 || dev1.length() >= eAosMaxDevNameLen)
	{
		rslt = "Missing/Incorrect <dev2>. "
			"Usage: bridge add <bridgeId> <dev1> <weight1> <dev2> <weight2>";
		return false;
	}

	int weight2 = parser.nextInt();
	if (weight2 < 0)
	{
		rslt = "Missing/Incorrect <weight2>. "
			"Usage: bridge add <bridgeId> <dev1> <weight1> <dev2> <weight2>";
		return false;
	}

	aosKapi5k data;
	data.mLength = 0;
	data.mOprId = eAosKid_BridgeAdd;
	data.mInt1 = bridgeId;
	data.mInt2 = weight1;
	data.mInt3 = weight2;
	data.mInt4 = intMethod;
	strcpy(data.mStr1, dev1);
	strcpy(data.mStr2, dev2);

	int ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));
	if (ret)
	{
		rslt = OmnKernelApi::getErr(ret) << ". " << data.mStr;
		return false;
	}

	rslt = "Bridge added";
	return true;
}


bool
OmnCliBridgeRemove::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// remove bridge <bridgeId>
	//
	OmnStrParser parser(parms);
	int bridgeId = parser.nextInt();
	if (bridgeId < 0 || bridgeId >= eAosMaxDev)
	{
		rslt << " Invalid bridge ID: " << parms;
		return false;
	}

	aosKapi5k data;
	data.mLength = 0;
	data.mOprId = eAosKid_BridgeRemove;
	data.mInt1 = bridgeId;
	int ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));
	if (ret)
	{
		rslt = OmnKernelApi::getErr(ret) << ". " << data.mStr;
		return false;
	}

	return true;
}


bool
OmnCliBridgeShow::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// show bridge [<bridgeId>]
	//
	int ret;
	OmnStrParser parser(parms);
	aosKapi5k data;

	data.mLength = 0;
	if (parser.peekWord().length() <= 0)
	{
		// 
		// No bridge ID. It is listing all bridges
		//
		data.mOprId = eAosKid_BridgeList;
		ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));

		if (ret)
		{
			rslt << OmnKernelApi::getErr(ret);
		}
		else
		{
			rslt = data.mStr;
		}
	}
	else
	{
		// 
		// It is showing a specific bridge
		//
		int bridgeId = parser.nextInt();
		if (bridgeId < 0 || bridgeId >= eAosMaxDev)
		{
			rslt << " Invalid bridge ID: " << parms;
			return false;
		}

		data.mLength = sizeof(aosKapiHeader) + sizeof(int);
		data.mOprId = eAosKid_BridgeShow;
		data.mInt1 = bridgeId;
		ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));

		if (ret)
		{
			rslt << OmnKernelApi::getErr(ret) << ": " << bridgeId;
			return false;
		}
		else
		{
			rslt = data.mStr;
		}
	}

	return true;
}


bool
OmnCliBridgeOnOff::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// bridge <on|off>
	//
	int ret;
	OmnStrParser parser(parms);

	OmnString status = parser.nextWord();
	aosKapi1 data;
	data.mLength = 0;
	data.mOprId = eAosKid_BridgeStatus;
	if (status == "on")
	{
		data.mInt1 = 1;
	}
	else if (status == "off")
	{
		data.mInt1 = 0;
	}
	else
	{
		rslt = "Command incorrect. Usage: bridge [on|off]";
		return false;
	}

	ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));
	if (ret)
	{
		rslt << "Failed to show bridge: " << OmnKernelApi::getErr(ret);
		return false;
	}
	return true;
}


bool
OmnCliBridgeReset::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// bridge reset <bridgeId> 
	//
	OmnStrParser parser(parms);
	int bridgeId = parser.nextInt();
	aosKapi2 data;
	if (bridgeId < 0 || bridgeId >= eAosMaxDev)
	{
		rslt << "Invalid bridge ID: " << parms;
		return false;
	}

	data.mLength = 0;
	data.mOprId = eAosKid_BridgeReset;
	data.mInt1 = bridgeId;

	int ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));

	return (ret == 0);
}


bool
OmnCliBridgeClearAll::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// bridge clear all
	//
	int oprId = eAosKid_BridgeClearAll;
	int ret = OmnKernelApi::sendToKernel((char *)&oprId, sizeof(oprId));
	if (ret)
	{
		rslt << "Failed to clear all bridges: " << OmnKernelApi::getErr(ret);
		return false;
	}

	return true;
}


bool
OmnCliBridgeDevAdd::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// bridge device add <bridgeId> <side> <devicename> <weight>
	//
	OmnStrParser parser(parms);
	int bridgeId = parser.nextInt();
	OmnString side = parser.nextWord();
	side.toLower();
	OmnString name = parser.nextWord();
	int weight = parser.nextInt(-1);

	if (side != "left" && side != "right" || name.length() <= 0 || weight == -1)
	{
		rslt << "Command incorrect. Usage: bridge device add <bridgeId> " 
			"<side> <devicename> <weight>" << side;
		return false;
	}


	aosKapi5k data;
	data.mLength = 0;
	data.mOprId = eAosKid_BridgeDevAdd;
	data.mInt1= bridgeId;

	if (side == "left")
	{
		data.mInt2 = 1;
	}
	else 
	{
		data.mInt2 = 2;
	}

	strcpy(data.mStr1, name.data());
	data.mInt3= weight;

	OmnTrace << "Got command: bridgeId = " << bridgeId
		<< ", side = " << side 
		<< ", name = " << name
		<< ", weight = " << weight << endl;

	int ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));
	if (ret)
	{
		rslt << OmnKernelApi::getErr(ret) << ". " << data.mStr;
		return false;
	}

	return true;
}


bool
OmnCliBridgeDevRemove::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// bridge device remove <bridgeId> <side> <devicename>
	//
	OmnStrParser parser(parms);
	int bridgeId = parser.nextInt();
	OmnString side = parser.nextWord();
	OmnString name = parser.nextWord();

	side.toLower();
	if (side != "left" && side != "right" || name.length() <= 0)
	{
		rslt << "Command incorrect. Usage: bridge device remove <bridgeId> " 
			"<side> <devicename>" << side;
		return false;
	}
	

	aosKapi5k data;
	data.mLength = 0;
	data.mOprId = eAosKid_BridgeDevRemove;
	data.mInt1 = bridgeId;

	if (side == "left")
	{
		data.mInt2 = 1;
	}
	else
	{
		data.mInt2 = 2;
	}

	strcpy(data.mStr1, name.data());

	OmnTrace << "Got command: bridgeId = " << bridgeId
		<< ", side = " << side 
		<< ", name = " << name << endl;

	int ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));
	if (ret)
	{
		rslt << OmnKernelApi::getErr(ret) << ". " << data.mStr;
		return false;
	}

	return true;
}


bool
OmnCliBridgeSaveConfig::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// bridge save 
	//

	aosKapi10k data;
	data.mLength = 0;
	data.mOprId = eAosKid_BridgeSaveConfig;

	int ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));
	rslt = data.mStr;
	return (ret == 0);
}


bool
OmnCliBridgeHealthCheck::run(const OmnString &parms, OmnString &rslt)
{
	// 
	// bridge save 
	//

	aosKapi10k data;
	data.mLength = 0;
	data.mOprId = eAosKid_BridgeHealthCheck;

	int ret = OmnKernelApi::sendToKernel((char *)&data, sizeof(data));
	rslt = data.mStr;
	return (ret == 0);
}
*/
#endif
