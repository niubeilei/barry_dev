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
// This condition data obtains its value from a variable.
//   
//
// Modification History:
// 01/28/2008: Created by Allen Xu 
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondDataVar.h"

#include "alarm/Alarm.h"
#include "Conditions/ExeData.h"
#include "RVG/Parm.h"
#include "SemanticData/Var.h"
#include "Torturer/Command.h"

AosCondDataVar::AosCondDataVar()
{
	mDataFlag = "Var";
	mCondDataType = AosCondDataType::eVariable;
}


AosCondDataVar::~AosCondDataVar()
{
}


bool 
AosCondDataVar::getData(AosValuePtr& value, const AosExeDataPtr &ed)
{
	aos_assert_r(ed, false);
	value = ed->getValue(mVarName);
	if (!value)
	{
		OmnAlarm << "Failed to retrieve variable: " << mVarName
			<< " in command: " << ed->getCommandName() << enderr;
		return false;
	}

	return true;
}


bool 
AosCondDataVar::serialize(TiXmlNode& node)
{
	// 
	// 	<CondData>
	// 		<Type>Variable</Type>
	// 		<VarName>
	// 	</CondData>
	//
	aos_assert_r(mVarName != "", false);
	node.SetValue("CondData");
	node.addElement("Type", "Variable");
	node.addElement("VarName", mVarName);
	return true;
}


bool 
AosCondDataVar::deserialize(TiXmlNode* node)
{
	aos_assert_r(node, false);
	aos_assert_r(node->ValueStr() == "CondData", false);
	std::string type;
	node->getElementTextValue("Type", type);
	aos_assert_r(type == "Variable", false);
	node->getElementTextValue("VarName", mVarName);
	aos_assert_r(mVarName != "", false);
	return true;
}

