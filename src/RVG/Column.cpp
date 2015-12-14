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
// Tuesday, December 18, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "RVG/Column.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "random_c/random_util.h"
#include "RVG/Record.h"
#include "RVG/Table.h"
#include "Util/OmnNew.h"


AosColumn::AosColumn()
{
	mTable = NULL;
}


AosColumn::AosColumn(const AosColumn& column)
{
	*this = column;
}


AosColumn::~AosColumn()
{
}


AosColumn& 
AosColumn::operator=(const AosColumn& column)
{
	if (this != &column)
	{
		mColumnName = column.mColumnName;
		mValueType = column.mValueType;
		mValues = column.mValues;
		mOrderState = column.mOrderState;
		mTable = column.mTable;
	}
	return *this;
}


bool 
AosColumn::serialize(TiXmlNode& node)
{
	//
	// <Column>
	// 		<Name></Name>
	// 		<ValueType></ValueType>
	// 		<OrderState>Random/ASC/DEC</OrderState>
	// 		<ValueList>
	// 			<Value></Value>
	// 			<Value></Value>
	//			...
	// 		</ValueList>
	// </Column>
	//
	node.SetValue("Column");

	//
	// add Name node
	//
	node.addElement("Name", mColumnName.data());

	//
	// add ValueType node
	//
	node.addElement("ValueType", AosValueType::enum2Str(mValueType).data());

	//
	// add OrderState node
	//
	node.addElement("OrderState", AosSequenceOrder::enum2Str(mOrderState).data());

	//
	// add ValueList
	//
	TiXmlElement valueList("ValueList");
	int size = mValues.size();
	for (int i=0; i<size; i++)
	{
		TiXmlElement value("Value");
		mValues[i]->serialize(value);
		valueList.InsertEndChild(value);
	}
	node.InsertEndChild(valueList);

	return true;
}


bool 
AosColumn::deserialize(TiXmlNode* node)
{
	//
	// <Column>
	// 		<Name></Name>
	// 		<ValueType></ValueType>
	// 		<OrderState>Random/ASC/DEC</OrderState>
	// 		<ValueList>
	// 			<Value></Value>
	// 			<Value></Value>
	//			...
	// 		</ValueList>
	// </Column>
	//
	aos_assert_r(node, false);

	if (node->ValueStr() != "Column")
	{
		return false;
	}

	//
	// parse name, mandatory field
	//
	OmnNotImplementedYet;
	/*
	 * Trying to replace std::string with OmnString, but broke this code.
	 * Chen Ding, 12/16/2008
	 *
	if (!node->getElementTextValue("Name", mColumnName))
	{
		OmnAlarm << "XML parse: name node must exist in Column object" 
			<< enderr;
		return false;
	}

	//
	// parse ValueType, mandatory field
	//
	OmnString valueType;
	if (!node->getElementTextValue("ValueType", valueType))
	{
		OmnAlarm << "XML parse: ValueType node must exist in "
			<< "Column object" << enderr;
		return false;
	}
	mValueType = AosValueType::str2Enum(valueType);

	//
	// parse OrderState, mandatory field
	//
	OmnString state;
	if (!node->getElementTextValue("OrderState", state))
	{
		OmnAlarm << "XML parse: OrderState node must exist in "
			<< "Column object" << enderr;
		return false;
	}
	mOrderState = AosSequenceOrder::str2Enum(state);

	//
	// parse value list
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("ValueList").ToElement();
	if (element)
	{
		TiXmlElement* child = element->FirstChildElement("Value");	
		for (; child!=NULL; child=child->NextSiblingElement("Value"))
		{
			AosValue value;
			if (value.deserialize(child))
			{
				int index;
				addRowData(index, value);
			}
		}
	}
	*/

	return true;
}


int 
AosColumn::getRowNum()
{
	return mValues.size();
}


void 
AosColumn::setTable(AosTablePtr table)
{
	mTable = table;
}


OmnString 
AosColumn::getColumnName()
{
	return mColumnName;
}


void 
AosColumn::setColumnName(OmnString &name)
{
	mColumnName = name;
}


AosValueType::E
AosColumn::getValueType()
{
	return mValueType;
}


void 
AosColumn::setValueType(AosValueType::E type)
{
	mValueType = type;
}


bool 
AosColumn::getRowData(int rowIndex, AosValuePtr& data)
{
	int size = mValues.size();
	aos_assert_r(rowIndex >= 0 && rowIndex < size, false);
	data = mValues[rowIndex];
	return true;
}


bool 
AosColumn::randomGetData(AosValuePtr& data)
{
	int idx = aos_next_int(0, mValues.size()-1);
	return getRowData(idx, data);
}


bool 
AosColumn::setRowData(int rowIndex, const AosValue& data)
{
	//
	// when row index is wrong or data type is wrong, return false
	//
	int size = mValues.size();
	if (rowIndex < 0 || 
		rowIndex >= size ||
		data.getType() != mValueType)
	{
		return false;
	}
	*(mValues[rowIndex]) = data;
	return true;
}


bool 
AosColumn::addRowData(int& rowIndex, const AosValue& data)
{
	if (data.getType() != mValueType)
	{
		return false;
	}
	AosValuePtr ptr = OmnNew AosValue(data);
	mValues.push_back(ptr);
	rowIndex = mValues.size() - 1;
	return true;
}


bool 
AosColumn::addRows(int startIndex, int numRows)
{
	int size = mValues.size();
	if (startIndex < 0 || startIndex > size)
	{
		//
		// add at the end of the value array
		//

		for (int i=0; i<numRows; i++)
		{
			AosValuePtr ptr = OmnNew AosValue();
			ptr->setType(mValueType);
			mValues.insert(mValues.end(), ptr);
		}
	}
	else
	{
		//
		// add at the start index
		//
		for (int i=0; i<numRows; i++)
		{
			AosValuePtr ptr = OmnNew AosValue();
			ptr->setType(mValueType);
			mValues.insert(mValues.begin()+startIndex, ptr);
		}
	}
	return true;
}


bool 
AosColumn::removeRows(int startIndex, int& numRows)
{
	int size = mValues.size();
	if (startIndex < 0 || startIndex > size)
	{
		//
		// remove from the end of the value array
		//
		return false;
	}
	int maxDelRows = mValues.size() - startIndex;
	int deleteRows = (numRows>maxDelRows) ? maxDelRows : numRows;
	for (int i=0; i<deleteRows; i++)
	{
		//
		// remove from the start index
		//
		mValues.erase(mValues.begin()+startIndex);
	}
	numRows = deleteRows;
	return true;
}


bool 
AosColumn::findData(int& rowIndex, const AosValue& data)
{
	int size = mValues.size();
	int i;
	for (i=0; i<size; i++)
	{
		if (*(mValues[i]) == data)
		{
			rowIndex = i;
			break;
		}
	}
	//
	// can not find the data
	//
	if (i >= size)
	{
		return false;
	}
	return true;
}


bool 
AosColumn::findData(std::vector<int>& rowIndexes, const AosValue& data)
{
	int size = mValues.size();
	for (int i=0; i<size; i++)
	{
		if (*(mValues[i]) == data)
		{
			rowIndexes.push_back(i);
		}
	}
	//
	// can not find the data
	//
	if (rowIndexes.empty())
	{
		return false;
	}
	return true;
}


void 
AosColumn::sort(const AosSequenceOrder::E order)
{
	//
	// sort data in the column, other columns in table need to 
	// be sorted  according to this column, too
	//
	mOrderState = order;
}

