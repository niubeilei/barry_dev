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

#include "Torturer/FuncModule.h"
#include "Torturer/FuncCmd.h"
#include "RVG/ConfigMgr.h"
#include "alarm_c/alarm.h"
#include "Util/OmnNew.h"
#include "Torturer/Torturer.h"

AosFuncModule::AosFuncModule()
{
	mFuncSelector = NULL;
}


AosFuncModule::AosFuncModule(const std::string &moduleName, const std::string &productName)
:AosModule(moduleName, productName)
{
	mFuncSelector = NULL;
}


AosFuncModule::~AosFuncModule()
{
}


bool 
AosFuncModule::serialize(TiXmlNode& node)
{
	// 
	// <FuncModule>
	// 		<Name>
	// 		<Weight>
	// 		<MinNumCmds>
	// 		<MaxNumCmds>
	// 		<Status>
	// 		<Cmds>
	// 			<CmdName></CmdName>
	// 			<CmdName></CmdName>
	//			...
	// 		</Cmds>
	// 	</FuncModule>
	//
	if (!AosModule::serialize(node))
	{
		return false;
	}
	node.SetValue("FuncModule");

	return true;
}


bool 
AosFuncModule::deserialize(TiXmlNode* node)
{
	// 
	// <FuncModule>
	// 		<Name>
	// 		<Weight>
	// 		<MinNumCmds>
	// 		<MaxNumCmds>
	// 		<Status>
	// 		<Cmds>
	// 			<CmdName></CmdName>
	// 			<CmdName></CmdName>
	//			...
	// 		</Cmds>
	// 	</FuncModule>
	//
	if (!AosModule::deserialize(node))
	{
		return false;
	}
	return true;
}


bool
AosFuncModule::addRunTimeFunc(const AosFuncCmdPtr& func)
{
	if (!func)
	{
		return false;
	}
	if (mRunTimeFuncs.size() >= eMaxNumFuncs)
	{
		OmnAlarm << "Too many Functions in a module. The maximum allowed is: " 
			<< eMaxNumFuncs << enderr;
		return false;
	}
	AosFuncCmdPtrArray::iterator iter;
	iter = std::find(mRunTimeFuncs.begin(), mRunTimeFuncs.end(), func);
	if (iter != mRunTimeFuncs.end())
	{
		//
		// exist already
		//
		OmnWarn << "The function is already in the list" << enderr;
		return false;
	}
	mRunTimeFuncs.push_back(func);
	return true;
}


bool
AosFuncModule::delRunTimeFunc(const AosFuncCmdPtr& func)
{
	mRunTimeFuncs.erase(std::remove(mRunTimeFuncs.begin(), mRunTimeFuncs.end(), func), mRunTimeFuncs.end());
	return true;
}


bool 
AosFuncModule::runCommands(u32 &cmdRuns, const OmnTestMgrPtr &tm)
{
	//
	// if the function selector is not exist, create it
	//
	if (!mFuncSelector)
	{
		createFuncSelector();
	}

	bool cmdResult;
	bool cmdGenerated;
	bool ignore;
	cmdRuns = aos_next_integer(mMinNumCmds, mMaxNumCmds);
	for (u32 i=0; i<cmdRuns; i++)
	{
		AosValue value;
		if (!mFuncSelector->nextValue(value))
		{
			OmnAlarm << "The function selector is error" << enderr;
			return false;
		}

		u32 index = value.toUint32();
		aos_assert_r(index < (u32)mRunTimeFuncs.size(), false);
		if (!mRunTimeFuncs[index])
		{
			OmnAlarm << "Runtime function not set" << enderr;
			return false;
		}

		if (!mRunTimeFuncs[index]->run(cmdResult, 
					AosTorturer::getCorrectCtrlFlag(), cmdGenerated))
		{
			OmnAlarm << "failed run function" << enderr;
			return false;
		}

		if (ignore) 
		{
			mTotalIgnoreCmds ++;
			continue;
		}

		mTotalCmdsRun++;
		if (!cmdResult) 
		{
			mTotalIncorrectCmds++;
		}
		else
		{
			mTotalCorrectCmds++;
		}
	}
	return true;
}


bool 
AosFuncModule::createFuncSelector()
{
	mFuncSelector = OmnNew AosRIG<u32>;
	int funcNum = mRunTimeFuncs.size();
	for (int i=0; i<funcNum; i++)
	{
		mFuncSelector->setIntegerPair(i, i, mRunTimeFuncs[i]->getWeight());	
	}
	return true;
}

