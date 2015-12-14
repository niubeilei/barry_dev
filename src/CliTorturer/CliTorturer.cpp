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
// 02/11/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "CliTorturer/CliTorturer.h"

#include "CliTorturer/Ptrs.h"
#include "CliTorturer/CliModuleTorturer.h"
#include "CliTorturer/FuncModule.h"
#include "CliTorturer/ApiCodeGen.h"
#include "CliTorturer/CliCmdTorturer.h"
#include "Debug/Debug.h"
#include "ProgramAid/Util.h"
#include "Random/RandomInteger.h"
#include "Tester/TestMgr.h"
#include "RVG/RVGInstGen.h"
#include "TorturerUtil/Util.h"
#include "Util/File.h"
#include "Util/GenTable.h"
#include "Util/VarList.h"
#include "Util1/Time.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"


static AosCliTorturerPtr sgCliTorturer;
static OmnString	sgAosHome;

bool AosIsLoggingCmd = false;
bool AosCliTorturer::mTestOnly = false;
bool AosCliTorturer::mCorrectOnly = false;

/*
AosCliTorturer::AosCliTorturer()
:
mTotalCmdsCollected(0),
mNumModulesRun(0),
mTotalCmdsRun(0),
mIsCodeGen(false), 
mFlushFlag(false)
{
	sgCliTorturer = this;
}
*/


AosCliTorturer::AosCliTorturer(const u32 cmdsToRun, 
							   const u32 torturingTime, 
							   const OmnString &confFilename, 
							   const OmnString &logFilename, 
							   const AosCliTorturerLogType logType, 
							   const bool codegen, 
							   const OmnString &datadir)
:
mLogType(logType),
mNumCmdsToRun(cmdsToRun),
mTorturingTime(torturingTime),
mTotalCmdsCollected(0),
mNumModulesRun(0),
mTotalCmdsRun(0),
mIsCodeGen(codegen),
mFlushFlag(false),
mLogFilename(logFilename)
{
	mDataDir = datadir;
	mIsGood = init(confFilename);
}


AosCliTorturer::AosCliTorturer(const u32 cmdsToRun, 
							   const u32 torturingTime, 
							   const OmnString &confFilename, 
							   const OmnString &logFilename, 
							   const AosCliTorturerLogType logType, 
							   const OmnString &srcdir, 
							   const OmnString &objname, 
							   const bool codegen, 
							   const OmnString &datadir)
:
mLogType(logType),
mNumCmdsToRun(cmdsToRun),
mTorturingTime(torturingTime),
mTotalCmdsCollected(0),
mNumModulesRun(0),
mTotalCmdsRun(0),
mIsCodeGen(codegen),
mFlushFlag(false),
mLogFilename(logFilename)
{
	mSrcDir = srcdir;
	mTorturerName = objname;
	mDataDir = datadir;
	mIsGood = init(confFilename);
}


bool
AosCliTorturer::init(const OmnString &confFilename)
{
	sgCliTorturer = this;
	if (!setDataDir()) return false;

	mVariables = OmnNew AosVarList();
	if (!config(confFilename, mIsCodeGen)) return false;

	if (mTorturingTime > 0)
	{
		mNumCmdsToRun = 0;
	}

	if (mLogType == eAosCliTorturerLogType_File)
	{
		if (!createNextLogFile())
		{
			return false;
		}
	}

	if (mNumCmdsToRun == 0 && mTorturingTime == 0)
	{
		mTorturingTime = eDefaultTorturingTime;
	}

	return true;
}


void
AosCliTorturer::setAosHome(const OmnString &home)
{
	sgAosHome = home;
}


bool
AosCliTorturer::setDataDir()
{
	if (mDataDir != "") return true;

	mDataDir = getenv("AOS_HOME");
	if (mIsCodeGen && mDataDir == "")
	{
		OmnAlarm << "The environment variable 'AOS_HOME' is not defined. "
			<< "Please define this environment variabel to the AOS directory!"
			<< enderr;
		return false;
	}

	if (mDataDir != "") mDataDir << "/Data/ApiTorturer";
	return true;
}


AosCliTorturer::~AosCliTorturer()
{
	if (!AosIsLoggingCmd && mLogFile)
	{
		OmnString str;
		str << "    cout << \"Finished executing all functions. \" << endl;\n"
			<< "    return true;\n"
			<< "}\n";
		mLogFile->append(str);
	}
}


AosCliTorturerPtr	
AosCliTorturer::getSelf()
{
	return sgCliTorturer;
}


bool
AosCliTorturer::config(const OmnString &filename, const bool codegen)
{
	mIsCodeGen = codegen;

	return processOneFile(filename);
}


bool
AosCliTorturer::processOneFile(const OmnString &filename)
{
	// 
	// The configure file is in the format of
	//  <TorturerInfo>
	//      <Name>
	//      <NumCommands>
	//      <TorturingTime>
	//  </TorturerInfo>
	// 	<Tables>
	// 		...
	// 	</Tables>
	// 	<InitCommands>
	// 		command
	// 		command
	// 		...
	// 	</InitCommands>
	// 	<LogIncludes>
	// 		...
	// 	</LogIncludes>
	// 	<InitVars>
	// 		name value
	// 		name value
	// 	</InitVars>
	// 	<InitAPI>
	// 	...
	// 	</InitAPI>
	// 	<CodeGenInfo>
	// 		<FuncTorturerDir>
	// 		<TorturerControllerDir>
	// 		<TorturerControllerName>
	// 		<LibName>
	// 		<AdditionalLibs>
	// 	</CodeGenInfo>
	// 	<Module>
	//  ...
	//  <Cmd>
	//  ...
	//  <FuncTest>
	//  ...
	//
	OmnXmlParser parser;
	OmnString fname = mDataDir;
	fname << "/" << filename;
	if (!parser.readFromFile(fname))
	{
		OmnAlarm << "Failed to read the config file from: " 
			<< filename << enderr;
		return false;
	}

	parser.reset();
	while (parser.hasMore())
	{
		OmnXmlItemPtr item = parser.nextItem();

		OmnString tagId = item->getTag();
		if (tagId == "InitVars")
		{
			mInitVars << item->getStr();
		}
		else if (tagId == "InitCommands")
		{
			mInitCommands = item->getStr();
		}
		else if (tagId == "TorturerInfo")
		{
		//	if (!parseTorturerInfo(item))
		//	{
		//		OmnAlarm << "Failed to parse TorturerInfo: " 
		//			<< item->toString() << enderr;
		//		return false;
		//	}
		}
		else if (tagId == "Table")
		{
			if (!createTable(item))
			{
				OmnAlarm << "Failed to create tables: " 
					<< item->toString() << enderr;
				return false;
			}
		}
		else if (tagId == "Module")
		{
			if (!parseModule(item))
			{
				OmnAlarm << "Failed to parse module: " 
					<< item->toString() << enderr;
				return false;
			}
		}
		else if (tagId == "Cmd")
		{
			if (!parseCmd(item))
			{
				OmnAlarm << "Failed to parse command: " 
					<< item->toString() << enderr;
				return false;
			}
			mCmdDefs.append(item);
		}
		else if (tagId == "InitAPI")
		{
			if (!parseInitAPI(item))
			{
				OmnAlarm << "Failed to parse InitAPI: " 
					<< item->toString() << enderr;
				return false;
			}
		}
		else if (tagId == "FuncTest")
		{
			addCommand(item->getStr("Name", ""), 
					   item->getBool("Status", true)); 
			if (mIsCodeGen)
			{
				if (!parseFunc(item))
				{
					OmnAlarm << "Failed to parse function: " 
						<< item->toString() << enderr;
					return false;
				}
			}
			else
			{
				if (!parseCmd(item))
				{
					OmnAlarm << "Failed to parse API: " 
						<< item->toString() << enderr;
					return false;
				}
			}
		}
		else if (tagId == "AdditionalLibs")
		{
			// mAdditionalLibs = item->getStr("AdditionalLibs", "");
			OmnString str = item->getStr();
			OmnStrParser parser(item->getStr());
			while (parser.hasMore())
			{
				OmnString word = parser.nextWord("", ' ', true);
				if (word == "")
				{
					break;
				}

				// 
				// Check whether the lib is already there
				//
				bool found = false;
				for (int i=0; i<mAdditionalLibs.entries(); i++)
				{
					if (mAdditionalLibs[i] == word)
					{
						found = true;
						break;
					}
				}

				if (!found) mAdditionalLibs.append(word);
			}
		}
		else if (tagId == "InstanceGen")
		{
			if (item->getBool("Status", true))
			{
				AosRVGInstGenPtr inst = 
					AosRVGInstGen::createInstanceGen(0, item, 
						tagId, mTables, mIsCodeGen);
				if (!inst)
				{
					OmnAlarm << "Failed to create Instance Generator: " 
						<< item->toString() << enderr;
					return false;
				}
			}
		}
		else if (tagId == "Commands")
		{
			addCommands(item);
		}
		else if (tagId == "Modules")
		{
			addModules(item);
		}
		else if (tagId == "XmlInclude")
		{
			OmnString filenames = item->getStr();
			processIncludes(filenames);
		}
		else if (tagId == "LogIncludes")
		{
			mLogIncludes = item->getStr();
			mLogIncludes.removeWhiteSpaces();
		}
		else if (tagId == "SourceCode")
		{
			processSourceCode(item);
		}
		else
		{
			OmnAlarm << "Unrecognized tag: " 
				<< item->toString() << enderr;
			return false;
		}
	}

	return true;
}


bool
AosCliTorturer::parseTorturerInfo(const OmnXmlItemPtr &torturerInfo)
{
	if (torturerInfo)
	{
		mName = torturerInfo->getStr("Name", "NoName");
		int numCmds = torturerInfo->getInt("NumCommands", 0);
		int torturingTime = torturerInfo->getInt("TorturingTime", 0);
		if (numCmds <= 0 && torturingTime <= 0)
		{
			// 
			// The default running one hour is assumed.
			//
			mNumCmdsToRun = 0;
			mTorturingTime = 3600;
		}
		else if (mTorturingTime > 0)
		{
			mTorturingTime = torturingTime;
			mNumCmdsToRun = 0;
		}
		else
		{
			mNumCmdsToRun = numCmds;
			mTorturingTime = 0;
		}
	}

	return true;
}


bool
AosCliTorturer::parseCmd(const OmnXmlItemPtr &def)
{
	AosCliTorturerPtr thisPtr(this, false);
	OmnString moduleId = def->getStr("ModuleId", "");
	AosCliModuleTorturerPtr module = getModule(moduleId);
	if (!module)
	{
		OmnAlarm << "Module not found: " << def->toString() << enderr;
		return false;
	}

	// 
	// Check whether the command is turned on
	//
	if (isCommandOn(def))
	{
		return module->parseCmd(def, 
					mLogType, mTables, mVariables, module, thisPtr);
	}

	// 
	// The command is not on
	//
	return true;
}


bool
AosCliTorturer::parseModule(const OmnXmlItemPtr &moduleDef)
{
	AosCliTorturerPtr thisPtr(this, false);
	AosCliModuleTorturerPtr module = AosCliModuleTorturer::createModule(
			moduleDef, mLogType, mTables, mVariables, thisPtr);
	if (!module)
	{
		OmnAlarm << "Failed to create the CLI module: " 
			<< moduleDef->toString() << enderr;
		return false;
	}
	
	if (module->getStatus())
	{
		if (!addModule(module))
		{
			OmnAlarm << "Failed to add module" << enderr;
			return false;
		}
	}

	return true;
}


bool	
AosCliTorturer::addModule(const AosCliModuleTorturerPtr &module)
{
	if (mModules.entries() >= eMaxModules)
	{
		OmnAlarm << "Too many modules: " << eMaxModules << enderr;
		return false;
	}

	mModules.append(module);
	addModule(module->getModuleId(), 
			  module->getStatus(), module->getWeight());

	module->setWeight(getModuleWeight(module->getModuleId()));
	return true;
}


bool
AosCliTorturer::start()
{
	createModuleSelector();

	if (!mTotalCmdsCollected)
	{
		OmnAlarm << "No commands collected!" << enderr;
		return false;
	}

	int startTime = OmnTime::getSecTick();
	mNumModulesRun = 0;
	mTotalCmdsRun = 0;
	// 
	// Run all the init commands
	//
	OmnString commands = mInitCommands;
	while (commands.length() > 0)
	{
		OmnString cmd = commands.getLine(true, 1);

        bool ret;
		OmnString rslt;
		aosRunCli1(cmd, true, ret, rslt, "");
	}

	// 
	// Initialize variables
	//
	mInitVars.removeWhiteSpaces();
	OmnString initVars = mInitVars;
	while (initVars.length() > 0)
	{
		OmnString line = initVars.getLine(true, 1);
		line.removeWhiteSpaces();
		if (line == "")
		{
			continue;
		}

		OmnStrParser parser(line);
		parser.skipWhiteSpace();
		OmnString name = parser.nextWord("", " ");
		OmnString value = parser.nextWord("", " ");
		if (name == "" || value == "")
		{
			OmnAlarm << "Invalid InitVars: " << mInitVars << enderr;
			return false;
		}

		if (!setVar(name, value))
		{
			OmnAlarm << "Failed to set value: " << name << ":" << value << enderr;
			return false;
		}
	}

	if (mNumCmdsToRun == 0 && mTorturingTime == 0) 
	{
		mTorturingTime = eDefaultTorturingTime;
	}

	while (1)
	{
		// 
		// Randomly determine whether to run APIs or CLIs
		//
		runClis();
			
		if (mTorturingTime)
		{
			if (OmnTime::getSecTick() - startTime >= mTorturingTime)
			{
				break;
			}
		}
		else if (mTotalCmdsRun >= mNumCmdsToRun)
		{
			break;
		}
	}

	u32 totalFailedCmds = 0;
	u32 totalFailedChecks = 0;
	for (int i=0; i<mModules.entries(); i++)
	{
		totalFailedCmds += mModules[i]->getTotalFailedCmds();
		totalFailedChecks += mModules[i]->getTotalFailedChecks();
	}

	cout << "Torturing Time:        " << OmnTime::getSecTick() - startTime << endl;
	cout << "Total Modules Run:     " << mNumModulesRun << endl;
	cout << "Total Commands Run:    " << mTotalCmdsRun << endl;
	cout << "Total Failed Commands: " << totalFailedCmds  << endl;
	cout << "Total Failed checks:   " << totalFailedChecks << endl;
	cout << "Success Percent:       " << (mTotalCmdsRun - totalFailedCmds) * 100.0 / mTotalCmdsRun << endl;
	cout << "Failed Percent:        " << totalFailedCmds * 100.0 / mTotalCmdsRun << endl;

	printTables();
	return true;
}


bool
AosCliTorturer::printTables()
{
	mTables.reset();
	while (mTables.hasMore())
	{
		AosGenTablePtr table = mTables.next();

		cout << "Table: " << table->getName() << endl;
		cout << table->toString() << endl;
	}

	return true;
}


bool
AosCliTorturer::createModuleSelector()
{
	if (mModuleSelector)
	{
		return true;
	}

	mTotalCmdsCollected = 0;
	int64_t *index = OmnNew int64_t[mModules.entries()];
	u16 *weight = OmnNew u16[mModules.entries()];
	for (int i=0; i<mModules.entries(); i++)
	{
		index[i] = i;
		if (mModules[i]->getNumCmds() > 0)
		{
			mTotalCmdsCollected += mModules[i]->getNumCmds();
			weight[i] = mModules[i]->getWeight();
		}
		else
		{
			weight[i] = 0;
		}
	}

	mModuleSelector = OmnNew AosRandomInteger(0);
	return mModuleSelector->set(index, index, weight, mModules.entries());
}
	

bool
AosCliTorturer::createTable(const OmnXmlItemPtr &def)
{
	try
	{
		AosGenTablePtr table = OmnNew AosGenTable(def);
		mTables.append(table);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to create table: " << def->toString() << enderr;
		return false;
	}

	return true;
}


OmnTestMgrPtr
AosCliTorturer::getTestMgr() const
{
	return mTestMgr;
}


bool
AosCliTorturer::setVar(const OmnString &name, const OmnString &value)
{
	if (!mVariables)
	{
		OmnAlarm << "mVariables is null: " << name << ":" << value << enderr;
		return false;
	}

	bool ret = mVariables->setValue(name, value, true);
	return ret;
}


bool
AosCliTorturer::setVar(const OmnString &name, const int value)
{
	OmnString str;
	str << value;
	return setVar(name, str);
}


bool
AosCliTorturer::getVar(const OmnString &name, OmnString &value) const
{
	if (!mVariables)
	{
		OmnAlarm << "mVariables is null: " << name << ":" << value << enderr;
		return false;
	}

	return mVariables->getValue(name, value);
}


AosCliModuleTorturerPtr
AosCliTorturer::getModule(const OmnString &moduleId) const
{
	for (int i=0; i<mModules.entries(); i++)
	{
		if (mModules[i]->getModuleId() == moduleId)
		{
			return mModules[i];
		}
	}

	return 0;
}


OmnXmlItemPtr
AosCliTorturer::getCmdDef(const OmnString &name) const
{
	for (int i=0; i<mCmdDefs.entries(); i++)
	{
		if (mCmdDefs[i]->getStr("Name", "") == name)
		{
			return mCmdDefs[i];
		}
	}

	return 0;
}


AosVarListPtr	
AosCliTorturer::getVariables()
{
	return mVariables;
}


OmnVList<AosGenTablePtr>& 
AosCliTorturer::getTables()
{
	return mTables;
}


OmnFilePtr 
AosCliTorturer::getLogFile() const
{
	return mLogFile;
}


AosCliTorturerLogType 
AosCliTorturer::getLogType() const
{
	return mLogType;
}


bool
AosCliTorturer::parseFunc(const OmnXmlItemPtr &def)
{
	if (!mFuncModule)
	{
		mFuncModule = OmnNew AosFuncModule("Test");
	}

	if (!mFuncModule->parseApi(def, mSrcDir))
	{
		OmnAlarm << "Failed to parse function: " << def->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosCliTorturer::genCode(const bool force) const
{
	if (!mFuncModule)
	{
		OmnAlarm << "No APIs are specified" << enderr;
		return false;
	}

	OmnString libs;
	for (int i=0; i<mAdditionalLibs.entries(); i++)
	{
		libs << " " << mAdditionalLibs[i];
	}

	if (!mFuncModule->genCode(mSrcDir, mTorturerName, libs, force))
	{
		OmnAlarm << "Failed to generate code" << enderr;
		return false;
	}

	return AosRVGInstGen::genAllCode(mSrcDir, force);
}


bool
AosCliTorturer::runClis()
{
	static u32 count = 10000;

	// 
	// Randomly select a module to run
	//
	u32 cmdsRun = 0;
	int index = mModuleSelector->nextInt();
	mModules[index]->runCli(cmdsRun, mTestMgr);
	mNumModulesRun++;
	mTotalCmdsRun += cmdsRun;

	if (mTotalCmdsRun > count)
	{
		cout << "Commands Run: " << count << endl;
		count += 10000;
	}

	// 
	// Check all the modules
	//
	for (int i=0; i<mModules.entries(); i++)
	{
		mModules[i]->check();
	}

	return true;
}


bool
AosCliTorturer::parseInitAPI(const OmnXmlItemPtr &def)
{
	// 
	// <InitAPI>
	//     <Includes> 
	//     		...
	//     </Includes>
	//     <API>
	//     		...
	//     </API>
	// </InitAPI>
	//
	OmnXmlItemPtr item = def->getItem("Includes");
	if (item)
	{
		mInitIncludes = item->getStr();
		mInitIncludes.removeLeadingWhiteSpace();
	}

	item = def->getItem("API");
	if (!item)
	{
		OmnAlarm << "Missing API tag: " << def->toString() << enderr;
		return false;
	}

	OmnString str = item->getStr();
	while (str != "")
	{
		OmnString line = str.getLine(true);
		line.removeWhiteSpaces();
		mInitAPIs << "    " << line << "\n";
	}
	return true;
}


// 
// This function checks whether the command 'def' 
// is on. If first checks the mCommands. If the command
// is found in it, it will tell whether it is on/off. 
// Otherwise, it checks 'def' to see whether it is on.
//
bool
AosCliTorturer::isCommandOn(const OmnXmlItemPtr &def)
{
	OmnString name = def->getStr("Name", "");
	for (int i=0; i<mCommands.entries(); i++)
	{
		if (mCommands[i] == name)
		{
			return mCommandStatus[i];
		}
	}

	bool status = def->getBool("Status", true);	
	return status;
}


bool
AosCliTorturer::isCommandOn(const OmnString &name)
{
	for (int i=0; i<mCommands.entries(); i++)
	{
		if (mCommands[i] == name)
		{
			return mCommandStatus[i];
		}
	}

	return false;
}


bool
AosCliTorturer::addCommand(const AosCliCmdTorturerPtr &cmd) 
{
	OmnString name = cmd->getName();
	for (int i=0; i<mCommands.entries(); i++)
	{
		if (mCommands[i] == name)
		{
			cmd->setStatus(mCommandStatus[i]);
			return true;
		}
	}

	mCommands.append(name);
	mCommandStatus.append(cmd->getStatus());
	return true;
}
bool
AosCliTorturer::addCommand(const OmnString &name, const bool status) 
{
	for (int i=0; i<mCommands.entries(); i++)
	{
		if (mCommands[i] == name)
		{
			return true;
		}
	}

	mCommands.append(name);
	mCommandStatus.append(status);
	return true;
}


bool
AosCliTorturer::saveCommandStatus(const OmnString &filename)
{
	// 
	// This function saves the commands:
	//	<Commands>
	//		Command Status
	//		Command Status
	//		...
	//	</Commands>
	//
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read the file: " << filename << enderr;
		return false;
	}

	// 
	// Calculate the max length of the commands.
	//
	int maxlen = 0;
	for (int i=0; i<mCommands.entries(); i++)
	{
		if (mCommands[i].length() > maxlen) maxlen = mCommands[i].length();
	}

	OmnString str;
	str << "<Commands>\n";
	for (int i=0; i<mCommands.entries(); i++)
	{
		OmnString cmd = mCommands[i];
		cmd.padChar(' ', maxlen+1, true);
		if (mCommandStatus[i])
		{
			str << "    " << cmd << "  true\n";
		}
		else
		{
			str << "    " << cmd << "  false\n";
		}
	}

	str << "</Commands>";
	
	if (!file.replace("<Commands>", "</Commands>", str))
	{
		OmnAlarm << "Failed the command lines" << enderr;
		return false;
	}

	file.flushFileContents();
	file.closeFile();
	return true;
}


bool
AosCliTorturer::addCommands(const OmnXmlItemPtr &item)
{
	// 
	// <Commands>
	//     Command Status
	//     Command Status
	//     ...
	// </Commands>
	//
	OmnString contents = item->getStr();
	contents.removeLeadingWhiteSpace();
	contents.removeTailWhiteSpace();
	while (contents != "")
	{
		OmnString line = contents.getLine(true);
		OmnStrParser parser(line);
		OmnString cmd = parser.nextWord("", ' ', true);
		OmnString status = parser.nextWord("", ' ', true);

		status.toLower();
		if (status != "true" && status != "false")
		{
			OmnAlarm << "Invalid status: " << cmd
				<< ":" << status << ". " << item->toString() << enderr;
			return false;
		}

		bool s = (status == "true")?true:false;
		addCommand(cmd, s);
	}

	return true;
}


bool
AosCliTorturer::addModules(const OmnXmlItemPtr &item)
{
	// 
	// <Modules>
	//     Module Status
	//     Module Status
	//     ...
	// </Modules>
	//
	OmnString contents = item->getStr();
	contents.removeLeadingWhiteSpace();
	contents.removeTailWhiteSpace();
	while (contents != "")
	{
		OmnString line = contents.getLine(true);
		OmnStrParser parser(line);
		OmnString module = parser.nextWord("", ' ', true);
		OmnString status = parser.nextWord("", ' ', true);
		OmnString weight = parser.nextWord("", ' ', true);

		status.toLower();
		if (status != "true" && status != "false")
		{
			OmnAlarm << "Invalid status: " << module 
				<< ":" << status << ". " << item->toString() << enderr;
			return false;
		}

		bool s = (status == "true")?true:false;
		addModule(module, s, atoi(weight));
	}

	return true;
}


bool
AosCliTorturer::addModule(const OmnString &name, 
						  const bool status, 
						  const u32 weight) 
{
	for (int i=0; i<mModuleInfo.entries(); i++)
	{
		if (mModuleInfo[i].name == name)
		{
			return true;
		}
	}

	ModuleInfo info;
	info.name = name;
	info.status = status;
	info.weight = weight;
	mModuleInfo.append(info);
	return true;
}


bool
AosCliTorturer::isModuleOn(const OmnXmlItemPtr &def)
{
	OmnString name = def->getStr("Name", "");
	for (int i=0; i<mModuleInfo.entries(); i++)
	{
		if (mModuleInfo[i].name == name)
		{
			return mModuleInfo[i].status;
		}
	}

	bool status = def->getBool("Status", true);	
	return status;
}


bool
AosCliTorturer::saveModuleStatus(const OmnString &filename)
{
	// 
	// This function saves the commands:
	//	<Modules>
	//		Module Status Weight
	//		Module Status Weight
	//		...
	//	</Modules>
	//
	OmnFile file(filename, OmnFile::eReadOnly);
	if (!file.isGood())
	{
		OmnAlarm << "Failed to read the file: " << filename << enderr;
		return false;
	}

	// 
	// Calculate the max length of the commands.
	//
	int maxlen = 0;
	for (int i=0; i<mModuleInfo.entries(); i++)
	{
		if (mModuleInfo[i].name.length() > maxlen) 
		{
			maxlen = mModuleInfo[i].name.length();
		}
	}

	OmnString str;
	str << "<Modules>\n";
	for (int i=0; i<mModuleInfo.entries(); i++)
	{
		OmnString module = mModuleInfo[i].name;
		module.padChar(' ', maxlen+1, true);
		if (mModuleInfo[i].status)
		{
			str << "    " << module << "  true  " 
				<< mModuleInfo[i].weight << "\n";
		}
		else
		{
			str << "    " << module << "  false " 
				<< mModuleInfo[i].weight << "\n";
		}
	}

	str << "</Modules>";
	
	if (!file.replace("<Modules>", "</Modules>", str))
	{
		OmnAlarm << "Failed the replace Modules" << enderr;
		return false;
	}

	file.flushFileContents();
	file.closeFile();
	return true;
}


u32
AosCliTorturer::getModuleWeight(const OmnString &name)
{
	for (int i=0; i<mModuleInfo.entries(); i++)
	{
		if (mModuleInfo[i].name == name) return mModuleInfo[i].weight;
	}

	OmnAlarm << "Module not found: " << name << enderr;
	return 0;
}


bool
AosCliTorturer::processIncludes(const OmnString &filenames)
{
	OmnString fnames = filenames;
	fnames.removeLeadingWhiteSpace();
	fnames.removeTailWhiteSpace();

	while (fnames != "")
	{
		OmnString fname = fnames.getLine(true);
		if (!processOneFile(fname))
		{
			OmnAlarm << "Failed to process the include: " 
				<< fname << ". " << filenames << enderr;
			return false;
		}
	}

	return true;
}


bool
AosCliTorturer::processSourceCode(const OmnXmlItemPtr &def)
{
	// 
	// This tag tells this facility to copy the file into 
	// the target directory.
	//
	//  <SourceCode>filename</SourceCode>
	//
	OmnString fn = def->getStr();

	OmnString cmd = "cp ";
	cmd << mDataDir << "/" << fn << " "
		<< AosProgramAid_getAosHomeDir()
		<< "/src/" << mSrcDir << "/" << fn;
	system(cmd.data());
	return true;
}


OmnString
AosCliTorturer::dumpVariables() const
{
	return mVariables->toString();
}


OmnFilePtr
AosCliTorturer::createNextLogFile()
{
	static int lsFileIndex = 0;

	if (mLogFile)
	{
		mLogFile->closeFile();
		AosCliCmdTorturer::setLogfile(0);
	}

	OmnString fn = mLogFilename;
	fn << "_" << lsFileIndex << ".cpp";
	mLogFile = OmnNew OmnFile(fn, OmnFile::eCreate);
	lsFileIndex++;
	if (!mLogFile->isGood())
	{
		OmnAlarm << "Failed to open the file: " 
			<< fn << enderr;
		return 0;
	}

	AosCliCmdTorturer::setLogfile(mLogFile);
	return mLogFile;
}


int
AosCliTorturer::getNextFuncIndex()
{
	static int lsFuncIndex = 1;
	return lsFuncIndex++;
}


OmnString		
AosCliTorturer::getLogIncludes() const
{
	return mLogIncludes;
}


OmnString AosCliTorturer_getVar(const OmnString &name)
{
	OmnString value;
	if (AosCliTorturer::getSelf()->getVar(name, value)) return value;

	OmnAlarm << "Failed to retrieve variable: " << name << enderr;
	return "NotFound";
}


bool AosCliTorturer_setVar(const OmnString &name, const OmnString &value)
{
	return AosCliTorturer::getSelf()->setVar(name, value);
}

