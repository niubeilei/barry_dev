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
// This data proc creates an IIL entry:
// 		[name|$$|time, value]
// where 'name' is a value retrieved from the input record, 'value' is 
// a value retrieved from the record, and 'time' is a value retrieved
// from the import data task. This is normally used for statistics. 
//
// Example:
// Assume the input records are CDRs:
// 	[calling_party_num, call_duration, ...]
//
// we want to collect the sum of call durations for every calling_party_num.
// When doing the importing, all the days for the inputs are the same and
// can be retrieved from the task. 
//
// Modification History:
// 04/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcTimed.h"


AosDataProcTimed::AosDataProcTimed(const bool flag)
:
AosDataProc(AOSDATAPROC_TIMED, AosDataProcId::eTimed, flag),
mKeyFieldIdx(-1),
mEpochDayIdx(-1),
mValueFieldIdx(-1),
mOutputFieldIdx(-1)		// Ketty 2014/01/09
{
}
	

AosDataProcTimed::AosDataProcTimed(const AosDataProcTimed &proc)
:
AosDataProc(proc),
mKeyFieldName(proc.mKeyFieldName),
mKeyFieldIdx(proc.mKeyFieldIdx),
mEpochDayName(proc.mEpochDayName),
mEpochDayIdx(proc.mEpochDayIdx),
mValueFieldName(proc.mValueFieldName),
mValueFieldIdx(proc.mValueFieldIdx),
mSep(proc.mSep),
mOutputFieldIdx(proc.mOutputFieldIdx)
{
	if(proc.mFilter) mFilter = proc.mFilter->clone();
}


AosDataProcTimed::~AosDataProcTimed()
{
}


bool
AosDataProcTimed::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	mOutputFieldName = def->getAttrStr("zky_output_field_name");

	mKeyFieldName = def->getAttrStr(AOSTAG_KEYFIELDNAME);
	aos_assert_r(mKeyFieldName != "", false);

	mEpochDayName = def->getAttrStr(AOSTAG_EPOCHDAYNAME);
	aos_assert_r(mEpochDayName != "", false);
	
	mValueFieldName = def->getAttrStr(AOSTAG_VALUEFIELDNAME);
	aos_assert_r(mValueFieldName != "", false);
	
	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	
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


AosDataProcStatus::E
AosDataProcTimed::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	// This data proc creates the following entry:
	// 		[name|$|epoch_day, value]
	// where 'name' is retrieved from a given field in 'record' and 'value'
	// is retrieved from another field in 'record'. The result is put in
	// an IIL in 'iilAsm'.
	//
	// This function assumes that the epoch day is retrieved from 'task'.
	// Its index is mEpochDayIdx.
	
	// 1. Retrieve the IIL assembler
	aos_assert_r(mIILAssembler, AosDataProcStatus::eError);
	
	// 2. Retrieve the key
	AosValueRslt key_rslt;
	bool rslt = record->getFieldValue(mKeyFieldIdx, key_rslt, false, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	if (mFilter && !mFilter->evalCond(key_rslt, rdata))
	{
		return AosDataProcStatus::eContinue;
	}

	OmnString key = key_rslt.getStr();
	key.normalizeWhiteSpace(true, true);  // trim
	if(key == "") return AosDataProcStatus::eContinue;

	// 2. Retrieve the time
	AosValueRslt time_rslt;
	rslt = record->getFieldValue(mEpochDayIdx, time_rslt, false, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	OmnString epoch_time = time_rslt.getStr();
	if(epoch_time == "") return AosDataProcStatus::eContinue;

	key << mSep << epoch_time;

	// 3. Retrieve the value
	AosValueRslt value_rslt;
	rslt = record->getFieldValue(mValueFieldIdx, value_rslt, false, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	u64 value;
	rslt = value_rslt.getU64();
	if (!rslt) return AosDataProcStatus::eError;

	AosValueRslt vv_rslt;
	//vv_rslt.setKeyValue(key, false, value);
	rslt = mIILAssembler->appendEntry(vv_rslt, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);

	return AosDataProcStatus::eContinue;
}

	
AosDataProcStatus::E
AosDataProcTimed::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	return AosDataProcStatus::eContinue;

	/*
	// Ketty 2013/12/27
	// 1. Retrieve the IIL assembler
	aos_assert_r(mOutputIdx >=0, AosDataProcStatus::eError);
	
	AosDataRecordObj * input_record = *input_records;
	AosDataRecordObj * output_record = output_records[mOutputIdx];

	aos_assert_r(input_record, AosDataProcStatus::eError);	
	aos_assert_r(output_record, AosDataProcStatus::eError);	
	aos_assert_r(mOutputFieldIdx >=0, AosDataProcStatus::eError);
	
	// 2. Retrieve the key
	AosValueRslt key_rslt;
	bool rslt = input_record->getFieldValue(mKeyFieldIdx, key_rslt, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	if (mFilter && !mFilter->evalCond(key_rslt, rdata_raw))
	{
		return AosDataProcStatus::eContinue;
	}

	OmnString key = key_rslt.getStr();
	key.normalizeWhiteSpace(true, true);  // trim
	if(key == "") return AosDataProcStatus::eContinue;

	// 2. Retrieve the time
	AosValueRslt time_rslt;
	rslt = input_record->getFieldValue(mEpochDayIdx, time_rslt, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	OmnString epoch_time = time_rslt.getStr();
	if(epoch_time == "") return AosDataProcStatus::eContinue;

	key << mSep << epoch_time;

	// 3. Retrieve the value
	AosValueRslt value_rslt;
	rslt = input_record->getFieldValue(mValueFieldIdx, value_rslt, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	u64 value;
	rslt = value_rslt.getU64();
	if (!rslt) return AosDataProcStatus::eError;

	AosValueRslt vv_rslt;
	vv_rslt.setKeyValue(key, false, value);
	
	bool outofmem = false;
	output_record->setDocid(value);
	output_record->setFieldValue(mOutputFieldIdx, vv_rslt, outofmem, rdata_raw);
	//rslt = mAssembler->appendEntry(vv_rslt, rdata_raw);
	//aos_assert_r(rslt, AosDataProcStatus::eError);

	return AosDataProcStatus::eContinue;
	*/
}

AosDataProcObjPtr
AosDataProcTimed::cloneProc()
{
	return OmnNew AosDataProcTimed(*this);
}


AosDataProcObjPtr
AosDataProcTimed::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcTimed * proc = OmnNew AosDataProcTimed(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcTimed::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	
	mKeyFieldIdx = record->getFieldIdx(mKeyFieldName, rdata.getPtrNoLock());
	aos_assert_r(mKeyFieldIdx >= 0, false);
	
	mEpochDayIdx = record->getFieldIdx(mEpochDayName, rdata.getPtrNoLock());
	aos_assert_r(mEpochDayIdx >= 0, false);
	
	mValueFieldIdx = record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
	aos_assert_r(mValueFieldIdx >= 0, false);

	return true;
}


bool
AosDataProcTimed::resolveDataProc(
		const AosRundataPtr &rdata,
		const AosDataRecordObjPtr &input_record,
		const AosDataRecordObjPtr &output_record)
{
	// Ketty 2013/12/20
	aos_assert_r(input_record, false);
	aos_assert_r(output_record, false);
	
	mKeyFieldIdx = input_record->getFieldIdx(mKeyFieldName, rdata.getPtrNoLock());
	aos_assert_r(mKeyFieldIdx >= 0, false);
	
	mEpochDayIdx = input_record->getFieldIdx(mEpochDayName, rdata.getPtrNoLock());
	aos_assert_r(mEpochDayIdx >= 0, false);
	
	mValueFieldIdx = input_record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
	aos_assert_r(mValueFieldIdx >= 0, false);

	aos_assert_r(mOutputFieldName != "", false);
	mOutputFieldIdx = output_record->getFieldIdx(mOutputFieldName, rdata.getPtrNoLock());
	aos_assert_r(mOutputFieldIdx >= 0, false);
	
	return true;
}

