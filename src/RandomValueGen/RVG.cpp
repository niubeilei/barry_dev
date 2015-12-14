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
// 02/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVG.h"

#include "alarm/Alarm.h"
#include "CliTorturer/CliTorturer.h"
#include "CliTorturer/CliCmdTorturer.h"
#include "LogicExpr/LogicTerm.h"
#include "Parms/RVGStr.h"
#include "Parms/RVGInt.h"
#include "Parms/RVGMac.h"
#include "Parms/RVGIP.h"
#include "Parms/RVGTimeStr.h"
#include "Parms/RVGNetmask.h"
#include "Parms/RVGStrEnum.h"
#include "Parms/RVGStrPair.h"
#include "Parms/RVGParm.h"
#include "Parms/RVGInstGen.h"
#include "TorturerAction/Action.h"
#include "Util/GenTable.h"
#include "Util/GenRecord.h"
#include "Util/Ptrs.h"
#include "Util/StrParser.h"
#include "XmlParser/XmlItem.h"



AosRVG::AosRVG(const AosCliCmdTorturerPtr &cmd)
:
mKeyType(eAosRVGKeyType_NoKey),
mCorrectPct(eDefaultCorrectPct),
mCorrectOnly(false),
mIsGood(false),
mIsSmartPtr(false),
mUsageType(eAosRVGUsageType_Unknown),
mOverrideFlag(false), 
mCommand(cmd),
mValueGenerated(false)
{
}


AosRVG::~AosRVG()
{
}


bool
AosRVG::config(const OmnXmlItemPtr &def, 
				const OmnString &cmdTag, 
				OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<Parm>
	// 		<Name>
	// 		<UsageType>
	// 		<FieldName>
	// 		<DataType>
	// 		<KeyType>
	// 		<InstanceGenName>
	// 		<CorrectOnly>
	// 		<TableName>
	// 		<DependedTables>
	// 		<SourceTableName>
	// 		<SourceFieldName>
	// 		<SmartPtr>
	// 		<OverrideFlag>
	// 		<VariableDecl>
	// 		<InitMemberData>
	// 		<Condition>
	//	</Parm>
	//
	mUsageType = AosGetRVGUsageType(def->getStr("UsageType", "InputOnly"));
	if (mUsageType == eAosRVGUsageType_Unknown)
	{
		OmnAlarm << "Unrecognized UsageType tag: "
			<< def->toString() << enderr;
		return false;
	}

	mArgName        = def->getStr("Name", "NoName");
	mFieldName      = def->getStr("FieldName", "");
	mDataType       = def->getStr("DataType", "");
	mKeyType        = AosGetRVGKeyType(def->getStr("KeyType", "NoKey"));
	mCorrectOnly    = def->getBool("CorrectOnly", false);
	mTable          = getTable(tables, def->getStr("TableName", ""));
	mIsSmartPtr 	= def->getBool("SmartPtr", false);
	mInstGenName	= def->getStr("InstanceGenName", "");
	mVarDecl		= def->getStr("VariableDecl", "");
	mInitMemData	= def->getStr("InitMemberData", "");
	mIsVerbose		= def->getBool("Verbose", true);
	mOverrideFlag	= def->getBool("OverrideFlag", false);
	mCondition		= AosLogicTerm::createTerm(def->getItem("Condition"));
	mVarDecl.removeWhiteSpaces();
	mInitMemData.removeWhiteSpaces();

	if (cmdTag == "FuncTest")
	{
		if (mDataType == "")
		{
			OmnAlarm << "Missing DataType tag: " 
				<< def->toString() << enderr;
			return false;
		}
	}

    if (mKeyType == eAosRVGKeyType_Unknown)
	{
		OmnAlarm << "Invalid key type: " << def->toString() << enderr;
		return false;
	}

	if (!parseDependedTables(def->tryItem("DependedTables"),
		    mDependedTables, mDependedFieldNames, tables))
	{
		return false;
	}

	OmnString stn = def->getStr("SourceTableName", "");
	if (stn != "")
	{
		mSourceTable = getTable(tables, stn);
		if (!mSourceTable)
		{
			OmnAlarm << "Source table not found: " << stn
				<< ". " << def->toString() << enderr;
			return false;
		}
	
		mSourceFieldName = def->getStr("SourceFieldName", "");
		if (mSourceFieldName == "")
		{
			OmnAlarm << "Missing SourceFieldName: " << def->toString() << enderr;
			return false;
		}
	}

	return true;
}


AosRVGPtr
AosRVG::createRVG(const AosCliCmdTorturerPtr &cmd, 
					const OmnXmlItemPtr &conf, 
					const OmnString &cmdTag,
					OmnVList<AosGenTablePtr> &tables, 
					const bool codegen)
{
	// 
	// 	<Parm>
	// 		<type>
	// 		...
	// 	</Parm>
	//
	OmnString type = conf->getStr("type", "");
	AosRVGPtr parm;
	type.toLower();
	if (type == "string")
	{
		parm = AosRVGStr::createInstance(cmd, conf, cmdTag, tables);
	}
	else if (type == "int" || type == "port")
	{
		parm = AosRVGInt::createInstance(cmd, conf, cmdTag, tables);
	}
	else if (type == "addr")
	{
		parm = AosRVGIP::createInstance(cmd, conf, cmdTag, tables);
	}
	else if (type == "netmask")
	{
		parm = AosRVGNetmask::createInstance(cmd, conf, cmdTag, tables); 
	}
	else if (type == "stringlist")
	{
		parm = AosRVGStrEnum::createInstance(cmd, conf, cmdTag, tables);
	}
	else if (type == "mac")
	{
		parm = AosRVGMac::createInstance(cmd, conf, cmdTag, tables);
	}
	else if (type == "timestr")
	{
		parm = AosRVGTimeStr::createInstance(cmd, conf, cmdTag, tables);
	}
	else if (type == "stringpair")
	{
		parm = AosRVGStrPair::createInstance(cmd, conf, cmdTag, tables);
	}
	else if (type == "parms")
	{
		parm = AosRVGParm::createInstance(cmd, conf, cmdTag, tables);
	}
	else if (type == "instancegen")
	{
		if (codegen && conf->getTag() == "InstanceGen")
		{
			parm = AosRVGInstGen::createInstanceGen(
						cmd, conf, cmdTag, tables, true);
		}
		else
		{
			OmnString instName = conf->getStr("InstanceGenName", "");
			if (instName == "")
			{
				OmnAlarm << "Missing InstanceGenName: " 
					<< conf->toString() << enderr;
				return 0;
			}

			AosRVGInstGenPtr pp = AosRVGInstGen::getInstance(instName);
			if (!pp)
			{
				OmnAlarm << "The Instance Generator: " 
					<< instName
					<< " is not found! The config is: " 
					<< conf->toString() << enderr;
				return 0;
			}

			AosRVGInstGenPtr ptr = pp->clone();
			if (!ptr->config(cmd, conf, cmdTag, tables, false))
			{
				OmnAlarm << "Faild to config Parm: " 
					<< conf->toString() << enderr;
				return 0;
			}

			ptr->setInstGen(pp);
			parm = ptr;
		}

		if (!parm)
		{
			OmnAlarm << "Failed to create Instance Generator: " 
				<< conf->toString() << enderr;
			return 0;
		}
	}
	else
	{
		OmnAlarm << "Unrecognized parm type: " << type << " in \n" << conf->toString() << enderr;
		return 0;
	}

	if (!parm)
	{
		OmnAlarm << "Failed to create the parm: " 
			<< conf->toString() << enderr;
		return 0;
	}

	if (!parm->isGood())
	{
		OmnAlarm << "Failed to construct the parm: " 
			<< conf->toString() << enderr;
		return 0;
	}

	return parm;
}


AosGenTablePtr	
AosRVG::getTable(OmnVList<AosGenTablePtr> &tables, const OmnString &name)
{
	tables.reset();
	while (tables.hasMore())
	{
		AosGenTablePtr tt = tables.next();
		if (tt->getName() == name) return tt;
	}

	return 0;
}


bool
AosRVG::isUsedByOtherTables(const int value, 
						  bool &isGood,
						  AosRVGReturnCode &rcode, 
						  OmnString &errmsg)
{
	OmnString str; 
	str << value;
	return isUsedByOtherTables(str, isGood, rcode, errmsg);
}


bool
AosRVG::isUsedByOtherTables(const OmnString &value, 
						  bool &isGood,
						  AosRVGReturnCode &rcode, 
						  OmnString &errmsg)
{
	// 
	// The value 'value' holds a valid value. But if 
	// the value is used by one of the tables in mDependedTables, 
	// it should be treated as incorrect one.
	//
	for (int i=0; i<mDependedTables.entries(); i++)
	{
		if (mDependedTables[i]->getRecord(mDependedFieldNames[i], value))
		{
			// 
			// This value is used by the ith table. 
			//
			rcode = eAosRVGRc_UsedByOtherTables; 
			isGood = false;
			errmsg << "Value used by table: " 
				<< mDependedTables[i]->getName() << ". ";
			return true;
		}
	}

	// 
	// The value is not used by other tables.
	//
	return false;
}


bool
AosRVG::parseDependedTables(const OmnXmlItemPtr &def, 
							 OmnDynArray<AosGenTablePtr> &depTables,
							 OmnDynArray<OmnString> &fieldNames,
							 OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<DependedTables>
	// 		<Entry>tablename, fieldname</Entry>
	// 		<Entry>tablename, fieldname</Entry>
	// 		...
	// 		<Entry>tablename, fieldname</Entry>
	// 	</DependedTables>
	//
	if (!def) return true;

	def->reset();
	while (def->hasMore())
	{
		OmnXmlItemPtr item = def->next();
		OmnString contents = item->getStr();
		OmnStrParser parser(contents);

		OmnString tablename = parser.nextWord("", ",");
		if (tablename == "")
		{
			OmnAlarm << "Failed to retrieve the table name: " 
				<< def->toString() << enderr;
			return false;
		}

		parser.nextChar();
		OmnString fieldname = parser.nextWord("", ",");
		if (!parser.finished())
		{
			OmnAlarm << "Incorrect entry: " << contents 
				<< " in: " << def->toString() << enderr;
			return false;
		}

		if (fieldname == "")
		{
			OmnAlarm << "Failed to retrieve field name: " 
				<< def->toString() << enderr;
			return false;
		}

		// 
		// Got the table name and the field name. Check whether the
		// table name identifies a table. If not, it is an error.
		// Otherwise, the table and field name are added to 
		// 'depTables' and 'fieldNames'.
		//
		tables.reset();
		bool found = false;
		// depTables.clear();
		// fieldNames.clear();
		while (tables.hasMore())
		{
			AosGenTablePtr table = tables.next();
			if (table->getName() == tablename)
			{
				found = true;
				depTables.append(table);
				fieldNames.append(fieldname);
				break;
			}
		}

		if (!found)
		{
			OmnAlarm << "Table not found: " << tablename 
				<< ". " << def->toString() << enderr;
			return false;
		}
	}

	return true;
}


bool
AosRVG::getCrtValue(short &) const
{
	OmnAlarm << "Cannot get short : " << getName() << enderr;
	return false;
}


bool
AosRVG::getCrtValue(OmnIpAddr &) const
{
	OmnAlarm << "Cannot get OmnIpAddr: " << getName() << enderr;
	return false;
}


bool
AosRVG::getCrtValue(char *&)const
{
	OmnAlarm << "Cannot get char *: " << getName() << enderr;
	return false;
}


bool
AosRVG::getCrtValue(OmnMacAddr &) const
{
	OmnAlarm << "Cannot get OmnIpAddr: " << getName() << enderr;
	return false;
}


bool 
AosRVG::nextStr(OmnString &value,
				const AosGenTablePtr &table, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode,
				OmnString &errmsg)
{
	OmnAlarm << "Function should not be called here: " 
		<< mArgName << ":" << getRVGType() << enderr;
	return false;
}


/*
 * Chen Ding, 10/16/2007
 *
bool 
AosRVG::nextInt(
				int &value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg)
{
	OmnAlarm << "Cannot call AosRVG::nextInt()" << enderr;
	return false;
}


bool 
AosRVG::nextInt(
				u32 &value,
				const AosGenTablePtr &data, 
				const AosGenRecordPtr &record, 
				bool &isCorrect, 
				const bool correctOnly,
				const bool selectFromRecord,
				AosRVGReturnCode &rcode, 
				OmnString &errmsg)
{
	OmnAlarm << "Cannot call AosRVG::nextInt()" << enderr;
	return false;
}
*/


bool AosRVG::getCrtValue(int &value) const 
{
	OmnAlarm << "Can't getCrtValue" << enderr;
	return false;
}


bool 
AosRVG::getCrtValue(uint8_t &value) const
{
	OmnAlarm << "Can't getCrtValue" << enderr;
	return false;
}


bool 
AosRVG::getCrtValue(u16 &value) const
{
	OmnAlarm << "Can't getCrtValue" << enderr;
	return false;
}


bool 
AosRVG::getCrtValue(u32 &value) const 
{
	OmnAlarm << "Can't getCrtValue" << enderr;
	return false;
}


bool 
AosRVG::getCrtValue(OmnString &value) const
{
	OmnAlarm << "Can't getCrtValue" << enderr;
	return false;
}


bool 
AosRVG::getCrtValue(AosRVGInstGenPtr &value) const
{
	OmnAlarm << "Can't getCrtValue" << enderr;
	return false;
}


bool 
AosRVG::getCrtValueAsArg(OmnString &value, 
						  OmnString &decl, 
						  const u32 argIndex) const
{
	OmnAlarm << "Can't getCrtValueAsArg" << enderr;
	return false;
}


bool 
AosRVG::newRecordAdded(const AosGenRecordPtr &newRecord) const
{
	OmnAlarm << "Can't newRecordAdded" << enderr;
	return false;
}


bool 
AosRVG::recordDeleted(const AosGenRecordPtr &record) const
{
	OmnAlarm << "Can't do recordDeleted" << enderr;
	return false;
}


bool 
AosRVG::setRecord(const AosGenRecordPtr &record) const
{
	OmnAlarm << "Can't setRecord" << enderr;
	return false;
}


OmnString 
AosRVG::toString(const u32 indent) const
{
	return "";
}


// 
// In constructing log.txt API call entries, an API may return 
// memory that needs to be freed by the caller. This function 
// will get the free memory statement. If a Parm does not need
// to free memory, nothing to do. This default implementation
// handles that case. Otherwise, one should override this function.
//
bool 
AosRVG::getDeleteStmt(OmnString &stmt, const u32 argIndex) const
{
	return true;
}


/* 
 * Chen Ding, 10/16/2007
 *
bool 
AosRVG::nextExist(OmnString &value, 
					const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
				    bool &isGood, 
					const bool correctOnly,
			 	    const bool selectFromRecord,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg)
{
	OmnAlarm << "Function should not be called here" << enderr;
	return false;
}

	
bool 
AosRVG::nextNew(OmnString &value, 
	                const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
			 	    const bool selectFromRecord,
				    bool &isGood, 
					const bool correctOnly,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg)
{
	OmnAlarm << "Function should not be called here" << enderr;
	return false;
}


bool 
AosRVG::nextNotKey(OmnString &value, 
	                const AosGenTablePtr &data, 
				    const AosGenRecordPtr &record,
				    bool &isGood, 
					const bool correctOnly,
					const bool selectFromRecord,
					AosRVGReturnCode &rcode,
				    OmnString &errmsg)
{
	OmnAlarm << "Function should not be called here" << enderr;
	return false;
}
*/


bool
AosRVG::selectValueFromSourceTable(
					OmnString &value, 
				    bool &isGood, 
					AosRVGReturnCode &rcode,
				    OmnString &errmsg)
{
	aos_assert_r(mSourceTable, false);
	aos_assert_r(mSourceFieldName != "", false);

	// 
	// This means a good value must be chosen from the table. 
	// If the table has nothing, we cannot create a correct value.
	//
	if (mSourceTable->entries() <= 0)
	{
		isGood = false;
		(errmsg = mArgName) << " not from " << mSourceTable->getName()
			<< " table. ";
		return false;
	}

	u32 index;
	value = mSourceTable->getRandomStr(mSourceFieldName, "", index);
	if (value == "")
	{
		errmsg = "Failed to retrieve a value from table: ";
		errmsg << mSourceTable->getName()
			<< "\n" << mSourceTable->toString();
		isGood = false;
		OmnAlarm << errmsg << enderr;
		rcode = eAosRVGRc_ProgramError;
		return false;
	}

	return true;
}


bool
AosRVG::isStrType(const OmnString &type) const
{
	//
	// This function returns true if 'type' is a string type.
	// Currently, the following is considered a string type:
	// 1. OmnString
	// 2. char *
	//
	return (type == "OmnString" || 
			type == "char *");
}


bool
AosRVG::checkCond(bool &rslt, OmnString &errmsg)
{
	if (!mCondition) return true;

	AosLogicObjPtr thisPtr(this, false);
	return mCondition->evaluate(rslt, errmsg, thisPtr);
}


bool 	
AosRVG::getValue(const AosOperandType type,
			      const OmnString &name,
			  	  OmnString &value,
			   	  OmnString &errmsg)
{
	// 
	// This is the virtual function for "AosLogicObj" class. 
	// Depending on 'type', it may retrieve values differently.
	// Supported types are:
	//
	switch (type)
	{
	case eAosOperandType_Parm:
		 if (!mCommand)
		 {
			 errmsg << "Program Error: Command is null: " << mArgName;
			 OmnAlarm << errmsg;
			 return false;
		 }
		 
		 if (!mCommand->getValue(name, value))
		 {
			 errmsg << "Failed to retrieve parm: " << name;
			 return false;
		 }

		 return true;

	case eAosOperandType_Var:
		 if (!AosCliTorturer::getSelf()->getVar(name, value))
		 {
			 errmsg << "Failed to get variable: " << name;
			 return false;
		 }

		 return true;

	case eAosOperandType_CrtValue:
		 value = mLastValue;
		 return true;

	case eAosOperandType_Constant:
		 value = name;
		 return true;

	default:
		 errmsg << "Unrecognized Operand Type: "
			 << type << " for Parameter: " << mArgName
			 << ", Name = " << name;
		 OmnAlarm << errmsg << enderr;
		 return false;
	}

	return true;
}


bool
AosRVG::doAction(bool &rslt, OmnString &errmsg)
{
	rslt = true;
	if (mActions.entries() == 0) return true;

	bool rr;
	AosActionObjPtr thisPtr(this, false);
	for (int i=0; i<mActions.entries(); i++)
	{
		if (!mActions[i]->doAction(rr, thisPtr, errmsg))
		{
			OmnAlarm << "Failed to run action: " 
				<< mActions[i]->toString() << enderr;
			return false;
		}

		rslt = (rslt && rr);
	}

	return true;
}


bool    
AosRVG::setParm(const OmnString &name, 
				 const OmnString &value, 
				 OmnString &errmsg)
{
	if (!mCommand)
	{
		errmsg << "To set parm but mCommand is null"
			<< "Name: " << name
			<< ". Value: " << value;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return mCommand->setRVG(name, value, errmsg);
}


bool    
AosRVG::setVar(const OmnString &name, 
				const OmnString &value, 
				OmnString &errmsg)
{
	if (!mCommand)
	{
		errmsg << "To set variable but mCommand is null"
			<< "Name: " << name
			<< ". Value: " << value;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return mCommand->setVar(name, value);
}


bool    
AosRVG::setCrtValue(const OmnString &name, 
				const OmnString &value, 
				OmnString &errmsg)
{
	if (!mCommand)
	{
		errmsg << "To set CrtValue but mCommand is null"
			<< "Name: " << name
			<< ". Value: " << value;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return mCommand->setCrtValue(name, value, errmsg);
}


bool
AosRVG::setValue(const OmnString &value)
{
	mLastValue = value;
	return true;
}


bool
AosRVG::setCrtValue(const OmnString &value)
{
	mLastValue = value;
	mValueGenerated = true;
	return true;
}


