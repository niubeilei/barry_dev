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
// This data proc filters a record.
//
//
// Modification History:
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcFilter.h"


AosDataProcFilter::AosDataProcFilter(const bool flag)
:
AosDataProc(AOSDATAPROC_FILTER, AosDataProcId::eFilter, flag),
mKeyFieldIdx(-1)
{
}
	

AosDataProcFilter::AosDataProcFilter(const AosDataProcFilter &rhs)
:
AosDataProc(rhs),
mKeyFieldName(rhs.mKeyFieldName),
mKeyFieldIdx(rhs.mKeyFieldIdx),
mUseRcd(rhs.mUseRcd)
{
	if (rhs.mFilter) mFilter = rhs.mFilter->clone();
}


AosDataProcFilter::~AosDataProcFilter()
{
}


bool
AosDataProcFilter::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_r(rslt, false);
		
	mKeyFieldName = def->getAttrStr(AOSTAG_KEYFIELDNAME);
	mUseRcd = def->getAttrBool("use_rcd");
	if (mKeyFieldName == "")
	{
		AosSetEntityError(rdata, 
				"dataprocfilter_missing_key", 
				"DataProc", 
				def->getAttrStr("proc_name"))
			<< def->toString() << enderr;
	}

	aos_assert_r(mKeyFieldName != "", false);

	AosXmlTagPtr tag = def->getFirstChild(AOSTAG_FILTER);
	aos_assert_r(tag, false);

	mFilter = AosConditionObj::getConditionStatic(tag, rdata);
	aos_assert_r(mFilter, false);

	return true;
}


AosDataProcStatus::E
AosDataProcFilter::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	// This function filters 'record' based on a configured condition. 
	//
	// Chen Ding, 12/25/2012
	// This data proc only filters records. Since filters may be embedded
	// in data procs, it does not seem that this proc is needed anymore.
	aos_assert_r(record, AosDataProcStatus::eError);
	aos_assert_r(mFilter, AosDataProcStatus::eError);

	AosValueRslt key_rslt;
	bool rslt = record->getFieldValue(mKeyFieldIdx, key_rslt, false, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);

	if (mUseRcd)
	{
		rslt = mFilter->evalCond(record, rdata);
	}
	else
	{
		rslt = mFilter->evalCond(key_rslt, rdata);
	}

	if (!rslt)
	{
		return AosDataProcStatus::eContinue;
	}

	return AosDataProcStatus::eContinue;
}

AosDataProcStatus::E
AosDataProcFilter::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	// Ketty 2013/12/27
	AosDataRecordObj * input_record = *input_records;
	
	aos_assert_r(input_record, AosDataProcStatus::eError);
	aos_assert_r(mFilter, AosDataProcStatus::eError);

	AosValueRslt key_rslt;
	bool rslt = input_record->getFieldValue(mKeyFieldIdx, key_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	if (mUseRcd)
	{
		AosRundataPtr rdata = rdata_raw;
		AosDataRecordObjPtr record = input_record;
		rslt = mFilter->evalCond(record, rdata);
	}
	else
	{
		rslt = mFilter->evalCond(key_rslt, rdata_raw);
	}

	if (!rslt)
	{
		// It is filtered out.
		return AosDataProcStatus::eContinue;
	}

	return AosDataProcStatus::eContinue;
}

AosDataProcObjPtr
AosDataProcFilter::cloneProc()
{
	return OmnNew AosDataProcFilter(*this);
}


AosDataProcObjPtr
AosDataProcFilter::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcFilter * proc = OmnNew AosDataProcFilter(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcFilter::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	bool rslt = resolveDataProc(rdata, record, 0);
	aos_assert_r(rslt, false)
	return true;
}

	
bool
AosDataProcFilter::resolveDataProc(
		const AosRundataPtr &rdata,
		const AosDataRecordObjPtr &input_record,
		const AosDataRecordObjPtr &output_record)
{
	// Ketty 2013/12/20
	aos_assert_r(input_record, false);
	mKeyFieldIdx = input_record->getFieldIdx(mKeyFieldName, rdata.getPtrNoLock());
	aos_assert_r(mKeyFieldIdx >= 0, false);
	return true;
}


