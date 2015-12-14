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
// 2015/07/03 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcSelectR1R2.h"
#include "DataProc/DataProc.h"

#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoDataProcSelectR1R2_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoDataProcSelectR1R2(version);
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


AosJimoDataProcSelectR1R2::AosJimoDataProcSelectR1R2(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcIf),
mData(0)
{
}


AosJimoDataProcSelectR1R2::AosJimoDataProcSelectR1R2(const AosJimoDataProcSelectR1R2 &proc)
:
AosJimoDataProc(proc),
mData(0)
{
	for (size_t i = 0; i < proc.mInputRecords.size(); i++)
	{
		if (proc.mInputRecords[i])
		{
			mInputRecords.push_back(proc.mInputRecords[i]->clone(0 AosMemoryCheckerArgs));
		}
	}

	if (proc.mOutputRecord)
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);

	if (proc.mDataRecord)
		mDataRecord = proc.mDataRecord->clone(0 AosMemoryCheckerArgs);
	mData = OmnNew char[1000];
}


AosJimoDataProcSelectR1R2::~AosJimoDataProcSelectR1R2()
{
	OmnDelete [] mData;
	mData = 0;
}


void
AosJimoDataProcSelectR1R2::setInputDataRecords(
		vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool
AosJimoDataProcSelectR1R2::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//	{
	//    "type":"selectr1r2",   
	//    "record_type":"fixbin|buff"
	//  }
	//]]></data_proc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);

	OmnString dp_jsonstr = def->getNodeText();
	mName = def->getAttrStr("zky_name", "");
	aos_assert_r(mName != "", false);

	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_jsonstr, json);
	aos_assert_r(rslt, false);

	rslt = createOutput(mName, json, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosJimoDataProcSelectR1R2::createOutput(
		const OmnString &dpname,
		const JSONValue &json,
		const AosRundataPtr &rdata)
{
	AosDataRecordObjPtr inputrcd = mInputRecords[0];
	aos_assert_r(inputrcd, false); 

	OmnString type_str = json["record_type"].asString(AOSRECORDTYPE_FIXBIN);
	AosDataRecordType::E record_type = AosDataRecordType::toEnum(type_str);
	aos_assert_r(record_type != AosDataRecordType::eInvalid, false);


	OmnString name = "";
	json["output_name"].asString() != "" ?
		name = json["output_name"].asString():
		name << dpname << "_output";

	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, record_type);

	OmnString fieldName;
	int fieldLen;
	AosDataFieldType::E fieldType;
	vector<AosDataFieldObjPtr> fields = inputrcd->getFields();
	size_t j = 0;
	for (size_t i = 0; i < fields.size()*2; i++)
	{
		if (i < fields.size())
		{
			fieldName = "r1_";
			j = i;
		}
		else
		{
			fieldName = "r2_";
			j = i - fields.size();
		}
		OmnString name = fields[j]->getName();
		fieldName << name;
		fieldLen = fields[j]->mFieldInfo.field_data_len;

		AosExprObjPtr expr = convertToExpr(name, rdata);
		aos_assert_r(expr, false);
		fieldType = convertToDataFieldType(rdata.getPtr(), expr, mInputRecords[0]);
		aos_assert_r(fieldType != AosDataFieldType::eInvalid, false);

		output->setField(fieldName, fieldType, fieldLen);
		//output->setCmpField(fieldName);
	}

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);
	return true;
}


AosDataProcStatus::E
AosJimoDataProcSelectR1R2::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;

	AosDataRecordObj * input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);

	int len = 0;
	char *data = 0;
	bool rslt = false, outofmem = false;
	if (mProcDataCount == 1)
	{
		mDataRecord = input_record->clone(0 AosMemoryCheckerArgs);
		data = input_record->getData(rdata_raw);
		len = input_record->getMemoryLen();
		aos_assert_r(len <= 1000, AosDataProcStatus::eError);
		memcpy(mData, data, len);
		mDataRecord->setMemory(mData, len);
	}
	else
	{
		mOutputRecord->clear();
		AosDataRecordObj *output_record = mOutputRecord.getPtr();
		aos_assert_r(output_record, AosDataProcStatus::eError);

		vector<AosDataFieldObjPtr> fields = input_record->getFields();
		//set value
		AosValueRslt value_rslt;
		aos_assert_r(mDataRecord, AosDataProcStatus::eError);
		for (size_t i = 0; i < fields.size(); i++)	
		{
			rslt = mDataRecord->getFieldValue(i, value_rslt, false, rdata_raw);
			aos_assert_r(rslt, AosDataProcStatus::eError);

			rslt = output_record->setFieldValue(i, value_rslt, outofmem, rdata_raw);
			aos_assert_r(rslt, AosDataProcStatus::eError);
		}
		for (size_t i = 0; i < fields.size(); i++)	
		{
			rslt = input_record->getFieldValue(i, value_rslt, false, rdata_raw);
			aos_assert_r(rslt, AosDataProcStatus::eError);

			rslt = output_record->setFieldValue(i + fields.size(), value_rslt, outofmem, rdata_raw);
			aos_assert_r(rslt, AosDataProcStatus::eError);
		}

		data = input_record->getData(rdata_raw);
		len = input_record->getMemoryLen();
		aos_assert_r(len <= 1000, AosDataProcStatus::eError);
		memcpy(mData, data, len);

		mDataRecord->clear();
		mDataRecord->setMemory(mData, len);

		rslt = flushRecord(output_records, mOutputRecord.getPtr(), rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
	}

	mOutputCount++;
	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosJimoDataProcSelectR1R2::cloneJimo() const
{
	return OmnNew AosJimoDataProcSelectR1R2(*this);
}


AosDataProcObjPtr
AosJimoDataProcSelectR1R2::cloneProc() 
{
	return OmnNew AosJimoDataProcSelectR1R2(*this);
}


bool 
AosJimoDataProcSelectR1R2::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataprocselect_r1r2_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//aos_assert_r(dp_xml, false);
	//
	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname));
	//return AosCreateDoc(dp_xml->toString(), true, rdata);

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


bool
AosJimoDataProcSelectR1R2::finish(
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
	OmnScreen << "DataProcSelectR1R2" << "(" << mName << ")" << "finished:" 
				<< ", procDataCount:" << procDataCount 
				<< ", OutputCount:" << procOutputCount << endl;
	return true;
}

