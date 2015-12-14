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
#include "JimoDataProc/DataProcRankOver.h"

#include "API/AosApi.h"
#include <boost/make_shared.hpp>
#include "JSON/JSON.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataProcRankOver_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcRankOver(version);
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


AosDataProcRankOver::AosDataProcRankOver(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcRankOver)
{
}


AosDataProcRankOver::AosDataProcRankOver(const AosDataProcRankOver &proc)
:
AosJimoDataProc(proc)
{
	if (proc.mOutputRecord)
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);

	if (!proc.mInputRecords.empty())
		mInputRecords= proc.mInputRecords;

	AosExprObjPtr expr;
	for (size_t i = 0; i < proc.mFields.size(); i++)
	{
		expr = proc.mFields[i]->cloneExpr();
		mFields.push_back(expr);
	}
	for (size_t i = 0; i < proc.mOrderByFields.size(); ++i)
	{
		expr = proc.mOrderByFields[i]->cloneExpr();
		mOrderByFields.push_back(expr);
	}
	for (size_t i = 0; i < proc.mPartitionByFields.size(); ++i)
	{
		expr = proc.mPartitionByFields[i]->cloneExpr();
		mPartitionByFields.push_back(expr);
	}
	if (proc.mTotalRank)
		mTotalRank = proc.mTotalRank;
	mInputRecordsMap = proc.mInputRecordsMap;
	mPartitionRankMap = proc.mPartitionRankMap;
	mOrderRankMap = proc.mOrderRankMap;
}


AosDataProcRankOver::~AosDataProcRankOver()
{
}


bool
AosDataProcRankOver::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
		//<dataproc zky_name = "dp2"><![CDATA[
		//{
		//"type":"rankover",
		//"record_type":"fixbin|buff",
		//"order_by": ["key_field3"],
		//"order_by_type":"desc",
		//"partition_by": ["key_field1","key_field2"],
		//"name":"aa"
		//};
	aos_assert_r(def, false);
	mName = def->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mName != "", false);
	OmnString dp_json = def->getNodeText();

	mInputRecordsMap.clear();
	mPartitionRankMap.clear();
	mOrderRankMap.clear();
	mTotalRank = 1;

	//json_str to JSONValue
	JSONValue json, fields;
	JSONReader reader;
	bool rslt = false;
	rslt = reader.parse(dp_json, json);
	aos_assert_r(rslt, false);

	//createmap fieldname -> AosDataFieldObjPtr 
	rslt = createInputRecordsMap(rdata);  
	aos_assert_r(rslt, false);

	//order by fields
	fields = json["order_by"];
	aos_assert_r(fields.size() > 0, false);
	rslt = configOrderByFields(fields, rdata);
	aos_assert_r(rslt, false);

	//partition bt fields
	fields = json["partition_by"];
	rslt = configPartitionByFields(fields, rdata);
	aos_assert_r(rslt, false);

	//create output
	rslt = createOutput(mName, json, rdata);
	aos_assert_r(rslt, false);

	return true;
}

	
bool
AosDataProcRankOver::configOrderByFields(
		const JSONValue &fields,
		const AosRundataPtr &rdata)
{
	OmnString exprStr, msg;
	AosExprObjPtr expr;
	for (size_t i = 0; i < fields.size(); i++)
	{
		exprStr = fields[i].asString();
		//mOrderByMap[exprStr] = exprStr;
		exprStr << ";";

		expr = AosParseExpr(exprStr, msg, rdata.getPtr());
		if (!expr)                         
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			OmnAlarm << exprStr << enderr;
			return false;
		}
		mOrderByFields.push_back(expr);
	}

	return true;
}


bool
AosDataProcRankOver::configPartitionByFields(
		const JSONValue &fields,
		const AosRundataPtr &rdata)
{
	OmnString exprStr, msg;
	AosExprObjPtr expr;
	for (size_t i = 0; i < fields.size(); i++)
	{
		exprStr = fields[i].asString();
		exprStr << ";";

		expr = AosParseExpr(exprStr, msg, rdata.getPtr());
		if (!expr)                         
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			OmnAlarm << exprStr << enderr;
			return false;
		}
		mPartitionByFields.push_back(expr);
	}

	return true;
}


bool
AosDataProcRankOver::createInputRecordsMap(
		const AosRundataPtr &rdata)
{
	int offset = 0, len = 0;
	OmnString name, type, msg;
	AosExprObjPtr expr;
	map<OmnString, AosDataFieldObjPtr>::iterator itr;

	aos_assert_r(mInputRecords.size() > 0, false);
	AosDataRecordObjPtr inputrcd = mInputRecords[0];
	aos_assert_r(inputrcd, false);

	vector<AosDataFieldObjPtr> input_fields = inputrcd->getFields();
	for(size_t i = 0; i < input_fields.size(); i++)
	{
		name = input_fields[i]->getName();
		type = input_fields[i]->getTypeName();
		len = input_fields[i]->mFieldInfo.field_data_len;
		mInputRecordsMap.insert(make_pair(name, input_fields[i]));

		if (type == "bin_u64") type = "u64";
		AosCompareFun::DataType data_type = AosCompareFun::getDataType(type);
		aos_assert_r(data_type != AosCompareFun::eInvalidDataType, false);
		
		//mFieldInfoMap[name] = FieldInfo(type, offset);

		name << ";";
		expr = AosParseExpr(name, msg, rdata.getPtr());
		if (!expr)                         
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			return false;
		}
		mFields.push_back(expr);
		offset += len;
	}
	return true;
}


void
AosDataProcRankOver::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool
AosDataProcRankOver::createOutput(
		const OmnString &dpname,
		const JSONValue &json,
		const AosRundataPtr &rdata)
{	
	//size_t i = 0;
	int len = 0;
	OmnString fieldname;

	AosDataFieldObjPtr field;
	map<OmnString, AosDataFieldObjPtr>::iterator itr;
	map<OmnString, FieldInfo>::iterator itr_info;
	boost::shared_ptr<AosConf::DataField> df;

	OmnString name = "";
	name << dpname << "_output";

//	OmnString orderByType = json["order_by_type"].asString();
//	if ((orderByType != "asc" && orderByType != "desc") || orderByType == "") orderByType = "asc";

	OmnString type_str = json["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}
	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);

	AosDataFieldType::E fieldType;
	for (itr = mInputRecordsMap.begin(); 
			itr != mInputRecordsMap.end(); ++itr)
	{
		field = itr->second;
		aos_assert_r(field, false);

		fieldname = field->getName();
		len = field->mFieldInfo.field_data_len;
		type_str = field->getTypeName();
		fieldType = AosDataFieldType::toEnum(type_str);

		output->setField(fieldname, fieldType, len);
	}
	fieldname = json["name"].asString();
	output->setField(fieldname, AosDataFieldType::eBinU64, sizeof(u64));

	//output->setUseCmp(false);
	//output->setAssemblerAttr("zky_type", "file");

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);

	return true;
}


AosDataProcStatus::E
AosDataProcRankOver::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;

	u64 rank = 1;
	size_t i = 0, j = 0;
	bool rslt = false;
	bool outofmem = false;
	OmnString par_values = "", order_values;
	map<OmnString, u64>::iterator itr;
	AosValueRslt value_rslt;
	AosDataRecordObj *input_record, *output_record;

	//input record
	input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	//create output record 
	mOutputRecord->clear();
	output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, AosDataProcStatus::eError);

	//set field to output
	for (i = 0; i < mFields.size(); i++)	
	{
		rslt = mFields[i]->getValue(rdata_raw, input_record, value_rslt);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		rslt = output_record->setFieldValue(i, value_rslt, outofmem, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
	}

	//set rank field to output
	//get orderbyfield values
	for (j = 0; j < mOrderByFields.size(); j++)
	{
		rslt = mOrderByFields[j]->getValue(rdata_raw, input_record, value_rslt);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		order_values << value_rslt.getStr();
	}

	if (mPartitionByFields.size() == 0)
	{
		itr = mOrderRankMap.find(order_values);

		if (itr == mOrderRankMap.end())
		{
			value_rslt.setU64(mTotalRank);
			mOrderRankMap[order_values] = mTotalRank;
		}
		else
		{
			rank = itr->second;
			value_rslt.setU64(rank);
		}
		mTotalRank++;
	}
	else
	{
		for (j = 0; j < mPartitionByFields.size(); j++)
		{
			rslt = mPartitionByFields[j]->getValue(rdata_raw, input_record, value_rslt);
			aos_assert_r(rslt, AosDataProcStatus::eError);

			par_values << value_rslt.getStr();
		}

		itr = mPartitionRankMap.find(par_values);
		if (itr == mPartitionRankMap.end())
		{
			mTotalRank = 1;
			mOrderRankMap.clear();
			value_rslt.setU64(1);
			mPartitionRankMap[par_values] = 1;
			mOrderRankMap[order_values] = 1;
			mTotalRank++;
		}
		else
		{
			rank = itr->second;
			itr = mOrderRankMap.find(order_values);
			if (itr == mOrderRankMap.end()) rank = mTotalRank;
			value_rslt.setU64(rank);
			mPartitionRankMap[par_values] = rank;
			mOrderRankMap[order_values] = 1;
			mTotalRank++;
		}
	}

	rslt = output_record->setFieldValue(i, value_rslt, outofmem, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	rslt = flushRecord(output_records, mOutputRecord.getPtr(), rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	//mOutputRecord->flushRecord(rdata_raw);
	mOutputCount++;

	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosDataProcRankOver::cloneJimo() const
{
	return OmnNew AosDataProcRankOver(*this);
}


AosDataProcObjPtr
AosDataProcRankOver::cloneProc() 
{
	return OmnNew AosDataProcRankOver(*this);
}



bool 
AosDataProcRankOver::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dpconf = "";	
	dpconf 
		<< "<jimodataproc " << AOSTAG_CTNR_PUBLIC "=\"true\" " << AOSTAG_PUBLIC_DOC "=\"true\" "
		<< 	AOSTAG_OBJID << "=\"" << objid << "\">"
		<< 		"<dataproc zky_name=\"" << dpname << "\" jimo_objid=\"dataprocrankover_jimodoc_v0\">"
		<< 			"<![CDATA[" << jsonstr << "]]>"
		<< 		"</dataproc>"
		<< "</jimodataproc>";
	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dpconf);
	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, objid);
	//return AosCreateDoc(dp_xml->toString(), true, rdata);
	
	prog->saveLogicDoc(rdata, objid, dpconf);
	return true;
}
	
	
bool
AosDataProcRankOver::finish(
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
	OmnScreen << "DataProcRankOver " << "(" << mName << ")" << " finished:" 
				<< " procDataCount:" << procDataCount 
				<< ", OutputCount:" << procOutputCount << endl;
	return true;
}
