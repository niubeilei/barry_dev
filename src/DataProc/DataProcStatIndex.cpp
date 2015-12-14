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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcStatIndex.h"

#include "API/AosApi.h"
#include "Util/StrSplit.h"

AosDataProcStatIndex::AosDataProcStatIndex(const bool flag)
:
AosDataProc(AOSDATAPROC_STATINDEX, AosDataProcId::eStatIndex, flag),
mSep("")
{
}
	

AosDataProcStatIndex::AosDataProcStatIndex(const AosDataProcStatIndex &proc)
:
AosDataProc(proc)
{
}


AosDataProcStatIndex::~AosDataProcStatIndex()
{
}


bool
AosDataProcStatIndex::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc proc_id="statindex" zky_sep="0x01">
	//<fields>
	//<keys zky_input_field_name="rcd_unst_system_type_one_epochday.str">
	//<key zky_output_field_name="rcd_unst_system_type_statidx_f1_epochday.str"/>
	//<key zky_output_field_name="rcd_unst_system_type_statidx_f2_epochday.str"/>
	//</keys>
	//<docids zky_input_field_name="rcd_unst_system_type_one_epochday.docid">
	//<docid zky_output_field_name="rcd_unst_system_type_statidx_f1_epochday.docid"/>
	//<docid zky_output_field_name="rcd_unst_system_type_statidx_f2_epochday.docid"/>
	//</docids>
	//</fields>
	//</dataproc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);

	AosXmlTagPtr fields = def->getFirstChild("fields");
	aos_assert_r(fields, false);

	AosXmlTagPtr tag = fields->getFirstChild("keys");
	aos_assert_r(tag, false);
	OmnString input_name_field = tag->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(input_name_field != "", false);
	mKeyInput.init(input_name_field, true);

	OmnString output_name_field;
	AosXmlTagPtr entry = tag->getFirstChild();
	while(entry)
	{
		output_name_field = entry->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
		aos_assert_r(output_name_field != "", false);

		OutputFieldInfo info;
		info.mKeyOutput.init(output_name_field, false);
		mOutputs.push_back(info);

		entry = tag->getNextChild();
	}
	aos_assert_r(mOutputs.size() > 0, false);

	tag = fields->getFirstChild("docids");
	aos_assert_r(tag, false);

	input_name_field = tag->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(input_name_field != "", false);
	mDocidInput.init(input_name_field, true);
		
	int numsubtags = tag->getNumSubtags();
	aos_assert_r(numsubtags > 0 && (u32)numsubtags == mOutputs.size(), false);
	for(int i = 0; i < numsubtags; i++)
	{
		entry = tag->getChild(i);
		aos_assert_r(entry, false);

		output_name_field = entry->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
		aos_assert_r(output_name_field != "", false);
		mOutputs[i].mDocidOutput.init(output_name_field, false);
	}
	
	mSep = def->getAttrStr("zky_sep", "");
	AosConvertAsciiBinary(mSep);
	aos_assert_r(mSep != "", false);
	return true;
}


AosDataProcStatus::E 
AosDataProcStatIndex::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eError;
}


AosDataProcObjPtr
AosDataProcStatIndex::cloneProc()
{
	return OmnNew AosDataProcStatIndex(*this);
}


AosDataProcObjPtr
AosDataProcStatIndex::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcStatIndex * proc = OmnNew AosDataProcStatIndex(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcStatIndex::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataProcStatIndex::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	rf_infos.push_back(&(mKeyInput));
	rf_infos.push_back(&(mDocidInput));
	for (u32 i = 0; i < mOutputs.size(); i++)
	{
		rf_infos.push_back(&(mOutputs[i].mKeyOutput));
		rf_infos.push_back(&(mOutputs[i].mDocidOutput));
	}
	return true;
}


AosDataProcStatus::E
AosDataProcStatIndex::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	//aos_assert_r(mKeyInput.mRecordIdx == mDocidInput.mRecordIdx, AosDataProcStatus::eError);

	//AosDataRecordObj * input_record = input_records[mKeyInput.mRecordIdx];
	AosDataRecordObj * input_record = getRecord(mKeyInput, input_records, output_records);
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	AosValueRslt key_rslt;
	bool rslt = input_record->getFieldValue(mKeyInput.mFieldIdx, key_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	input_record = getRecord(mDocidInput, input_records, output_records);
	AosValueRslt docid_rslt;
	aos_assert_r(input_record, AosDataProcStatus::eError);	
	rslt = input_record->getFieldValue(mDocidInput.mFieldIdx, docid_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	vector<OmnString> str;
	AosDataRecordObj * output_record;
	OmnString key = key_rslt.getStr();
	AosStrSplit::splitStrBySubstr(key.data(), mSep.data(), str, 100);
	aos_assert_r(str.size() > 0 && str.size() == mOutputs.size(), AosDataProcStatus::eError);
	for (u32 i = 0; i < str.size(); i++)
	{
		key_rslt.reset();
		key_rslt.setStr(str[i]);

		output_record = getRecord(mOutputs[i].mKeyOutput, input_records, output_records);
		aos_assert_r(output_record, AosDataProcStatus::eError);	
		
		bool outofmem = false;
		rslt = output_record->setFieldValue(mOutputs[i].mKeyOutput.mFieldIdx, key_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;
	
		outofmem = false;
		rslt = output_record->setFieldValue(mOutputs[i].mDocidOutput.mFieldIdx, docid_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;
	}

	//AosDataRecordObj * output_record = output_records[mKey.mOutput.mRecordIdx];
	//AosDataRecordObj * output_record = getRecord(mKey.mOutput, input_records, output_records);
	//aos_assert_r(output_record, AosDataProcStatus::eError);	
	//
	//OmnString field;
	//OmnString key = key_rslt.getStr();
	//OmnStrParser1 parser(key, mSep);
	//while ((field = parser.nextWord()) != "")
	//{
	//	key_rslt.reset();
	//	key_rslt.setStr(field);
	//
	//  bool outofmem = false;
	//	rslt = output_record->setFieldValue(mKey.mOutput.mFieldIdx, key_rslt, outofmem, rdata_raw);
	//	if (!rslt) return AosDataProcStatus::eError;
	//
	//  outofmem = false;
	//	rslt = output_record->setFieldValue(mDocid.mOutput.mFieldIdx, docid_rslt, outofmem, rdata_raw);
	//	if (!rslt) return AosDataProcStatus::eError;
	//
	//	output_record->flush();
	//}
		
	return AosDataProcStatus::eContinue;
}

