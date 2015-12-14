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
// 2013/03/18 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcStatComp.h"

#include "API/AosApi.h"
#include "CounterUtil/CounterUtil.h"


AosDataProcStatComp::AosDataProcStatComp(const bool flag)
:
AosDataProc(AOSDATAPROC_STATCOMP, AosDataProcId::eStatComp, flag),
mStatType(AosStatType::eInvalid),
mTimeGran(AosTimeGran::eInvalid),
mTimeFieldIdx(-1),
mValueFieldIdx(-1),
mReserveNum(0),
mOutputFieldIdx(-1)		// Ketty 2014/01/09
{
}


AosDataProcStatComp::AosDataProcStatComp(const AosDataProcStatComp &proc)
:
AosDataProc(proc),
mKeyFields(proc.mKeyFields),
mStatType(proc.mStatType),
mTimeGran(proc.mTimeGran),
mTimeFieldIdx(proc.mTimeFieldIdx),
mSep(proc.mSep),
mValueFieldName(proc.mValueFieldName),
mValueFieldIdx(proc.mValueFieldIdx),
mReserveNum(proc.mReserveNum),
mOutputFieldIdx(proc.mOutputFieldIdx)
{
}


AosDataProcStatComp::~AosDataProcStatComp()
{
}


bool
AosDataProcStatComp::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	
	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_r(rslt, false);
	
	mOutputFieldName = def->getAttrStr("zky_output_field_name");

	mValueFieldName = def->getAttrStr("zky_value_field_name");
	if(mValueFieldName == "") mNeedDocid = true;
		
	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	AosConvertAsciiBinary(mSep);
	
	AosXmlTagPtr entry = def->getFirstChild();
	aos_assert_r(entry, false);

	OmnString key_field_name;
	OmnString sep;
	AosConditionObjPtr filter;
	AosXmlTagPtr tag;
	AosCondType::E type;
	while(entry)
	{
		key_field_name = entry->getAttrStr(AOSTAG_KEYFIELDNAME);
		aos_assert_r(key_field_name != "", false);

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
		cp.mFilter = filter;
		
		mKeyFields.push_back(cp);	
		entry = def->getNextChild();
	}
	
	aos_assert_r(mKeyFields.size() > 0, false);

	OmnString stattype = def->getAttrStr("zky_stattype");
	mStatType = AosStatType::toEnum(stattype);
	if (!AosStatType::isValid(mStatType))
	{
		AosSetErrorU(rdata, "missing stat_type") << ": " 
			<< def->toString() << enderr;
		return false;
	}

	OmnString timegran = def->getAttrStr(AOSTAG_TIMEGRAN);
	mTimeGran = AosTimeGran::toEnum(timegran); 
	if (!AosTimeGran::isValid(mTimeGran))
	{
		AosSetErrorU(rdata, "missing time_gran") << ": " 
			<< def->toString() << enderr;
		return false;
	}
	
	OmnString timefieldname = def->getAttrStr("zky_time_field_name");
	if(timefieldname != "")
	{
		if(mTimeGran == AosTimeGran::eNoTime)
		{
			timefieldname = "";	
		}
		else
		{
			bool found = false;
			for(u32 i=0; i<mKeyFields.size(); i++)
			{
				if(mKeyFields[i].mKeyFieldName == timefieldname)
				{
					mTimeFieldIdx = i;
					found = true;
					break;
				}
			}
			if(!found)
			{
				AosSetErrorU(rdata, "missing timefield") << ": " 
					<< def->toString() << enderr;
				return false;
			}
		}
	}

	mReserveNum = def->getAttrU64("zky_reserve_num", 0);
	if(mReserveNum > 0)
	{
		if(mReserveNum >= (int)mKeyFields.size())
		{
			AosSetErrorU(rdata, "invalid_reserve_num") << ": " 
				<< def->toString() << enderr;
			return false;
		}
	}
	return true;
}


AosDataProcStatus::E
AosDataProcStatComp::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	OmnAlarm << enderr;
	/*
	aos_assert_r(record, AosDataProcStatus::eError);
	aos_assert_r(mIILAssembler, AosDataProcStatus::eError);
		
	int size = mKeyFields.size();
	aos_assert_r(size > 0, AosDataProcStatus::eError);

	bool rslt = true;
	OmnString rslt_key;
	vector<OmnString> rsltKeys;
	for(int i=0; i<size; i++)
	{
		AosValueRslt key_rslt;
		rslt = record->getFieldValue(mKeyFields[i].mKeyFieldIdx, key_rslt, false, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (mKeyFields[i].mFilter
				&& !mKeyFields[i].mFilter->evalCond(key_rslt, rdata))
		{
			return AosDataProcStatus::eContinue;
		}

		rslt_key = key_rslt.getStr();
		rslt_key.normalizeWhiteSpace(true, true);  // trim
		if(rslt_key == "") return AosDataProcStatus::eContinue;
		
		rsltKeys.push_back(rslt_key);

		//key << mKeyFields[i].mSep << rslt_key;
	}

	u64 value = docid;
	if (mValueFieldIdx >= 0)
	{
		AosValueRslt val_rslt;
		rslt = record->getFieldValue(mValueFieldIdx, val_rslt, false, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);
		
		rslt = val_rslt.getU64();
		if (!rslt) return AosDataProcStatus::eError;
	}

	int time_field_idx = mTimeFieldIdx;
	AosTimeGran::E timeGran = mTimeGran;
		
//	OmnScreen << "===================================================" << endl;
//	OmnScreen << "datacolid:" << mDataColId << " mReserveNum:" << mReserveNum << endl;
	while (1)
	{
		OmnString key = "";
		int append_bit = (time_field_idx == 0) ? 0 : 1;
		u16 head = AosCounterUtil::composeTimeEntryType(rsltKeys.size(), append_bit, mStatType, timeGran);
		aos_assert_r(head, AosDataProcStatus::eError);
		
		key.assign((char *)&head, sizeof(u16));
		key << rsltKeys[0];
		for(u32 i=1; i<rsltKeys.size(); i++)
		{
			key << mSep << rsltKeys[i];	
		}

		AosValueRslt value_rslt;
		//value_rslt.setKeyValue(key.data(), key.length(), false, value);

//		OmnScreen << " datacolid:" << mDataColId
//				  << " append entry:" << &key.data()[2]
//				  << " level:" << rsltKeys.size()
//				  << " append_bit:" << append_bit
//				  << " timeGran:" << AosTimeGran::toChar(timeGran) << endl;
		rslt = mIILAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);

		int remove_idx = (int)rsltKeys.size() - mReserveNum - 1;
		if (remove_idx < 0 || (remove_idx == 0 && rsltKeys.size() == 1)) break;

		if (remove_idx == time_field_idx)
		{
			timeGran = AosTimeGran::eNoTime;
			time_field_idx = -1;
		}
		else if (time_field_idx > remove_idx)
		{	
			time_field_idx--;
		}
		rsltKeys.erase(rsltKeys.begin() + remove_idx);
	}

	return AosDataProcStatus::eContinue;
}


AosDataProcStatus::E
AosDataProcStatComp::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	return AosDataProcStatus::eContinue;
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
	OmnString rslt_key;
	vector<OmnString> rsltKeys;
	for(int i=0; i<size; i++)
	{
		AosValueRslt key_rslt;
		rslt = input_record->getFieldValue(mKeyFields[i].mKeyFieldIdx, key_rslt, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (mKeyFields[i].mFilter
				&& !mKeyFields[i].mFilter->evalCond(key_rslt, rdata_raw))
		{
			return AosDataProcStatus::eContinue;
		}

		rslt_key = key_rslt.getStr();
		rslt_key.normalizeWhiteSpace(true, true);  // trim
		if(rslt_key == "") return AosDataProcStatus::eContinue;
		
		rsltKeys.push_back(rslt_key);

		//key << mKeyFields[i].mSep << rslt_key;
	}

	u64 value = 0;
	AosValueRslt val_rslt;
	rslt = input_record->getFieldValue(mValueFieldIdx, val_rslt, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
		
	rslt = val_rslt.getU64();
	if (!rslt) return AosDataProcStatus::eError;

	int time_field_idx = mTimeFieldIdx;
	AosTimeGran::E timeGran = mTimeGran;
		
//	OmnScreen << "===================================================" << endl;
//	OmnScreen << "datacolid:" << mDataColId << " mReserveNum:" << mReserveNum << endl;
	while (1)
	{
		OmnString key = "";
		int append_bit = (time_field_idx == 0) ? 0 : 1;
		u16 head = AosCounterUtil::composeTimeEntryType(rsltKeys.size(), append_bit, mStatType, timeGran);
		aos_assert_r(head, AosDataProcStatus::eError);
		
		key.assign((char *)&head, sizeof(u16));
		key << rsltKeys[0];
		for(u32 i=1; i<rsltKeys.size(); i++)
		{
			key << mSep << rsltKeys[i];	
		}

		AosValueRslt value_rslt;
		value_rslt.setKeyValue(key.data(), key.length(), false, value);

//		OmnScreen << " datacolid:" << mDataColId
//				  << " append entry:" << &key.data()[2]
//				  << " level:" << rsltKeys.size()
//				  << " append_bit:" << append_bit
//				  << " timeGran:" << AosTimeGran::toChar(timeGran) << endl;
		
		output_record->setDocid(value);
		bool outofmem = false;
		output_record->setFieldValue(mOutputFieldIdx, value_rslt, outofmem, rdata_raw);
		
		//rslt = mAssembler->appendEntry(value_rslt, rdata_raw);
		//aos_assert_r(rslt, AosDataProcStatus::eError);

		int remove_idx = (int)rsltKeys.size() - mReserveNum - 1;
		if (remove_idx < 0 || (remove_idx == 0 && rsltKeys.size() == 1)) break;

		if (remove_idx == time_field_idx)
		{
			timeGran = AosTimeGran::eNoTime;
			time_field_idx = -1;
		}
		else if (time_field_idx > remove_idx)
		{	
			time_field_idx--;
		}
		rsltKeys.erase(rsltKeys.begin() + remove_idx);
	}

	*/
	return AosDataProcStatus::eContinue;
}

AosDataProcObjPtr
AosDataProcStatComp::cloneProc()
{
	return OmnNew AosDataProcStatComp(*this);
}


AosDataProcObjPtr
AosDataProcStatComp::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcStatComp * proc = OmnNew AosDataProcStatComp(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}
 
bool
AosDataProcStatComp::resolveDataProc(
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
		idx = record->getFieldIdx(mKeyFields[i].mKeyFieldName, rdata.getPtrNoLock());
		aos_assert_r(idx >= 0, false);

		mKeyFields[i].mKeyFieldIdx = idx;
	}
	
	if(mValueFieldName != "")
	{
		mValueFieldIdx = record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
		aos_assert_r(mValueFieldIdx >= 0, false);
	}

	return true;
}

bool
AosDataProcStatComp::resolveDataProc(
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
		idx = input_record->getFieldIdx(mKeyFields[i].mKeyFieldName, rdata.getPtrNoLock());
		aos_assert_r(idx >= 0, false);

		mKeyFields[i].mKeyFieldIdx = idx;
	}
	
	aos_assert_r(mValueFieldName != "", false);
	mValueFieldIdx = input_record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
	aos_assert_r(mValueFieldIdx >= 0, false);

	aos_assert_r(mOutputFieldName != "", false);
	mOutputFieldIdx = output_record->getFieldIdx(mOutputFieldName, rdata.getPtrNoLock());
	aos_assert_r(mOutputFieldIdx >= 0, false);
	
	
	return true;
}


