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
#include "Conditions/CondDataTable.h"

#include "alarm/Alarm.h"
#include "Conditions/ExeData.h"
#include "RVG/Parm.h"
#include "RVG/Table.h"
#include "Torturer/Command.h"

AosCondDataTable::AosCondDataTable()
{
	mDataFlag = "Table";
	mCondDataType = AosCondDataType::eTable;
}


AosCondDataTable::~AosCondDataTable()
{
}


bool 
AosCondDataTable::getData(AosValuePtr& value, const AosExeDataPtr &ed)
{
	aos_assert_r(ed, false);
	AosTablePtr table = ed->getTable(mTableName);
	if (!table)
	{
		OmnAlarm << "Failed to retrieve table: " << mTableName << enderr;
		return false;
	}
	value->setTable(table);
	return true;
}


bool 
AosCondDataTable::serialize(TiXmlNode& node)
{
	// 
	// 	<CondData>
	// 		<Type>Table</Type>
	// 		<TableName>
	// 	</CondData>
	//
	aos_assert_r(mTableName != "", false);
	node.SetValue("CondData");
	node.addElement("Type", "Table");
	node.addElement("TableName", mTableName);
	return true;
}


bool 
AosCondDataTable::deserialize(TiXmlNode* node)
{
	aos_assert_r(node, false);
	aos_assert_r(node->ValueStr() == "CondData", false);
	std::string type;
	node->getElementTextValue("Type", type);
	aos_assert_r(type == "Table", false);
	node->getElementTextValue("TableName", mTableName);
	aos_assert_r(mTableName != "", false);
	return true;
}

