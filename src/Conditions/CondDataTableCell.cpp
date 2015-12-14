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
// Wednesday, January 02, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondDataTableCell.h"

#include "alarm/Alarm.h"
#include "Conditions/ExeData.h"
#include "RVG/Table.h"
#include "Torturer/Command.h"
#include "Util/string.h"


AosCondDataTableCell::AosCondDataTableCell()
{
	mDataFlag = "TableCell";
	mCondDataType = AosCondDataType::eTableCell;
}


AosCondDataTableCell::~AosCondDataTableCell()
{
}


bool
AosCondDataTableCell::getData(AosValuePtr& value, const AosExeDataPtr &ed)
{
	aos_assert_r(ed, false);
	if (!mTable)	
	{
		mTable = ed->getTable(mTableName);
		aos_assert_r(mTable, false);
	}

	int columnIndex;
	if (!mTable->getColumnIndex(columnIndex, mFieldName))
	{
		return false;
	}

	if (!mTable->getData(mRowIndex, columnIndex, value))
	{
		return false;
	}

	return true;
}


std::string 
AosCondDataTableCell::getTableName()
{
	return mTableName;
}


void 
AosCondDataTableCell::setTableName(const std::string& name)
{
	mTableName = name;
}


std::string 
AosCondDataTableCell::getFieldName()
{
	return mFieldName;
}


void 
AosCondDataTableCell::setFieldName(const std::string& name)
{
	mFieldName = name;
}


int 
AosCondDataTableCell::getRowIndex()
{
	return mRowIndex;
}


void 
AosCondDataTableCell::setRowIndex(int index)
{
	mRowIndex = index;
}


bool 
AosCondDataTableCell::serialize(TiXmlNode& node)
{
	// 
	// 	<CondData>
	// 		<Type>TableCell</Type>
	// 		<RowIndex>
	// 		<FieldName>
	// 	</CondData>
	//
	aos_assert_r(mRowIndex >= 0, false);
	aos_assert_r(mFieldName != "", false);
	node.SetValue("CondData");
	node.addElement("Type", "TableCell");
	node.addElement("RowIndex", aos_string_itos(mRowIndex));
	node.addElement("FieldName", mFieldName);
	return true;
}


bool 
AosCondDataTableCell::deserialize(TiXmlNode* node)
{
	aos_assert_r(node, false);
	std::string type, rowIndex;
	node->getElementTextValue("Type", type);
	node->getElementTextValue("RowIndex", rowIndex);
	node->getElementTextValue("FieldName", mFieldName);
	aos_assert_r(type == "TableCell", false);
	mRowIndex = atoi(rowIndex.c_str());
	aos_assert_r(mRowIndex >= 0, false);
	return true;
}

