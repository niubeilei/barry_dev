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

#if 0
#include "RVG/RVGFromTable.h"
#include "RVG/RIG.h"
#include "Conditions/Condition.h"


AosRVGFromTable::AosRVGFromTable()
{
}


AosRVGFromTable::~AosRVGFromTable()
{
}


bool 
AosRVGFromTable::nextValue(AosValue& value)
{
	//
	// Randomly get value from the field of the table
	//
	if (!mTable)
	{
		return false;
	}
	AosColumnPtr column;
	if (!mTable->getColumn(mFieldName, column))
	{
		return false;
	}

	//
	// No condition, select from all possible values in the field
	//
	if (mFindConds.empty())
	{
		AosValuePtr valuePtr;
		if (!column->randomGetData(valuePtr))
		{
			return false;
		}
		if (valuePtr)
		{
			value = *valuePtr;
		}
	}
	else
	{
	//
	// Single/Multiple conditions, get available row indexes from which can be selected.
	// then randomly select value from the available row indexes.
	//
		std::vector<int> rowIndexes;
		getAvailRowIndexes(rowIndexes);

		//
		// Randomly select one of the rowIndexes
		//
		AosRIG<u32> rowRIG;
		AosValue selectedValue;
		rowRIG.setIntegerPair(0, rowIndexes.size(), 10);
		rowRIG.nextValue(selectedValue);
		u32 selected = selectedValue.toUint32();
		int rowIndex = rowIndexes.at(selected);

		//
		// get value from the selected row index
		//
		AosValuePtr valuePtr;
		if (!column->getRowData(rowIndex, valuePtr))
		{
			return false;
		}
		if (valuePtr)
		{
			value = *valuePtr;
		}
	}

	return true;
}


//
// get available row indexes by the conditions
//
void
AosRVGFromTable::getAvailRowIndexes(std::vector<int>& rowIndexes)
{
	int rowNum = mTable->getRowNum();
	for (int i=0; i<rowNum; i++)
	{
		//
		// get a record from table 
		//
		AosValuePtrArray ptrArray;
		if (mTable->getRecord(i,ptrArray))
		{
			//
			// check whether all conditions is right
			//
			AosFindCondList::iterator iter;
			for (iter=mFindConds.begin(); iter!=mFindConds.end(); iter++)
			{
				int columnIndex;
				if (mTable->getColumnIndex(columnIndex, iter->mFieldName))
				{
					AosValuePtr ptr = ptrArray.at(columnIndex);

					// 
					// Temperarily commented out by Chen Ding, 01/29/2008
					//
					/*
					if (ptr && iter->mCond && iter->mCond->check(*ptr))
					{
						continue;
					}
					else
					{
						break;
					}
					*/
				}
			}
			//
			// all conditions are right
			// save this row index
			//
			if (iter == mFindConds.end())
			{
				rowIndexes.push_back(i);
			}
		}
	}

/*
	for (int i=0; i<keyNum; i++)
	{
		AosColumnPtr column;
		if (mTable->getColumn(mFindConds[i].mFieldName, column))
		{
			column->findData(rowIndexes, mFindConds[i].mValue);
		}
	}
*/
}


bool 
AosRVGFromTable::serialize(TiXmlNode& node)
{
//
//	<RVGFromTable>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType></ValueType>
//		<Weight>100</Weight>
//		<ComposeWeight>100</ComposeWeight>
//		<TableName></TableName>
//		<FieldName></FieldName>
//		<FindKeys>
//			<Key>
//				<FieldName></FieldName>
//				<Condition></Condition>
//			</Key>
//			<Key>
//				<FieldName></FieldName>
//				<Condition></Condition>
//			</Key>
//			...
//		</FindKeys>
//	</RVGFromTable>
//
	if (! AosRVG::serialize(node))
	{
		return false;
	}
	node.addElement("TableName", mTableName);
	node.addElement("FieldName", mFieldName);
	//
	// add FindKeys list
	//
	TiXmlElement keyList("FindKeys");
	AosFindCondList::iterator iter;
	for (iter=mFindConds.begin(); iter!=mFindConds.end(); iter++)
	{
		TiXmlElement key("Key");
		key.addElement("FieldName", iter->mFieldName);
		TiXmlElement cond("Condition");
		TiXmlElement detailCond("cond");
		if (iter->mCond)
		{
			iter->mCond->serialize(detailCond);
			cond.InsertEndChild(detailCond);
		}
		key.InsertEndChild(cond);
		keyList.InsertEndChild(key);
	}
	node.InsertEndChild(keyList);
	return true;
}


bool 
AosRVGFromTable::deserialize(TiXmlNode* node)
{
//
//	<RVGFromTable>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType></ValueType>
//		<Weight>100</Weight>
//		<ComposeWeight>100</ComposeWeight>
//		<TableName></TableName>
//		<FieldName></FieldName>
//		<FindKeys>
//			<Key>
//				<FieldName></FieldName>
//				<Condition></Condition>
//			</Key>
//			<Key>
//				<FieldName></FieldName>
//				<Condition></Condition>
//			</Key>
//			...
//		</FindKeys>
//	</RVGFromTable>
//
	if (! AosRVG::deserialize(node))
	{
		return false;
	}

	//
	// parse TableName, mandatory field
	//
	if (!node->getElementTextValue("TableName", mTableName))
	{
		OmnAlarm << "XML parse: TableName must exist in RVGFromTable" << enderr;
		return false;
	}
	//
	// parse FieldName, mandatory field
	//
	if (!node->getElementTextValue("FieldName", mFieldName))
	{
		OmnAlarm << "XML parse: FieldName must exist in RVGFromTable" << enderr;
		return false;
	}
	//
	// parse FindKey list
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("FindKeys").ToElement();
	if (element)
	{
		OmnString fieldName;
		AosConditionPtr cond;	
		TiXmlElement* child = element->FirstChildElement("Key");	
		for (; child!=NULL; child=child->NextSiblingElement("Key"))
		{
			if (child->getElementTextValue("FieldName", fieldName))
			{
				TiXmlElement* element = child->FirstChildElement("Condition");
				if (element)
				{
					TiXmlElement* condEle = element->FirstChildElement();
					cond = AosCondition::factory(condEle);
					if (cond)
					{
						addCond(fieldName, cond);
					}
				}
			}
		}
	}
	return true;
}


AosTablePtr 
AosRVGFromTable::getTable()
{
	return mTable;
}


OmnString 
AosRVGFromTable::getFieldName()
{
	return mFieldName;
}


OmnString 
AosRVGFromTable::getTableName()
{
	return mTableName;
}


void 
AosRVGFromTable::setTable(AosTablePtr table)
{
	mTable = table;
}


void 
AosRVGFromTable::setFieldName(const OmnString &field)
{
	mFieldName = field;
}


void 
AosRVGFromTable::setTableName(const OmnString &name)
{
	mTableName = name;
}


void 
AosRVGFromTable::getConds(AosRVGFromTable::AosFindCondList& conds)
{
	conds = mFindConds;
}


//
// add a find condition, if the field is exist, update the condition 
//
void 
AosRVGFromTable::addCond(const OmnString &fieldName, const AosConditionPtr& cond)
{
	AosFindCondList::iterator iter;
	iter = std::find_if(mFindConds.begin(),
						mFindConds.end(),
						bind2nd(FunctorFindField(), fieldName));
	if (iter != mFindConds.end())
	{
		iter->mCond = cond;
	}
	else
	{
		FindCond findCond;
		findCond.mFieldName = fieldName;
		findCond.mCond = cond;
		mFindConds.push_back(findCond);
	}
}


void 
AosRVGFromTable::removeCond(const OmnString &fieldName)
{
	AosFindCondList::iterator iter;
	iter = std::find_if(mFindConds.begin(),
						mFindConds.end(),
						bind2nd(FunctorFindField(), fieldName));
	if (iter != mFindConds.end())
	{
		mFindConds.erase(iter);
	}
}
#endif


