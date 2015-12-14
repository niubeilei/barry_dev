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
// Modification History:
// 02/19/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerGen/TestGen.h"

#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "RVG/RVG.h"
#include "RVG/Ptrs.h"
#include "RVG/TestCheck.h"
#include "TorturerGen/MemberFuncTestGen.h"
#include "TorturerGen/ClassTestGen.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"
#include "XmlParser/XmlItem.h"


AosTestGen::AosTestGen()
{
}


AosTestGen::AosTestGen(const OmnString &name)
:
mFuncName(name)
{
}


AosTestGen::~AosTestGen()
{
}



AosTestGenPtr
AosTestGen::createTest(const OmnXmlItemPtr &def)
{
	// 	<Test>
	// 		<Type>
	// 		<Name> // Serve as member function name
	// 		<Code> 
	// 	</Test>

	OmnString type = def->getStr("Type", "FuncTest");

	if (type == "MemberFuncTest")
	{
		AosMemberFuncTestGenPtr mf = OmnNew AosMemberFuncTestGen();
		if (mf->config(def))
		{
			return mf.getPtr();
		}

		return 0;
	}

	if (type == "ClassTest")
	{
		AosClassTestGenPtr ct = OmnNew AosClassTestGen();
		if (ct->config(def))
		{
			return ct;
		}

		return 0;
	}

	OmnAlarm << "Unrecognized tester type: " << type
		<< ". The def: " << def->toString() << enderr;
	return 0;
}


bool
AosTestGen::parseParms(const OmnXmlItemPtr &def)
{
	OmnXmlItemPtr parms = def->tryItem("Parms");
	if (!parms)
	{
		// 
		// There is no parm section. Do nothing.
		//
		return true;
	}

	return true;
}


bool
AosTestGen::parseChecks(const OmnXmlItemPtr &def)
{
	// 
	//	<Checks>
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
	//				<Cond>
	//					<Code>$ret.isDigitString()</Code>
	//					<Errmsg>...</Errmsg>
	//				</Cond>
	//				<Cond>$ret.length() >= $min</Cond>
	//				<Cond>$ret.length() <= $max</Cond>
	//			</Conditions>
	//		</Check>
	//	</Checks>
	//
//	def->reset();
//	while (def->hasMore())
//	{
//		OmnXmlItemPtr item = def->next();
//		AosTestCheckPtr check = AosTestCheck::createTestCheck(item);
//		if (!check)
//		{
//			OmnAlarm << "Failed to create test check: " 
//				<< def->toString() << enderr;
//			return false;
//		}
//
//		mChecks.append(check);
//	}

	return true;
}


OmnString	
AosTestGen::getFuncDeclaration() const
{
	OmnString func = "bool ";
	func << mFuncName << "(const u32 repeat, const AosGenTablePtr &table);";
	return func;
}


OmnString	
AosTestGen::getFuncName() const
{
	return mFuncName;
}


