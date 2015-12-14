////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliModuleTorturer.h
// Description:
//   
//
// Modification History:
// 10/29/2006: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "CliTorturer/CliModuleTorturer.h"

#include "Alarm/Alarm.h"
#include "alarm/Alarm.h"
#include "CliTorturer/FuncModule.h"
#include "CliTorturer/CliTorturer.h"
#include "CliTorturer/CliCmdTorturer.h"
#include "CliTorturer/CliCmdGeneric.h"
#include "Random/RandomSelector.h"
#include "Random/RandomUtil.h"
#include "Tester/TestMgr.h"
#include "Util/SPtr.h"
#include "Util/VarList.h"
#include "Util/GenTable.h"
#include "Util/File.h"

#include "Parms/RVGStr.h"
#include "Parms/RVGInt.h"

AosCliModuleTorturer::AosCliModuleTorturer(
			const OmnString &moduleId, 
			const u32 minNumCmds, 
			const u32 maxNumCmds, 
			const u32 weight, 
			const AosCliTorturerPtr &cliTorturer)
:
mModuleId(moduleId),
mCliTorturer(cliTorturer),
mCheckType(eCheckAfterEveryCommand),
mCmdRun(0),
mMinNumCmds(minNumCmds),
mMaxNumCmds(maxNumCmds),
mTotalCmdsRun(0),
mTotalFailedCmds(0),
mTotalFailedChecks(0),
mWeight(weight)
{
	return;
}


AosCliModuleTorturer::~AosCliModuleTorturer()
{
}


AosCliModuleTorturerPtr 
AosCliModuleTorturer::createModule(const OmnXmlItemPtr &def, 
								const AosCliTorturerLogType logType, 
								OmnVList<AosGenTablePtr> &tables, 
								AosVarListPtr &variables,
								const AosCliTorturerPtr &cliTorturer)
{
	// 
	// <Module>
	// 		<Name>
	// 		<Weight>
	// 		<MinNumCmds>
	// 		<MaxNumCmds>
	// 		<Status>
	// 	</Module>
	//
	OmnString name = def->getStr("Name", "NoName");
	if (name == "NoName")
	{
		OmnAlarm << "Missing Name tag: " << def->toString() << enderr;
		return 0;
	}

	int minCmds = def->getInt("MinNumCmds", 1);
	int maxCmds = def->getInt("MaxNumCmds", 100);
	int weight  = def->getInt("Weight", eDefaultModuleWeight);
	bool status = def->getBool("Status", true);
	
	AosCliModuleTorturerPtr module = 
			OmnNew AosCliModuleTorturer(name, minCmds, maxCmds, 
						weight, cliTorturer);

	if (!module)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return 0;
	}

	module->setStatus(status);
	return module;
}


bool
AosCliModuleTorturer::createSelector()
{
	int64_t *index = OmnNew int64_t[mCmds.entries()];
	u16 *weights = OmnNew u16[mCmds.entries()];
	for (int i=0; i<mCmds.entries(); i++)
	{
		index[i] = i;
		weights[i] = mCmds[i]->getWeight();
	}

	mCmdSelector = OmnNew AosRandomInteger(0);
	if (!mCmdSelector)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return false;
	}

	if (!mCmdSelector->set(index, index, weights, (u32)mCmds.entries()))
	{
		OmnAlarm << "Failed to set index" << enderr;
		return false;
	}

	return true;
}


bool
AosCliModuleTorturer::parseCmd(const OmnXmlItemPtr &item,
							   const AosCliTorturerLogType logType, 
							   OmnVList<AosGenTablePtr> &tables, 
							   AosVarListPtr &variables,
							   const AosCliModuleTorturerPtr &module,
							   const AosCliTorturerPtr &cliTorturer)
{
	if (!cliTorturer->isCommandOn(item)) 
	{
		return true;
	}

	AosCliCmdTorturerPtr cmd = AosCliCmdTorturer::createCmd(
		item, logType, tables, variables, cliTorturer);
	if (!cmd)
	{
		OmnAlarm << "Failed to create command: " << item->toString() << enderr;
		return false;
	}

	if (!module->addCommand(cmd))
	{
		OmnAlarm << "Failed to add command" << enderr;
		return false;
	}

	return true;
}


bool 			
AosCliModuleTorturer::addCommand(const AosCliCmdTorturerPtr &cmd)
{
	// 
	// Make sure the same command is not added multiple times
	//
	if (mCmds.entries() >= eMaxCommands)
	{
		OmnAlarm << "Too many commands in a module. The maximum allowed is: " 
			<< eMaxCommands << enderr;
		return false;
	}

	// 
	// Make sure the command is not already in the array
	//
	for (int i=0; i<mCmds.entries(); i++)
	{
		if (mCmds[i] == cmd)
		{
			// The command is already in the list
			OmnWarn << "The command is already in the list" << enderr;
			return false;
		}
	}

	// The command is not in the list. 
	AosCliTorturer::getSelf()->addCommand(cmd);
	if (cmd->getStatus()) mCmds.append(cmd);

	return true;
}


bool	
AosCliModuleTorturer::runCli(u32 &cmdsRun, const OmnTestMgrPtr &tm)
{
	static bool lsCorrectOnly = AosCliTorturer::isCorrectOnly();

	// 
	// This function will randomly determine how many commands to 
	// run, and then randomly select CLI commands to run. 
	// It will use the 
	// distribution model mDistModel to select command. 
	//
	if (!mCmdSelector)
	{
		createSelector();
	}

	cmdsRun = (u32)OmnRandom::nextInt(mMinNumCmds, mMaxNumCmds);
	if (!mCmdSelector->isGood())
	{
		cmdsRun = 0;
		OmnAlarm << "Command selector is not created properly before calling" 
			<< " this function: " << mModuleId << enderr;
		return false;
	}

	bool cmdGood;
	bool ignore;
	for (u32 i=0; i<cmdsRun; i++)
	{
		u32 index = (u32)mCmdSelector->nextInt();

		aos_assert_r(index < (u32)mCmds.entries(), false);

		if (!mCmds[index])
		{
			OmnAlarm << "No command set: " << (int)index << enderr;
			return false;
		}

		if (!mCmds[index]->run(cmdGood, lsCorrectOnly, tm, ignore))
		{
			OmnAlarm << "Failed to run the command: " 
				<< mCmds[index]->getName() << enderr;
			return false;
		}

		if (ignore) continue;

		mTotalCmdsRun++;
		if (!cmdGood) mTotalFailedCmds++;

		switch (mCheckType)
		{
		case eCheckAfterEveryCommand:
		 	 if (!check())
			 {
			 	return false;
			 }
			 break;

		case eCheckAfterEveryBlock:
		 	 mCmdRun++;
		 	 if (mCmdRun >= mCheckGroupSize)
		 	 {
			 	mCmdRun = 0;
			 	if (!check())
				{
					return false;
				}
		 	 }
		 	 break;

		default:
		 	 OmnAlarm << "Unrecognized check type: " << mCheckType << enderr;
		 	 return false;
		}
	}

	return true;
}

	
bool 	
AosCliModuleTorturer::check()
{
	bool s = true;
	for (int i=0; i<mCmds.entries(); i++)
	{
		if (!mCmds[i]->check())
		{
			s = false;
			mTotalFailedChecks++;
		}
	}

	return s;
}


u32				
AosCliModuleTorturer::getNumCmds() const
{
	return mCmds.entries();
}

