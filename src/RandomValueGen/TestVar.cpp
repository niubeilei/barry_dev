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
#include "Parms/TestVar.h"

#if 0

AosTestVar::AosTestVar(const OmnString &name)
{
}


AosTestVar::~AosTestVar()
{
}


bool 
AosTestVar::config(const OmnXmlItemPtr &def)
{
	//
	//		<Variable>
	//			<Name>
	//			<Type>
	//			<DataType>
	//		<Variable>
	//

	mName = def->getStr("Name", "");
	if (mName == "")
	{
		OmnAlarm << "Missing variable name: " << def->toString() << enderr;
		return false;
	}

	OmnString type = def->getStr("Type", "");
	if (type == "FuncReturnValue")
	{
		mType = eFuncReturnValue;
	}
	else 
	{
		OmnAlarm << "Unrecognized variable type: " << type 
			<< ". Def: " << def->toString() << enderr;
		return false;
	}

	mDataType = def->getStr("DataType", "");
	if (datatype == "")
	{
		OmnAlarm << "Missing data type: " << def->toString() << enderr;
		return false;
	}

	return true;
}


AosTestVarPtr
AosTestVar::createTestVar(const OmnXmlItemPtr &def)
{
	//
	//		<Variable>
	//			<Name>
	//			<Type>
	//			<DataType>
	//		<Variable>
	//
	AosTestVarPtr var = OmnNew AosTestVar();
	if (!var->config(def))
	{
		OmnAlarm << "Failed to create the variable: " << def->toString() << enderr;
		return 0;
	}

	return var;
}

#endif

