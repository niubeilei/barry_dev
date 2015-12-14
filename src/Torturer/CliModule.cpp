////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// Friday, January 04, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#include "Torturer/CliModule.h"

AosCliModule::AosCliModule()
{
}


AosCliModule::~AosCliModule()
{
}


bool
AosCliModule::addCli(const AosCliCmdPtr& cli)
{
	if (!cli)
	{
		return false;
	}
	if (mClis.size() >= eMaxNumClis)
	{
		OmnAlarm << "Too many Clis in a module. The maximum allowed is: " 
			<< eMaxNumClis << enderr;
		return false;
	}
	AosCliCmdPtrArray::iterator iter;
	iter = std::find(mClis.begin(), mClis.end(), cli);
	if (iter != mClis.end())
	{
		//
		// exist already
		//
		OmnWarn << "The cli is already in the list" << enderr;
		return false;
	}
	mClis.push_back(cli);
	return true;
}


bool
AosCliModule::delCli(const AosCliCmdPtr& cli)
{
	mClis.erase(std::remove(mClis.begin(), mClis.end(), cli), mClis.end());
	return true;
}


bool 
AosCliModule::serialize(TiXmlNode& node)
{
	// 
	// <CliModule>
	// 		<Name>
	// 		<Weight>
	// 		<MinNumCmds>
	// 		<MaxNumCmds>
	// 		<Status>
	// 		<Clis>
	// 			<CliName></CliName>
	// 			<CliName></CliName>
	//			...
	// 		</Clis>
	// 	</CliModule>
	//
	if (!AosModule::serialize(node))
	{
		return false;
	}

	node.SetValue("CliModule");

	return true;
}


bool 
AosCliModule::deserialize(TiXmlNode* node)
{
	// 
	// <CliModule>
	// 		<Name>
	// 		<Weight>
	// 		<MinNumCmds>
	// 		<MaxNumCmds>
	// 		<Status>
	// 		<Clis>
	// 			<CliName></CliName>
	// 			<CliName></CliName>
	//			...
	// 		</Clis>
	// 	</CliModule>
	//
	if (!AosModule::deserialize(node))
	{
		return false;
	}
	if (node->ValueStr() != "CliModule")
	{
		return false;
	}

	return true;
}


bool 
AosCliModule::runCommands(u32 &cmdRuns, const OmnTestMgrPtr &tm)
{
	return false;
}

