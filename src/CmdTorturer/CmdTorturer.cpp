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
// 11/24/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CmdTorturer/CmdTorturer.h"

#include "aos/aosReturnCode.h"
#include "alarm/Alarm.h"
#include "Action/Action.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "RVG/Parm.h"
#include "RVG/Table.h"
#include "RVG/RVGMultiFromTable.h"
#include "Random/RandomUtil.h"
#include "SemanticRules/SemanticRule.h"
#include "Semantics/SemanticsReturnCodes.h"
#include "Semantics/SemanticsUtil.h"
#include "SemanticData/SdContainer.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Torturer/Torturer.h"
#include "Conditions/Condition.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include <math.h>

static OmnMutex sgLock;
static u32 sgCmdId = 0;
static std::string sgObjectTableName = "ObjectTable";


AosCmdTorturer::AosCmdTorturer(const AosTorturerPtr &torturer)
{
	mCorrectMultiFromTable = NULL;
	mIncorrectMultiFromTable = NULL;
	sgLock.lock();
	mId = sgCmdId++;
	sgLock.unlock();
}


AosCmdTorturer::~AosCmdTorturer()
{
}


u32
AosCmdTorturer::calculateParmCrtPct(
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


bool
AosCmdTorturer::setParmCorrectPct()
{
	// 
	// Set the successful percentage
	//
	u32 crtPct = calculateParmCrtPct(mCorrectPct, mArgs.size());
	AosParmPtrArray::iterator itr;
	for (itr = mArgs.begin(); itr != mArgs.end(); itr++)
	{
		(*itr)->setCorrectPct(crtPct);
	}

	return true;
}


bool
AosCmdTorturer::addArg(const AosParmPtr &arg)
{
	mArgs.push_back(arg);
	return true;
}


bool 
AosCmdTorturer::serialize(TiXmlNode& node)
{
	return true;
}


bool 
AosCmdTorturer::deserialize(TiXmlNode* node)
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
	// 		<OverrideFlag>
	// 		<Parms>
	// 			<Parm>
	// 				<type>
	// 			</Parm>
	// 			...
	// 		</Parms>
	// 	</Cmd>
	//

	return true;
}


// 
// A Command contains zero or more arguments. Arguments
// are implemented as Parms. Generating an instance of
// a command is to generate values for each of its
// arguments. This function retrieves the last value
// of the named argument, the value will be converted to string. 
//
bool
AosCmdTorturer::getCrtValue(const std::string &name, 
						 std::string &value)
{
	AosParmPtr parm = getParm(name);
	if (!parm)
	{
		OmnAlarm << "RVG not found: " << name << enderr;
		return false;
	}

	AosValuePtr valuePtr;
	if (!parm->getCrtValue(valuePtr))
	{
		OmnAlarm << "Failed to retrieve parm value: " 
			<< name << enderr;
		return false;
	}
	if (valuePtr)
	{
		value = valuePtr->toString();
	}

	return true;
}


AosParmPtr 
AosCmdTorturer::getParm(const std::string &name)
{
	AosParmPtrArray::iterator itr;
	for (itr = mArgs.begin(); itr != mArgs.end(); itr++)
	{
		if ((*itr)->getName() == name)
		{
			return *itr;
		}
	}

	return 0;
}


bool
AosCmdTorturer::setCrtValue(const std::string &name, 
						   const std::string &value, 
						   std::string &errmsg)
{
	AosParmPtr parm = getParm(name);
	if (!parm)
	{
		errmsg += "To set Parameter value but not found: ";
		errmsg += name;
		errmsg += ":";
		errmsg += value;
	    errmsg += ". ";
		return false;
	}

	return parm->setCrtValue(value);
}


// 
// Description:
// This function will generate a new command. A command
// is in the form of:
// 		<command_prefix> <arg1> <arg2> ... <argn>
// Generating a command is to generate a value for each of
// the arguments, randomly. 
//
// cmdCorrect(OUT): 	
// 		Indicate whether the command generated is 
//      correct or incorrect.
// correctFlag:
// 		Correct
// 		Incorrect
// 		Random: randomly determine whether correct/incorrect.
// tm:  Pointer to TestManager
//
// Return: 
//    true:   if a command was generated.
//    false:  if 'mCorrectCtrl' == 
//
bool
AosCmdTorturer::run(bool &cmdCorrect, 
					const AosCorCtrl::E correctFlag,
					const OmnTestMgrPtr &tm) 
{
	static u32 cmdId = 0;

	mTestMgr = tm;
	std::string cmdErrmsg;
	cmdCorrect = true;
	std::string cmd = getCmdPrefix();
	AosCorCtrl::E crtCtrl = correctFlag;//calculateCorrectCtrl(correctFlag);

	if (!mCorrectPctSet)
	{
		setParmCorrectPct();
		mCorrectPctSet = true;
	}
	bool selectFromObj = false;

	// 
	// Determine whether need to use a record in the databasae table.
	//
	// choose a MultiFromTable object between mIncorrectMultiFromTable 
	// and mCorrectMultiFromTable to get a record
	//
	AosRVGMultiFromTablePtr	multiFromTable;
	if (!selectObject(crtCtrl, multiFromTable, selectFromObj, cmdCorrect)) 
	{
		OmnAlarm << "Failed to generate a command!" << enderr;
		return false;
	}

	bool isCorrect;
	AosValue arg;
	std::string errmsg;
	std::string decl;
	std::string deleteStmt;
	int index = 0;

	// 
	// Generate the values for each RVG.
	// Note that not all RVGs need to generate values. An RVG 
	// needs to generate value only when it is of 'input' type. 
	//
	int argsNum = mArgs.size();
	for (index=0; index<argsNum; index++)
	{
		isCorrect = true;

		// 
		// Check whether need to generate values for it.
		//
		if (mArgs[index]->isInputType())
		{
			//
			// get key parameter's value from the record, 
			// if the parameter can not get value from the record
			// it will generate value in parameter level
			//
			AosValuePtr ptr;
			if (selectFromObj && multiFromTable->getParmValue(mArgs[index]->getName(), ptr))
			{
				arg = *ptr;
				mArgs[index]->setCrtValue(ptr);
			}
			else
			{
				mArgs[index]->setCorrectnessFlag(crtCtrl);
				bool ret = mArgs[index]->nextValue(arg, isCorrect, errmsg);
				if (!ret)
				{
					OmnAlarm << "Failed to create parm: " 
						<< mArgs[index]->getName() << enderr;
					return false; 
				}

				if (!isCorrect)
				{
					// 
					// Generated command is an incorrect one. Collect
					// the incorrect message. 
					//
					cmdErrmsg += errmsg;
					cmdErrmsg += ". ";
					cmdCorrect = false;
				}
			}
		}

		// 
		// Construct the command. This is command type dependent.
		//
		if (!constructCmd(index, decl, deleteStmt, cmd))
		{
			return false;
		}
	}

	// 
	// All RVGs have been instantiated. Continue constructing
	// the command. 
	//
	finishUpCmd(cmdId, decl, deleteStmt, cmd);

	// 
	// PreExecution Processing
	// The function preExecutionProc(...) is a function that
	// can be overridden by users to do any customized processing
	// before executing the command.
	//
	if (doPreExecuteProc(cmdCorrect, errmsg) != 0)
	{
		return false;
	}

	// 
	// Perform PreSemantics checking
	//
	if (cmdCorrect)
	{
		std::string errmsg;
		switch (doPreSemantics(errmsg))
		{
		case eAosRc_RuleEvaluateTrue:
			 break;

		case eAosRc_RuleEvaluateFalse:
			 // 
			 // Semantics evaluated false. This means the command
			 // should be incorrect.
			 //
			 cmdCorrect = false;
			 cmdErrmsg += errmsg;
			 break;

		default:
			 // 
			 // Errors were encountered or the semantic rules failed
			 // the evaluation and they demand abort the execution.
			 //
			 return false;
		}
	}

	// 
	// PreExecution Actions
	//
	if (cmdCorrect)
	{
		std::string errmsg;
		if (doCorrectPreActions(errmsg) != 0) return false;
	}
	else
	{
		std::string errmsg;
		if (doIncorrectPreActions(errmsg) != 0) return false;
	}

	OmnTrace << "To run command: " << getName() << endl;
	bool cmdExecuted = false;

	// 
	// Run the command. If errors were encountered, it will 
	// return false. Otherwise, it returns true. Execution should
	// always return true. Whether the command is actually 
	// executed is indicated by "cmdExecuted". Note that it is
	// possible that the command is not executed at all. 
	// Also note that whether
	// the command is correct or incorrect is still controlled
	// by 'cmdCorrect'. 
	//
	bool executionRslt = runCommand(crtCtrl, mTorturer, 
			tm, cmdCorrect, cmdErrmsg, cmdExecuted);

	OmnTCTrue(executionRslt) << "Failed to run command: " 
		<< cmd << ". Execution Error Message: " << mExecutionErrmsg
		<< endtc;

	if (cmdExecuted) cmdExecutedStat(mId);
	if (!executionRslt) cmdFailedStat(mId);

	if (cmdExecuted) logCmd(cmd, cmdCorrect, cmdErrmsg, cmdId);

	// 
	// Post Execution Customized Processing
	//
	if (cmdExecuted)
	{
		if (cmdCorrect)
		{
			if (doCorrectPostProc(errmsg) != 0)
			{
				OmnAlarm << "Failed correct post processing: " 
					<< errmsg << enderr;
				return false;
			}
		}
		else
		{
			if (doIncorrectPostProc(errmsg) != 0)
			{
				OmnAlarm << "Failed correct post processing: " 
					<< errmsg << enderr;
				return false;
			}
		}
	}

	// 
	// Do post execution actions
	//
	if (cmdExecuted)
	{
		if (cmdCorrect)
		{
			if (doCorrectPostActions(errmsg) != 0)
			{
				OmnAlarm << "Failed correct post actions: " 
					<< errmsg << enderr;
				return false;
			}
		}
		else
		{
			if (doIncorrectPostActions(errmsg) != 0)
			{
				OmnAlarm << "Failed incorrect post actions: " 
					<< errmsg << enderr;
				return false;
			}
		}
	}

	u32 crtCmdId = cmdId++;

	// 
	// Ready to run post semantics now. But make sure that it
	// will not run the semantics unless the command was actually
	// executed.
	//
	bool actual = true;
	if (executionRslt && cmdExecuted)
	{
		std::string errmsg;
		switch (doPostSemantics(errmsg))
		{
		case eAosRc_RuleEvaluateTrue:
			 // 
			 // Rules are evaluated true. 
			 //
			 break;

		case eAosRc_RuleEvaluateFalse:
			 // 
			 // Rules were evaluated false. This changes the 
			 // command status to incorrect. 
			 //
			 actual = false;
			 cmdErrmsg += errmsg;
			 break;

		default:
			 // 
			 // Errors were encountered or the semantic rules failed
			 // the evaluation and they demand abort the execution.
			 //
			 return false;
		}
	}

	if (!executionRslt)
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
		mTorturer->printTables();
	}
	return true;
}


// 
// Description:
// The function determines whether to select an object from
// its object table and construct the command based on the
// object. The caller should not call this function unless
// the Object Table is not empty. 
//
// correctFlag:
//      Correct:   generate a correct command, if possible
//      Incorrect: generate an incorrect command, if possible
//      Ramdom:    whether correct or not is randomly determined.
// selectFromObj(Out): 
// 		If an object is selected, this parameter
// 		specifies the command's RVG how to select values from
// 		the object. 
// cmdCorrect(Out): 
// 		If this function knows the command's 
// 		correctness, this parameter is modified accordingly.
//
// Return:
// 		The function always return true unless there are 
// 		program errors.
//
bool
AosCmdTorturer::selectObject(const bool correctFlag, 
							 AosRVGMultiFromTablePtr &multiFromTable,
							 bool &selectFromObj, 
							 bool &cmdCorrect) 
{
	switch (correctFlag)
	{
	case AosCorCtrl::eCorrect:
		 // 
		 // We need to check whether mCorrectMultiFromTable is 
		 // null. If it is null, we do not have to select from the
		 // table.
		 //
		 cmdCorrect = true;
		 if (!selectFromTable(mCorrectMultiFromTable, selectFromObj))
		 {
			 cmdCorrect = false;
			 selectFromObj = false;
		 }
		 else
		 {
		 	 multiFromTable = mCorrectMultiFromTable;
		 }
		 break;

	case AosCorCtrl::eIncorrect:
		 // 
		 // Need to create an incorrect command. Whether an 
		 // incorrect command must be created based on a
		 // record from mIncorrectMultFromTable or not is
		 // depended on whether it is null. If not null, 
		 // we will randomly determine whether to select from
		 // it or generate an incorrect command syntactically.
		 //
		 // Note that if 'selectFromTable(...)' returns false, 
		 // it means it is not possible to select from table. 
		 // To incorrect case, this is ok. We can generate an 
		 // incorrect command through syntax errors.
		 //
		 cmdCorrect = false;
		 if (!selectFromTable(mIncorrectMultiFromTable, selectFromObj))
		 {
			 selectFromObj = false;
		 }
		 else
		 {
		 	 multiFromTable = mIncorrectMultiFromTable;
		 }
		 break;

	case AosCorCtrl::eRandom:
		 if (aos_next_pct(mCorrectPct))
		 {
			 // 
			 // Need to generate a correct command. 
			 //
			 // Note that if 'selectFromTable(...)' returns false, 
			 // it means it is not able to select from the table, 
			 // or in other word, it is not possible to create a
			 // correct command. When this happens, we should change
			 // to generate an incorrect command instead of returning
			 // false.
			 //
			 cmdCorrect = true;
			 if (!selectFromTable(mCorrectMultiFromTable, selectFromObj))
			 {
				 cmdCorrect = false;
				 if (!selectFromTable(mIncorrectMultiFromTable, selectFromObj))
				 {
					 return false;
				 }
				 multiFromTable = mIncorrectMultiFromTable;
				 return true;
			 }
			 multiFromTable = mCorrectMultiFromTable;
		 }
		 else
		 {
			 cmdCorrect = false;
			 if (!selectFromTable(mIncorrectMultiFromTable, selectFromObj))
			 {
				 selectFromObj = false;
			 }
			 else
			 {
			  	 multiFromTable = mIncorrectMultiFromTable;
			 }
		 }
		 break;

	default:
		 OmnAlarm << "Unrecognized correctFlag: " << correctFlag << enderr;
		 return false;
	}
	return true;
}


// 
// The purpose of this function is to determine whether
// to generate the command by selecting a record from 
// the table. If yes, 'selectFromObj = true'. Otherwise, 
// the command shall be created on its own. It sets 
// 'selectFromObj = false'. If it must select from table
// but the table is empty, 'selectFromObj = false'
// and it returns false.
//
// Case 1: 'table' is null:
// The command shall be created on its own. It sets
// 'selectFromObj = false' and returns true.
//
// Case 2: 'table' is not null, pct == 100, table is empty:
// This means that the command must be created based on a record
// from the table but the table is empty. Not possible. 
// 'selectFromObj = false', and returns false.
//
// Case 3: 'table' is not null, pct < 100, and table is not empty
// It randomly determines whether to select from the table. 
// If it determines to select from table, it sets 
// 'selectFromObj = true'. Otherwise, it sets 'selectFromObj = false'
// It returns true.
//
// Case 4: 'table' is not null, pct < 100, and table is empty
// it sets 'selectFromObj = false'
//
// Return Value:
//    The function returns false if and only if:
// 		a. 'table' is not null
// 		b. 'table->getSelectPct() == 100'
// 		c. 'table->isEmpty()'
//
bool
AosCmdTorturer::selectFromTable(const AosRVGMultiFromTablePtr &table, 
								bool &selectFromObj)
{
	selectFromObj = false;
	bool bRet = false;

	if (table)
	{
		u32 selectPct = table->getSelectPct();
		if (selectPct == AosRVGMultiFromTable::eSelectPctMax)
		{
			if (table->isEmpty())
			{
				// 
				// This means it is not possible to do what the command
				// wanted. 
				//
				selectFromObj = false;
				bRet = false;
			}
			else
			{
				// 
				// The table is not empty. Select a record.
				//
				aos_assert_r(table->nextValue(), false);
				selectFromObj = true;
				bRet = true;
			}
		}
		else if (aos_next_pct(selectPct))
		{
			if (table->isEmpty())
			{
				selectFromObj = false;
			}
			else
			{
				// 
				// The table is not empty. Select a record.
				//
				aos_assert_r(table->nextValue(), false);
				selectFromObj = true;
			}
			bRet = true;
		}
	}
	return bRet;
}


AosCorCtrl::E
AosCmdTorturer::calculateCorrectCtrl(const AosCorCtrl::E c)
{
	if (c == AosCorCtrl::eCorrect ||
		c == AosCorCtrl::eIncorrect) 
	{
		return c;
	}

	if (mCorrectPct > 100 || mCorrectPct < 0)
	{
		OmnAlarm << "Invalid percentage: " << mCorrectPct << enderr;
		return AosCorCtrl::eCorrect;
	}
	return (aos_next_pct(mCorrectPct) ? 
			AosCorCtrl::eCorrect: 
			AosCorCtrl::eIncorrect);
}


bool
AosCmdTorturer::isObjectTableEmpty() const
{
	// 
	// The function returns true if the Object Table is null
	// or it is empty.
	//
	return (!mSemanticData || mSemanticData->isTableEmpty(sgObjectTableName));
}


int
AosCmdTorturer::doPreSemantics(std::string &theErrmsg)
{
	return AosEvaluateSemanticRules(mPreSemanticRules, mSemanticData, theErrmsg);
}


int
AosCmdTorturer::doPostSemantics(std::string &theErrmsg)
{
	return AosEvaluateSemanticRules(mPostSemanticRules, mSemanticData, theErrmsg);
}


int
AosCmdTorturer::doCorrectPreActions(std::string &errmsg)
{
	return AosAction_doActions(mPreCorrectActions, 
				mSemanticData, mActionData, errmsg);
}


int
AosCmdTorturer::doIncorrectPreActions(std::string &errmsg)
{
	return AosAction_doActions(mPreIncorrectActions, 
				mSemanticData, mActionData, errmsg);
}


int
AosCmdTorturer::doCorrectPostActions(std::string &errmsg)
{
	return AosAction_doActions(mPostCorrectActions, 
				mSemanticData, mActionData, errmsg);
}


int
AosCmdTorturer::doIncorrectPostActions(std::string &errmsg)
{
	return AosAction_doActions(mPostIncorrectActions, 
				mSemanticData, mActionData, errmsg);
}


int
AosCmdTorturer::doPreExecuteProc(const bool cmdCorrect,
				std::string &errmsg)
{
	// 
	// This function is supposed to be overridden by users.
	//
	return 0;
}

