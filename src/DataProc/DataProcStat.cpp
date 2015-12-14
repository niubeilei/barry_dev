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
// 07/23/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcStat.h"


AosDataProcStat::AosDataProcStat(const bool flag)
:
AosDataProc(AOSDATAPROC_STAT, AosDataProcId::eStat, flag),
mValueFieldIdx(-1),
mTimeFieldIdx(-1),
mOutputFieldIdx(-1)		// Ketty 2014/01/09
{
}

	
AosDataProcStat::AosDataProcStat(const AosDataProcStat &proc)
:
AosDataProc(proc),
mKeyFieldNameArray(proc.mKeyFieldNameArray),
mKeyFieldIdxArray(proc.mKeyFieldIdxArray),
mValueFieldName(proc.mValueFieldName),
mValueFieldIdx(proc.mValueFieldIdx),
mTimeFieldName(proc.mTimeFieldName),
mTimeFieldIdx(proc.mTimeFieldIdx),
mSep(proc.mSep),
mStatType(proc.mStatType),
mOutputFieldIdx(proc.mOutputFieldIdx)
{
	if(proc.mFilter) mFilter = proc.mFilter->clone();
}


AosDataProcStat::~AosDataProcStat()
{
}


bool
AosDataProcStat::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//[1key1.138,1000]
	//[1key1.139,2000]
	//[1key1,3000]
	//[2key1.key2.key3,value]
	//[2key1.key3,value]
	//[2key1,value]
	//<dataproc 
	//		zky_sep="xxxx"
	//		zky_value_idx="xxxx">
	//	<keyfieldinfo>
	//		<entry zky_key_idx="xxxx"/>
	//		<entry zky_key_idx="xxxx"/>
	//		<entry zky_key_idx="xxxx"/>
	//		<entry zky_key_idx="xxxx"/>
	//	</keyfieldinfo>
	//	<filter>
	//		...
	//	</filter>
	//</dataproc>
	aos_assert_r(def, false);

	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	mOutputFieldName = def->getAttrStr("zky_output_field_name");

	AosXmlTagPtr keyFieldInfos = def->getFirstChild("zky_keyfieldinfo");
	OmnString keyFieldName;
	mKeyFieldNameArray.clear();
	if (keyFieldInfos)
	{
		keyFieldName = keyFieldInfos->getAttrStr(AOSTAG_KEYFIELDNAME);
		aos_assert_r(keyFieldName != "", false);
		mKeyFieldNameArray.push_back(keyFieldName);
	}

	mValueFieldName = def->getAttrStr(AOSTAG_VALUEFIELDNAME);
	aos_assert_r(mValueFieldName != "", false);

	mTimeFieldName = def->getAttrStr("zky_timefield_name");
	aos_assert_r(mTimeFieldName != "", false);

	mSep = def->getAttrStr("zky_sep");

	AosXmlTagPtr tag = def->getFirstChild(AOSTAG_FILTER);
	if (tag)
	{
		AosCondType::E type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
		if (AosCondType::isValid(type))
		{
			mFilter = AosConditionObj::getConditionStatic(tag, rdata);
		}
	}
	return true;
}


// Chen Ding, 12/25/2012
// bool
// AosDataProcStat::compseKeyStr(
// 		vector<OmnString> key_strs,
// 		const AosDataRecordObjPtr &record,
// 		const AosRundataPtr &rdata)
// {
	/*
	OmnString key_str = mStatType;
	if (key_str != "")
	{
		key_strs.push_back(key_str);
	}

	AosValueRslt time_value;
	if (mTimeFieldIdx >= 0)
	{

	}

	AosValueRslt key_rslt;
	int num = mKeyFieldInfos->getNumSubtags();
	AosXmlTagPtr keyfieldInfo;
	for (int i=0; i<num; i++)
	{
		keyfieldInfo = mKeyFieldInfos->getChild(i);
		int key_field_idx = keyfieldInfo->getAttrInt(AOSTAG_KEYFIELDIDX, -1);
		bool rslt = record->getField(key_field_idx, key_rslt, false, rdata);
		aos_assert_rr(rslt, rdata, false);
		if (mFilter)
		{
			mFilter->evalCond(key_rslt, rdata);
		}

		int ss_len;
		const char *ss = key_rslt.getCharStr(ss_len);

		// OmnString crt_str = key_rslt.getStr();
		// crt_str.normalizeWhiteSpace(true, true);
		if (!ss || ss_len <= 0)
		{
			continue;
		}

		key_str << mSep;
		key_str.append(ss, ss_len);

		iil_assembler.append(...);

		if (mTimeFieldIdx >= 0)
		{
			char *data = (char *)key_str.data();
			data[0] = mTimeStatType;
			int len = key_str.length();

			key_str << mSep << time_value.getStr();

			iil_assembler.append(...);
			key_str.setLength(len);
		}

		// key_strs.push_back(key_str);
	}
	*/
//	return true;
//}


AosDataProcStatus::E 
AosDataProcStat::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	/*
	aos_assert_r(mIILAssembler, AosDataProcStatus::eError);

	// 1. Retrieve the value
	AosValueRslt value_rslt;
	bool rslt = record->getFieldValue(mValueFieldIdx, value_rslt, false, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);

	u64 value;
	rslt = value_rslt.getU64();
	aos_assert_rr(rslt, rdata, AosDataProcStatus::eError);

	OmnString key_str;
	key_str << mStatType;
	AosValueRslt key_rslt;
	for (u32 i=0; i<mKeyFieldIdxArray.size(); i++)
	{
		int key_field_idx = mKeyFieldIdxArray.at(i);
		rslt = record->getFieldValue(key_field_idx, key_rslt, false, rdata.getPtrNoLock());
		aos_assert_rr(rslt, rdata, AosDataProcStatus::eError);
		if (mFilter)
		{
			mFilter->evalCond(key_rslt, rdata);
		}

		int ss_len;
		const char *ss = key_rslt.getCharStr(ss_len);
		if (!ss || ss_len <= 0)
		{
			continue;
		}

		key_str << mSep;
		key_str.append(ss, ss_len);

		AosValueRslt vv_rslt;
		vv_rslt.setKeyValue(key_str.data(), key_str.length(), false, value);

		rslt = mIILAssembler->appendEntry(vv_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (mTimeFieldIdx >= 0)
		{
			AosValueRslt time_value;
			rslt = record->getFieldValue(mTimeFieldIdx, time_value, false, rdata.getPtrNoLock());
			char *data = (char *)key_str.data();
			data[0] = mStatType;
			int len = key_str.length();

			key_str << mSep << time_value.getStr();

			vv_rslt.setKeyValue(key_str.data(), key_str.length(), false, value);
			rslt = mIILAssembler->appendEntry(vv_rslt, rdata.getPtrNoLock());
			aos_assert_r(rslt, AosDataProcStatus::eError);
			key_str.setLength(len);
		}
	}
	*/
	return AosDataProcStatus::eContinue;
}


AosDataProcStatus::E
AosDataProcStat::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	return AosDataProcStatus::eContinue;
	/*
	// Ketty 2013/12/27
	aos_assert_r(mOutputIdx >=0, AosDataProcStatus::eError);
	OmnNotImplementedYet;
	return AosDataProcStatus::eError;	// two mAssembler opr.	
	
	AosDataRecordObj * input_record = *input_records;
	AosDataRecordObj * output_record = output_records[mOutputIdx];

	aos_assert_r(input_record, AosDataProcStatus::eError);	
	aos_assert_r(output_record, AosDataProcStatus::eError);	
	aos_assert_r(mOutputFieldIdx >=0, AosDataProcStatus::eError);

	// 1. Retrieve the value
	AosValueRslt value_rslt;
	bool rslt = input_record->getFieldValue(mValueFieldIdx, value_rslt, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	u64 value;
	rslt = value_rslt.getU64();
	aos_assert_rr(rslt, rdata_raw, AosDataProcStatus::eError);

	OmnString key_str;
	key_str << mStatType;
	AosValueRslt key_rslt;
	for (u32 i=0; i<mKeyFieldIdxArray.size(); i++)
	{
		int key_field_idx = mKeyFieldIdxArray.at(i);
		rslt = input_record->getFieldValue(key_field_idx, key_rslt, rdata_raw);
		aos_assert_rr(rslt, rdata_raw, AosDataProcStatus::eError);
		if (mFilter)
		{
			mFilter->evalCond(key_rslt, rdata_raw);
		}

		int ss_len;
		const char *ss = key_rslt.getCharStr(ss_len);
		if (!ss || ss_len <= 0)
		{
			continue;
		}

		key_str << mSep;
		key_str.append(ss, ss_len);

		AosValueRslt vv_rslt;
		vv_rslt.setKeyValue(key_str.data(), key_str.length(), false, value);

		//rslt = mAssembler->appendEntry(vv_rslt, rdata_raw);
		//aos_assert_r(rslt, AosDataProcStatus::eError);

		if (mTimeFieldIdx >= 0)
		{
			AosValueRslt time_value;
			rslt = input_record->getFieldValue(mTimeFieldIdx, time_value, rdata_raw);
			char *data = (char *)key_str.data();
			data[0] = mStatType;
			int len = key_str.length();

			key_str << mSep << time_value.getStr();

			vv_rslt.setKeyValue(key_str.data(), key_str.length(), false, value);
			//rslt = mAssembler->appendEntry(vv_rslt, rdata_raw);
			//aos_assert_r(rslt, AosDataProcStatus::eError);
			key_str.setLength(len);
		}
	}
	*/
	return AosDataProcStatus::eContinue;
}


AosDataProcObjPtr
AosDataProcStat::cloneProc()
{
	return OmnNew AosDataProcStat(*this);
}


AosDataProcObjPtr
AosDataProcStat::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcStat *proc = OmnNew AosDataProcStat(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcStat::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);

	aos_assert_r(record, false);

	int idx = -1;
	mKeyFieldIdxArray.clear();
	for(u32 i=0; i<mKeyFieldNameArray.size(); i++)
	{
		idx = record->getFieldIdx(mKeyFieldNameArray[i], rdata.getPtrNoLock());
		aos_assert_r(idx >= 0, false);
		mKeyFieldIdxArray.push_back(idx);
	}

	mValueFieldIdx = record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
	aos_assert_r(mValueFieldIdx >= 0, false);

	mTimeFieldIdx = record->getFieldIdx(mTimeFieldName, rdata.getPtrNoLock());
	aos_assert_r(mTimeFieldIdx >= 0, false);
	
	return true;
}

bool
AosDataProcStat::resolveDataProc(
		const AosRundataPtr &rdata,
		const AosDataRecordObjPtr &input_record,
		const AosDataRecordObjPtr &output_record)
{
	// Ketty 2013/12/20
	aos_assert_r(input_record, false);
	aos_assert_r(output_record, false);

	int idx = -1;
	mKeyFieldIdxArray.clear();
	for(u32 i=0; i<mKeyFieldNameArray.size(); i++)
	{
		idx = input_record->getFieldIdx(mKeyFieldNameArray[i], rdata.getPtrNoLock());
		aos_assert_r(idx >= 0, false);
		mKeyFieldIdxArray.push_back(idx);
	}

	mValueFieldIdx = input_record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
	aos_assert_r(mValueFieldIdx >= 0, false);

	mTimeFieldIdx = input_record->getFieldIdx(mTimeFieldName, rdata.getPtrNoLock());
	aos_assert_r(mTimeFieldIdx >= 0, false);
	
	aos_assert_r(mOutputFieldName != "", false);
	mOutputFieldIdx = output_record->getFieldIdx(mOutputFieldName, rdata.getPtrNoLock());
	aos_assert_r(mOutputFieldIdx >= 0, false);
	
	return true;
}

