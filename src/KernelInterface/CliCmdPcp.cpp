////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCmdPcp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelInterface/CliCmdPcp.h"

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
	// pcp status [on|off] <pcp>
	//
	// <pcp>:
	//	network-input
	//
	OmnStrParser parser(parms);
	OmnString status = parser.nextWord();
	rslt = "";
	status.toLower();

	if (status != "on" && status != "off")
	{
		rslt << " Invalid status. Usage: pcp status [on|off] <pcp>";
		return false;
	}

	OmnString pcp = parser.nextWord();
	method.toLower();
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
*/


