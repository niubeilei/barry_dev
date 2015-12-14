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
#include "DataProc/DataProcJoin.h"

#include "API/AosApi.h"

#if 0
AosDataProcJoin::AosDataProcJoin(const bool flag)
:
AosDataProc(AOSDATAPROC_JOIN, AosDataProcId::eJoin, flag)
{
}
	

AosDataProcJoin::AosDataProcJoin(const AosDataProcJoin &proc)
:
AosDataProc(proc)
{
}
	

AosDataProcJoin::~AosDataProcJoin()
{
}


bool
AosDataProcJoin::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Its format is:
	// 	<dataproc>
	// 		<input>
	// 			<AOSTAG_LHS AOSTAG_INPUT_FIELD_NAME="xxx"/>
	// 			<AOSTAG_RHS AOSTAG_INPUT_FIELD_NAME="xxx"/>
	// 		</input>
	// 	</dataproc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);	
	
	AosXmlTagPtr input_tag = def->getFirstChild();
	aos_assert_r(input_tag, false);

	AosXmlTagPtr lhs_tag = input_tag->getFirstChild(AOSTAG_LHS);
	if (!lhs_tag)
	{
		AosSetEntityError(rdata, "miss_lhs_cond_tag", lhs_tag->getTagname(), "")
			<< lhs_tag << enderr;
		return false;
	}
	OmnString lhs_input_name_field = lhs_tag->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(lhs_input_name_field != "", false);

	AosXmlTagPtr rhs_tag = input_tag->getFirstChild(AOSTAG_RHS);
	if (!rhs_tag)
	{
		AosSetEntityError(rdata, "miss_rhs_cond_tag", rhs_tag->getTagname(), "")
			<< rhs_tag << enderr;
		return false;
	}
	OmnString rhs_input_name_field = rhs_tag->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	aos_assert_r(rhs_input_name_field != "", false);
		
	mLhsInput.init(lhs_input_name_field, true);
	mRhsInput.init(rhs_input_name_field, true);
	return true;
}


AosDataProcStatus::E
AosDataProcJoin::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eError;
}


AosDataProcStatus::E
AosDataProcJoin::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	AosValueRslt lhs_rslt;
	//AosDataRecordObj * lhs_input_record = input_records[mLhsInput.mRecordIdx];
	AosDataRecordObj * lhs_input_record = getRecord(mLhsInput, input_records, output_records);
	aos_assert_r(lhs_input_record, AosDataProcStatus::eError);
	bool rslt = lhs_input_record->getFieldValue(
		mLhsInput.mFieldIdx, lhs_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	OmnString rslt_lhs = lhs_rslt.getStr();
	rslt_lhs.normalizeWhiteSpace(true, true);  // trim
	if (rslt_lhs == "")
	{
		return AosDataProcStatus::eError;
	}

	//AosDataRecordObj * rhs_input_record = input_records[mRhsInput.mRecordIdx];
	AosDataRecordObj * rhs_input_record = getRecord(mRhsInput, input_records, output_records);
	aos_assert_r(rhs_input_record, AosDataProcStatus::eError);
	AosValueRslt rhs_rslt;
	rslt = rhs_input_record->getFieldValue(
		mRhsInput.mFieldIdx, rhs_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	OmnString rslt_rhs = rhs_rslt.getStr();
	rslt_rhs.normalizeWhiteSpace(true, true);  // trim
	if (rslt_rhs == "")
	{
		return AosDataProcStatus::eError;
	}

	int vv = strcmp(rslt_lhs.data(), rslt_rhs.data());
	if (vv < 0)
	{
		return AosDataProcStatus::eLT;
	}

	if (vv == 0)
	{
		return AosDataProcStatus::eEQ;
	}

	// vv > 0
	return AosDataProcStatus::eGT;
}


AosDataProcObjPtr
AosDataProcJoin::cloneProc()
{
	return OmnNew AosDataProcJoin(*this);
}


AosDataProcObjPtr
AosDataProcJoin::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcJoin * proc = OmnNew AosDataProcJoin(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}
 

bool
AosDataProcJoin::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosDataProcJoin::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	rf_infos.push_back(&mLhsInput);
	rf_infos.push_back(&mRhsInput);
	return true;
}


/*
bool
AosDataProcJoin::resolveDataProc(
		const AosRundataPtr &rdata,
		const AosDataRecordObjPtr &input_record,
		const AosDataRecordObjPtr &output_record)
{
	aos_assert_r(input_record, false);

	aos_assert_r(mLhsFieldName!= "", false);
	mLhsFieldIdx = input_record->getFieldIdx(mLhsFieldName, rdata);
	aos_assert_r(mLhsFieldIdx >= 0, false);

	aos_assert_r(mRhsFieldName!= "", false);
	mRhsFieldIdx = input_record->getFieldIdx(mRhsFieldName, rdata);
	aos_assert_r(mRhsFieldIdx >= 0, false);

	return true;
}
*/

#endif
