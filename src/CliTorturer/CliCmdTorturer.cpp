////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliConfigCmd.h
// Description:
//   
//
// Modification History:
// 02/10/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CliTorturer/CliCmdTorturer.h"

#include "alarm/Alarm.h"
#include "Alarm/Alarm.h"
#include "CliTorturer/CliCmdGeneric.h"
#include "CliTorturer/FuncModule.h"
#include "CliTorturer/CliTorturer.h"
#include "CliTorturer/CliModuleTorturer.h"
#include "CliTorturer/ApiTorturer.h"
#include "Debug/Debug.h"
#include "TorturerConds/Cond.h"
#include "TorturerConds/CondData.h"
#include "Random/RandomInteger.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Parms/RVG.h"
#include "TorturerConds/Util.h"
#include "TorturerConds/CondArith.h"
#include "TorturerConds/CondNameValue.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/GenTable.h"
#include "Util/GenRecord.h"
#include "Util/VarList.h"
#include "Util/StrParser.h"
#include "XmlParser/XmlItem.h"
#include <math.h>

extern bool AosIsLoggingCmd;

static OmnMutex sgLock;
static u32 sgCmdId = 0;
static OmnDynArray<u32>			sgCorrectCmdStat;
static OmnDynArray<u32>			sgIncorrectCmdStat;
static OmnDynArray<u32>			sgFailedCmdStat;
static OmnDynArray<u32>			sgCmdExecutedStat;
static OmnDynArray<OmnString>	sgCmdNames;
static int sgFuncIndex = 0;

OmnFilePtr AosCliCmdTorturer::mLogFile;


AosCliCmdTorturer::AosCliCmdTorturer(const OmnString &name)
:
mLogType(eAosCliTorturerLogType_NoLog),
mCmdName(name),
mCorrectPct(eDefaultCorrectPct),
mAction(eUnknownAction),
// mKeyType(eAosRVGKeyType_Unknown), Temp commented out by Chen Ding
mWeight(eDefaultCmdWeight),
mCorrectPctSet(false),
mSelectFromTablePct(eDefaultSelectFromTablePct),
mStatus(false), 
mCorrectOnly(false)
{
	mTestOnly = AosCliTorturer::getTestOnlyFlag();
}


AosCliCmdTorturer::AosCliCmdTorturer(
					const AosCliTorturerLogType logType, 
					const AosCliTorturerPtr &cliTorturer)
:
mLogType(logType),
mCorrectPct(eDefaultCorrectPct),
mAction(eUnknownAction),
// mKeyType(eAosRVGKeyType_Unknown), Temp commented out by Chen Ding
mWeight(eDefaultCmdWeight),
mCorrectPctSet(false),
mSelectFromTablePct(eDefaultSelectFromTablePct),
mStatus(false), 
mCorrectOnly(false)
{
	mTestOnly = AosCliTorturer::getTestOnlyFlag();
}


AosCliCmdTorturer::~AosCliCmdTorturer()
{
}


bool
AosCliCmdTorturer::setCmdId()
{
	sgLock.lock();
	mId = sgCmdId++;
	sgCorrectCmdStat.append(0);
	sgIncorrectCmdStat.append(0);
	sgFailedCmdStat.append(0);
	sgCmdExecutedStat.append(0);
	sgCmdNames.append("");
	sgLock.unlock();
	return true;
}


bool
AosCliCmdTorturer::logCmd(const OmnString &cmd, 
						  const bool isCorrect, 
						  const OmnString &errmsg, 
						  const u32 cmdId)
{
	static bool lsFlushFlag = AosCliTorturer::getSelf()->isFlushOn();
	OmnString str;

	cmdStat(mId, isCorrect);

	AosIsLoggingCmd = true;
	if (isCorrect)
	{
		(str = "CLI Command(y): ") << cmdId << " \"";
		str << cmd << "\"\n";
	}
	else
	{
		(str = "Cli Command(n): ") << cmdId << " \"";
		str << cmd << "\". Errmsg: " << errmsg << "\n";
	}

	switch (mLogType)
	{
	case eAosCliTorturerLogType_NoLog:
		 return true;

	case eAosCliTorturerLogType_OnScreen:
		 cout << str;
		 return true;

	case eAosCliTorturerLogType_File:
		 if (!mLogFile)
		 {
		 	// 
			// Ignore the logging
			//
			return true;
		 }
		 mLogFile->append(str, lsFlushFlag);
		 return true;

	default:
		 OmnAlarm << "Unrecognized log type: " << mLogType << enderr;
		 return false;
	}
}


u32
AosCliCmdTorturer::calculateParmCrtPct(
						const u32 cmdCorrectPct, 
						const u32 numParms)
{
	// 
	// If the correct percentage for a command is 'cmdCorrectPct'
	// and that command has n number of arguments, each argument's
	// correct percent will be 1 - root_n(cmdCorrectPct).
	//
	if (cmdCorrectPct > 100 || cmdCorrectPct <= 0)
	{
		OmnAlarm << "Invalid percentage: " << cmdCorrectPct << enderr;
		return cmdCorrectPct;
	}
	
	if (numParms >= eMaxArgs)
	{
		OmnAlarm << "Too many arguments: " << numParms << enderr;
		return cmdCorrectPct;
	}

	float fff = pow(cmdCorrectPct/100.0, 1.0/numParms);

	return (u32) ((fff + 0.005)* 100);
}


// 
// This is a virtual function that can be overridden by a torturer
// to do something before a record is added to a table. By default,
// this function does nothing.
//
bool
AosCliCmdTorturer::preAddToTableProc(const AosGenTablePtr &table, 
							  const AosGenRecordPtr &record,
							  bool &cmdCorrect, 
							  OmnString &cmdErrmsg)
{
	return true;
}


bool
AosCliCmdTorturer::addToTable(const AosGenTablePtr &table, 
							  bool &cmdCorrect, 
							  OmnString &cmdErrmsg)
{
	// 
	// Need to add a record based on the command to the table.
	//
	cmdCorrect = false;
	aos_assert_r(table, false);
	if (table->isFull())
	{
		cmdErrmsg << "Exceeded the maximum: " << table->getMax();
		return true;
	}

	AosGenRecordPtr record = table->createRecord();
	aos_assert_r(record, false);
	record->resetMarks();

	for (int i=0; i<mArgs.entries(); i++)
	{
		if (!mArgs[i]->newRecordAdded(record))
		{
			OmnAlarm << "Failed to process the event: newRecordAdded" << enderr;
			return false;
		}
	}

	cmdCorrect = true;
	if (!preAddToTableProc(table, record, cmdCorrect, cmdErrmsg) || !cmdCorrect)
	{
		return false;
	}

	record->removeMarks();
	// 
	// Append the record
	//
	return table->appendRecord(record);
}


bool
AosCliCmdTorturer::setParmCorrectPct()
{
	// 
	// Set the successful percentage
	//
	u32 crtPct = calculateParmCrtPct(mCorrectPct, mArgs.entries());
	for (int i=0; i<mArgs.entries(); i++)
	{
		mArgs[i]->setCorrectPct(crtPct);
	}

	return true;
}


/*
bool	
AosCliCmdTorturer::deleteFromTable(const AosGenRecordPtr &record,
					   OmnString &cmdErrmsg)
{
	// 
	// A record is removed from the table. We will inform every
	// parms in the command and let the parms decide whether 
	// they need to do anything about it.
	//
	aos_assert_r(record, false);

	record->resetMarks();
	for (int i=0; i<mArgs.entries(); i++)
	{
		if (!mArgs[i]->recordDeleted(record))
		{
			OmnAlarm << "Failed to process the event: record removed" <<enderr;
			return false;
		}
	}
	record->removeMarks();

	return true; 
}


bool
AosCliCmdTorturer::doesCommandIdentifyRecord(const AosGenTablePtr &table) 
{
	// 
	// Need to handle the subkey cases. If the key type
	// is SubkeyNew, and the command identifies a record
	// in 'table', it returns true. Otherwise, it returns
	// false.
	//
	if (!table) return true;

	// 
	// Create a record based on the command
	//
	AosGenRecordPtr record = table->createRecord();
	aos_assert_r(record, false);
	record->resetMarks();
	for (int i=0; i<mArgs.entries(); i++)
	{
		if (mArgs[i]->setRecord(record))
		{
			OmnAlarm << "Failed to process the event: setRecord" << enderr;
			return false;
		}
	}

	return table->recordExist(record);	
}
*/


bool
AosCliCmdTorturer::addArg(const AosRVGPtr &arg)
{
	mArgs.append(arg);
	return true;
}


AosCliCmdTorturerPtr
AosCliCmdTorturer::createCmd(const OmnXmlItemPtr &def, 
							 const AosCliTorturerLogType logType, 
							 OmnVList<AosGenTablePtr> &tables, 
							 AosVarListPtr &variables,
							 const AosCliTorturerPtr &cliTorturer)
{
	AosCliCmdTorturerPtr cmd;
	OmnString type = def->getStr("CmdType", "");
	if (type == "Generic")
	{
		// 
		// Create a generic CLI Cmd torturer
		//
		cmd = OmnNew AosCliCmdGeneric(logType, cliTorturer);

		if (!cmd)
		{
			OmnAlarm << "Run out of memory" << enderr;
			return 0;
		}

		if (!cmd->config(def, cliTorturer))
		{
			OmnAlarm << "Failed to create command: " << def->toString() << enderr;
			return 0;
		}
	}
	else if (type == "API")
	{
		cmd = AosApiTorturer::createTorturer(def, logType, cliTorturer);
	}
	else
	{
		OmnAlarm << "Unrecognized command type: " << type 
			<< def->toString() << enderr;
		return 0;
	}

	return cmd;
}


bool
AosCliCmdTorturer::config(const OmnXmlItemPtr &def, 
						  const AosCliTorturerPtr &torturer) 
{
	// 
	// 	<Cmd>
	// 		...
	// 		<CmdType>
	// 		<Action>
	// 		<Name>
	// 		<CorrectPct>
	// 		<Prefix>
	// 		<KeyType>
	// 		<Weight>
	// 		<TableName>
	// 		<Conditions>
	// 		<DepTables>
	// 			<Entry>tablename field field ...</Entry>
	// 			...
	// 		</DepTables>
	// 		<OverrideFlag>
	// 		<SetVars>
	// 			<Entry>name value</Entry>
	// 			...
	// 		</SetVars>
	// 		<Parms>
	// 			<Parm>
	// 				<type>
	// 			</Parm>
	// 			...
	// 		</Parms>
	// 	</Cmd>
	//
	/*
	 * Temp commented out by Chen Ding, 12/02/2007
	 *
	AosVarListPtr variables = torturer->getVariables();
	OmnVList<AosGenTablePtr> &tables = torturer->getTables();

	mCorrectPct = def->getInt("CorrectPct", eDefaultCorrectPct);
	mCmdName = def->getStr("Name", "NoName");
	mAction = getAction(def->getStr("Action", "DoNothing"));
	mWeight = def->getInt("Weight", eDefaultCmdWeight);
	mKeyType = AosGetRVGKeyType(def->getStr("KeyType", "NoKey"));
	OmnString tableName = def->getStr("TableName", "");
	mStatus = def->getBool("Status", true);
	mLogType = torturer->getLogType();
	mLogFile = torturer->getLogFile();
	mOverrideFlag = def->getBool("OverrideFlag", false);

	setCmdId();
	(sgCmdNames[mId] = def->getStr("ModuleId", ""))
		<< "::" << mCmdName;

	mConds.clear();
	if (!AosTortCond::parseConditions(def->getItem("Conditions"), mConds))
	{
		OmnAlarm << "Failed to parse conditions: " << def->toString() << enderr;
		return false;
	}

	OmnDynArray<DelInfo> depTables;
	if (!parseDepTables(def->getItem("DepTables"), 
			mDelFromOtherTablesEntries, tables))
	{
		OmnAlarm << "Failed to parse DepTables: " << def->toString() << enderr;
		return false;
	}

	if (!parseSetvar(def->getItem("SetVars"), mVarNames, mVarValues, variables))
	{
		OmnAlarm << "Failed to parse SetVar tag: " << def->toString() << enderr;
		return false;
	}

	if (tableName != "")
	{
		tables.reset();
		while (tables.hasMore())
		{
			AosGenTablePtr tt = tables.next();
			if (tt->getName() == tableName)
			{
				mTable = tt;
				break;
			}
		}

		if (!mTable)
		{
			OmnAlarm << "Table not defined: " << tableName 
				<< " in the def: " << def->toString() << enderr;
			return false;
		}
	}

	if (mAction == eUnknownAction)
	{
		OmnAlarm << "Unrecognized action: " << def->toString() << enderr;
		return false;
	}

	if (mKeyType == eAosRVGKeyType_Unknown)
	{
		OmnAlarm << "Unrecognized key type: " << def->toString() << enderr;
		return false;
	}


	OmnXmlItemPtr parms = def->getItem("Parms");
	if (!parms)
	{
		OmnAlarm << "Missing parms tag: " << def->toString() << enderr;
		return false;
	}

	parms->reset();
	AosCliCmdTorturerPtr thisPtr(this, false);
	while (parms->hasMore())
	{
		OmnXmlItemPtr parmDef = parms->next();
		AosRVGPtr parm = AosRVG::createRVG(thisPtr, parmDef, 
			def->getTag(), tables, false);
		if (!parm)
		{
			OmnAlarm << "Failed to create parm: " 
				<< parmDef->toString() << enderr;
			return false;
		}

		addArg(parm);
	}
	*/

	return true;
}


AosCliCmdTorturer::CmdAction
AosCliCmdTorturer::getAction(const OmnString &str)
{
	OmnString name = str;
	name.toLower();

	if (name == "donothing")
	{
		return eDoNothing;
	}
	
	if (name == "addtotable")
	{
		return eAddToTable;
	}

	if (name == "deletefromtable")
	{
		return eDeleteFromTable;
	}

	if (name == "deletefromselfanddeptables")
	{
		return eDeleteFromSelfAndDepTables;
	}

	if (name == "setvar")
	{
		return eSetVar;
	}

	if (name == "cleartable")
	{
		return eClearTable;
	}

	OmnAlarm << "Unrecognized action: " << str << enderr;
	return eUnknownAction;
}


bool
AosCliCmdTorturer::checkCmdConditions(
			bool &cmdGood, 
			AosRVGReturnCode &rcode, 
			OmnString &errmsg, 
			const bool beforeExecution)
{
	AosCliCmdTorturerPtr thisPtr(this, false);
	bool rslt;
	mCondData.setArgs(mArgs);
	mCondData.setVars(AosCliTorturer::getSelf()->getVariables());

	for (int i=0; i<mConds.entries(); i++)
	{
		AosTortCondPtr c = mConds[i];
		bool check = false;
		switch (c->getCheckTime())
		{
		case AosTortCond::eCheckBeforeExecution:
			 check = beforeExecution;
			 break;

		case AosTortCond::eAlwaysCheck:
			 check = true;
			 break;

		case AosTortCond::eCheckAfterExecution:
			 check = !beforeExecution;
			 break;

		default:
			 OmnAlarm << "Unrecognized check time: " 
			 	<< c->getCheckTime() << enderr;
			 return false;
		}
			 		
		if (check)
		{
			if (!c->check(mCondData, rslt, errmsg) || !rslt)
			{
				cmdGood = false;
				return true;
			}
		}
	}
	return true;
}


bool
AosCliCmdTorturer::getValue(const OmnString &argName, 
							OmnString &value) const
{
	OmnString fn; 
	AosRVGType type;
	return getValue(argName, fn, value, type);
}


bool
AosCliCmdTorturer::getValue(const OmnString &argName, 
							OmnString &fieldName,
							OmnString &value, 
							AosRVGType &type) const
{
	/*
	 * Temp commented out by Chen Ding, 12/02/2007
	 *
	for (int i=0; i<mArgs.entries(); i++)
	{
		if (mArgs[i]->getName() == argName)
		{
			type = mArgs[i]->getRVGType();
			fieldName = mArgs[i]->getFieldName();
			if (!mArgs[i]->getCrtValue(value))
			{
				OmnAlarm << "Failed to retrieve parm value: " << argName << enderr;
				return false;
			}

			return true;
		}
	}

	OmnAlarm << "Parm not found: " << argName << enderr;
	*/
	return false;
}
	

bool
AosCliCmdTorturer::deleteFromDepTables()
{
	// 
	// This means the current command wants to delete the entries
	// from dependent tables. The entries to be deleted are identified
	// by some of the arguments constructed in the command. 
	//
	for (int i=0; i<mDelFromOtherTablesEntries.entries(); i++)
	{
		// 
		// Retrieve the values
		//
		OmnDynArray<OmnString> values;
		OmnDynArray<OmnString> fieldNames;
		OmnDynArray<OmnString> args = mDelFromOtherTablesEntries[i].mArgs;
		AosGenTablePtr table = mDelFromOtherTablesEntries[i].mTable;
		for (int k=0; k<args.entries(); k++)
		{
			OmnString fieldName;
			OmnString v;
			AosRVGType type;
			if (!getValue(args[k], fieldName, v, type))
			{
				OmnAlarm << "Failed to retrieve the value: " 
					<< table->getName() << ":" << args[k] << enderr;
				return false;
			}

			values.append(v);
			fieldNames.append(fieldName);
		}

		if (!table->removeRecords(fieldNames, values))
		{
			OmnString fff;
			for (int k=0; k<fieldNames.entries(); k++)
			{
				fff << fieldNames[k];
			}
			OmnAlarm << "Failed to delete records from other tables: " 
				<< table->getName() << ":" << fff << enderr;
			return false;
		}
	}

	return true;
}


bool
AosCliCmdTorturer::parseDepTables(
						const OmnXmlItemPtr &def, 
						OmnDynArray<DelInfo> &depTables, 
						OmnVList<AosGenTablePtr> &tables) 
{
	// 
	// 		<DepTables>
	// 			<Entry>tablename argname argname ...</Entry>
	// 			...
	// 		</DepTables>
	// 
	if (!def) return true;

	def->reset();
	while (def->hasMore())
	{
		OmnXmlItemPtr item = def->next();
		OmnString values = item->getStr();
		OmnStrParser parser(values);
		OmnString tablename = parser.nextWord("", " ");

		// 
		// Find the table
		//
		tables.reset();
		DelInfo entry;
		while (tables.hasMore())
		{
			AosGenTablePtr table = tables.next();
			if (table->getName() == tablename)
			{
				entry.mTable = table;
			}
		}

		if (!entry.mTable)
		{
			OmnAlarm << "Failed to find the table: " << tablename
				<< ". " << def->toString() << enderr;
			return false;
		}

		while (!parser.finished())
		{
			entry.mArgs.append(parser.nextWord("", " "));

		}

		depTables.append(entry);
	}

	return true;
}


bool
AosCliCmdTorturer::setVars(const OmnDynArray<OmnString> &names, 
						   const OmnDynArray<OmnString> &values)
{
	mVarNames = names;
	mVarValues = values;
	return true;
}


bool
AosCliCmdTorturer::parseSetvar(const OmnXmlItemPtr &def, 
							   OmnDynArray<OmnString> &names, 
							   OmnDynArray<OmnString> &values, 
							   AosVarListPtr &variables) 
{
	// 
	// 	<SetVars>
	// 		<Entry>name value</Entry>
	// 		<Entry>name value</Entry>
	//	...
	//	</SetVars>
	//
	if (!def) return true;

	def->reset();
	while (def->hasMore())
	{
		OmnXmlItemPtr item = def->next();
		OmnStrParser parser(item->getStr());
		OmnString name = parser.nextWord("", " ");
		OmnString value = parser.nextWord("", " ");
		if (name == "" || value == "")
		{
			OmnAlarm << "Incorrect setvar tag: " << def->toString() << enderr;
			return false;
		}

		names.append(name);
		values.append(value);
		variables->addVar(name, false);
	}

	return true;
}


bool		
AosCliCmdTorturer::getGlobalVar(const OmnString &varname, OmnString &varvalue)
{
	return AosCliTorturer::getSelf()->getVar(varname, varvalue);
}


bool
AosCliCmdTorturer::logApi(const OmnString &cmd, 
						  const bool isCorrect, 
						  const OmnString &errmsg, 
						  const u32 cmdId)
{
	static bool lsFlushFlag = AosCliTorturer::getSelf()->isFlushOn();
	static int  lsNumCmdsLogged = 0;

	OmnString str;
	cmdStat(mId, isCorrect);

	if (isCorrect)
	{
		str << "    // Command " << cmdId << " Correct"
		    << "    " << cmd << "\n\n";
	}
	else
	{
		str << "    // Command " << cmdId << " Incorrect: " << errmsg
		    << "    " << cmd << "\n\n";
	}

	switch (mLogType)
	{
	case eAosCliTorturerLogType_NoLog:
		 return true;

	case eAosCliTorturerLogType_OnScreen:
		 cout << str;
		 return true;

	case eAosCliTorturerLogType_File:
		 if (!mLogFile)
		 {
		 	// 
			// Ignore the logging
			//
			return true;
		 }
		 
		 if (lsNumCmdsLogged > eMaxCmdsPerLogFile)
		 {
			 // 
			 // Need to close the current log file and create
			 // a new log file.
			 //
			 OmnString str;
			 str << "    return api_torturer_special_case"
				 << sgFuncIndex + 1
				 << "();\n}\n";
			 mLogFile->append(str, false);
			 mLogFile = AosCliTorturer::getSelf()->createNextLogFile();
			 if (!mLogFile)
			 {
				 OmnAlarm << "Failed to create new log file" << enderr;
				 return false;
			 }

			 lsNumCmdsLogged = 0;
		 }
		 mLogFile->append(str, lsFlushFlag);
		 lsNumCmdsLogged++;
		 return true;

	default:
		 OmnAlarm << "Unrecognized log type: " << mLogType << enderr;
		 return false;
	}
}


bool
AosCliCmdTorturer::cmdExecutedStat(const u32 cmdId)
{
	if (cmdId >= (u32)sgCmdExecutedStat.entries())
	{
		OmnAlarm << "Invalid command ID: " << cmdId
			<< ". Total commands: " << sgCmdExecutedStat.entries() << enderr;
		return false;
	}

	sgCmdExecutedStat[cmdId]++;
	return true;
}


bool
AosCliCmdTorturer::cmdFailedStat(const u32 cmdId)
{
	if (cmdId >= (u32)sgFailedCmdStat.entries())
	{
		OmnAlarm << "Invalid command ID: " << cmdId
			<< ". Total commands: " << sgFailedCmdStat.entries() << enderr;
		return false;
	}

	sgFailedCmdStat[cmdId]++;
	return true;
}


bool
AosCliCmdTorturer::cmdStat(const u32 cmdId, const bool success)
{
	if (cmdId >= (u32)sgCorrectCmdStat.entries())
	{
		OmnAlarm << "Invalid command ID: " << cmdId
			<< ". Total commands: " << sgCorrectCmdStat.entries() << enderr;
		return false;
	}

	if (success)
	{
		sgCorrectCmdStat[cmdId]++;
	}
	else
	{
		sgIncorrectCmdStat[cmdId]++;
	}

	return true;
}


bool
AosCliCmdTorturer::init()
{
	for (int i=0; i<sgCorrectCmdStat.entries(); i++)
	{
		sgCorrectCmdStat[i] = 0;
		sgIncorrectCmdStat[i] = 0;
		sgFailedCmdStat[i] = 0;
		sgCmdExecutedStat[i] = 0;
	}

	return true;
}


OmnString
AosCliCmdTorturer::getStat()
{
	OmnString str;
	
	// 
	// Find the maximum length of the CmdNames
	//
	int i;
	u32 cmdMaxLen = 0;
	u32 totalCorrect = 0;
	u32 totalIncorrect = 0;
	u32 totalFailed = 0;
	u32 totalExecuted = 0;
	for (i=0; i<sgCmdNames.entries(); i++)
	{
		if (cmdMaxLen < (u32)sgCmdNames[i].length()) 
		{
			cmdMaxLen = sgCmdNames[i].length();
		}

		totalCorrect += sgCorrectCmdStat[i];
		totalIncorrect += sgIncorrectCmdStat[i];
		totalFailed += sgFailedCmdStat[i];
		totalExecuted += sgCmdExecutedStat[i];
	}

	u32 cmdMaxLen1 = cmdMaxLen;
	cmdMaxLen += 3;
	u32 totalCmdsRun = totalCorrect + totalIncorrect;
	str << "Total Commands Run: " << totalCmdsRun << "\n\n";
	OmnString line;
	line << "Command Names";
	line.padChar(' ', cmdMaxLen, false);
	line << "Total";
	line.padChar(' ', cmdMaxLen + 10, false);
	line << "Correct";
	line.padChar(' ', cmdMaxLen + 20, false);
	line << "Incorrect";
	line.padChar(' ', cmdMaxLen + 30, false);
	line << "Failed";
	line.padChar(' ', cmdMaxLen + 40, false);
	line << "Executed";
	line.padChar(' ', cmdMaxLen + 50, false);
	line << "Percent";
	str << line << "\n";

	line = "";
	line.padChar('-', cmdMaxLen + 58, false);
	str << line << "\n";
	u32 maxlen;
	for (i=0; i<sgCmdNames.entries(); i++)
	{
		maxlen = cmdMaxLen;
		OmnString line = sgCmdNames[i];
		line.padChar(' ', maxlen, true);
		u32 tt = sgCorrectCmdStat[i] + sgIncorrectCmdStat[i];

		maxlen += 10;
		line << tt;
		line.padChar(' ', maxlen, true);

		maxlen += 10;
		line << sgCorrectCmdStat[i];
		line.padChar(' ', maxlen, true);

		maxlen += 10;
		line << sgIncorrectCmdStat[i];
		line.padChar(' ', maxlen, true);
	
		maxlen += 10;
		line << sgFailedCmdStat[i];
		line.padChar(' ', maxlen, true);
	
		maxlen += 10;
		line << sgCmdExecutedStat[i];
		line.padChar(' ', maxlen, true);
	
		maxlen += 10;
		line << (float)(tt * 100.0 / totalCmdsRun);
	
		str << line << "\n";
	}

	line = "";
	line.padChar('-', cmdMaxLen + 58, false);
	str << line << "\n";

	line = "Total: ";
	line << sgCmdNames.entries();
	line.padChar(' ', cmdMaxLen1+3, false);

	line << totalCmdsRun;
	maxlen = cmdMaxLen1 + 13;
	line.padChar(' ', maxlen, true);
	line << totalCorrect;

	maxlen += 10;
	line.padChar(' ', maxlen, true);
	line << totalIncorrect;

	maxlen += 10;
	line.padChar(' ', maxlen, true);
	line << totalFailed;

	maxlen += 10;
	line.padChar(' ', maxlen, true);
	line << totalExecuted;

	str << line;
	return str;
}


bool
AosCliCmdTorturer::setLogfile(const OmnFilePtr &file)
{
	mLogFile = file;

	if (!file) return true;
	
	OmnString str;
	str << "#include \"aosUtil/Types.h\"\n"
		<< "#include \"Debug/Debug.h\"\n" 
		<< "#include \"Util/String.h\"\n" 
		<< "#include \"Util/IpAddr.h\"\n";

	sgFuncIndex = AosCliTorturer::getNextFuncIndex();
	OmnString incls = AosCliTorturer::getSelf()->getLogIncludes();
	while (incls != "")
	{
		OmnString line = incls.getLine(true);
		line.removeWhiteSpaces();
		str << "#include \"" << line << "\"\n"
			<< "extern int api_torturer_special_case"
			<< sgFuncIndex + 1 << "();\n";
	}

	if (sgFuncIndex == 1)
	{
		str << "\nbool api_torturer_special_case() \n"
			<< "{\n";
		str << "    " << AosCliTorturer::getSelf()->getInitAPI() << "\n\n";
	}
	else
	{
		str << "\nbool api_torturer_special_case" << sgFuncIndex
			<< "() \n" << "{\n";
	}

	mLogFile->append(str, false);
	return true;
}


void		
AosCliCmdTorturer::setDepTables(const OmnDynArray<DelInfo> &tables)
{
	mDelFromOtherTablesEntries = tables;
}


bool
AosCliCmdTorturer::preCheck(bool &cmdCorrect)
{
	return true;
}


bool
AosCliCmdTorturer::postCheck(bool &cmdCorrect)
{
	return true;
}


bool
AosCliCmdTorturer::checkVar(const OmnString &name, const int value)
{
	OmnString str;
	str << value;
	return checkVar(name, str);
}


bool
AosCliCmdTorturer::checkVar(const OmnString &name, const OmnString &value)
{
	OmnString str;
	if (!AosCliTorturer::getSelf()->getVar(name, str)) 
	{
		OmnTCTrue(false) << "The variable: " 
			<< name << " is not defined!" << endtc;
		return false;
	}

	if (value != str)
	{
		OmnTCTrue(false) << "Expecting the variable: " 
			<< name << " to be: " << value 
			<< " but actual value is: " << str << endtc;
		return false;
	}

	return true;
}


bool
AosCliCmdTorturer::setVar(const OmnString &name, const int value)
{
	OmnString str;
	str << value;
	return setVar(name, str);
}


bool
AosCliCmdTorturer::setVar(const OmnString &name, const OmnString &value)
{
OmnTrace << "To set variable: " << name << ":" << value << endl;
	if (!AosCliTorturer::getSelf()->setVar(name, value)) 
	{
		OmnTCTrue(false) << "Failed to set var: " 
			<< name << ". Make sure the variable is defined!" << endtc;
		return false;
	}

	return true;
}


OmnString
AosCliCmdTorturer::getVar(const OmnString &name) 
{
OmnTrace << "To get variable: " << name << endl;
	OmnString value;
	if (!AosCliTorturer::getSelf()->getVar(name, value)) 
	{
		OmnTCTrue(false) << "Failed to retrieve varable: " 
			<< name << ". Make sure the variable is defined!" << endtc;
		return false;
	}

	return value;
}


bool
// AosCliCmdTorturer::setParm(const OmnString &name, 
AosCliCmdTorturer::setRVG(const OmnString &name, 
						   const OmnString &value, 
						   OmnString &errmsg)
{
	/*
	 * Temp commented out by Chen Ding, 12/02/2007
	 *
	for (int i=0; i<mArgs.entries(); i++)
	{
		if (mArgs[i]->getName() == name)
		{
			return mArgs[i]->setValue(value);
		}
	}

	errmsg << "To set Parameter value but not found: " 
		<< name << ":" << value << ". ";
	*/
	return false;
}


bool
AosCliCmdTorturer::setCrtValue(const OmnString &name, 
						   const OmnString &value, 
						   OmnString &errmsg)
{
	/*
	 * Temp commented out by Chen Ding, 12/02/2007
	 *
	for (int i=0; i<mArgs.entries(); i++)
	{
		if (mArgs[i]->getName() == name)
		{
			return mArgs[i]->setCrtValue(value);
		}
	}

	errmsg << "To set Parameter value but not found: " 
		<< name << ":" << value << ". ";
	*/
	return false;
}


OmnString
AosCliCmdTorturer::getParm(const OmnString &name) const
{
	OmnString value;
	if (getValue(name, value)) return value;

	OmnAlarm << "Failed to retrieve parameter: " << name << enderr;
	return "NotFOund";
}

