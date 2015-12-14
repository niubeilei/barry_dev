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
// mAddIfCorrect
// If this command creates a new command, the command is correct, 
// and this member data is true, it will add a record into the 
// table. The record will be set based on the command.
//
// mTestOnly
// If this is set to true, it will not run the command, but just
// generate commands.
//
// Modification History:
// 02/08/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "CliTorturer/CliCmdGeneric.h"

#include "alarm/Alarm.h"
#include "CliTorturer/CliTorturer.h"
#include "CliTorturer/FuncModule.h"
#include "CliTorturer/CliModuleTorturer.h"
#include "Parms/ReturnCode.h"
#include "Parms/RVGStr.h"
#include "Parms/RVG.h"
#include "KernelInterface/CliProc.h"
#include "Random/RandomInteger.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/GenTable.h"
#include "Util/File.h"
#include "Util/VarList.h"
#include "Util/GenRecord.h"

AosCliCmdGeneric::AosCliCmdGeneric(
					const AosCliTorturerLogType logType, 
					const AosCliTorturerPtr &cliTorturer)
:
AosCliCmdTorturer(logType, cliTorturer)
{
}


/*
AosCliCmdGeneric::AosCliCmdGeneric(
					const OmnString &name, 
					const u32 correctPct,
					const CmdAction action,
					const OmnString &prefix, 
					const AosRVGKeyType type, 
					const u32 weight,  
					const AosCliTorturerLogType logType, 
					const AosGenTablePtr &table, 
					const AosCliTorturerPtr &cliTorturer, 
					const OmnDynArray<AosTortCondPtr> &conds) 
:
AosCliCmdTorturer(name, correctPct, action, type, weight, logType, table, cliTorturer, conds),
mCmdPrefix(prefix)
{
	if (!cliTorturer)
	{
		OmnAlarm << "CliTorturer is null" << enderr;
		mTestOnly = true;
	}

	mTestOnly = AosCliTorturer::getTestOnlyFlag();
	mTestMgr = cliTorturer->getTestMgr();
}
*/


AosCliCmdGeneric::~AosCliCmdGeneric()
{
}


bool 	
AosCliCmdGeneric::config(const OmnXmlItemPtr &def, 
						 const AosCliTorturerPtr &torturer)
{
	if (!AosCliCmdTorturer::config(def, torturer))
	{
		return false;
	}

	mCmdPrefix = def->getStr("Prefix", "");
	if (mCmdPrefix == "")
	{
		OmnAlarm << "Missing Prefix tag: " << def->toString()
			<< enderr;
		return false;
	}

	return true;
}


// 
// This function will generate a new command. A command
// is in the form of:
// 		<command_prefix> <arg1> <arg2> ... <argn>
// Generating a command is to generate a value for each of
// the arguments, randomly. 
//
// The generated command can be a correct one or bad one. 
// A command can be bad in one of the following ways:
// 	1. One or more arguments are not good.
// 	2. The command missed some of the arguments.
// 	
// Below is how to generate a new command:
// 1. Subkey_Exist
// The key to identify records contain more than one field. 
// If the table is not null and empty, it will decide whether
// to generate a new command that identifies a record in the
// table. This is controlled by the random variable: 
// SelectFromTablePct. 
//
bool
AosCliCmdGeneric::run(bool &cmdCorrect, 
					  const bool correctOnly,
					  const OmnTestMgrPtr &tm, 
					  bool &ignore)
{
	/*
	 * Temp commented out by Chen Ding, 12/02/2007
	 *
	static u32 cmdId = 0;

	OmnString cmd;
	OmnString arg;
	OmnString cmdErrmsg;
	OmnString errmsg;
	bool isCorrect = false;
	bool ret;

	cmdCorrect = true;
	cmd << mCmdPrefix;

	AosGenRecordPtr record;
	bool selectFromRecord = false;

	if (!mCorrectPctSet)
	{
		setParmCorrectPct();
		mCorrectPctSet = true;
	}

	// 
	// Handle the subkey cases first
	//
	switch (mKeyType)
	{
	case eAosRVGKeyType_MultiKeyExist:
		 // 
		 // The command uses more than one arguments to identify 
		 // a record in the table. 
		 // If the table is not null and empty, we will determine
		 // whether to select the values based on a record. 
		 //
		 if (mTable && mTable->entries() && aos_next_pct(mSelectFromTablePct))
		 {
			// 
			// The record has not been picked yet.
			//
			aos_assert_r(record = mTable->getRandomRecord(), false);
			selectFromRecord = true;
		 }
		 break;

	case eAosRVGKeyType_MultiKeyNew:
		 // 
		 // THe command uses more than one argument to identify
		 // records in the table. One incorrect situation that 
		 // cannot be handled by individual parms but here is
		 // to generate a command whose arguments identify a 
		 // record. 
		 //
		 if (mTable && mTable->entries() && !aos_next_pct(mCorrectPct))
		 {
			// 
			// We want to simulate an error condition that 
			// the command will identify a record.
			//
			aos_assert_r(record = mTable->getRandomRecord(), false);
			selectFromRecord = true;
			cmdCorrect = false;
			cmdErrmsg << "Command identifies an entry. ";
		 }
		 break;

	case eAosRVGKeyType_SingleKeyExist:
	case eAosRVGKeyType_SingleKeyNew:
	case eAosRVGKeyType_NoKey:
		 selectFromRecord = false;
		 break;

	case eAosRVGKeyType_Unknown:
	case eAosRVGKeyType_LastEntry:
		 OmnAlarm << "Unknown arg type" << enderr;
		 return false;
	}

	AosRVGReturnCode rcode;
	if (!record)
	{
		if (!mTable)
		{
			selectFromRecord = false;
		}
		else
		{
			record = mTable->createRecord();
			aos_assert_r(record, false);
		}
	}

	OmnString fieldName;
	for (int i=0; i<mArgs.entries(); i++)
	{
		arg = "";
		ret = mArgs[i]->nextStr(arg, mTable, record, isCorrect, 
					mCorrectOnly || correctOnly, selectFromRecord, 
					rcode, errmsg);
		OmnTCTrue(ret) << endtc;
		if (mCliTorturer)
		{
			OmnString tcname = mCliTorturer->nextTcName();
			OmnExpected<bool> exp(true);
			OmnActual<bool>  act(ret);
			// mCliTorturer->nextTc(OmnExpected<bool>(true), OmnActual<bool>(ret), __FILE__, __LINE__);
			mCliTorturer->nextTc(exp, act, __FILE__, __LINE__);
			OmnEndTc ec;
			mCliTorturer->operator << (ec);
		}

		if (!ret)
		{
			OmnAlarm << "Failed to create parm: " << mArgs[i]->getName() << enderr;
			return false; 
		}

		if (!isCorrect)
		{
			cmdErrmsg << errmsg << ". ";
			cmdCorrect = false;
		}

		fieldName = mArgs[i]->getFieldName();
		if (fieldName != "" && !selectFromRecord && record)
		{
			record->set(fieldName, arg);
		}

		cmd << " " << arg;
	}

	// 
	// If a new record has been created and the command is correct, 
	// we need to set values to the record and add the record
	// to the table.
	//
	if (cmdCorrect)
	{
		if (!checkCmdConditions(cmdCorrect, rcode, errmsg, true))
		{
			OmnAlarm << "Failed to check the conditions" << enderr;
			return false;
		}

		if (!cmdCorrect)
		{
			cmdErrmsg << errmsg;
		}
	}

	// 
	// If the command is still correct and if the command is multiKeyNew,
	// we need to check whether it identifies a record in the table. 
	// If yes, it is incorrect.
	//
	if (cmdCorrect && mKeyType == eAosRVGKeyType_MultiKeyNew)
	{
		if (mTable && mTable->recordExist(record))
		{
			cmdErrmsg = "Record exist in the table";
			cmdCorrect = false;
		}
	}

	if (cmdCorrect)
	{
		switch (mAction)
		{
		case eDoNothing:
		 	 break;

		case eAddToTable:
		 	 addToTable(mTable, cmdCorrect, cmdErrmsg);
		 	 break;

		case eDeleteFromTable:
		 	 if (!mTable)
		 	 {
		 	 	OmnAlarm << "To delete record but table is null: " << mCmdName << enderr;
		 	 	break;
		 	 }

		 	 if (!mTable->removeRecord(record))
			 {
			 	OmnAlarm << "Failed to remove record: " << mCmdName << enderr;
			 }
		 	 break;

		case eDeleteFromSelfAndDepTables:
			 if (!mTable)
			 {
		 	 	OmnAlarm << "To delete record but table is null: " << mCmdName << enderr;
		 	 	break;
		 	 }

		 	 if (!mTable->removeRecord(record))
			 {
			 	OmnAlarm << "Failed to remove record: " << mCmdName << enderr;
			 }
			 	
			 if (!deleteFromDepTables())
			 {
			 	OmnAlarm << "Failed to delete from depended tables: " << cmd << enderr;
				return false;
			 }
			 break;

		case eSetVar:
			 for (int i=0; i<mVarNames.entries(); i++)
			 {
			 	if (!mCliTorturer->setVar(mVarNames[i], mVarValues[i]))
				{
					OmnAlarm << "Failed to set value: "
						<< mVarNames[i] << ":" << mVarValues[i] << enderr;
					return false;
				}
			 }
			 break;

		default:
		 	 OmnAlarm << "Unrecognized CmdAction: " << mAction << enderr;
		}
	}

	cmdId++;
	if (!mTestOnly)
	{
		// OmnString syscmd("/usr/local/rhc/bin/Cli.exe -cmd \"");
		// syscmd << cmd << "\"";
		// system(syscmd);
		// cout << "Run the command: " << syscmd << endl;
		bool ret;
		OmnString rslt;
		aosRunCli1(cmd, cmdCorrect, ret, rslt, cmdErrmsg);

		if (cmdCorrect != ret)
		{
			if (cmdCorrect)
			{	
				OmnAlarm << "Mismatch found: " << cmdCorrect << ":" << ret 
					<< ":" << cmdId << ":" << cmd 
					<< ". CLI Reported Error: " << rslt << enderr;
			}
			else
			{
				OmnAlarm << "Mismatch found: " << cmdCorrect << ":" << ret 
					<< ":" << cmdId << ":" << cmd << ". Torturer Error: " << cmdErrmsg 
					<< ". CLI Reported Error: " << rslt << enderr;
			}
		}
	}

	logCmd(cmd, cmdCorrect, cmdErrmsg, cmdId);
	*/
	return true;
}


bool 	
AosCliCmdGeneric::check()
{
	return true;
}

OmnString
AosCliCmdGeneric::toString() const
{
	OmnString str;
	str = "CliCmdGeneric: \n";
	str << "   	Prefix:              " << mCmdPrefix << "\n"
		<< "    Number of Arguments: " << mArgs.entries() << "\n";

	for (int i=0; i<mArgs.entries(); i++)
	{
		str << mArgs[i]->toString(4) << "\n";
	}

	return str;
}

