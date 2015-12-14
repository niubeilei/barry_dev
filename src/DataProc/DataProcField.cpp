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
// This data proc uses one data element to convert/process one data field.
//
//
// Modification History:
// 04/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcField.h"


AosDataProcField::AosDataProcField(const bool flag)
:
AosDataProc(AOSDATAPROC_FIELD, AosDataProcId::eField, flag),
mKeyFieldIdx(-1)
{
}

	
AosDataProcField::AosDataProcField(const AosDataProcField &proc)
:
AosDataProc(proc),
mKeyFieldName(proc.mKeyFieldName),
mKeyFieldIdx(proc.mKeyFieldIdx),
mColComp(proc.mColComp)
{
}

AosDataProcField::~AosDataProcField()
{
}


bool
AosDataProcField::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	if(isVersion1(def))
	{
		// Ketty 2014/01/11
		return configV1(def, rdata);	
	}

	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);

	mKeyFieldName = def->getAttrStr(AOSTAG_KEYFIELDNAME);
	aos_assert_r(mKeyFieldName != "", false);

	mNeedDocid = true;
	AosXmlTagPtr tag = def->getFirstChild(AOSTAG_COLUMNCOMPRESSION);
	if (tag)
	{
		mColComp = AosDataColComp::createDataColComp(tag, rdata);
		aos_assert_r(mColComp, false);
	}
	return true;
}


AosDataProcStatus::E
AosDataProcField::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	OmnAlarm  << enderr;
	/*
	// This function reads in a record pointed to by 'record', 
	// creates the IILs as needed, and then creates a new record
	// based on 'target_formatter'. The new record is appended 
	// to 'buff'.
	
	aos_assert_rr(record, rdata, AosDataProcStatus::eError);

	// 1. Retrieve the DataElement
	AosValueRslt key_rslt;
	bool rslt = record->getFieldValue(mKeyFieldIdx, key_rslt, false, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);

	int len;
	const char *data = key_rslt.getCharStr(len);
	aos_assert_r(data, AosDataProcStatus::eError);
	aos_assert_r(len > 0, AosDataProcStatus::eError);

	aos_assert_r(mColComp, AosDataProcStatus::eError);
	AosValueRslt valueRslt;
	AosDataType::E data_type;
	rslt = mColComp->convertToInteger(data, len, valueRslt, data_type, rdata);
	if (!rslt)  return AosDataProcStatus::eError;

	u64 vv = 0;
	rslt = valueRslt.getU64();
	aos_assert_r(rslt,  AosDataProcStatus::eError);

	rslt = AosDataType::isValid(data_type);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	len = AosDataType::getValueSize(data_type); 

	valueRslt.reset();
	valueRslt.setCharStr1((char *)&vv, len, true);

	aos_assert_r(output, AosDataProcStatus::eContinue);
	if (mNeedConvert)
	{
		if (!output) return AosDataProcStatus::eError;
		bool outofmem = false;
		rslt = output->setFieldValue(mConvertName, valueRslt, outofmem, rdata.getPtrNoLock());
		if (!rslt) return AosDataProcStatus::eError;
	}
	
	return AosDataProcStatus::eContinue;
	*/
}


AosDataProcObjPtr
AosDataProcField::cloneProc()
{
	return OmnNew AosDataProcField(*this);
}


AosDataProcObjPtr
AosDataProcField::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcField * proc = OmnNew AosDataProcField(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}

bool
AosDataProcField::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	
	mKeyFieldIdx = record->getFieldIdx(mKeyFieldName, rdata.getPtrNoLock());
	aos_assert_r(mKeyFieldIdx >= 0, false);
	
	return true;
}


bool
AosDataProcField::configV1(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr fields = def->getFirstChild("fields");
	aos_assert_r(fields, false);
	
	AosXmlTagPtr field_xml = fields->getFirstChild();
	aos_assert_r(field_xml, false);
	
	OmnString input_name_field = field_xml->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(input_name_field != "", false);
		
	OmnString output_name_field = field_xml->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
	aos_assert_r(output_name_field != "", false);
		
	mField.mInput.init(input_name_field, true);
	mField.mOutput.init(output_name_field, false);

	//AosXmlTagPtr tag = def->getFirstChild(AOSTAG_COLUMNCOMPRESSION);
	//if (tag)
	//{
	//	mColComp = AosDataColComp::createDataColComp(tag, rdata);
	//	aos_assert_r(mColComp, false);
	//}
	return true;
}

bool
AosDataProcField::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	rf_infos.push_back(&(mField.mInput));
	rf_infos.push_back(&(mField.mOutput));
	return true;
}

AosDataProcStatus::E
AosDataProcField::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	// Ketty 2013/12/27
	AosDataRecordObj * from_record = getRecord(mField.mInput, input_records, output_records);
	aos_assert_r(from_record, AosDataProcStatus::eError);	
	
	AosDataRecordObj * to_record = getRecord(mField.mOutput, input_records, output_records);
	aos_assert_r(to_record, AosDataProcStatus::eError);	
	
	AosValueRslt key_rslt;
	bool rslt = from_record->getFieldValue(mField.mInput.mFieldIdx, key_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	bool outofmem = false;
	rslt = to_record->setFieldValue(mField.mOutput.mFieldIdx, key_rslt, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;
	
	return AosDataProcStatus::eContinue;
}


