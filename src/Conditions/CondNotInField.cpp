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
// Monday, December 17, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondNotInField.h"

#include "alarm/Alarm.h"


AosCondNotInField::AosCondNotInField()
{
}


AosCondNotInField::AosCondNotInField(const std::string& tableName, const std::string& fieldName)
:mTableName(tableName),mFieldName(fieldName)
{
}


AosCondNotInField::~AosCondNotInField()
{
}


bool 
AosCondNotInField::serialize(TiXmlNode& node)
{
//
// <CondNotInField>
// 		<TableName></TableName>
// 		<FieldName></FieldName>
// </CondNotInField>
//
	return true;
}


bool 
AosCondNotInField::deserialize(TiXmlNode* node)
{
//
// <CondNotInField>
// 		<TableName></TableName>
// 		<FieldName></FieldName>
// </CondNotInField>
//
	return true;
}


bool 
AosCondNotInField::check(const AosExeDataPtr &ed)
{
	OmnAlarm << "Can not invoke this interface" << enderr;
	return false;
}


bool 
AosCondNotInField::check(const AosValue& value)
{
	return true;
}


bool 
AosCondNotInField::check(const AosValue& valueLeft, const AosValue& valueRight)
{
	OmnAlarm << "Can not invoke this function" << enderr;
	return false;
}


void 
AosCondNotInField::setTable(AosTablePtr table)
{
	mTable = table;
}


AosTablePtr 
AosCondNotInField::getTable()
{
	return mTable;
}


void 
AosCondNotInField::setTableName(const std::string& name)
{
	mTableName = name;
}


std::string 
AosCondNotInField::getTableName()
{
	return mTableName;
}


void 
AosCondNotInField::setFieldName(const std::string& name)
{
	mFieldName = name;
}


std::string 
AosCondNotInField::getFieldName()
{
	return mFieldName;
}

