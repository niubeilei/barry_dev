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
// Modification History:
// 03/05/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcSelect.h"

#include "API/AosApi.h"
#include "AosConf/DataRecord.h"
#include "AosConf/OutputDataset.h"
#include <boost/make_shared.hpp>
#include "JSON/JSON.h"
#include "TaskMgr/Task.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataProcSelect_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcSelect(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosDataProcSelect::AosDataProcSelect(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcSelect)
{
}


AosDataProcSelect::AosDataProcSelect(const AosDataProcSelect &proc)
:
AosStreamDataProc(proc)
{
	AosExprObjPtr expr = NULL;
	for (size_t i = 0; i < proc.mFields.size(); i++)
	{
		expr = proc.mFields[i].mValue->cloneExpr();

		SelectField field( proc.mFields[i].mName, expr, proc.mFields[i].mType, 
				proc.mFields[i].mLen, proc.mFields[i].mIsAgrFunc, 
				proc.mFields[i].mFuncName) ;

		mFields.push_back(field);
	}
	mFieldsSize = mFields.size();

	for (size_t i = 0; i < (proc.mGroupByKeys).size(); ++i)
	{
		expr = (proc.mGroupByKeys)[i]->cloneExpr();
		mGroupByKeys.push_back(expr);
		mRawGroupByKeys.push_back(expr.getPtr());
	}
	mGroupByKeysSize = mGroupByKeys.size();

	for (size_t i = 0; i < (proc.mKeyFields).size(); ++i)
	{
		expr = (proc.mKeyFields)[i]->cloneExpr();
		mKeyFields.push_back(expr);
		mRawKeyFields.push_back(expr.getPtr());
	}
	mKeyFieldsSize = mKeyFields.size();
	
	if (proc.mInputDocid)
	{
		mInputDocid = proc.mInputDocid->cloneExpr();
		mRawInputDocid = mInputDocid.getPtr();
	}

	if (proc.mCondition)
	{
		mCondition = proc.mCondition->cloneExpr();
	}
	mRawCondition = mCondition.getPtr();

	AosDataRecordObjPtr rcd = NULL;
	for (size_t i = 0; i < proc.mInputRecords.size(); i++)
	{
		rcd = proc.mInputRecords[i]->clone(0 AosMemoryCheckerArgs);
		mInputRecords.push_back(rcd);
	}
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	}
	mRawOutputRecord = mOutputRecord.getPtr();

	mInputRecordsMap = proc.mInputRecordsMap;
	mSelectNameMap = proc.mSelectNameMap;
	mOrderByTypeMap = proc.mOrderByTypeMap;

	mStatus = proc.mStatus;
}


AosDataProcSelect::~AosDataProcSelect()
{
}


bool
AosDataProcSelect::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//example 1(no group by):
	//create dataprocselect dp1
	//{
	//	"type":"select",
	//	"fields":[
	//        	   {
	//        	   "alias":"f1",
	//        	   "fname":"key_field1",      
	//        	   "type":"u64",
	//        	   "max_length":8
	//        	   },
	//        	   {
	//        	   "fname":"key_field2",      
	//        	   "type":"str"
	//        	   },
	//        	   {
	//        	   "alias":"f3",         
	//        	   "fname":"key_field3",
	//        	   "type":"str",
	//        	   "max_length":15
	//        	   }
	//         ],
	//   "condition":"key_field1>0",	//optional
	//   "record_type":"fixbin|buff",	//optional
	//   "shuffle_type":"cube",			//optional
	//   "shuffle_field":"docid%10",	//optional
	//   "output_name":"xxxxx",			//optional
	//   "schema":"xxx"					//optional
	//};                                  
	//
	//
	//example 2(have group by):
	//create dataprocselect dp1
	//{
	//"type":"select",
	//"fields":[
	//         {
	//         "alias":"f1",
	//         "fname":"sum(key_field1)",      
	//         "type":"u64",
	//         "max_length":8
	//         },
	//         {
	//         "fname":"key_field2",           
	//         "type":"str"
	//         },
	//         {
	//         "alias":"service_option",         
	//         "fname":"100",
	//         "type":"u64",
	//         "max_length":8
	//         }
	//         ],
	//"condition":"key_field1>0",		//optional
	//"groupby_fields":["key_field2","key_field3"],
	//"order_by_type":["asc", "desc"], 	//optional 
	//"record_type":"fixbin|buff",		//optional
	//"shuffle_type":"cube",		//optional
	//"shuffle_field":"docid%10",		//optional
	//"output_name":"xxxxx",		//optional
	//"schema":"xxx"		//optional
	//};                                            
	//
	//example 3(distinct):
	//create dataprocselect dp1
	//{
	//	"type":"select",
	//	"distinct": ["key_field2", "key_field3"],
	//	"condition":"key_field1>0",
	//	"record_type":"fixbin|buff",
	//	"shuffle_type":"cube",
	//	"shuffle_field":"docid%10",
	//	"output_name":"xxxxx",
	//	"schema":"xxx"
	//};
	//
	//example 4(distinct)
	//create dataprocselect dp1
	//	{
	//	"type":"select",
	//	"fields":[
	//	         {
	//	         "alias":"f1",
	//	         "fname":"key_field1 + 2",
	//	         "type":"u64",    
	//	         "max_length":8   
	//	         },
	//	         {
	//	         "fname":"key_field2"
	//	         }
	//	         ],
	//	"distinct": ["all"],
	//	"condition":"key_field1>0",
	//	"record_type":"fixbin|buff",
	//	"shuffle_type":"cube",
	//	"shuffle_field":"docid%10",
	//	"output_name":"xxxxx",
	//	"schema":"xxx"
	//	};
	///
	//example 5(distinct):
	//create dataprocselect dp1
	//{
	//	"type":"select",
	//	"distinct": ["all"],
	//	"condition":"key_field1>0",
	//	"record_type":"fixbin|buff",
	//	"shuffle_type":"cube",
	//	"shuffle_field":"docid%10",
	//	"output_name":"xxxxx",
	//	"schema":"xxx"
	//};
	//
	//example 6(order by):
	//create dataprocselect dp1
	//{
	// "fields":[
	//         {
	//         "alias":"f1",
	//         "fname":"key_field1",      
	//         "type":"u64",
	//         "max_length":8
	//         },
	//         {
	//         "fname":"key_field2",           
	//         "type":"str"
	//         }
	//         ],
	//	"type":"select",
	//	"orderby_fields": ["f1", "key_field2"],
	//	"order_by_type":["asc", "desc"],	//optional
	//	"condition":"key_field1>0",
	//	"record_type":"fixbin|buff",
	//	"shuffle_type":"cube",
	//	"shuffle_field":"docid%10",
	//	"output_name":"xxxxx",
	//	"schema":"xxx"
	//};
	aos_assert_r(def, false);
	OmnString dp_json = def->getNodeText();

	mFields.clear();
	mFieldsSize = 0;
	mGroupByKeys.clear();
	mGroupByKeysSize = 0;
	mKeyFields.clear();
	mKeyFieldsSize = 0;
	mSelectNameMap.clear();
	mOrderByTypeMap.clear();
	mStatus = AosDataProcSelect::eNULL;

	//json_str to JSONValue
	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_json, json);
	aos_assert_r(rslt, false);
	mJson = json;

	if(mJson.isMember("isconfig_done"))
	{
		OmnString config_done_str = mJson["isconfig_done"].asString();
		if(config_done_str == "false")
			mConfigDone = false;
		else
			mConfigDone = true;
		mJson["flag"] = false;
	}

	mName = def->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mName != "", false);

	//createmap fieldname -> AosDataFieldObjPtr 
	if(!mIsStreaming)
	{
		rslt = createInputRecordsMap();  
		aos_assert_r(rslt, false);

		//select fields
		rslt = configSelectFields(json, rdata);	
		aos_assert_r(rslt, false);

		//condition
		OmnString name = json["condition"].asString();
		if (name != "")
		{
			mCondition = convertToExpr(name, rdata);
			if (!mCondition) return false;
			mRawCondition = mCondition.getPtr();
		}

		//create output
		rslt = createOutput(mName, json, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosDataProcSelect::createInputRecordsMap()
{
	aos_assert_r(mInputRecords.size() > 0, false);

	AosDataRecordObjPtr inputrcd = mInputRecords[0];
	aos_assert_r(inputrcd, false);

	vector<AosDataFieldObjPtr> input_fields = inputrcd->getFields();
	for(size_t i = 0; i < input_fields.size(); i++)
	{
		OmnString name = input_fields[i]->getName();
		mInputRecordsMap.insert(make_pair(name, input_fields[i]));
	}

	return true;
}


bool
AosDataProcSelect::configSelectFields(
		const JSONValue &json, 
		const AosRundataPtr &rdata)
{
	// 1. If "groupby_fields" is not null, the results will be merged
	//    and sorted based on the groupby_fields. "order_by" will be
	//    ignored. "order_by_type" may be specified. "distinct" 
	//    will be ignored. "fields" cannot null.
	// 2. If "groupby_fields" is null but "distinct" is not null, 
	//    the results will be sorted based on the distinct fields. 
	//    Duplicated records will be discarded. "order_by" and "fields"
	//    are ignored. 
	// 3. If both "groupby_fields" and "distinct" are both null, "fields"
	//    must exist. The resutls may or may not be sorted, depending
	//    on whether "order_by" exists. If not, results are not sorted.
	//    Otherwise, the results will be sorted based on "order_by"
	//    and "order_by_type" attributes.

	JSONValue fields = json["fields"];
	JSONValue groupby_fields = json["groupby_fields"];
	JSONValue distincts = json["distinct"];
	JSONValue orderby_fields = json["orderby_fields"];
	JSONValue orderby_types = json["order_by_type"];

	//group by
	if (groupby_fields.size() > 0)
	{
		aos_assert_r(fields.size() > 0, false);
		mStatus = AosDataProcSelect::eGroupBy;

		bool rslt = configFields(fields, rdata);
		aos_assert_r(rslt, false);
		
		rslt = configKeyFields(groupby_fields, orderby_types, rdata);
		aos_assert_r(rslt, false);

		return true;
	}
	//distinct
	if (distincts.size() > 0)
	{
		//for temp "fields" will not be ignored
		mStatus = AosDataProcSelect::eDistinct;

		if (fields.size() > 0)
		{
			aos_assert_r(distincts[0u].asString() == "all", false);

			bool rslt = configFields(fields, rdata);
			aos_assert_r(rslt, false);

			return true;
		}
		else
			return configDistinctFields(distincts, rdata);
	}
	//order_by
	aos_assert_r(fields.size() > 0, false);

	bool rslt = configFields(fields, rdata);
	aos_assert_r(rslt, false);

	if (orderby_fields.size() <= 0)
	{
		//no order by
		mStatus = AosDataProcSelect::eNorm;
	}
	else
	{
		mStatus = AosDataProcSelect::eOrderBy;   

		rslt = configKeyFields(orderby_fields, orderby_types, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosDataProcSelect::configDistinctFields(
		const JSONValue &distincts,
		const AosRundataPtr &rdata)
{
	// This function is called only when 'fields' is null. 
	int len = 0;
	AosExprObjPtr expr;
	AosDataFieldObjPtr field;
	OmnString name = "", value_str= "";
	map<OmnString, AosDataFieldObjPtr>::iterator itr;
	AosDataFieldType::E type = AosDataFieldType::eInvalid;
	//distinct all fields
	if (distincts.size() == 1 && distincts[0u].asString() == "all")
	{
		for (itr = mInputRecordsMap.begin(); itr != mInputRecordsMap.end(); ++itr)
		{
			name = itr->first;
			field = itr->second;
			aos_assert_r(field, false);

			len = field->mFieldInfo.field_data_len;
			
			value_str = name;
			expr = convertToExpr(value_str, rdata);
			if (!expr) return false;

			type = convertToDataFieldType(rdata.getPtr(), expr, mInputRecords[0]);
			aos_assert_r(type != AosDataFieldType::eInvalid, false);
			resetDataFieldLen(type, len);

			mFields.push_back(SelectField(name, expr, type, len));
			mGroupByKeys.push_back(expr);
			mRawGroupByKeys.push_back(expr.getPtr());
		}
		mFieldsSize = mFields.size();
		mGroupByKeysSize = mGroupByKeys.size();
	}
	else
	{
		for (size_t i = 0; i < distincts.size(); i++)
		{
			name = distincts[i].asString();
			itr = mInputRecordsMap.find(name);
			if (itr == mInputRecordsMap.end())
			{
				AosLogError(rdata, true, "distinct_field_not_found")
					<< AosFN("Field Name") << name << enderr;
				return false;
			}

			field = itr->second;
			aos_assert_r(field, false);

			//type = field->getTypeName();

			len = field->mFieldInfo.field_data_len;

			value_str = name;
			expr = convertToExpr(value_str, rdata);
			if (!expr) return false;

			type = convertToDataFieldType(rdata.getPtr(), expr, mInputRecords[0]);
			aos_assert_r(type != AosDataFieldType::eInvalid, false);

			resetDataFieldLen(type, len);
			mFields.push_back(SelectField(name, expr, type, len));
			mGroupByKeys.push_back(expr);
			mRawGroupByKeys.push_back(expr.getPtr());
		}
		mFieldsSize = mFields.size();
		mGroupByKeysSize = mGroupByKeys.size();
	}
	return true;
}


bool
AosDataProcSelect::buildOrderByTypeMap(
		const JSONValue &fields,
		const JSONValue &orderby_types,
		const AosRundataPtr &rdata)
{
	//set orderByTypeMap [name, type(asc, desc)]
	OmnString name = "", type = "";
	if (orderby_types.size() == 0)
	{
		//if do not set orderby_type,
		//default orderby type is asc
		for (size_t i = 0; i < fields.size(); i++)
		{
			name = fields[i].asString();
			mOrderByTypeMap[name] = "asc";
		}
	}
	else if (orderby_types.size() == 1)
	{
		//if only have one orderby type 
		//the all groupby_fields type is the same
		type = orderby_types[0u].asString();
		if (type != "asc" && type != "desc")
		{
			AosLogError(rdata, true, "invalid_order_type")
				<< AosFN("Order Type") << type
				<< AosFN("Allowed order types are: 'asc' and 'desc'") << enderr;
			return false;
		}

		for (size_t i = 0; i < fields.size(); i++)
		{
			name = fields[i].asString();
			mOrderByTypeMap[name] = type;
		}
	}
	else 
	{
		if (orderby_types.size() != fields.size())
		{
			AosLogError(rdata, true, "order_by_type_mismatch_order_by_fiels")
				<< AosFN("Number of Order By Types") << orderby_types.size()
				<< AosFN("Number of Order By Fields") << fields.size() << enderr;
			return false;
		}

		for (size_t i = 0; i < fields.size(); i++)
		{
			name = fields[i].asString();
			type = orderby_types[i].asString();
			if (type != "asc" && type != "desc")
			{
				AosLogError(rdata, true, "invalid_order_type")
					<< AosFN("Order Type") << type
					<< AosFN("Allowed order types are: 'asc' and 'desc'") << enderr;
				return false;
			}

			mOrderByTypeMap[name] = type;
		}
	}

	return true;
}


bool
AosDataProcSelect::configFields(
		const JSONValue &fields,
		const AosRundataPtr &rdata)
{
	// Fields are specified in the form:
	// 	fields: 
	// 	[
	// 		{fname:xxx, type:xxx, max_length:xxx, alias:xxx},
	// 		{fname:xxx, type:xxx, max_length:xxx, alias:xxx},
	// 		...
	// 		{fname:xxx, type:xxx, max_length:xxx, alias:xxx}
	// 	]
	// This function will construct mSelectNameMap. Its key
	// is the alias or fname (if alias is not specified).
	// The key must be unique. Otherwise, it is an error.
	map<OmnString, AosDataFieldObjPtr>::iterator itr;
	OmnString value_str = "";
	OmnString field_name = "";
	AosExprObjPtr expr = NULL, value_expr = NULL;
	bool isAgrFunc = false;
	OmnString funcName = "";
	AosExprGenFunc* funcExpr = NULL;
	AosDataFieldObjPtr field = NULL;
	int len = 0;
	AosDataFieldType::E type = AosDataFieldType::eInvalid;
	OmnString str_type = "";
	for(size_t i = 0; i < fields.size(); i++)
	{
		//fname(value)
		value_str = fields[i]["fname"].asString();
		aos_assert_r(value_str != "", false);

		//alias
		field_name = fields[i]["alias"].asString();
		if (field_name == "") field_name = value_str;
		aos_assert_r(field_name != "", false);

		//check duplicate field name
		if (mSelectNameMap.count(field_name))
		{
			AosLogError(rdata, true, "already_have_the_field")
				<< AosFN("Field Name") << field_name << enderr;
			return false;
		}
		mSelectNameMap.insert(make_pair(field_name, 1));
		
		//convert value to expr
		expr = convertToExpr(value_str, rdata);
		aos_assert_r(expr, false);
		value_expr = expr;

		isAgrFunc = false;
		funcName = "";
		if(AosExpr::checkIsAgrFunc(expr))
		{
			// It is an aggregation function
			isAgrFunc = true;
			funcExpr = dynamic_cast<AosExprGenFunc*>(expr.getPtr());       
			aos_assert_r(funcExpr, false);
			aos_assert_r(funcExpr->getParmList()->size() > 0, false);                 

			value_expr = (*(funcExpr->getParmList()))[0];                     
			aos_assert_r(value_expr, false);

			funcName = funcExpr->getFuctName();
			aos_assert_r(funcName != "", false);
		}

		//get len 
		len = fields[i]["max_length"].asInt();
		if (len == 0)
		{
			itr = mInputRecordsMap.find(value_expr->dumpByNoEscape());
			if (itr != mInputRecordsMap.end())
			{
				field = itr->second;
				aos_assert_r(field, false);
				len = field->mFieldInfo.field_data_len;
			}
			else len = -1;
		}

		//get type
		str_type = fields[i]["type"].asString();
		if (str_type != "")
		{
			type = convertToDataFieldType(rdata.getPtr(), str_type);
			aos_assert_r(type != AosDataFieldType::eInvalid, false);
		}
		else
		{
			type = convertToDataFieldType(rdata.getPtr(), value_expr, mInputRecords[0]);
			aos_assert_r(type != AosDataFieldType::eInvalid, false);
		}
		
		resetDataFieldLen(type, len);

		mFields.push_back(SelectField(field_name, value_expr, type, len, isAgrFunc, funcName));
		if (mStatus == AosDataProcSelect::eDistinct)
		{
			mGroupByKeys.push_back(value_expr);
			mRawGroupByKeys.push_back(value_expr.getPtr());
		}
	}
	mFieldsSize = mFields.size();
	mGroupByKeysSize = mGroupByKeys.size();
	return true;
}

bool
AosDataProcSelect::configKeyFields(
		const JSONValue &keys,
		const JSONValue &orderby_types,
		const AosRundataPtr &rdata)
{
	// This function configures the group-by fields. In the current
	// implementations, group-by fields must be individual fields.
	// It is configured as:
	//		"groupby_fields":["key_field2", "key_field3"],
	OmnString name = "";
	AosExprObjPtr expr = NULL;
	for (size_t i = 0; i < keys.size(); i++)
	{
		name = keys[i].asString();
		if (name != "")
		{
			//check key field is valid
			if (!mSelectNameMap.count(name))
			{
				if (!mInputRecordsMap.count(name))
				{
					AosLogError(rdata, true, "groupby_field_not_found")
						<< AosFN("Field Name") << name << enderr;
					return false;
				}
				expr = convertToExpr(name, rdata);
				aos_assert_r(expr, false);

				mKeyFields.push_back(expr);
				mRawKeyFields.push_back(expr.getPtr());
				mGroupByKeys.push_back(expr);
				mRawGroupByKeys.push_back(expr.getPtr());
				continue;
			}

			expr = convertToExpr(name, rdata);
			aos_assert_r(expr, false);

			mGroupByKeys.push_back(expr);
			mRawGroupByKeys.push_back(expr.getPtr());
		}
	}
	mKeyFieldsSize = mKeyFields.size();
	mGroupByKeysSize = mGroupByKeys.size();

	//Build orderby type map(keyname->asc|desc)
	bool rslt = buildOrderByTypeMap(keys, orderby_types, rdata);
	aos_assert_r(rslt, false);

	return true;
}


void
AosDataProcSelect::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool
AosDataProcSelect::createOutput(
		const OmnString &dpname,
		const JSONValue &json,
		const AosRundataPtr &rdata)
{
	//output_name
	aos_assert_r(json, false);
	OmnString name = "";
	bool rslt;
	json["output_name"].asString() != "" ?
		name = json["output_name"].asString():
		name << dpname << "_output";   

	OmnString record_type = json["record_type"].asString();
	if (record_type == "")
		record_type = AOSRECORDTYPE_FIXBIN;
	AosDataRecordType::E type = AosDataRecordType::toEnum(record_type);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);
	if(!mIsStreaming)
	{
		rslt = createOutputShuffle(json, output, rdata);
		aos_assert_r(rslt, false);
	}
	else
		mStatus = AosDataProcSelect::eNorm;

	//set fields
	for (size_t i = 0; i < mFieldsSize; i++)
	{
		AosDataFieldType::E fieldType = mFields[i].mType;
		aos_assert_r(fieldType != AosDataFieldType::eInvalid, false);

		output->setField(mFields[i].mName, fieldType, mFields[i].mLen);
		output->setCmpFun("custom");
		if (mFields[i].mIsAgrFunc)
			output->setAggrField(mFields[i].mName, mFields[i].mFuncName);
	}

	if(mIsStreaming)
	{
		//for docid
		OmnString docid = "docid";
		OmnString field_type = "bin_u64";                                
		AosDataFieldType::E fieldType = AosDataFieldType::toEnum(field_type);
		output->setField(docid, fieldType, 8);
	
		//for add comp
		boost::shared_ptr<Output> compOutput = boost::make_shared<Output>(name, type);
		for (size_t i = 0; i < mFields.size(); i++)
		{
			compOutput->setField(mFields[i].mName, mFields[i].mType, mFields[i].mLen);
		}
		AosDataFieldType::E docidType = convertToDataFieldType(rdata.getPtr(), "u64");
		compOutput->setField("docid", docidType, 8);

		compOutput->setCmpFun("custom");
		compOutput->setCmpField(mFields[0].mName);
	//	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
		AosXmlTagPtr datasetXml = compOutput->init2(NULL, rdata);
		datasetXml = datasetXml->getFirstChild();
		aos_assert_r(datasetXml, false);
		AosXmlTagPtr collectXml = datasetXml->getFirstChild();
		aos_assert_r(collectXml, false);
		AosXmlTagPtr asmXml = collectXml->getFirstChild();
		aos_assert_r(asmXml, false);
		AosXmlTagPtr compDoc = asmXml->getFirstChild("CompareFun");
		aos_assert_r(compDoc, false);
		
		//addComp("output", compDoc);
	}

	switch (mStatus)
	{
		case AosDataProcSelect::eNorm:
			break;

		case AosDataProcSelect::eDistinct:
			for (size_t i = 0; i < mFieldsSize; i++)
			{
				output->setCmpField(mFields[i].mName);
			}
			output->setAggrField(mFields[0].mName, "set");
			break;

		case AosDataProcSelect::eOrderBy:
		case AosDataProcSelect::eGroupBy:
			rslt = createOutputKey(output, rdata);
			aos_assert_r(rslt, false);
			break;

		default:
			 OmnAlarm << "missing select status:" << mStatus << enderr;
			 return false;
	}

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mRawOutputRecord = mOutputRecord.getPtr();
	if(mIsStreaming)
	{
		AosXmlTagPtr xml = mOutputRecord->getRecordDoc();
		AosXmlTagPtr recordXml = xml->getFirstChild("datarecord");
		aos_assert_r(recordXml, false);
		//create record
		AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(xml, mTask, rdata.getPtr() AosMemoryCheckerArgs);

		AosDataRecordObjPtr record_docid = AosDataRecordObj::createDataRecordStatic(recordXml, mTask, rdata.getPtr() AosMemoryCheckerArgs);

		mOutputRecordDocid = record_docid;
		mOutputRecord = record;
		mRawOutputRecord = mOutputRecord.getPtr();
	}
	mOutputs.push_back(output);

	if(mIsStreaming && mRecordType == "fixbin")
	{	
		OmnString dr_name;
		dr_name = name;
		//recordctnr
		boost::shared_ptr<AosConf::DataRecordCtnr> recordCtnr = boost::make_shared<AosConf::DataRecordCtnr>();
		recordCtnr->setAttribute("zky_name", dr_name);

		//datarecord
		u32 offset = 0;
		boost::shared_ptr<AosConf::DataRecordFixbin> dr = boost::make_shared<AosConf::DataRecordFixbin>();
		dr->setAttribute("zky_name", dr_name);
		dr->setAttribute("type", "fixbin");


		for(u32 i=0; i<mFields.size(); i++)
		{
			boost::shared_ptr<AosConf::DataField> keyfield = boost::make_shared<AosConf::DataField>();
			keyfield->setAttribute("zky_name", mFields[i].mName);
			keyfield->setAttribute("type", "str");
			keyfield->setAttribute("zky_datatooshortplc", "cstr");
			keyfield->setAttribute("zky_offset", offset);
			keyfield->setAttribute("zky_length", mFields[i].mLen);

			dr->setField(keyfield);
			offset += mFields[i].mLen;
		}

		//for add docid
		boost::shared_ptr<AosConf::DataField> fieldDocid = boost::make_shared<AosConf::DataField>();	
		fieldDocid->setAttribute("zky_name", "docid");
		fieldDocid->setAttribute("type", "bin_u64");
		fieldDocid->setAttribute("zky_datatooshortplc", "cstr");
		fieldDocid->setAttribute("zky_offset", offset);
		fieldDocid->setAttribute("zky_length", 8);

		dr->setField(fieldDocid);

		recordCtnr->setRecord(dr);                                                 
		string dr_str = recordCtnr->getConfig();
		AosXmlTagPtr xml = AosStr2Xml(rdata.getPtr(), dr_str AosMemoryCheckerArgs);

		AosXmlTagPtr recordXml = xml->getFirstChild("datarecord");
		aos_assert_r(recordXml, false);
		//create record
		AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(xml, mTask, rdata.getPtr() AosMemoryCheckerArgs);

		AosDataRecordObjPtr record_docid = AosDataRecordObj::createDataRecordStatic(recordXml, mTask, rdata.getPtr() AosMemoryCheckerArgs);

		mOutputRecordDocid = record_docid;
		mOutputRecord = record;
		mRawOutputRecord = mOutputRecord.getPtr();
	}


	//if assign docid is needed, add docid field to be the last 
	//field of the output record
	/*
	   if (mAssignDocid && isStreaming())
	   {
	   output->setField("docid", AosDataFieldType::eBinU64, 8);
	   }
   */

	return true;
}


bool
AosDataProcSelect::createOutputShuffle(
		const JSONValue &json,
		const boost::shared_ptr<Output> &output,
		const AosRundataPtr &rdata)
{
	OmnString shuffle_type, shuffle_field, datacol_type, child_type;
	shuffle_type = json["shuffle_type"].asString();
	shuffle_field = json["shuffle_field"].asString();

	//Jozhi 2015-05-04 
	OmnString schema = json["schema"].asString();
	if (schema != "")
	{
		output->setDataColAttr("schema", schema);
	}

	if (mStatus == AosDataProcSelect::eNorm && mGroupByKeysSize == 0)
	{
		if (schema != "")
		{
			datacol_type = "sorted_file";
			//output->setUseCmp(true);
		}
		else
		{
			datacol_type = "file";
			//output->setUseCmp(false);
		}
	}
	else 
	{
		datacol_type = "sorted_file";
		//output->setUseCmp(true);
	}

	if (shuffle_type != "")
	{
		output->setAssemblerAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		output->setAssemblerAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		output->setAssemblerAttr("type", "group");

		output->setDataColAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		output->setDataColAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		output->setDataColAttr("zky_type", "group");
		output->setDataColAttr("zky_child_type", "sorted_file");

	}
	return true;
}


bool
AosDataProcSelect::createOutputKey(
		const boost::shared_ptr<Output> &output,
		const AosRundataPtr &rdata)

{
	aos_assert_r(mGroupByKeysSize > 0, false);

	int len = 0;
	OmnString fieldname = "";
	AosDataFieldObjPtr field = NULL;
	AosDataFieldType::E fieldType;
	OmnString cmp_reserve = "false";
	map<OmnString, AosDataFieldObjPtr>::iterator itr;
	map<OmnString, OmnString>::iterator itr_orderby;

	for (size_t i = 0; i < mGroupByKeysSize; i++)
	{
		fieldname = mRawGroupByKeys[i]->dumpByNoEscape();
		if (!mSelectNameMap.count(fieldname))
		{
			//set field
			itr = mInputRecordsMap.find(fieldname);
			aos_assert_r(itr != mInputRecordsMap.end(), false);

			field = itr->second;
			aos_assert_r(field, false);

			fieldType = convertToDataFieldType(rdata.getPtr(), mGroupByKeys[i], mInputRecords[0]);
			aos_assert_r(fieldType != AosDataFieldType::eInvalid, false);

			len = field->mFieldInfo.field_data_len;
			output->setField(fieldname, fieldType, len);
		}

		itr_orderby = mOrderByTypeMap.find(fieldname);
		aos_assert_r(itr_orderby != mOrderByTypeMap.end(), false);

		if (itr_orderby->second == "desc") cmp_reserve = "true";
		else cmp_reserve = "false";

		output->setCmpField(fieldname, cmp_reserve);
	}
	//if (mStatus == AosDataProcSelect::eOrderBy)
		//output->setAggrField(mGroupByKeys[0]->dumpByNoEscape(), "set");
	return true;
}


AosDataProcStatus::E
AosDataProcSelect::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	//return AosDataProcStatus::eContinue;
	mProcDataCount++;
	//input record
	AosDataRecordObj *input_record = input_records[0];
	AosDataRecordObjPtr inputRecord = input_record;
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	//dump all the input data from upstream
	DumpRecord(input_record, "Select input is: ");
	//approach 1: finish configuration when get the first 
	//input record
	//if (mIsStreaming && !mConfigDone)
	if (mIsStreaming && !mJson["flag"].asBool())
	{
		mInputRecords.push_back(inputRecord);
		resumConfig(rdata_raw);
		mConfigDone = true;
		mJson["flag"] = true;
	}

	//condition
	bool rslt = false;
	AosValueRslt value_rslt;
	if (mRawCondition)
	{
		mRawCondition->getValue(rdata_raw, input_record, value_rslt);
		rslt = value_rslt.getBool();
		if (!rslt)
		{
			return AosDataProcStatus::eContinue;
		}
	}

	//create output record 
	mRawOutputRecord->clear();

	//set fields
	size_t i = 0;
	bool outofmem = false;
	OmnString value_time = "";
	for (i = 0; i < mFieldsSize; i++)	
	{
		rslt = mFields[i].mRawValue->getValue(rdata_raw, input_record, value_rslt);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		rslt = mRawOutputRecord->setFieldValue(i, value_rslt, outofmem, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
	}

	//set group by or order by fields
	for (size_t j = 0; j < mKeyFieldsSize; j++)	
	{
		rslt = mRawKeyFields[j]->getValue(rdata_raw, input_record, value_rslt);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		rslt = mRawOutputRecord->setFieldValue(i, value_rslt, outofmem, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		i++;
	}

	if (mIsStreaming)
	{
		aos_assert_r(mInputDocid,  AosDataProcStatus::eError);

		AosValueRslt docid_valst;
		rslt = mInputDocid->getValue(rdata_raw, input_record, docid_valst);
		if (!rslt) return  AosDataProcStatus::eError;
		
		rslt = mRawOutputRecord->setFieldValue(
				i, docid_valst, outofmem, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		
		/*
		if (!mInputRecordWithDocid)
		{
			mInputRecordWithDocid = input_record;
		}
		*/

		i++;
	}

	//display all the output data
	DumpRecord(mOutputRecord,"Select output is: ");
	//flush the record to output dataset
	if(!mIsStreaming)
	{
		rslt = flushRecord(output_records, mRawOutputRecord, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
	}
	else
	{
		addOutput(rdata_raw, "output", mOutputRecord); 
	}
	mOutputCount++;
	return AosDataProcStatus::eContinue;
}


bool
AosDataProcSelect::resumConfig(AosRundata *rdata)
{
	// set mNumDocids 
	mNumDocids = 0;
	bool rslt = createInputRecordsMap();  
	aos_assert_r(rslt, false);

	if(mIsStreaming)
	{
		JSONValue orderby_fields = mJson["orderby_fields"];
		JSONValue orderby_types = mJson["order_by_type"];
		if (orderby_fields.size() <= 0)
		{
			//no order by
			mStatus = AosDataProcSelect::eNorm;
		}
		else
		{
			mStatus = AosDataProcSelect::eOrderBy;   

			rslt = configKeyFields(orderby_fields, orderby_types, rdata);
			aos_assert_r(rslt, false);
		}
	}

	//input docid
	OmnString docid_expr = mJson["docid"].asString();
	aos_assert_r(docid_expr != "", false);

	OmnString msg;
	docid_expr << ";";
	mInputDocid = AosParseExpr(docid_expr, msg, rdata);
	if (!mInputDocid)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " 
			<< "" << enderr;
		OmnAlarm << docid_expr << enderr;
		return false;
	}
	mRawInputDocid = mInputDocid.getPtr();

	//condition
	OmnString name = mJson["condition"].asString();
	if (name != "")
	{
		mCondition = convertToExpr(name, rdata);
		if (!mCondition) return false;
	}

	if(!(mJson["fields"].isNull()))
	{
		bool rslt = configFields(mJson["fields"], rdata);
		aos_assert_r(rslt, false);
	}

	if(mIsStreaming && mFields.size()==0)
	{
		OmnString schema_name = "";
		AosXmlTagPtr doc;
		schema_name = mJson["format"].asString("");
		if (schema_name != "")
		{
			doc = AosJqlStatement::getDoc(
				rdata, JQLTypes::eDataRecordDoc, schema_name);
		}
		aos_assert_r(doc, false);

		//AosDataRecordObjPtr dataRecord = mInputRecords[0];
		//AosXmlTagPtr recordXml = dataRecord->getRecordDoc();
		AosXmlTagPtr recordXml = doc;
		aos_assert_r(recordXml, false);
		AosXmlTagPtr dataFieldsXml = recordXml->getFirstChild("datafields");
		aos_assert_r(dataFieldsXml, false);
		AosXmlTagPtr dataFieldXml = dataFieldsXml->getFirstChild();
		aos_assert_r(dataFieldXml, false);

		int len = 0;
		AosDataFieldType::E type;
		OmnString str_type, value_str;
		AosExprObjPtr expr;
		while (dataFieldXml)
		{
			name = dataFieldXml->getAttrStr("zky_name");
			len = dataFieldXml->getAttrInt("zky_length", 0);
			str_type = dataFieldXml->getAttrStr("data_type");
			if(str_type == "varchar")
				str_type = "str";
			if(str_type == "")
				str_type = dataFieldXml->getAttrStr("type");
			type = convertToDataFieldType(rdata, str_type);

			value_str = name;                         
			expr = convertToExpr(value_str, rdata);

			mFields.push_back(SelectField(name, expr, type, len));      

			dataFieldXml = dataFieldsXml->getNextChild();
		}
	}

	mFieldsSize = mFields.size();
	//create output
	rslt = createOutput(mName, mJson, rdata);
	aos_assert_r(rslt, false);
	return true;
}


AosJimoPtr
AosDataProcSelect::cloneJimo() const
{
	return OmnNew AosDataProcSelect(*this);
}


AosDataProcObjPtr                                           
AosDataProcSelect::cloneProc()
{
	return OmnNew AosDataProcSelect(*this);
}


bool 
AosDataProcSelect::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
//format
	if(mIsStreaming)
	{
		JSONValue jsonObj;
		JSONReader reader;
		OmnString schema_name;

		bool rslt = reader.parse(jsonstr, jsonObj);
		aos_assert_r(rslt, false);
		
		AosXmlTagPtr doc;
		schema_name = jsonObj["format"].asString("");
		if (schema_name != "")
		{
			doc = AosJqlStatement::getDoc(
				rdata, JQLTypes::eDataRecordDoc, schema_name);
		}
		aos_assert_r(doc, false);
		OmnString type, name, strLen;
		AosXmlTagPtr dataFieldsXml = doc->getFirstChild("datafields"); 
		aos_assert_r(dataFieldsXml, false);                                  
		AosXmlTagPtr dataFieldXml = dataFieldsXml->getFirstChild();          
		aos_assert_r(dataFieldXml, false);
		keyNameList.clear();
		keyTypeList.clear();
		keyLenList.clear();
		while (dataFieldXml)
		{
			name = dataFieldXml->getAttrStr("zky_name"); 
			type = dataFieldXml->getAttrStr("type"); 
			strLen = dataFieldXml->getAttrStr("zky_length");
			
			keyTypeList.push_back(type);
			keyNameList.push_back(name);
			keyLenList.push_back(strLen.toInt64());

			dataFieldXml = dataFieldsXml->getNextChild();
		}
	}
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dpconf = "";	
	dpconf 
		<< "<jimodataproc " << AOSTAG_CTNR_PUBLIC "=\"true\" " << AOSTAG_PUBLIC_DOC "=\"true\" "
		<< 	AOSTAG_OBJID << "=\"" << objid << "\">"
		<< 		"<dataproc zky_name=\"" << dpname << "\" jimo_objid=\"dataprocselect_jimodoc_v0\">"
		<< 			"<![CDATA[" << jsonstr << "]]>"
		<< 		"</dataproc>"
		<< "</jimodataproc>";
	prog->saveLogicDoc(rdata, objid, dpconf);
	return true;
}
	

bool
AosDataProcSelect::finish(const AosRundataPtr &rdata)
{
	if (mOutputRecord)
	{
		mOutputRecord->clear();
	}
	if (mOutputRecordDocid)
	{
		mOutputRecordDocid->clear();
	}
	if (mInputDocid)
	{
		mInputDocid = mInputDocid->cloneExpr();
		mRawInputDocid = mInputDocid.getPtr();
	}
	mInputRecords.clear();
	mInputRecordsMap.clear();
	return true;
}

bool
AosDataProcSelect::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
	}
	OmnScreen << "DataProcSelect" << "(" << mName << ")" << "finished:" 
				<< ", procDataCount:" << procDataCount 
				<< ", OutputCount:" << procOutputCount << endl;
	showDataProcInfo(__FILE__, __LINE__, procDataCount, procOutputCount);
	showDataProcInfo(__FILE__, __LINE__, "finish", "DataProcSelect");
	return true;
}

bool 
AosDataProcSelect::start(const AosRundataPtr &rdata)
{
	showDataProcInfo(__FILE__, __LINE__, "start", "DataProcSelect");
	return true;
}

