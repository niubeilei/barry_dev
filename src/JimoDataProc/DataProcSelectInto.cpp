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
// 2014/12/24 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcSelectInto.h"

#include "API/AosApi.h"
#include "DataProc/DataProc.h"
#include <boost/make_shared.hpp>
#include "JSON/JSON.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataProcSelectInto_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcSelectInto(version);
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


AosDataProcSelectInto::AosDataProcSelectInto(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcSelectInto)
{
}


AosDataProcSelectInto::AosDataProcSelectInto(const AosDataProcSelectInto &proc)
:AosJimoDataProc(proc)
{
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	}

	for (u32 i=0; i<proc.mInputRecords.size(); i++)
	{
		mInputRecords.push_back((proc.mInputRecords[i])->clone(0 AosMemoryCheckerArgs));
	}



}


AosDataProcSelectInto::~AosDataProcSelectInto()
{
}


bool
AosDataProcSelectInto::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	mName = def->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mName != "", false);
	OmnString dp_json = def->getNodeText();
	JSONValue json;
	JSONReader reader;
	reader.parse(dp_json, json);
	bool rslt = createOutput(mName, json, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDataProcSelectInto::createOutput(
					const OmnString &dpname,
					const JSONValue &json_conf,
					const AosRundataPtr &rdata)
{
	OmnString name = "";
	name << dpname << "_output";
	AosDataRecordObjPtr input_record = mInputRecords[0];
	AosXmlTagPtr record_ctnr = input_record->getRecordDoc();
	aos_assert_r(record_ctnr, false);
	AosXmlTagPtr record_tag = record_ctnr->getFirstChild(true);
	aos_assert_r(record_tag, false);
	AosDataRecordType::E type = AosDataRecordType::toEnum(record_tag->getAttrStr("type"));
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);
	output->setRecordConfig(record_tag);
	OmnString file_name = json_conf["filename"].asString("");
	aos_assert_r(file_name != "", false);
	output->setAssemblerAttr("zky_file_name", file_name);
	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);
	return true;
}


AosDataProcStatus::E
AosDataProcSelectInto::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	AosDataRecordObj* input_record = input_records[0];
	bool outofmem = false;
	mOutputRecord->clear();
	vector<AosDataFieldObjPtr> fields = input_record->getFields();
	for (u32 i=0; i<fields.size(); i++)
	{
		AosValueRslt value;
		input_record->getFieldValue(i, value, outofmem, rdata_raw);
		AosDataFieldObj* field = input_record->getFieldByIdx1(i);
		OmnString field_name = field->getName();
		int idx = mOutputRecord->getFieldIdx(field_name, rdata_raw);
		aos_assert_r(idx != -1, AosDataProcStatus::eError);
		mOutputRecord->setFieldValue(idx, value, outofmem, rdata_raw);
	}
	bool rslt = flushRecord(output_records, mOutputRecord.getPtr(), rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosDataProcSelectInto::cloneJimo() const
{
	return  OmnNew AosDataProcSelectInto(*this);
}

AosDataProcObjPtr
AosDataProcSelectInto::cloneProc() 
{
	return  OmnNew AosDataProcSelectInto(*this);
}


bool
AosDataProcSelectInto::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	return true;
}


bool
AosDataProcSelectInto::start(const AosRundataPtr &rdata)
{
	return true;
}

bool
AosDataProcSelectInto::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosDataProcSelectInto::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr)
{
	return false;
}

void
AosDataProcSelectInto::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}
