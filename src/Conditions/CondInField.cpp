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
#include "Conditions/CondInField.h"

#include "alarm/Alarm.h"

AosCondInField::AosCondInField()
{
}


AosCondInField::AosCondInField(const std::string& tableName, 
							   const std::string& fieldName)
:mTableName(tableName),mFieldName(fieldName)
{
}


AosCondInField::~AosCondInField()
{
}


bool 
AosCondInField::serialize(TiXmlNode& node)
{
//
// <CondInField>
// 		<TableName></TableName>
// 		<FieldName></FieldName>
// </CondInField>
//
	return true;
}


bool 
AosCondInField::deserialize(TiXmlNode* node)
{
//
// <CondInField>
// 		<TableName></TableName>
// 		<FieldName></FieldName>
// </CondInField>
//
	return true;
}


bool 
AosCondInField::check(const AosExeDataPtr &ed)
{
	OmnAlarm << "Can not invoke this interface" << enderr;
	return false;
}


bool 
AosCondInField::check(const AosValue& value)
{
	return true;
}


bool 
AosCondInField::check(const AosValue& valueLeft, const AosValue& valueRight)
{
	OmnAlarm << "Can not invoke this function" << enderr;
	return false;
}


void 
AosCondInField::setTable(AosTablePtr table)
{
	mTable = table;
}


AosTablePtr 
AosCondInField::getTable()
{
	return mTable;
}


void 
AosCondInField::setTableName(const std::string& name)
{
	mTableName = name;
}


std::string 
AosCondInField::getTableName()
{
	return mTableName;
}


void 
AosCondInField::setFieldName(const std::string& name)
{
	mFieldName = name;
}


std::string 
AosCondInField::getFieldName()
{
	return mFieldName;
}

