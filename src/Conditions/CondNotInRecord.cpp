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
// December 28, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondNotInRecord.h"

#include "alarm/Alarm.h"
#include "Conditions/ExeData.h"
#include "Torturer/Command.h"



AosCondNotInRecord::AosCondNotInRecord()
{
}


AosCondNotInRecord::AosCondNotInRecord(const std::string& tableName)
:mTableName(tableName)
{
}


AosCondNotInRecord::~AosCondNotInRecord()
{
}


bool 
AosCondNotInRecord::serialize(TiXmlNode& node)
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
AosCondNotInRecord::deserialize(TiXmlNode* node)
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
AosCondNotInRecord::check(const AosExeDataPtr &ed)
{
	aos_assert_r(ed, false);
	if (!mTable)
	{
		mTable = ed->getTable(mTableName);
		aos_assert_r(mTable, false);
	}

	int rowNum = mTable->getRowNum();
	for (int row=0; row<rowNum; row++)
	{
		bool bExist = true;
		std::map<std::string, std::string>::iterator iter;
		for (iter=mParmFieldMap.begin(); iter!=mParmFieldMap.end(); iter++)
		{
			//
			// get the value of the parameter
			//
			AosValuePtr valuePtr;
			if (!getParmValue(iter->first, valuePtr))
			{
				return true;
			}
			//
			// get the value in the field 
			//
			int columnIndex;
			if (!mTable->getColumnIndex(columnIndex, iter->second))
			{
				return true;
			}
			AosValuePtr tableValuePtr;
			if (!mTable->getData(row, columnIndex, tableValuePtr))
			{
				return true;
			}
			//
			// check the two values whether they are equal or not
			//
			if (!(*valuePtr == *tableValuePtr))
			{
				bExist = false;
			}
		}
		if (bExist)
		{
			return false;
		}
	}
	return true;
}


void 
AosCondNotInRecord::setTable(AosTablePtr table)
{
	mTable = table;
}


AosTablePtr 
AosCondNotInRecord::getTable()
{
	return mTable;
}


void 
AosCondNotInRecord::setTableName(const std::string& name)
{
	mTableName = name;
}


std::string 
AosCondNotInRecord::getTableName()
{
	return mTableName;
}


bool 
AosCondNotInRecord::addParmFieldMap(const std::string& parmName, const std::string& fieldName)
{
	mParmFieldMap[parmName] = fieldName;
	return true;
}


bool 
AosCondNotInRecord::getParmFieldMap(const std::string& parmName, std::string& fieldName)
{
	std::map<std::string, std::string>::iterator iter = mParmFieldMap.find(parmName);
	if (iter != mParmFieldMap.end())
	{
		fieldName = (*iter).second;
		return true;
	}
	return false;
}


void 
AosCondNotInRecord::delParmFieldMap(const std::string& parmName)
{
	mParmFieldMap.erase(mParmFieldMap.find(parmName));
}


bool 
AosCondNotInRecord::setParmValue(const std::string& parmName, const AosValuePtr& value)
{
	std::string fieldName;
	if (!getParmFieldMap(parmName, fieldName))
	{
		return false;
	}
	mParmValueMap[parmName] = value;

	return true;
}


bool 
AosCondNotInRecord::getParmValue(const std::string& parmName, AosValuePtr& value)
{
	std::map<std::string, AosValuePtr>::iterator iter = mParmValueMap.find(parmName);
	if (iter != mParmValueMap.end())
	{
		value = (*iter).second;
		return true;
	}
	return false;
}


bool 
AosCondNotInRecord::check(const AosValue& value)
{
	OmnAlarm << "Can not invoke this interface" << enderr;
	return false;
}


bool 
AosCondNotInRecord::check(const AosValue& valueLeft, const AosValue& valueRight)
{
	OmnAlarm << "Can not invoke this interface" << enderr;
	return false;
}

