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
// December 26, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#if 0

#include "RVG/RVGMultiFromTable.h"


AosRVGMultiFromTable::AosRVGMultiFromTable()
{
}


AosRVGMultiFromTable::~AosRVGMultiFromTable()
{
}


bool 
AosRVGMultiFromTable::nextValue()
{
	if (!mTable)
	{
		return false;
	}

	mGenValues.clear();
	int index;
	if (!mTable->randomGetRecord(index, mGenValues))
	{
		return false;
	}

	return true;
}


bool 
AosRVGMultiFromTable::serialize(TiXmlNode& node)
{
//
//	<RVGMultiFromTable>
// 		<Name>Local_unique_name</Name>
//		<SelectPct>100</SelectPct>
//		<TableName></TableName>
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
//	</RVGMultiFromTable>
//
	node.addElement("Name", mName);
	node.addElement("SelectPct", AosValue::itoa(mSelectPct));
	node.addElement("TableName", mTableName);
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
	return true;
}


bool 
AosRVGMultiFromTable::deserialize(TiXmlNode* node)
{
//
//	<RVGMultiFromTable>
// 		<Name>Local_unique_name</Name>
//		<SelectPct>100</SelectPct>
//		<TableName></TableName>
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
//	</RVGMultiFromTable>
//
	//
	// parse Name, mandatory field
	//
	if (!node->getElementTextValue("Name", mName))
	{
		OmnAlarm << "XML parse: Name must exist in RVGMultiFromTable" << enderr;
		return false;
	}
	//
	// parse SelectPct, mandatory field
	//
	if (!node->getElementTextValue("SelectPct", mSelectPct))
	{
		OmnAlarm << "XML parse: SelectPct must exist in RVGMultiFromTable" << enderr;
		return false;
	}

	//
	// parse TableName, mandatory field
	//
	if (!node->getElementTextValue("TableName", mTableName))
	{
		OmnAlarm << "XML parse: TableName must exist in RVGMultiFromTable" << enderr;
		return false;
	}
	//
	// parse ParmFieldMap
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("ParmFieldMap").ToElement();
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
	return true;
}


u32 
AosRVGMultiFromTable::getSelectPct()
{
	return mSelectPct;
}


void 
AosRVGMultiFromTable::setSelectPct(u32 pct)
{
	mSelectPct = pct;
}


OmnString 
AosRVGMultiFromTable::getName()
{
	return mName;
}


void 
AosRVGMultiFromTable::setName(const OmnString & name)
{
	mName = name;
}


AosTablePtr 
AosRVGMultiFromTable::getTable()
{
	return mTable;
}


OmnString 
AosRVGMultiFromTable::getTableName()
{
	return mTableName;
}


void 
AosRVGMultiFromTable::setTable(AosTablePtr table)
{
	mTable = table;
}


void 
AosRVGMultiFromTable::setTableName(const OmnString &name)
{
	mTableName = name;
}


bool 
AosRVGMultiFromTable::addParmFieldMap(const OmnString &parmName, const OmnString &fieldName)
{
	mParmFieldMap[parmName] = fieldName;
	return true;
}


bool 
AosRVGMultiFromTable::getParmFieldMap(const OmnString &parmName, OmnString &fieldName)
{
	std::map<OmnString, OmnString>::iterator iter = mParmFieldMap.find(parmName);
	if (iter != mParmFieldMap.end())
	{
		fieldName = (*iter).second;
		return true;
	}
	return false;
}


void 
AosRVGMultiFromTable::delParmFieldMap(const OmnString &parmName)
{
	mParmFieldMap.erase(mParmFieldMap.find(parmName));
}


bool 
AosRVGMultiFromTable::getParmValue(const OmnString &parmName, AosValuePtr& value)
{
	if (!mTable)
	{
		return false;
	}
	OmnString fieldName;
	if (!getParmFieldMap(parmName, fieldName))
	{
		return false;
	}
	int index;
	if (!mTable->getColumnIndex(index, fieldName))
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


bool 
AosRVGMultiFromTable::isEmpty()
{
	if (!mTable)
	{
		return true;
	}
	return mTable->isEmpty();
}

#endif
