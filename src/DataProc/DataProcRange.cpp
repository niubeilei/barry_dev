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
#include "DataProc/DataProcRange.h"

#include "API/AosApi.h"


AosDataProcRange::AosDataProcRange(const bool flag)
:
AosDataProc(AOSDATAPROC_RANGE, AosDataProcId::eRange, flag),
mValueFieldIdx(-1),
mHasRange(false),
mStartFieldIdx(-1),
mEndFieldIdx(-1),
mOutputFieldIdx(-1)
{
}
	

AosDataProcRange::AosDataProcRange(const AosDataProcRange &proc)
:
AosDataProc(proc),
mKeyFields(proc.mKeyFields),
mValueFieldName(proc.mValueFieldName),
mValueFieldIdx(proc.mValueFieldIdx),
mHasRange(proc.mHasRange),
mStartFieldName(proc.mStartFieldName),
mEndFieldName(proc.mEndFieldName),
mStartFieldIdx(proc.mStartFieldIdx),
mEndFieldIdx(proc.mEndFieldIdx),
mOutputFieldIdx(proc.mOutputFieldIdx)
{
}


AosDataProcRange::~AosDataProcRange()
{
}


bool
AosDataProcRange::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Its format is:
	// 	<dataproc
	// 		zky_value_field_name1="xxx"
	// 		zky_value_field_name2="xxx"
	// 		shift1="xxx"
	// 		shift2="xxx">
	// 		<keyfields>
	// 			AOSTAG_KEYFIELDNAME="xxx"
	// 			AOSTAG_ADDTOKEY="xxx"
	// 			AOSTAG_SEPARATOR="xxx">
	// 			<AOSTAG_FILTER ...>
	// 				...
	// 			</AOSTAG_FILTER>
	// 		</keyfields>
	// 	</dataproc>
	aos_assert_r(def, false);
	
	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_r(rslt, false);
	
	mOutputFieldName = def->getAttrStr("zky_output_field_name");
	
	// Chen Ding, 2013/05/30
	AosXmlTagPtr keyfields_tag = def->getFirstChild("keyfields");
	if (keyfields_tag)
	{
		// It is the new format. 
		return configNew(rdata, def);
	}

	mValueFieldName = def->getAttrStr(AOSTAG_VALUEFIELDNAME);
	if (mValueFieldName == "")
	{
		mNeedDocid = true;
	}
	
	AosXmlTagPtr entry = def->getFirstChild();
	aos_assert_r(entry, false);

	OmnString key_field_name;
	bool addtokey, ignore_empty, is_range;
	OmnString sep;
	AosConditionObjPtr filter;
	AosXmlTagPtr tag;
	AosCondType::E type;
	while(entry)
	{
		is_range = entry->getAttrBool(AOSTAG_IS_RANGE_FIELD, false);
		if (!mHasRange) 
		{
			mHasRange = is_range;
		}
		else
		{
			aos_assert_r(!is_range, false);
		}

		if (!is_range)
		{
			key_field_name = entry->getAttrStr(AOSTAG_KEYFIELDNAME);
			aos_assert_r(key_field_name != "", false);
		}
		else
		{
			key_field_name = "";
		}

		addtokey = entry->getAttrBool(AOSTAG_ADDTOKEY, true);
		ignore_empty = entry->getAttrBool(AOSTAG_IGNORE_EMPTY, false);

		sep = entry->getAttrStr(AOSTAG_SEPARATOR);
		AosConvertAsciiBinary(sep);

		filter = 0;
		tag = entry->getFirstChild(AOSTAG_FILTER);
		if (tag)
		{
			type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
			if (AosCondType::isValid(type))
			{
				filter = AosConditionObj::getConditionStatic(tag, rdata);
			}
		}

		composeField cp;
		cp.mKeyFieldName = key_field_name;
		cp.mAddToKey = addtokey;
		cp.mIgnoreEmpty = ignore_empty;
		cp.mSep = sep;
		cp.mFilter = filter;
		cp.mIsRange = is_range;

		mKeyFields.push_back(cp);	
		entry = def->getNextChild();
	}

	mStartFieldName = def->getAttrStr(AOSTAG_START_FIELD_NAME); 
	mEndFieldName = def->getAttrStr(AOSTAG_END_FIELD_NAME);
	if (mHasRange)
	{
		aos_assert_r(mStartFieldName != "" && mEndFieldName != "", false);
	}
	
	aos_assert_r(mKeyFields.size() > 0, false);
	return true;
}


bool
AosDataProcRange::configNew(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// Its format is:
	// 	<dataproc
	// 		<key_fields>
	// 			<key_field
	// 				AOSTAG_KEYFIELDNAME="xxx"
	// 				AOSTAG_ADDTOKEY="xxx"
	// 				AOSTAG_SEPARATOR="xxx">
	// 				<AOSTAG_FILTER ...>
	// 					...
	// 				</AOSTAG_FILTER>
	// 			</key_field>
	// 			<key_field
	// 				AOSTAG_KEYFIELDNAME="xxx"
	// 				AOSTAG_ADDTOKEY="xxx"
	// 				AOSTAG_SEPARATOR="xxx">
	// 				<AOSTAG_FILTER ...>
	// 					...
	// 				</AOSTAG_FILTER>
	// 			</key_field>
	// 			...
	// 		</key_fields>
	// 		<value_fields>
	// 			<field field_name="xxx" shift="xxx">
	// 				<AOSTAG_FILTER ...>
	// 					...
	// 				</AOSTAG_FILTER>
	// 			</field>
	// 			<field field_name="xxx" shift="xxx">
	// 				<AOSTAG_FILTER ...>
	// 					...
	// 				</AOSTAG_FILTER>
	// 			</field>
	// 		</value_fields>
	// 	</dataproc>
	
	aos_assert_r(def, false);
/*	
	AosXmlTagPtr valuefields_tag = def->getFirstChild("value_fields");
	AosCondType::E type;
	if (valuefields_tag)
	{
		AosXmlTagPtr entry = valuefields_tag->getFirstChild();
		if (entry)
		{
			mValueFieldName1 = entry->getAttrStr("field_name");
			mShift1 = entry->getAttrInt("shift", 0);
			AosXmlTagPtr tag = entry->getFirstChild(AOSTAG_FILTER);
			if (tag)
			{
				type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
				if (AosCondType::isValid(type))
				{
					mValueFilter1 = AosConditionObj::getConditionStatic(tag, rdata);
				}
			}

			entry = valuefields_tag->getNextChild();
			if (entry)
			{
				mValueFieldName2 = entry->getAttrStr("field_name");
				mShift2 = entry->getAttrInt("shift", 0);
				tag = entry->getFirstChild(AOSTAG_FILTER);
				if (tag)
				{
					type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
					if (AosCondType::isValid(type))
					{
						mValueFilter2 = AosConditionObj::getConditionStatic(tag, rdata);
					}
				}
			}
		}
	}

	AosXmlTagPtr keyfields_tag = def->getFirstChild("key_fields");
	if (!keyfields_tag)
	{
		AosSetErrorU(rdata, "missing_keyfields") << def->toString() << enderr;
		return false;
	}
	
	AosXmlTagPtr entry = keyfields_tag->getFirstChild();
	aos_assert_r(entry, false);

	OmnString key_field_name;
	bool addtokey;
	OmnString sep;
	AosConditionObjPtr filter;
	AosXmlTagPtr tag;
	while(entry)
	{
		key_field_name = entry->getAttrStr(AOSTAG_KEYFIELDNAME);
		aos_assert_r(key_field_name != "", false);

		addtokey = entry->getAttrBool(AOSTAG_ADDTOKEY, true);
		sep = entry->getAttrStr(AOSTAG_SEPARATOR);
		AosConvertAsciiBinary(sep);
		
		filter = 0;
		tag = entry->getFirstChild(AOSTAG_FILTER);
		if (tag)
		{
			type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
			if (AosCondType::isValid(type))
			{
				filter = AosConditionObj::getConditionStatic(tag, rdata);
			}
		}
			
		composeField cp;
		cp.mKeyFieldName = key_field_name;
		cp.mAddToKey = addtokey;
		cp.mSep = sep;
		cp.mFilter = filter;
		
		mKeyFields.push_back(cp);	
		entry = keyfields_tag->getNextChild();
	}
	*/
	aos_assert_r(mKeyFields.size() > 0, false);
	return true;
}


AosDataProcStatus::E
AosDataProcRange::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	// This function composes an entry by two data elements:
	aos_assert_r(record, AosDataProcStatus::eError);
	aos_assert_r(mIILAssembler, AosDataProcStatus::eError);
	
	int size = mKeyFields.size();
	aos_assert_r(size > 0, AosDataProcStatus::eError);

	bool rslt = true;

	u64 start = 0;
	u64 end = 1;
	if (mHasRange)
	{
		AosValueRslt start_rslt;
		rslt = record->getFieldValue(mStartFieldIdx, start_rslt, false, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);

		rslt = start_rslt.getU64();
		aos_assert_r(rslt, AosDataProcStatus::eError);

		AosValueRslt end_rslt;
		rslt = record->getFieldValue(mEndFieldIdx, end_rslt, false, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);
		
		rslt = end_rslt.getU64();
		aos_assert_r(rslt, AosDataProcStatus::eError);

		aos_assert_r(end > start, AosDataProcStatus::eError);
	}

	u64 value = docid;
	if (mValueFieldIdx >= 0)
	{
		AosValueRslt value_rslt;
		rslt = record->getFieldValue(mValueFieldIdx, value_rslt, false, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);
		
		rslt = value_rslt.getU64();
		if (!rslt)
		{
			return AosDataProcStatus::eContinue;
		}
	
		// Chen Ding, 2013/05/30
		if (mValueFilter && !mValueFilter->evalCond(value_rslt, rdata))
		{
			return AosDataProcStatus::eContinue;
		}
	}

	OmnString key, rslt_key;
	for (u64 s=start; s<=end; s++)
	{
		key = "";
		for(int i=0; i<size; i++)
		{
			AosValueRslt key_rslt;
			if (mKeyFields[i].mIsRange)
			{
				OmnString vv;
				vv << s;
				key_rslt.setStr(vv);
			}
			else
			{
				rslt = record->getFieldValue(
					mKeyFields[i].mKeyFieldIdx, key_rslt, false, rdata.getPtrNoLock());
				aos_assert_r(rslt, AosDataProcStatus::eError);
			}

			if (mKeyFields[i].mFilter
				&& !mKeyFields[i].mFilter->evalCond(key_rslt, rdata))
			{	
				return AosDataProcStatus::eContinue;
			}

			if (!mKeyFields[i].mAddToKey)
			{
				continue;
			}

			rslt_key = key_rslt.getStr();
			rslt_key.normalizeWhiteSpace(true, true);  // trim
			if (rslt_key == "" && !mKeyFields[i].mIgnoreEmpty)
			{
				return AosDataProcStatus::eContinue;
			}

			key << mKeyFields[i].mSep << rslt_key;
		}

		AosValueRslt value_rslt;
		OmnAlarm << enderr;
		//value_rslt.setKeyValue(key.data(), key.length(), false, value);

		rslt = mIILAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);
	}

	return AosDataProcStatus::eContinue;
}


AosDataProcStatus::E
AosDataProcRange::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	return AosDataProcStatus::eContinue;
	/*
	// This function composes an entry by two data elements:
	aos_assert_r(mOutputIdx >=0, AosDataProcStatus::eError);
	
	AosDataRecordObj * input_record = *input_records;
	AosDataRecordObj * output_record = output_records[mOutputIdx];

	aos_assert_r(input_record, AosDataProcStatus::eError);	
	aos_assert_r(output_record, AosDataProcStatus::eError);	
	aos_assert_r(mOutputFieldIdx >=0, AosDataProcStatus::eError);
	aos_assert_r(mValueFieldIdx >=0, AosDataProcStatus::eError);
	
	int size = mKeyFields.size();
	aos_assert_r(size > 0, AosDataProcStatus::eError);

	bool rslt = true;

	u64 start = 0;
	u64 end = 1;
	if (mHasRange)
	{
		AosValueRslt start_rslt;
		rslt = input_record->getFieldValue(mStartFieldIdx, start_rslt, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		rslt = start_rslt.getU64();
		aos_assert_r(rslt, AosDataProcStatus::eError);

		AosValueRslt end_rslt;
		rslt = input_record->getFieldValue(mEndFieldIdx, end_rslt, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		
		rslt = end_rslt.getU64();
		aos_assert_r(rslt, AosDataProcStatus::eError);

		aos_assert_r(end > start, AosDataProcStatus::eError);
	}

	u64 value=0;
	AosValueRslt input_value_rslt;
	rslt = input_record->getFieldValue(mValueFieldIdx, input_value_rslt, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	rslt = input_value_rslt.getU64();
	if (!rslt)
	{
		return AosDataProcStatus::eContinue;
	}
	
	// Chen Ding, 2013/05/30
	if (mValueFilter && !mValueFilter->evalCond(input_value_rslt, rdata_raw))
	{
		return AosDataProcStatus::eContinue;
	}

	OmnString key, rslt_key;
	for (u64 s=start; s<=end; s++)
	{
		key = "";
		for(int i=0; i<size; i++)
		{
			AosValueRslt key_rslt;
			if (mKeyFields[i].mIsRange)
			{
				OmnString vv;
				vv << s;
				key_rslt.setValue(vv);
			}
			else
			{
				rslt = input_record->getFieldValue(
					mKeyFields[i].mKeyFieldIdx, key_rslt, rdata_raw);
				aos_assert_r(rslt, AosDataProcStatus::eError);
			}

			if (mKeyFields[i].mFilter
				&& !mKeyFields[i].mFilter->evalCond(key_rslt, rdata_raw))
			{	
				return AosDataProcStatus::eContinue;
			}

			if (!mKeyFields[i].mAddToKey)
			{
				continue;
			}

			rslt_key = key_rslt.getStr();
			rslt_key.normalizeWhiteSpace(true, true);  // trim
			if (rslt_key == "" && !mKeyFields[i].mIgnoreEmpty)
			{
				return AosDataProcStatus::eContinue;
			}

			key << mKeyFields[i].mSep << rslt_key;
		}

		AosValueRslt value_rslt;
		value_rslt.setKeyValue(key.data(), key.length(), false, value);
	
		output_record->setDocid(value);
		bool outofmem = false;
		output_record->setFieldValue(mOutputFieldIdx, value_rslt, outofmem, rdata_raw);

		//rslt = mAssembler->appendEntry(value_rslt, rdata_raw);
		//aos_assert_r(rslt, AosDataProcStatus::eError);
	}

	return AosDataProcStatus::eContinue;
	*/
}


AosDataProcObjPtr
AosDataProcRange::cloneProc()
{
	return OmnNew AosDataProcRange(*this);
}


AosDataProcObjPtr
AosDataProcRange::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcRange * proc = OmnNew AosDataProcRange(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcRange::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	int idx = -1;
	for(u32 i=0; i<mKeyFields.size(); i++)
	{
		if (!mKeyFields[i].mIsRange)
		{
			idx = record->getFieldIdx(mKeyFields[i].mKeyFieldName, rdata.getPtrNoLock());
			aos_assert_r(idx >= 0, false);

			mKeyFields[i].mKeyFieldIdx = idx;
		}
	}
	
	if(mValueFieldName != "")
	{
		mValueFieldIdx = record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
		aos_assert_r(mValueFieldIdx >= 0, false);
	}
	
	if (mHasRange)
	{
		mStartFieldIdx = record->getFieldIdx(mStartFieldName, rdata.getPtrNoLock());
		aos_assert_r(mStartFieldIdx >= 0, false);

		mEndFieldIdx = record->getFieldIdx(mEndFieldName, rdata.getPtrNoLock());
		aos_assert_r(mEndFieldIdx >= 0, false);
	}

	return true;
}


bool
AosDataProcRange::resolveDataProc(
		const AosRundataPtr &rdata,
		const AosDataRecordObjPtr &input_record,
		const AosDataRecordObjPtr &output_record)
{
	// Ketty 2013/12/20
	aos_assert_r(input_record, false);
	aos_assert_r(output_record, false);

	int idx = -1;
	for(u32 i=0; i<mKeyFields.size(); i++)
	{
		if (!mKeyFields[i].mIsRange)
		{
			idx = input_record->getFieldIdx(mKeyFields[i].mKeyFieldName, rdata.getPtrNoLock());
			aos_assert_r(idx >= 0, false);

			mKeyFields[i].mKeyFieldIdx = idx;
		}
	}

	aos_assert_r(mValueFieldName != "", false);
	mValueFieldIdx = input_record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
	aos_assert_r(mValueFieldIdx >= 0, false);

	if (mHasRange)
	{
		mStartFieldIdx = input_record->getFieldIdx(mStartFieldName, rdata.getPtrNoLock());
		aos_assert_r(mStartFieldIdx >= 0, false);

		mEndFieldIdx = input_record->getFieldIdx(mEndFieldName, rdata.getPtrNoLock());
		aos_assert_r(mEndFieldIdx >= 0, false);
	}
	
	aos_assert_r(mOutputFieldName != "", false);
	mOutputFieldIdx = output_record->getFieldIdx(mOutputFieldName, rdata.getPtrNoLock());
	aos_assert_r(mOutputFieldIdx >= 0, false);
	
	return true;
}


