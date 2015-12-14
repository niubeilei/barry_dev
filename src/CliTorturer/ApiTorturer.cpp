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
#include "CliTorturer/ApiTorturer.h"

#include "alarm/Alarm.h"
#include "CliTorturer/FuncModule.h"
#include "CliTorturer/CliTorturer.h"
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

OmnDynArray<AosApiTorturerPtr>	AosApiTorturer::mApiTorturers;

AosApiTorturer::AosApiTorturer(
					const AosCliTorturerLogType logType, 
					const AosCliTorturerPtr &cliTorturer)
:
AosCliCmdTorturer(logType, cliTorturer)
{
}


AosApiTorturer::AosApiTorturer(const OmnString &name)
:
AosCliCmdTorturer(name)
{
}


AosApiTorturer::~AosApiTorturer()
{
}


bool
AosApiTorturer::config(const OmnXmlItemPtr &def, 
					   const AosCliTorturerPtr &torturer)
{
	if (!AosCliCmdTorturer::config(def, torturer))
	{
		OmnAlarm << "Failed to config the object" << enderr;
		return false;
	}

	mFuncName = def->getStr("FuncName", "");
	if (mFuncName == "")
	{
		OmnAlarm << "Missing FuncName tag: " << def->toString() << enderr;
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
AosApiTorturer::run(bool &cmdCorrect, 
					const bool correctOnly,
					const OmnTestMgrPtr &tm, 
					bool &ignore)
{
	static u32 cmdId = 0;

	ignore = false; // If 'ignore' is true, the caller should ignore
					// this invocation. This means that this call
					// did not generate an API function call.
	mTestMgr = tm;
	AosGenRecordPtr record;
	bool selectFromRecord = false;
	OmnString cmdErrmsg;
	cmdCorrect = true;
	OmnString cmd;
	cmd << mFuncName << "(";

	if (!mCorrectPctSet)
	{
		setParmCorrectPct();
		mCorrectPctSet = true;
	}

	// 
	// Determine whether need to use a record in the databasae table.
	// If the databasse is not null and empty and If it is MultiKeyExist 
	// or MultiKeyNew, we may randomly select a record from the table.
	//
	switch (mKeyType)
	{
	case AosCmdKeyType::eRead:
	case AosCmdKeyType::eModify:
	case AosCmdKeyType::eNewModify:
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
			cmdCorrect = true;
		 }
		 break;

	case AosCmdKeyType::eNewOnly:
		 // 
		 // The command uses more than one argument to identify
		 // records in the table. One incorrect situation that 
		 // cannot be handled by individual parms but here is
		 // to generate a command whose arguments identify a 
		 // record (where mOverrideFlag == false). 
		 //
		 if (!(correctOnly || mCorrectOnly) && mTable && 
			  mTable->entries() && aos_next_pct(mSelectFromTablePct))
		 {
			// 
			// We want to simulate an error condition that 
			// the command will identify a record.
			//
			aos_assert_r(record = mTable->getRandomRecord(), false);
			selectFromRecord = true;

			if (mOverrideFlag)
			{
				cmdCorrect = true;
			}
			else
			{
				cmdCorrect = false;
				cmdErrmsg << "Command identifies an entry. ";
			}
		 }
		 break;

	default:
		 OmnAlarm << "Unknown arg type" << enderr;
		 return false;
	}

	// 
	// If this class uses a tabale, we need to create a record
	// regardless of whether this run should use a record from
	// the table or not.
	//
	AosRVGReturnCode rcode;
	if (!record && mTable)
	{
		record = mTable->createRecord();
		aos_assert_r(record, false);
	}

	OmnString fieldName;
	bool isCorrect;
	std::string arg;
	std::string errmsg;
	bool first = true;
	OmnString decl;
	OmnString deleteStmt;

	// 
	// Generate the values for each parameter.
	// Note that not all parameters need to generate values. A parameter
	// needs to generate value only when it is 'input' type. 
	//
	for (int i=0; i<mArgs.entries(); i++)
	{
		isCorrect = true;
		fieldName = mArgs[i]->getFieldName();

		// 
		// Check whether need to generate values for it.
		//
		if (mArgs[i]->getUsageMethod() == AosRVGUsageType::eInputOnly ||
		    mArgs[i]->getUsageMethod() == AosRVGUsageType::eInputOutputThrRef ||
		    mArgs[i]->getUsageMethod() == AosRVGUsageType::eInputOutputThrAddr)
		{
			/*
			 * Temp commented out by Chen Ding, 12/02/2007
			 *
			arg = "";
			bool ret = mArgs[i]->nextStr(arg, mTable, record, isCorrect, 
					(correctOnly || mCorrectOnly), selectFromRecord, 
					rcode, errmsg);
			OmnTCTrue(ret) << ret << ". Failed to generate value for ParmName: " 
				<< mArgs[i]->getName() 
				<< ". Function Name: " << mFuncName 
				<< "(...). Errmsg: " << errmsg << endtc;

			if (!ret)
			{
				OmnAlarm << "Failed to create parm: " 
					<< mArgs[i]->getName() << enderr;
				return false; 
			}
			*/

			// 
			// 'isCorrect' is true means that a syntactically correct
			// value was generated. But its correctness may be changed
			// by other conditions. 
			//
			// Chen Ding, 10/08/2007
			// But why do we need this? It should
			// be done at argument level, not here.
			//
			/*
			 * Temp commented out by Chen Ding, 12/02/2007
			 *
			if (isCorrect) 
			{
				if (!mArgs[i]->checkCond(isCorrect, errmsg))
				{
					OmnAlarm << "Failed to evaluate the condition: " 
						<< mArgs[i]->getName() << enderr;
					return false;
				}
			}

			if ((mCorrectOnly || correctOnly)&& !isCorrect)
			{
				// 
				// The command wants to generate correct cases only. 
				// Since the parm failed to generate a correct value, 
				// ignore it.
				//
				ignore = true;
				return true;
			}

			if (!isCorrect)
			{
				cmdErrmsg << errmsg << ". ";
				cmdCorrect = false;
			}

			if (fieldName != "" && !selectFromRecord && record)
			{
				record->set(fieldName, arg);
			}
			*/
		}

		//
		// Argument value was generated, we need now to generate 
		// a string for how to use the value in the function call. 
		// This is done by calling the function "getCrtValueAsArg".
		// If it needs to declare some local variables, the declarations
		// are returned through "decl".
		//
		/*
		 * Temp commented out by Chen Ding, 12/03/2007
		 *
		if (!mArgs[i]->getCrtValueAsArg(arg, decl, i))
		{
			OmnAlarm << "Failed to get the argument: " 
				<< mArgs[i]->getName() << enderr;
			return false;
		}

		// 
		// If local variables are declared and memory are allocated
		// for them, then we need to construct statements to delete
		// the memory after calling the funciton. This is what 
		// "getDeleteStmt(...)" does.
		//
		if (!mArgs[i]->getDeleteStmt(deleteStmt, i))
		{
			OmnAlarm << "Failed to get the delete statement: " 
				<< mArgs[i]->getName() << enderr;
			return false;
		}

		if (first)
		{
			first = false;
			cmd << arg;
		}
		else
		{
			cmd << ", " << arg;
		}
		*/
	}

	// 
	// The function call has been constructed. Need to determine 
	// whether local variable declarations are needed. This is 
	// determined by "decl".
	//
	cmd << ")";
	if (decl != "")
	{
		OmnString theCmd = "\n    {\n";
		theCmd << "        " << decl 
			<< "\n        cout << \"CmdId "
			<< cmdId << ": \" << " << cmd << " << endl;\n";
		if (deleteStmt != "")
		{
			theCmd << "        " << deleteStmt << "\n";
		}
		theCmd << "    }";
		cmd = theCmd;
	}
	else
	{
		OmnString theCmd = "\n    cout << \"CmdId ";
		theCmd << cmdId << ": \" << ";
		theCmd << cmd << " << endl;\n";
		cmd = theCmd;
	}
OmnTrace << "To command: " << cmd << endl;

	// 
	// If we just want correct function call but the generated is not, 
	// ignore it. 
	//
	if ((correctOnly || mCorrectOnly) && !cmdCorrect)
	{
		ignore = true;
		return true;
	}

	// 
	// Perform system defined command level semantics checks.
	//
	if (cmdCorrect)
	{
		switch (mKeyType)
		{
		case AosCmdKeyType::eNewOnly:
			 // 
			 // If it is a New Object key and the command identifies
			 // a record, it is incorrect. Note that if it is single
			 // key New Object Key, its semantics check was already
			 // done at parameter level.
			 //
			 if (mTable && mTable->recordExist(record))
			 {
				 cmdErrmsg = "Record exist in the table";
				 cmdCorrect = false;
			 }
			 break;

		case AosCmdKeyType::eRead:
		case AosCmdKeyType::eModify:
		case AosCmdKeyType::eNewModify:
			 // 
			 // If it is an Existing Object key and the command
			 // does not identify a record, it is incorrect. 
			 // Note that if it is single key Existing object key,
			 // its semantics check was already done at 
			 // parameter level.
			 //
			 if (mTable && !mTable->recordExist(record))
			 {
			 	cmdErrmsg = "Record does not exist in the table";
				cmdCorrect = false;
			 }
			 break;

		default: 
		 	 OmnAlarm << "Unrecognized key type: " << mKeyType << enderr;
			 return false;
		}
	}

	if ((correctOnly || mCorrectOnly) && !cmdCorrect)
	{
		ignore = true;
		return true;
	}

	// 
	// Perform command level user defined semantics checks. 
	//
	if (cmdCorrect)
	{
		/*
		 * Temp commented out by Chen Ding, 12/02/2007
		 *
		if (!checkCmdConditions(cmdCorrect, rcode, errmsg, true))
		{
			OmnAlarm << "Failed to check the conditions" 
				<< cmd << ":" << cmdId+1 << enderr;
			return false;
		}

		if ((correctOnly || mCorrectOnly) && !cmdCorrect)
		{
			ignore = true;
			return true;
		}

		if (!cmdCorrect)
		{
			cmdErrmsg << errmsg;
		}
		*/
	}

	OmnTrace << "API: " << cmdId << ", cmdCorrect: " << cmdCorrect << ":" << cmd << endl;

	if (cmdCorrect)
	{
		switch (mAction)
		{
		case eDoNothing:
		 	 break;

		case eClearTable:
			 mTable->clearTable();
			 break;

		case eAddToTable:
			 if (selectFromRecord && mOverrideFlag)
			 {
				 // 
				 // The record exists in the table and the command
				 // allows to override the record. Do not need to
				 // add to the table.
				 //
				 break;
			 }

		 	 if (!addToTable(mTable, cmdCorrect, cmdErrmsg))
			 {
			 	OmnAlarm << "Failed to add to table: " 
					<< cmd << ". CmdId: " << cmdId << enderr;
				return false;
			 }
		 	 break;

		case eDeleteFromTable:
		 	 if (!mTable)
		 	 {
		 	 	OmnAlarm << "To delete record but table is null: " 
					<< cmd << ". CmdId: " << cmdId << enderr;
		 	 	break;
		 	 }

		 	 if (!mTable->removeRecord(record))
			 {
			 	OmnAlarm << "Failed to remove record: " 
					<< cmd << ". CmdId: " << cmdId  
					<< "\n Record: " << record->toString()
					<< "\n Table: " << mTable->toString() << enderr;
			 }
		 	 break;

		case eDeleteFromSelfAndDepTables:
			 if (!mTable)
			 {
		 	 	OmnAlarm << "To delete record but table is null: " 
					<< cmd << ". CmdId: " << cmdId << enderr;
		 	 	break;
		 	 }

		 	 if (!mTable->removeRecord(record))
			 {
			 	OmnAlarm << "Failed to remove record: " 
					<< cmd << ". CmdId: " << cmdId << enderr;
			 }
			 	
			 if (!deleteFromDepTables())
			 {
			 	OmnAlarm << "Failed to delete from depended tables: " 
					<< cmd << ". CmdId: " << cmdId << enderr;
				return false;
			 }
			 break;

		case eSetVar:
			 for (int i=0; i<mVarNames.entries(); i++)
			 {
			 	if (!AosCliTorturer::getSelf()->
						setVar(mVarNames[i], mVarValues[i]))
				{
					OmnAlarm << "Failed to set value: "
						<< mVarNames[i] << ":" << mVarValues[i] 
						<< ". Cmd: " << cmd << ". CmdId: " << cmdId << enderr;
					return false;
				}
			 }
			 break;

		default:
		 	 OmnAlarm << "Unrecognized CmdAction: " << mAction << enderr;
		}
	}

	// 
	// Perform argument level actions. 
	//
	for (int i=0; i<mArgs.entries(); i++)
	{
		/*
		 * Temp commented out by Chen Ding, 12/02/2007
		 *
		bool rslt;
		if (!mArgs[i]->doAction(rslt, errmsg))
		{
			OmnAlarm << "Failed to do action: " << mArgs[i]->getName()
				<< enderr;
			return false;
		}
		 
		cmdCorrect = (cmdCorrect && rslt);
		*/
	}

	if ((mCorrectOnly || correctOnly) && !cmdCorrect)
	{
		ignore = true;
		return true;
	}

	// logApi(cmd, cmdCorrect, cmdErrmsg, cmdId);
	OmnTrace << "To run func: " << getName() << endl;
	bool crtOnly = mCorrectOnly || correctOnly;
	bool cmdExecuted = false;

	// 
	// To run the pre-execution actions
	//
	bool rslt;
	if (!doPreAction(rslt, cmdErrmsg))
	{
		OmnAlarm << "Failed to do the action: " 
			<< getName() << ". Errmsg: " << cmdErrmsg << enderr;
		return false;
	}

	// To run the function
	bool execution = runFunc(crtOnly, AosCliTorturer::getSelf(), 
			tm, cmdCorrect, cmdErrmsg, cmdExecuted);
	OmnTCTrue(execution) << "Failed to run the function: " 
		<< cmd << ". Execution Error Message: " << mExecutionErrmsg
		<< endtc;

	if (cmdExecuted) cmdExecutedStat(mId);
	if (!execution) cmdFailedStat(mId);

	if (mTestOnly || cmdExecuted) logApi(cmd, cmdCorrect, cmdErrmsg, cmdId);

	if (!doPostAction(rslt, cmdErrmsg))
	{
		OmnAlarm << "Failed to do the action: " 
			<< getName() << ". Errmsg: " << cmdErrmsg << enderr;
		return false;
	}

	u32 crtCmdId = cmdId++;
	if (execution)
	{
		/*
		 * Temp commented out by Chen Ding, 12/02/2007
		 *
		bool actual = true;
		if (!checkCmdConditions(actual, rcode, errmsg, false))
		{
			OmnAlarm << "Failed to check the conditions. Cmd: " 
				<< cmd << ". CmdId: " << crtCmdId << enderr;
			return false;
		}

		if (mExecutionFailed)
		{
			OmnTCTrue(false) << "Execution failed. Errmsg: " 
				<< mExecutionErrmsg 
				<< ". Cmd: " << cmd << ". CmdId: " << crtCmdId << endtc;
			cmdFailedStat(mId);
		}
		else if (cmdCorrect != actual)
		{
			cmdFailedStat(mId);
			if (cmdCorrect)
			{
				OmnTCTrue(false) << "Expecting success but "
					<< "actual failed. CmdID: " 
					<< crtCmdId << ". Command: " << cmd 
					<< ". CmdId: " << crtCmdId 
					<< ". Errmsg: " << cmdErrmsg << ":" << errmsg << endtc;
			}
			else
			{
				OmnTCTrue(false) << "Mismatch found: Expecting failed but "
					<< "actual success. CmdID: "
					<< crtCmdId << ". Function: " << cmd 
					<< ". CmdId: " << crtCmdId
					<< ". Errmsg: " << cmdErrmsg << endtc;
			}
			AosCliTorturer::getSelf()->printTables();
		}
		*/
	}
	return true;
}


bool 	
AosApiTorturer::check()
{
	return true;
}


OmnString
AosApiTorturer::toString() const
{
	OmnString str;
	str = "ApiTorturer: \n";
	str << "   	FuncName:            " << mFuncName << "\n"
		<< "    Number of Arguments: " << mArgs.entries() << "\n";

	for (int i=0; i<mArgs.entries(); i++)
	{
		str << mArgs[i]->toString(4) << "\n";
	}

	return str;
}


bool	
AosApiTorturer::addApiTorturer(const AosApiTorturerPtr &t)
{
OmnTrace << "To add API torturer: " << t->getName() << endl;

	for (int i=0; i<mApiTorturers.entries(); i++)
	{
		if (mApiTorturers[i]->getName() == t->getName())
		{
			OmnAlarm << "Torturer already in the list: " 
				<< t->getName() << enderr;
			return false;
		}
	}
	mApiTorturers.append(t);
	return true;
}


AosApiTorturerPtr
AosApiTorturer::createTorturer(
					const OmnXmlItemPtr &def, 
					const AosCliTorturerLogType logType, 
					const AosCliTorturerPtr &cliTorturer) 
{
	OmnString name = def->getStr("Name", "");
	AosApiTorturerPtr torturer = getTorturer(name);
	if (!torturer)
	{
		OmnAlarm << "Torturer: " << name << " not found!" << enderr;
		return 0;
	}

	return torturer->createInstance(def, cliTorturer);
}


AosApiTorturerPtr
AosApiTorturer::getTorturer(const OmnString &name)
{
	for (int i=0; i<mApiTorturers.entries(); i++)
	{
		if (mApiTorturers[i]->getName() == name)
		{
			return mApiTorturers[i];
		}
	}

	return 0;
}


bool	
AosApiTorturer::checkCmdConditions(
					bool &cmdGood, 
					AosRVGReturnCode &rcode, 
					OmnString &errmsg, 
					const bool beforeExecution)
{
	mCondData.setReturnValue(mFuncRet);
	return AosCliCmdTorturer::checkCmdConditions(
				cmdGood, rcode, errmsg, beforeExecution);
}


