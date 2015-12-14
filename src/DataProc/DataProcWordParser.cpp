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
#include "DataProc/DataProcWordParser.h"

#include "API/AosApi.h"
#include "Util/StrSplit.h"
__thread AosDataProcWordParser::ParserUtil* AosDataProcWordParser::stParserUtil = 0;

#if 0
AosDataProcWordParser::AosDataProcWordParser(const bool flag)
:
AosDataProc(AOSDATAPROC_WORDPARSER, AosDataProcId::eWordParser, flag)
{
	if (!stParserUtil)
		stParserUtil = OmnNew ParserUtil();
}
	

AosDataProcWordParser::AosDataProcWordParser(const AosDataProcWordParser &proc)
:
AosDataProc(proc)
{
	if (!stParserUtil)
		stParserUtil = OmnNew ParserUtil();
}


AosDataProcWordParser::~AosDataProcWordParser()
{
	OmnDelete stParserUtil;
	stParserUtil = 0;
}

void AosDataProcWordParser::onThreadInit(const AosRundataPtr &, void **data)
{

	if (!stParserUtil)
		stParserUtil = OmnNew ParserUtil();
}

void AosDataProcWordParser::onThreadExit(const AosRundataPtr &, void **data)
{
	OmnDelete stParserUtil;
	stParserUtil = 0;
}

bool
AosDataProcWordParser::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc proc_id="statindex" zky_sep="0x01">
	//<fields>
	//<key zky_input_field_name="rcd_unst_system_type_one_epochday.str" zky_output_field_name="rcd_unst_system_type_statidx_f1_epochday.str" />
	//<docid zky_input_field_name="rcd_unst_system_type_one_epochday.docid" zky_output_field_name="rcd_unst_system_type_statidx_f1_epochday.docid" />
	//</fields>
	//</dataproc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);

	AosXmlTagPtr fields = def->getFirstChild("fields");
	aos_assert_r(fields, false);

	AosXmlTagPtr tag = fields->getFirstChild("key");
	aos_assert_r(tag, false);
	OmnString input_name_field = tag->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(input_name_field != "", false);
	mKeyInput.init(input_name_field, true);

	OmnString output_name_field = tag->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
	aos_assert_r(output_name_field != "", false);
	RecordFieldInfo keyoutput;
	keyoutput.init(output_name_field, false);

	tag = fields->getFirstChild("value");
	aos_assert_r(tag, false);

	input_name_field = tag->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(input_name_field != "", false);
	mDocidInput.init(input_name_field, true);
	
	output_name_field = tag->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
	aos_assert_r(output_name_field != "", false);
	RecordFieldInfo docidoutput;
	docidoutput.init(output_name_field, false);
	
	aos_assert_r(keyoutput.mRecordName == docidoutput.mRecordName, false);

	vector<u32> svr_ids = AosGetServerIds();		
	aos_assert_r(svr_ids.size() > 0, false);
	for(size_t i = 0; i < svr_ids.size(); i++)
	{
		WordInfo info;
		info.mKeyOutput = keyoutput;
		info.mDocidOutput = docidoutput;
		info.mKeyOutput.mRecordName << "_" << i;
		info.mDocidOutput.mRecordName << "_" << i;
		mInfo.push_back(info);
	}
	
	return true;
}


AosDataProcStatus::E 
AosDataProcWordParser::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eError;
}


AosDataProcObjPtr
AosDataProcWordParser::cloneProc()
{
	return OmnNew AosDataProcWordParser(*this);
}


AosDataProcObjPtr
AosDataProcWordParser::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcWordParser * proc = OmnNew AosDataProcWordParser(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcWordParser::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataProcWordParser::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	rf_infos.push_back(&(mKeyInput));
	rf_infos.push_back(&(mDocidInput));
	for(size_t i=0; i<mInfo.size(); i++)
	{
		rf_infos.push_back(&(mInfo[i].mKeyOutput));
		rf_infos.push_back(&(mInfo[i].mDocidOutput));
	}
	return true;
}

AosDataProcStatus::E
AosDataProcWordParser::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
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

	OmnString field;
	OmnString key = key_rslt.getStr();
	aos_assert_r(stParserUtil, AosDataProcStatus::eError);

	char word_str[AosXmlTag::eMaxWordLen+1];
	int str_len;
	int pid;
	AosDataRecordObj * output_record;
	(stParserUtil->mWordParser)->setSrc(key.data(), 0, key.length());

	while(1)
	{
		rslt = (stParserUtil->mWordParser)->nextWord(word_str, str_len);
		if (!rslt) 
		{
			break;
		}
		
		//pid = AosGetServeridByIILName(word_str, str_len, false);
		pid = 0;
		aos_assert_r(pid >= 0, AosDataProcStatus::eError);

		key_rslt.reset();
		//key_rslt.setStr(word);
		// key_rslt.setValue(word_str, str_len);
		key_rslt.setCharStr1(word_str, str_len, false);

		output_record = getRecord(mInfo[pid].mKeyOutput, input_records, output_records);
		aos_assert_r(output_record, AosDataProcStatus::eError);	
		
		output_record->flush();
	
		bool outofmem = false;
		rslt = output_record->setFieldValue(mInfo[pid].mKeyOutput.mFieldIdx, key_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;

		outofmem = false;
		rslt = output_record->setFieldValue(mInfo[pid].mDocidOutput.mFieldIdx, docid_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;
	}
	return AosDataProcStatus::eContinue;
}

bool 
AosDataProcWordParser::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	return true;
}

#endif 
