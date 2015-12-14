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
#include "Parms/TestCond.h"

#if 0

AosTestCond::AosTestCond(const OmnString &name)
{
}


AosTestCond::~AosTestCond()
{
}


bool 
AosTestCond::config(const OmnXmlItemPtr &def)
{
	//
	//	<Cond>
	//		<Code>$ret.isDigitString()</Code>
	//		<Errmsg>...</Errmsg>
	//	</Cond>
	//
	mCode = def->getStr("Code", "");
	if (mCode == "")
	{
		OmnAlarm << "Failed to retrieve the code: " << def->toString() << enderr;
		return false;
	}

	mErrmsg = def->getStr("Errmsg", "");

	return true;
}


AosTestCondPtr
AosTestCond::createTestCond(const OmnXmlItemPtr &def)
{
	AosTestCondPtr cond = OmnNew AosTestCond();
	if (!cond->config(def))
	{
		OmnAlarm << "Failed to create the condition: " << def->toString() << enderr;
		return 0;
	}

	return cond;
}


OmnString
AosTestCond::genCode(const AosTesterGenPtr &gen)
{
	OmnStrParser parser(mCode);
	
	// 
	// We need to replace all the variables with real ones. Each variable is 
	// identified by "$<word>". 
	//
	while (1)
	{
		OmnString name = parser.getWord('$');
		if (name == "")
		{
			// 
			// No more variables
			//
			break;
		}

		// 
		// Found a variable. Retrieve the variable from 'gen'.
		//
		OmnString varname = get->getVarName(name);
		if (varname == "")
		{
			OmnAlarm << "Failed to retrieve the variable name: " 
				<< name << " defined in: " << mCode << enderr;
			return "";
		}

		OmnString token = "$";
		token << name;
		parser.replace(token, varname);
	}

	// 
	// Now, we need to generate the following code:
	// 
	// "OmnTC(OmnExpected<bool>(true), OmnActual<bool>(cond)) << errmsg << endtc;
	//
	OmnString code = "OmnTC(OmnExpected<bool>(true), OmnActual<Bool>(";
	code << mCode << "))" << mErrmsg << " endtc";
	return code;
}

#endif

