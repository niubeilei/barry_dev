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
#include "RVG/Table.h"

#include "random/random_util.h"
#include "RVG/Record.h"
#include "RVG/RIG.h"
#include "Util/OmnNew.h"



AosTable::AosTable()
{
	mRowNum = 0;
}


AosTable::~AosTable()
{
}


bool 
AosTable::serialize(TiXmlNode& node)
{
	//
	// <Table>
	// 		<Name>Local_Unique_Name</Name>
	// 		<MaxRows></MaxRows>
	//		<SelectPct>100</SelectPct>
	//		<Name></Name>
	//		<ParmFieldMap>
	//			<ParmField>
	//				<ParmName></ParmName>
	//				<FieldName></FieldName>
	//			</ParmField>
	//			<ParmField>
	//				<ParmName></ParmName>
	//				<FieldName></FieldName>
	//			</ParmField>
	//			...
	//		</ParmFieldMap>
	// 		<Columns>
	// 			<Column> use AosColumn serialize </Column>
	// 			<Column> use AosColumn serialize </Column>
	//			...
	//		</Columns>
	// </Table>
	//

	node.SetValue("Table");
	node.addElement("Name", mName);
	node.addElement("SelectPct", AosValue::itoa(mSelectPct));
	node.addElement("Name", mName);

	//
	// add FindKeys list
	//
	TiXmlElement maps("ParmFieldMap");
	std::map<OmnString, OmnString>::iterator iter;
	for (iter=mParmFieldMap.begin(); iter!=mParmFieldMap.end(); iter++)
	{
		TiXmlElement pair("ParmField");
		pair.addElement("ParmName", iter->first);
		pair.addElement("FieldName", iter->second);
		maps.InsertEndChild(pair);
	}
	node.InsertEndChild(maps);

	//
	// add MaxRows node
	//
	node.addElement("MaxRows", AosValue::itoa(mMaxRows));

	//
	// add Columns
	//
	TiXmlElement columns("Columns");
	int size = mColumns.size();
	for (int i=0; i<size; i++)
	{
		TiXmlElement column("Column");
		mColumns[i]->serialize(column);
		columns.InsertEndChild(column);
	}
	node.InsertEndChild(columns);

	return true;
}


bool 
AosTable::deserialize(TiXmlNode* node)
{
	//
	// <Table>
	// 		<Name>Local_Unique_Name</Name>
	// 		<MaxRows></MaxRows>
	// 		<Columns>
	// 			<Column> use AosColumn deserialize </Column>
	// 			<Column> use AosColumn deserialize </Column>
	//			...
	//		</Columns>
	// </Table>
	//
	
	//
	// parse SelectPct, mandatory field
	//
	if (!node->getElementTextValue("SelectPct", mSelectPct))
	{
		OmnAlarm << "XML parse: SelectPct must exist" << enderr;
		return false;
	}

	//
	// parse Name, mandatory field
	//
	if (!node->getElementTextValue("Name", mName))
	{
		OmnAlarm << "XML parse: Name must exist" << enderr;
		return false;
	}

	//
	// parse ParmFieldMap
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = 
		docHandle.FirstChild("ParmFieldMap").ToElement();
	if (element)
	{
		OmnString parmName;
		OmnString fieldName;
		TiXmlElement* child = element->FirstChildElement("ParmField");	
		for (; child!=NULL; child=child->NextSiblingElement("ParmField"))
		{
			if (child->getElementTextValue("ParmName", parmName))
			{
				if (child->getElementTextValue("FieldName", fieldName))
				{
					addParmFieldMap(parmName, fieldName);
				}
			}
		}
	}

	if (node->ValueStr() != "Table")
	{
		return false;
	}

	//
	// parse MaxRows, mandatory field
	//
	if (!node->getElementTextValue("MaxRows", mMaxRows))
	{
		OmnAlarm << "XML parse: MaxRows node must exist in Table object" 
			<< enderr;
		return false;
	}

	//
	// parse Columns
	//
	element = docHandle.FirstChild("Columns").ToElement();
	if (element)
	{
		TiXmlElement* child = element->FirstChildElement("Column");	
		for (; child!=NULL; child=child->NextSiblingElement("Column"))
		{
			AosColumn column;
			if (column.deserialize(child))
			{
				int index;
				addColumn(index, column);
			}
		}
	}

	if (!checkRowNum())
	{
		return false;
	}

	return true;
}


bool
AosTable::checkRowNum()
{
	int rowNum;
	int size = mColumns.size();
	for (int i=0; i<size; i++)
	{
		if (i == 0)
		{
			rowNum = mColumns[i]->getRowNum();
		}
		int num = mColumns[i]->getRowNum();
		rowNum = (rowNum > num)	? num : rowNum;
	}
	mRowNum = rowNum;
	return true;
}


int 
AosTable::getMaxRows()
{
	return mMaxRows;
}


void 
AosTable::setMaxRows(int maxRows)
{
	mMaxRows = maxRows;
}


bool AosTable::getColumn(int index, AosColumnPtr& column)
{
	int size = mColumns.size();
	if (index < 0 || index >= size)
	{
		return false;
	}
	column = mColumns[index];
	return true;
}


bool 
AosTable::getColumn(OmnString &columnName, AosColumnPtr& column)
{
	int columnSize=mColumns.size();
	for (int i=0; i<columnSize; i++)
	{
		if (mColumns[i]->getColumnName() == columnName)
		{
			column = mColumns[i];
			return true;
		}
	}
	return false;
}


bool 
AosTable::addColumn(int& index, const AosColumn& column)
{
	AosColumnPtr ptr = OmnNew AosColumn(column);
	mColumns.push_back(ptr);
	index = mColumns.size() - 1;
	return true;
}


bool 
AosTable::removeColumn(int columnIndex)
{
	int size = mColumns.size();
	if (columnIndex < 0 || columnIndex >= size)
	{
		return false;
	}

	mColumns.erase(mColumns.begin()+columnIndex);

	return true;
}


bool 
AosTable::getColumnIndex(int& index, const AosColumnPtr& column)
{
	AosColumnPtrArray::iterator iter = 
		std::find(mColumns.begin(), mColumns.end(), column);
	if (iter != mColumns.end())
	{
		index = std::distance(mColumns.begin(), iter);
		return true;
	}
	return false;
}


bool 
AosTable::getColumnIndex(int& index, const OmnString &columnName)
{
	int columnSize = mColumns.size();
	for (int i=0; i<columnSize; i++)
	{
		if (mColumns[i]->getColumnName() == columnName)
		{
			index = i;
			return true;
		}
	}
	return false;
}


bool 
AosTable::addRows(int startIndex, int numRows)
{
	if (mRowNum + numRows > mMaxRows)
	{
		return false;
	}
	bool ret = true;
	int columnSize = mColumns.size();
	for (int i=0; i<columnSize; i++)
	{
		if (!mColumns[i]->addRows(startIndex, numRows))	
		{
			ret = false;
		}
	}
	mRowNum += numRows;
	return ret;
}


bool 
AosTable::removeRows(int startIndex, int numRows)
{
	bool ret = true;
	int columnSize = mColumns.size();
	for (int i=0; i<columnSize; i++)
	{
		if (!mColumns[i]->removeRows(startIndex, numRows))	
		{
			ret = false;
		}
	}
	mRowNum -= numRows;
	return ret;
}


bool 
AosTable::isEmpty()
{
	return (mColumns.empty() || mRowNum == 0);
}


int  
AosTable::getColumnNum()
{
	return mColumns.size();
}


int  
AosTable::getRowNum()
{
	return mRowNum;
}


bool 
AosTable::getRecord(int rowIndex, AosRecord& record)
{
	AosValuePtr valuePtr;
	int columnSize = mColumns.size();
	for (int i=0; i<columnSize; i++)
	{
		if (!mColumns[i]->getRowData(rowIndex, valuePtr))
		{
			return false;
		}
		record.push_back(valuePtr);
	}
	return true;
}


bool 
AosTable::setRecord(int rowIndex, const AosValuePtrArray& record)
{
	int columnSize = mColumns.size();
	int recordSize = record.size();
	if (columnSize != recordSize)
	{
		return false;
	}
	for (int i=0; i<columnSize; i++)
	{
		if (!mColumns[i]->setRowData(rowIndex, *(record[i])))
		{
			return false;
		}
	}
	return true;
}


bool 
AosTable::addRecord(int &rowIndex, const AosValuePtrArray& record)
{
	if (!addRows())
	{
		return false;
	}
	rowIndex = mRowNum - 1;
	return setRecord(rowIndex, record);
}


bool 
AosTable::getNewRecord(int &rowIndex, AosRecord &record)
{
	if (!addRows())
	{
		return false;
	}
	rowIndex = mRowNum - 1;
	return getRecord(rowIndex, record);
}


bool 
AosTable::randomGetRecord(int& rowIndex, AosRecord& record)
{
	int idx = aos_next_int(0, mRowNum-1);
	return getRecord(idx, record);
}


bool 
AosTable::getData(int row, int column, AosValuePtr& data)
{
	int size = mColumns.size();
	if (column < 0 || column >= size)
	{
		return false;
	}
	return mColumns[column]->getRowData(row, data);
}


bool 
AosTable::setData(int row, int column, const AosValue& data)
{
	int size = mColumns.size();
	if (column < 0 || column >= size)
	{
		return false;
	}
	return mColumns[column]->setRowData(row, data);
}


bool 
AosTable::sortbyColumn(int columnIndex, AosSequenceOrder::E order)
{
	int size = mColumns.size();
	if (columnIndex < 0 || columnIndex >= size)
	{
		return false;
	}
	//
	// sort in one column, other columns need to be sort as this column too
	//
	mColumns[columnIndex]->sort(order);
	return true;
}


bool 
AosTable::sortbyColumn(OmnString &columnName, AosSequenceOrder::E order)
{
	int index;
	if (!getColumnIndex(index, columnName))
	{
		return false;
	}

	//
	// sort in one column, other columns need to be sort as this column too
	//
	mColumns[index]->sort(order);
	return true;
}


OmnString 
AosTable::getName()
{
	return mName;
}


void 
AosTable::setName(const OmnString &name)
{
	mName = name;
}


OmnString 
AosTable::toString()
{
	OmnString str;
	return str;
}


bool 
AosTable::createInstance(const OmnString &configFile, AosTablePtr& table)
{
	TiXmlDocument doc(configFile);
	if (!doc.LoadFile())
	{
		OmnAlarm << "The torturer config file doesn't exist or syntax error." 
				<< doc.ErrorDesc()
				<< ":"
				<< doc.ErrorRow()
				<< ":"
				<< doc.ErrorId()
				<< enderr;
		return false;
	}
	TiXmlElement* child = doc.FirstChildElement();
	table = OmnNew AosTable();
	return table->deserialize(child);
}


bool 
AosTable::randomGetRecord(AosRecord&record)
{
	int idx;
	return randomGetRecord(idx, record);
}


bool 
AosTable::addParmFieldMap(
		const OmnString &parmName, 
		const OmnString &fieldName)
{
	mParmFieldMap[parmName] = fieldName;
	return true;
}


bool 
AosTable::getParmFieldMap(
		const OmnString &parmName, 
		OmnString &fieldName)
{
	std::map<OmnString, OmnString>::iterator iter = 
		mParmFieldMap.find(parmName);
	if (iter != mParmFieldMap.end())
	{
		fieldName = (*iter).second;
		return true;
	}
	return false;
}


void 
AosTable::delParmFieldMap(const OmnString &parmName)
{
	mParmFieldMap.erase(mParmFieldMap.find(parmName));
}


/*
bool 
AosTable::getParmValue(
		const OmnString &parmName, 
		AosValuePtr& value)
{
	OmnString fieldName;
	if (!getParmFieldMap(parmName, fieldName))
	{
		return false;
	}
	int index;
	if (!getColumnIndex(index, fieldName))
	{
		return false;
	}
	int size = mGenValues.size();
	if (index < 0 || index > size)
	{
		return false;
	}
	value = mGenValues.at(index);
	return true;
}
*/

