////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApiCodeGen.h
// Description:
//   
//
// Modification History:
// 06/30/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "CliTorturer/ApiCodeGen.h"

#include "Alarm/Alarm.h"
#include "aosUtil/Alarm.h"
#include "Debug/Debug.h"
#include "CliTorturer/CliTorturer.h"
#include "ProgramAid/Util.h"
#include "Tester/TestMgr.h"
#include "TorturerGen/FuncTesterGen.h"
#include "TorturerUtil/Util.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"



AosApiCodeGen::AosApiCodeGen()
{
	return;
}


AosApiCodeGen::~AosApiCodeGen()
{
}


bool 			
AosApiCodeGen::addApiCodeGen(const AosTesterGenPtr &gen)
{
	// 
	// Make sure the same command is not added multiple times
	//
	if (mApiCodeGens.entries() >= eMaxApiCodeGens)
	{
		OmnAlarm << "Too many objects. The maximum allowed is: " 
			<< eMaxApiCodeGens << enderr;
		return false;
	}

	// 
	// Make sure the command is not already in the array
	//
	for (int i=0; i<mApiCodeGens.entries(); i++)
	{
		if (mApiCodeGens[i] == gen)
		{
			// Already in the list
			OmnWarn << "The object is already in the list" << enderr;
			return false;
		}
	}

	mApiCodeGens.append(gen);

	return true;
}


bool	
AosApiCodeGen::genCode(const OmnString &libDir, 
					const OmnString &libName, 
					const OmnString &torturerDir,
					const OmnString &torturerName,
					const OmnString &additionalLibs,
					const bool force) const
{
	for (int i=0; i<mApiCodeGens.entries(); i++)
	{
		if (!mApiCodeGens[i]->generateCode(force))
		{
			OmnAlarm << "Failed to generate code" << enderr;
			return false;
		}
	}

	return true;
}


bool	
AosApiCodeGen::parseApi(const OmnXmlItemPtr &item,
					const OmnString &srcDir,
					const OmnString &testDir)
{
	OmnAlarm << "Not implemented yet" << enderr;
	return false;
}

