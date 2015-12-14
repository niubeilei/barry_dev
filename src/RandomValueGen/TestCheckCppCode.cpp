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
// 02/19/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/TestCheckCppCode.h"

#if 0

AosTestCheckCppCode::AosTestCheckCppCode(const OmnString &name)
{
}


AosTestCheckCppCode::~AosTestCheckCppCode()
{
}


bool 
AosTestCheckCppCode::config(const OmnXmlItemPtr &def)
{
	// 
	//		<Check>
	//			<CheckType>CPPCode</CheckType>
	//			<Variables>
	//				<Variable>
	//					<Name>ret</Name>
	//					<Type>FuncReturnValue</Type>
	//					<DataType>OmnString</DataType>
	//				<Variable>
	//			</Variables>
	//			<Conditions>
	//				<Cond>$ret.isDigitString()</Cond>
	//				<Cond>$ret.length() >= $min</Cond>
	//				<Cond>$ret.length() <= $max</Cond>
	//			</Conditions>
	//		</Check>
	//
	OmnXmlItemPtr variables = def->getItem("Variables");
	if (variables)
	{
		if (!parseVariables(variables))
		{
			OmnAlarm << "Failed to parse variables: " 
				<< def->toString() << enderr;
			return false;
		}
	}

	OmnXmlItemPtr conditions = def->getItem("Conditions");
	if (!conditions)
	{
		OmnAlarm << "Missing conditions: " << def->toString() << enderr;
		return false;
	}

	if (!parseConditions(conditions))
	{
		OmnAlarm << "Failed to parse conditions: " << def->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosTestCheckCppCode::parseVariables(const OmnXmlItemPtr &def)
{
	//
	//	<Variables>
	//		<Variable>
	//			<Name>
	//			<Type>
	//			<DataType>
	//		<Variable>
	//		...
	//	</Variables>
	//

	def->reset();
	while (def->hasMore())
	{
		OmnXmlItemPtr item = def->next();
		AosTestVarPtr var = AosTestVar::createVar(item);
		if (!var)
		{
			OmnAlarm << "Failed to create the variable: " << item->toString() << enderr;
			return false;
		}

		mVariables.append(var);
	}

	return true;
}


bool
AosTestCheckCppCode::parseConditions(const OmnXmlItemPtr &def)
{
	//
	//	<Conditions>
	//		<Cond>$ret.isDigitString()</Cond>
	//		<Cond>$ret.length() >= $min</Cond>
	//		<Cond>$ret.length() <= $max</Cond>
	//	</Conditions>
	//
	def->reset();
	while (def->hasMore())
	{
		OmnXmlItemPtr item = def->next();
		AosTestCondPtr cond = AosTestCond::createCond(item);
		if (!cond)
		{
			OmnAlarm << "Failed to create the condition: " << item->toString() << enderr;
			return false;
		}

		mConditions.append(cond);
	}

	return true;
}

#endif

