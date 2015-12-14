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
// This condition data obtains its value from a command's parameter. 
// The parameter is identified by parameter name. At runtime, when 
// it needs to retrieve the data, it retrieves the parameter from 
// ExeData and then value from it.
//   
//
// Modification History:
// Wednesday, January 02, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondDataParm.h"

#include "alarm/Alarm.h"
#include "Conditions/ExeData.h"
#include "RVG/Parm.h"
#include "Torturer/Command.h"

AosCondDataParm::AosCondDataParm()
{
	mDataFlag = "Parameter";
	mCondDataType = AosCondDataType::eParmValue;
}


AosCondDataParm::~AosCondDataParm()
{
}


bool 
AosCondDataParm::getData(AosValuePtr& value, const AosExeDataPtr &ed)
{
	aos_assert_r(ed, false);
	AosParmPtr parm = ed->getParm(mParmName);
	if (!parm)
	{
		OmnAlarm << "Parameter: " << mParmName
			<< " not found in command: " << ed->getCommandName() << enderr;
		return false;
	}

	aos_assert_r(parm->getCrtValue(value), false);
	return true;
}


bool 
AosCondDataParm::serialize(TiXmlNode& node)
{
	// 
	// 	<CondData>
	// 		<Type>Parm</Type>
	// 		<ParmName>
	// 	</CondData>
	//
	aos_assert_r(mParmName != "", false);
	node.SetValue("CondData");
	node.addElement("Type", "Parm");
	node.addElement("ParmName", mParmName);
	return true;
}


bool 
AosCondDataParm::deserialize(TiXmlNode* node)
{
	aos_assert_r(node, false);
	aos_assert_r(node->ValueStr() == "CondData", false);
	std::string type;
	node->getElementTextValue("Type", type);
	aos_assert_r(type == "Parm", false);
	node->getElementTextValue("ParmName", mParmName);
	aos_assert_r(mParmName != "", false);
	return true;
}


std::string 
AosCondDataParm::getParmName()
{
	return mParmName;
}


void 
AosCondDataParm::setParmName(const std::string& name)
{
	mParmName = name;
}

