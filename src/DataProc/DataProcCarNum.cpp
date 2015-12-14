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
#include "DataProc/DataProcCarNum.h"

#include "API/AosApi.h"
#include "Util/StrSplit.h"

AosDataProcCarNum::AosDataProcCarNum(const bool flag)
:
AosDataProc(AOSDATAPROC_CARNUM, AosDataProcId::eCarNum, flag)
{
}
	

AosDataProcCarNum::AosDataProcCarNum(const AosDataProcCarNum &proc)
:
AosDataProc(proc)
{
}


AosDataProcCarNum::~AosDataProcCarNum()
{
}


bool
AosDataProcCarNum::config(
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
	
	input_name_field = def->getAttrStr("zky_time_field");
	aos_assert_r(input_name_field != "", false);
	mTimeInput.init(input_name_field, true);
	aos_assert_r(mKeyInput.mRecordName == mTimeInput.mRecordName, false);
		
	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	AosConvertAsciiBinary(mSep);

	OmnString output_name_field = tag->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
	aos_assert_r(output_name_field != "", false);
	RecordFieldInfo keyoutput;
	mKeyOutput.init(output_name_field, false);

	tag = fields->getFirstChild("value");
	aos_assert_r(tag, false);

	input_name_field = tag->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(input_name_field != "", false);
	mDocidInput.init(input_name_field, true);
	
	output_name_field = tag->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
	aos_assert_r(output_name_field != "", false);
	RecordFieldInfo docidoutput;
	mDocidOutput.init(output_name_field, false);
	
	aos_assert_r(mKeyOutput.mRecordName == mDocidOutput.mRecordName, false);

	return true;
}


AosDataProcStatus::E 
AosDataProcCarNum::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eError;
}


AosDataProcObjPtr
AosDataProcCarNum::cloneProc()
{
	return OmnNew AosDataProcCarNum(*this);
}


AosDataProcObjPtr
AosDataProcCarNum::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcCarNum * proc = OmnNew AosDataProcCarNum(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcCarNum::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataProcCarNum::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	rf_infos.push_back(&(mKeyInput));
	rf_infos.push_back(&(mTimeInput));
	rf_infos.push_back(&(mDocidInput));
	rf_infos.push_back(&(mKeyOutput));
	rf_infos.push_back(&(mDocidOutput));
	return true;
}

AosDataProcStatus::E
AosDataProcCarNum::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	AosDataRecordObj * input_record = getRecord(mKeyInput, input_records, output_records);
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	AosValueRslt key_rslt, time_rslt;
	bool rslt = input_record->getFieldValue(mKeyInput.mFieldIdx, key_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	rslt = input_record->getFieldValue(mTimeInput.mFieldIdx, time_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	input_record = getRecord(mDocidInput, input_records, output_records);
	AosValueRslt docid_rslt;
	aos_assert_r(input_record, AosDataProcStatus::eError);	
	rslt = input_record->getFieldValue(mDocidInput.mFieldIdx, docid_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	OmnString field;
	OmnString key = key_rslt.getStr();
	OmnString time = time_rslt.getStr();

	set<OmnString>  new_words;
	if(key == "拒识无牌" || key == "号牌未知")
	{
		new_words.insert(key);
	}
	else
	{
		splitWords(key, new_words);
	}

	aos_assert_r(new_words.size() > 0, AosDataProcStatus::eError);
	AosDataRecordObj * output_record;
//OmnScreen << "================= key : " << key << endl;
	set<OmnString>::iterator itr = new_words.begin();
	for(; itr!=new_words.end(); ++itr)
	{
//OmnScreen << "================= " <<  *itr << endl;
		key = (*itr);
		key << mSep << time;
		key_rslt.reset();
		key_rslt.setStr(key);

		output_record = getRecord(mKeyOutput, input_records, output_records);
		aos_assert_r(output_record, AosDataProcStatus::eError);	
		
		output_record->flush();
	
		bool outofmem = false;
		rslt = output_record->setFieldValue(mKeyOutput.mFieldIdx, key_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;

		outofmem = false;
		rslt = output_record->setFieldValue(mDocidOutput.mFieldIdx, docid_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;
	}
	return AosDataProcStatus::eContinue;
}

void
AosDataProcCarNum::splitWords(
		const OmnString &key,
		set<OmnString> &new_words)
{
	const char *crt = key.data();
	int len = key.length();
	OmnString word;
	vector<OmnString> words;
	for(int i=0; i<len; )
	{
		if(crt[i] >= 0 && crt[i] <= 127)
		{
			word = "";
			word << crt[i++];
			words.push_back(word);
			continue;
		}

		char str[4] = {0};
		str[0] = crt[i++];
		str[1] = crt[i++];
		str[2] = crt[i++];
		word = "";
		word << str;
		words.push_back(word);
	}

	for(int i=1; i<len; i++)
	{
		for(u32 j=0; j<words.size(); j++)
		{
			word = "";
			int num = 0;
			u32 k = j;
			while(num<i && j+i<=words.size())
			{
				word << words[k];
				k++;
				num++;
			}
			if(word != "")
			{
				//new_words.push_back(word);
				new_words.insert(word);
			}
		}
	}
}

bool 
AosDataProcCarNum::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	return true;
}

