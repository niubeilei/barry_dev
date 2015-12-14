////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProcDyn/DLLDataProcStatBasic.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ConditionObj.h"
#include "TransBasic/Trans.h"
#include "Util/DirDesc.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


bool AosRunDLL(const AosRundataPtr &rdata, const AosXmlTagPtr &sdoc) 
{
	aos_assert_rr(sdoc, rdata, false);

	try
	{
		AosDLLObjPtr obj = OmnNew AosDLLDataProcStatBasic(rdata, sdoc);
		aos_assert_rr(obj, rdata, false);
		rdata->setDLLObj(obj);
		return true;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed running DLLDataProcStatBasic DLL function");
		return false;
	}
}


AosDLLDataProcStatBasic::AosDLLDataProcStatBasic(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc)
:
AosDataProc(AOSDATAPROC_DLLPROC, AosDataProcId::eDLLProc, false),
AosDLLObj(AosDLLObj::eDLLDataProc)
{
	if (!config(rdata, sdoc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDLLDataProcStatBasic::~AosDLLDataProcStatBasic()
{
}


bool
AosDLLDataProcStatBasic::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
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


AosDataProcStatus::E 
AosDLLDataProcStatBasic::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mIILAssembler, AosDataProcStatus::eError);

	// 1. Retrieve the value
	AosValueRslt value_rslt;
	bool rslt = record->getFieldValue(mValueFieldIdx, value_rslt, rdata);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	u64 value;
	rslt = value_rslt.getU64Value(value, rdata);
	aos_assert_rr(rslt, rdata, AosDataProcStatus::eError);

	OmnString key_str;
	key_str << mStatType;
	AosValueRslt key_rslt;
	for (u32 i=0; i<mKeyFieldIdxArray.size(); i++)
	{
		int key_field_idx = mKeyFieldIdxArray.at(i);
		rslt = record->getFieldValue(key_field_idx, key_rslt, rdata);
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

		rslt = mIILAssembler->appendEntry(vv_rslt, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (mTimeFieldIdx >= 0)
		{
			AosValueRslt time_value;
			rslt = record->getFieldValue(mTimeFieldIdx, time_value, rdata);
			char *data = (char *)key_str.data();
			data[0] = mStatType;
			int len = key_str.length();

			key_str << mSep << time_value.getValueStr1();

			vv_rslt.setKeyValue(key_str.data(), key_str.length(), false, value);
			rslt = mIILAssembler->appendEntry(vv_rslt, rdata);
			aos_assert_r(rslt, AosDataProcStatus::eError);
			key_str.setLength(len);
		}
	}

	return AosDataProcStatus::eContinue;
}


bool
AosDLLDataProcStatBasic::resolveDataProc(
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
		idx = record->getFieldIdx(mKeyFieldNameArray[i], rdata);
		aos_assert_r(idx >= 0, false);
		mKeyFieldIdxArray.push_back(idx);
	}

	mValueFieldIdx = record->getFieldIdx(mValueFieldName, rdata);
	aos_assert_r(mValueFieldIdx >= 0, false);

	mTimeFieldIdx = record->getFieldIdx(mTimeFieldName, rdata);
	aos_assert_r(mTimeFieldIdx >= 0, false);
	
	return true;
}

