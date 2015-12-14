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
// Field names can be specified as:
// 		name
// 		alias
// 		table.name
// 		database.table.name
//
// 		fieldname, containername, constant, operator, sort(ascd, desc)
//
// Handling Query:
// 1. Check whether there are multiple tables. 
// 2. Check whether the field name exists 
// 3. Check type mismatch
// 4. Check atomic field: age = 30 (atomic), age*100 = 3000
// 5. For compound 'field name', we need to try to transform
//    it into an atomic field, if possible.
//    	age = 3000/100
//
// Modification History:
// 01/04/2013 Created by Kerong.Li
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprFieldName.h"

#include "API/AosApi.h"
#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSelectField.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

	
AosExprFieldName::AosExprFieldName()
:
mFieldIdx(-1),
mIsParsedFieldIdx(false),
mIsMultiRecord(false)
{
}


AosExprFieldName::AosExprFieldName(const char *data)
:
mFieldIdx(-1),
mIsParsedFieldIdx(false),
mIsMultiRecord(false)
{
	aos_assert(strlen(data) > 0);
	mFieldName = data;
}


AosExprFieldName::AosExprFieldName(const char *data, const char *xpath)
:
mFieldIdx(-1),
mIsParsedFieldIdx(false),
mIsMultiRecord(false)
{   
	aos_assert(strlen(data) > 0);
	mFieldName = data;
	aos_assert(strlen(xpath) > 0);
	mXpathName = xpath;
}


bool
AosExprFieldName::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value_rslt)
{
	aos_assert_r(mFieldName != "", false);

	if (record)
	{
		//if (mFieldName == "zky_docid")
		//{
		//	value_rslt.setValue(record->getDocid());
		//	return true;
		//}

		bool rslt = true;
		if (!mIsParsedFieldIdx)
		{
			rslt = parseFieldIdx(rdata, record);
			aos_assert_r(rslt, false);
		}

		if (!mIsMultiRecord)
		{
			//yang,ignore this field
			if(mFieldIdx < 0)
			{
				value_rslt.setNull();
				return true;
			}

			rslt = record->getFieldValue(mFieldIdx, value_rslt, true, rdata);
		}
		else
		{
			int idx = -1;
			OmnString rcd_name = record->getRecordName();
			if (mFieldIdxMap.count(rcd_name) > 0)
			{
				idx = mFieldIdxMap[rcd_name];
			}

			if (idx >= 0)
			{
				rslt = record->getFieldValue(idx, value_rslt, true, rdata);
			}
			else
			{
				value_rslt.setNull();
				return true;
			}
		}
		if (rslt)
		{
			AosDataType::E type = value_rslt.getType();
			if (type == AosDataType::eString 
					|| type == AosDataType::eCharStr 
					|| type == AosDataType::eChar)
			{
				OmnString vv = value_rslt.getStr();
				vv.removeLeadingWhiteSpace();
				value_rslt.setStr(vv);
			}
			return true;
		}
		
		OmnString errmgr = "Record is not find this field : field name is ";
		errmgr << mFieldName;

		AosSetEntityError(rdata, errmgr, 
			"Deny Reason", errmgr) << enderr;
		return false;
	}

	value_rslt.setStr(mFieldName);
	return true;
}


bool
AosExprFieldName::parseFieldIdx(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	aos_assert_r(record, false);

	if (mIsParsedFieldIdx) return true;
	mIsParsedFieldIdx = true;
		
	mFieldIdx = record->getFieldIdx(mFieldName, rdata);
	//Gavin 2015/08/07 JIMODB-299
	if (mFieldIdx < 0)
	{
		OmnString error;                      
		error << " [ERR] : invalid Field '" << mFieldName << "'!" ;    
		rdata->setJqlMsg(error);              
	}
	aos_assert_r(mFieldIdx >= 0, false);
	return true;
/*
	OmnString table_objid = rdata->getArg1(AOSTAG_TABLEOBJID);	
	if (table_objid == "")
	{
		mFieldIdx = record->getFieldIdx(mFieldName, rdata);
		//aos_assert_r(mFieldIdx >= 0, false);
		return true;
	}
		
	AosXmlTagPtr xml = AosGetDocByObjid(table_objid, rdata);
	if (!xml)
	{
		mFieldIdx = record->getFieldIdx(mFieldName, rdata);
		aos_assert_r(mFieldIdx >= 0, false);
		return true;
	}

	bool is_virtual = xml->getAttrBool("zky_virtual", false);
	if (!is_virtual)
	{
		mFieldIdx = record->getFieldIdx(mFieldName, rdata);
		aos_assert_r(mFieldIdx >= 0, false);
		return true;
	}
	
	mIsMultiRecord = true;

	AosXmlTagPtr columns = xml->getFirstChild("columns");
	aos_assert_r(columns, false);

	AosXmlTagPtr f_column;
	AosXmlTagPtr column = columns->getFirstChild();
	while (column)
	{
		OmnString name = column->getAttrStr("name");
		if (name == mFieldName)
		{
			f_column = column;
			break;
		}
		column = columns->getNextChild();
	}
	aos_assert_r(f_column, false);

	map<OmnString, OmnString> rcd_fd_name_map;
	map<OmnString, OmnString>::iterator rcd_fd_name_map_itr;
	AosXmlTagPtr value = column->getFirstChild();
	while (value)
	{
		OmnString record_name = value->getAttrStr("recordname");
		OmnString field_name = value->getAttrStr("fieldname");

		aos_assert_r(rcd_fd_name_map.count(record_name) == 0, false);
		rcd_fd_name_map[record_name] = field_name;
		value = column->getNextChild();
	}
	aos_assert_r(!rcd_fd_name_map.empty(), false);
	
	AosXmlTagPtr datarecords = xml->xpathGetChild("schema_docs/dataschema/datarecords");
	aos_assert_r(datarecords, false);

	AosXmlTagPtr datarecord_tag = datarecords->getFirstChild();
	while (datarecord_tag)
	{
		AosXmlTagPtr record_tag = datarecord_tag->getFirstChild();
		aos_assert_r(record_tag, false);

		OmnString rcd_name = record_tag->getAttrStr("zky_name");
		rcd_fd_name_map_itr = rcd_fd_name_map.find(rcd_name);
		if (rcd_fd_name_map_itr == rcd_fd_name_map.end())
		{
			if (mFieldIdxMap.count(rcd_name) == 0)
			{
				mFieldIdxMap[rcd_name] = -1;
			}
		}
		else
		{
			OmnString field_name = rcd_fd_name_map_itr->second;
			rcd_fd_name_map.erase(rcd_fd_name_map_itr);

			AosXmlTagPtr fields_tag = record_tag->getFirstChild("datafields");
			aos_assert_r(fields_tag, false);

			int idx = 0;
			bool found_field = false;
			AosXmlTagPtr field_tag = fields_tag->getFirstChild();
			while (field_tag)
			{
				OmnString fd_name = field_tag->getAttrStr("zky_name");
				if (fd_name == field_name)
				{
					mFieldIdxMap[rcd_name] = idx;
					found_field = true;
					break;
				}

				idx++;
				field_tag = fields_tag->getNextChild();
			}
							
			aos_assert_r(found_field, false);
		}
						
		datarecord_tag = datarecords->getNextChild();
	}

	aos_assert_r(rcd_fd_name_map.empty(), false);
	return true;
	*/
}


bool
AosExprFieldName::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	AosJqlSelectFieldPtr n_field = OmnNew AosJqlSelectField;
	AosExprObjPtr expr = cloneExpr();
	aos_assert_r(expr, false);
	n_field->setField(expr);
	fields.push_back(n_field);
	return true;
}


OmnString 
AosExprFieldName::dump() const 
{
	OmnString str;
	str << "`" << mFieldName << "`";
	return str;
}


OmnString 
AosExprFieldName::dumpByNoEscape() const 
{
	return mFieldName ;
}


OmnString 
AosExprFieldName::dumpByStat() const 
{
	return dump();
}


AosExprObjPtr
AosExprFieldName::cloneExpr() const
{
	try                                                                   
	{                                                                     
		AosExprFieldNamePtr expr = OmnNew AosExprFieldName();
		expr->mFieldName = mFieldName;
		expr->mXpathName = mXpathName;
		expr->mFieldIdx = mFieldIdx;
		expr->mIsParsedFieldIdx = mIsParsedFieldIdx;
		expr->mIsMultiRecord = mIsMultiRecord;
		expr->mFieldIdxMap = mFieldIdxMap;

		return expr;                                                      
	}                                                                     
	catch (...)                                                           
	{                                                                     
		OmnAlarm << "failed create expr" << enderr;                       
		return 0;                                                         
	}                                                                     

	OmnShouldNeverComeHere;                                               
	return 0;                                                             
}


AosDataType::E 
AosExprFieldName::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	aos_assert_r(record, AosDataType::eInvalid);		
	AosDataFieldObj *field = record->getDataField(rdata, mFieldName);
	aos_assert_r(field, AosDataType::eInvalid);

	return field->getDataType(rdata, record);
}
