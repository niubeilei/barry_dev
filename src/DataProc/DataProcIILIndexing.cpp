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
#include "DataProc/DataProcIILIndexing.h"

#include "API/AosApi.h"

AosDataProcIILIndexing::AosDataProcIILIndexing(const bool flag)
:
AosDataProc(AOSDATAPROC_IILINDEXING, AosDataProcId::eIILIndexing, flag),
mKeyFieldIdx(-1),
mDocidFieldIdx(-1),		// Ketty 2013/12/23
mOutputFieldIdx(-1),	// Ketty 2014/01/09
mIILType(eAosIILType_Invalid)
{
}
	

AosDataProcIILIndexing::AosDataProcIILIndexing(const AosDataProcIILIndexing &proc)
:
AosDataProc(proc),
mKeyFieldName(proc.mKeyFieldName),
mKeyFieldIdx(proc.mKeyFieldIdx),
mDocidFieldIdx(proc.mDocidFieldIdx),	// Ketty 2013/12/23
mOutputFieldIdx(proc.mOutputFieldIdx),
mIILType(proc.mIILType)
{
	if (proc.mFilter) mFilter = proc.mFilter->clone();
}


AosDataProcIILIndexing::~AosDataProcIILIndexing()
{
}


bool
AosDataProcIILIndexing::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	mOutputFieldName = def->getAttrStr("zky_output_field_name");

	mKeyFieldName = def->getAttrStr(AOSTAG_KEYFIELDNAME);
	//aos_assert_r(mKeyFieldName != "", false);
	if (mKeyFieldName == "")
	{
		AosSetEntityError(rdata, "DataProcIILIndexing_missing_fieldname", 
		"DataProc", "DataProcIILIndexing") << def->toString() << enderr;
		return false;
	}
	
	mNeedDocid = true;
	mIILType = AosIILType_toCode(def->getAttrStr(AOSTAG_IILTYPE));
	switch (mIILType)
	{
	case eAosIILType_U64:
	case eAosIILType_Str:
		 break;

	default:
		 AosSetErrorU(rdata, "invalid_iiltype:") << def->toString();
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}
	
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
AosDataProcIILIndexing::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	OmnAlarm << enderr;
	/*
	// AOS_DECLARE_TIMESTAMPS(4, 1000000);
	aos_assert_r(mIILAssembler || mNeedConvert, AosDataProcStatus::eError);
	aos_assert_r(mKeyFieldIdx >= 0, AosDataProcStatus::eError);

	// 1. Retrieve the key
	AosValueRslt key_rslt;
	bool rslt = record->getFieldValue(mKeyFieldIdx, key_rslt, false, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	// 2. Filter it as needed.
	if (mFilter && !mFilter->evalCond(key_rslt, rdata))
	{
		return AosDataProcStatus::eContinue;
	}
	
	// 3. Assemble the entry
	if(mNeedConvert)
	{
		aos_assert_r(output, AosDataProcStatus::eError);
		bool outofmem = false;
		rslt = output->setFieldValue(mConvertName, key_rslt, outofmem, rdata.getPtrNoLock());
		if (!rslt) return AosDataProcStatus::eError;
	}

	if(mIILAssembler)
	{
		AosValueRslt value_rslt;
		OmnString vv;
		u64 uu;
		switch (mIILType)
		{
		case eAosIILType_Str:
			 vv = key_rslt.getStr();
			 vv.normalizeWhiteSpace(true, true);  // trim
			 if(vv == "") return AosDataProcStatus::eContinue;
			 value_rslt.setKeyValue(vv, false, docid);
			 break;

		case eAosIILType_U64:
		 	 rslt = key_rslt.getU64();
		 	 if (!rslt) return AosDataProcStatus::eError;
			 value_rslt.setKeyValue(uu, docid);
			 break;

		default:
			 AosSetErrorU(rdata, "unrecog_iiltype") << ": " << mIILType << enderr;
			 return AosDataProcStatus::eError;
		}
		 
		rslt = mIILAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
		if (!rslt) return AosDataProcStatus::eError;
	}
	*/
	return AosDataProcStatus::eContinue;
}


AosDataProcStatus::E
AosDataProcIILIndexing::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	// Ketty 2013/12/23
	// remove the mNeedConvert mem.
	return AosDataProcStatus::eContinue;
	/*
	aos_assert_r(mNeedConvert, AosDataProcStatus::eError);
	
	aos_assert_r(mOutputIdx >=0, AosDataProcStatus::eError);
	
	AosDataRecordObj * input_record = *input_records;
	AosDataRecordObj * output_record = output_records[mOutputIdx];
	aos_assert_r(input_record, AosDataProcStatus::eError);	
	aos_assert_r(output_record, AosDataProcStatus::eError);	
	aos_assert_r(mOutputFieldIdx >=0, AosDataProcStatus::eError);
	
	// 1. Retrieve the docid.
	u64 docid = getFieldU64Value(rdata_raw, input_record, mDocidFieldIdx); 
	aos_assert_r(docid != 0, AosDataProcStatus::eError);
	
	// 2. retrieve the value.	
	AosValueRslt key_rslt;
	bool rslt = input_record->getFieldValue(mKeyFieldIdx, key_rslt, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	// 3. Filter it as needed.
	if (mFilter && !mFilter->evalCond(key_rslt, rdata_raw))
	{
		return AosDataProcStatus::eContinue;
	}
	
	// 4. Assemble the entry
	AosValueRslt value_rslt;
	OmnString vv;
	u64 uu;
	switch (mIILType)
	{
	case eAosIILType_Str:
		 vv = key_rslt.getStr();
		 vv.normalizeWhiteSpace(true, true);  // trim
		 if(vv == "") return AosDataProcStatus::eContinue;
		 value_rslt.setKeyValue(vv, false, docid);
		 break;

	case eAosIILType_U64:
		 rslt = key_rslt.getU64();
		 if (!rslt) return AosDataProcStatus::eError;
		 value_rslt.setKeyValue(uu, docid);
		 break;

	default:
		 AosSetErrorU(rdata_raw, "unrecog_iiltype") << ": " << mIILType << enderr;
		 return AosDataProcStatus::eError;
	}
	
	output_record->setDocid(docid);
	bool outofmem = false;
	output_record->setFieldValue(mOutputFieldIdx, value_rslt, outofmem, rdata_raw);
	 
	//rslt = mAssembler->appendEntry(value_rslt, rdata_raw);
	//if (!rslt) return AosDataProcStatus::eError;
	return AosDataProcStatus::eContinue;
	*/
}


AosDataProcObjPtr
AosDataProcIILIndexing::cloneProc()
{
	return OmnNew AosDataProcIILIndexing(*this);
}


AosDataProcObjPtr
AosDataProcIILIndexing::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcIILIndexing * proc = OmnNew AosDataProcIILIndexing(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}

bool
AosDataProcIILIndexing::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	mKeyFieldIdx = record->getFieldIdx(mKeyFieldName, rdata.getPtrNoLock());
	if (mKeyFieldIdx < 0)
	{
		AosSetEntityError(rdata, "dataprociilindexing_missing_fieldname",
				"dataproc", "dataprociilindexing") 
			<< "Field Name: " << mKeyFieldName 
			<< ", Record Name: " << record->getRecordName() 
			<< ", Record Objid: " << record->getRecordObjid() << enderr;
		return false;
	}
	//aos_assert_r(mKeyFieldIdx >= 0, false);
	return true;
}


bool
AosDataProcIILIndexing::resolveDataProc(
		const AosRundataPtr &rdata,
		const AosDataRecordObjPtr &input_record,
		const AosDataRecordObjPtr &output_record)
{
	// Ketty 2013/12/23
	aos_assert_r(input_record, false);
	aos_assert_r(output_record, false);
	
	mKeyFieldIdx = input_record->getFieldIdx(mKeyFieldName, rdata.getPtrNoLock());
	//aos_assert_r(mKeyFieldIdx >= 0, false);
	if (mKeyFieldIdx < 0)
	{
		AosSetEntityError(rdata, "dataprociilindexing_missing_fieldname",
				"dataproc", "dataprociilindexing") 
			<< "Field Name: " << mKeyFieldName 
			<< ", Record Name: " << input_record->getRecordName() 
			<< ", Record Objid: " << input_record->getRecordObjid() << enderr;
		return false;
	}

	mDocidFieldIdx = input_record->getFieldIdx("zky_docid", rdata.getPtrNoLock());
	aos_assert_r(mDocidFieldIdx>= 0, false);
	
	aos_assert_r(mOutputFieldName != "", false);
	mOutputFieldIdx = output_record->getFieldIdx(mOutputFieldName, rdata.getPtrNoLock());
	aos_assert_r(mOutputFieldIdx >= 0, false);
	
	return true;
}


