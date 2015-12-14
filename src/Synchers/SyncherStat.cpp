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
// An Index Syncher does the following:
// 	For each input record:
// 		1. Update the IIL (i.e., add, delete, or modify)
// 		2. Collect the entry
// 	When finish the above loop, it constructs a SyncEvent:
// 		1. IILID
//	
// Modification History:
// 2015/11/13 Created by Andy 
////////////////////////////////////////////////////////////////////////////
#include "Synchers/SyncherStat.h"

#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/DataFieldType.h"
#include "SEUtil/SeTypes.h"
#include "Util/Buff.h"
#include "Util/DataTypes.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataRecordBuff.h"
#include "Rundata/Rundata.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/JqlUtil.h"
#include "SEUtil/ArithOpr.h"
#include "JimoCall/Ptrs.h"
#include "JimoCall/JimoCall.h"
#include "JimoAPI/JimoSynchers.h"
#include <boost/make_shared.hpp>


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosSyncherStat(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosSyncherStat(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}
		catch (...)
		{
			AosLogError(rdata, false, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosSyncherStat::AosSyncherStat(const int version)
:
AosSyncher(version), 
mLock(OmnNew OmnMutex())
{
}


AosSyncherStat::AosSyncherStat(const AosSyncherStat &rhs)
:
AosSyncher(rhs),
mLock(OmnNew OmnMutex()),
mKeys(rhs.mKeys),
mMeasures(rhs.mMeasures),
mTimeField(rhs.mTimeField),
mFormat(rhs.mFormat),
mTimeUnit(rhs.mTimeUnit)
{
}


AosSyncherStat::~AosSyncherStat()
{
}


bool
AosSyncherStat::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	//<statistic zky_stat_identify_key="k1" >
	//	<key_fields>
	//		<field field_name="k1"></field>
	//	</key_fields>
	//	<measures>
	//		<measure field_name="v1" agr_type="dist_count" zky_name="dist_count0x28v10x29" zky_data_type="int64" type"64"/>
	//	</measures>
	//	<time_field time_format ="yyyy-mm-dd" grpby_time_unit="_day" time_field_name="t1"/>
	//	<stat_key_conn/>
	//	<vt2d_conn time_bucket_weight="1000" start_time_slot="14610"/>
	//</statistic>

	AosXmlTagPtr key_fields = def->getFirstChild("key_fields");
	aos_assert_r(key_fields, false);

	AosXmlTagPtr field = key_fields->getFirstChild("field");
	while (field)
	{
		OmnString name = field->getAttrStr("field_name");
		aos_assert_r(name != "", false);
		mKeys.push_back(name);
		field = key_fields->getNextChild();
	}

	AosXmlTagPtr measures = def->getFirstChild("measures");
	aos_assert_r(measures, false);

	AosXmlTagPtr measure = measures->getFirstChild("measure");
	while (measure)
	{
		StatMeasure m;
		m.mFieldName = measure->getAttrStr("field_name");
		aos_assert_r(m.mFieldName != "", false);

		m.mEscapeName = measure->getAttrStr("zky_name");
		aos_assert_r(m.mEscapeName != "", false);

		OmnString agr_type = measure->getAttrStr("agr_type");
		m.mAggrType = AosAggregationType::toEnum(agr_type);
		aos_assert_r(AosAggregationType::isValid(m.mAggrType), false);

		OmnString data_type = measure->getAttrStr("zky_data_type");
		m.mDataType = AosDataType::toEnum(data_type);
		aos_assert_r(AosDataType::isValid(m.mDataType), false);

		measure = measures->getNextChild();
	}

	AosXmlTagPtr time_field = def->getFirstChild("time_field");
	aos_assert_r(time_field, false);
	mTimeField = time_field->getAttrStr("time_field_name");
	aos_assert_r(mTimeField != "", false);

	mFormat = time_field->getAttrStr("time_format");
	mTimeUnit = time_field->getAttrStr("grpby_time_unit");
	aos_assert_r(mTimeUnit != "", false);
	return true;
}


bool 
AosSyncherStat::proc()
{
	OmnShouldNeverComeHere;
	return false;
}


AosBuffPtr
AosSyncherStat::serializeToBuff()
{
	OmnShouldNeverComeHere;
	return NULL;
}


bool
AosSyncherStat::serializeFromBuff(const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


AosJimoPtr 
AosSyncherStat::cloneJimo() const
{
	return OmnNew AosSyncherStat(*this);
}


bool	
AosSyncherStat::proc(
		AosRundata *rdata, 
		const AosDatasetObjPtr &dataset)
{
	OmnShouldNeverComeHere;
	return true;
}


bool
AosSyncherStat::procData(
		AosRundata *rdata, 
		AosDataRecordObj *input_record)	
{
	///////////////////////////

	return true;
}

bool
AosSyncherStat::procData(
		AosRundata *rdata, 
		AosDataRecordObj **input_records)	
{
	bool rslt = false;
	AosDataRecordObj *record = input_records[0];
	if (!mOutputRecord)
	{
		rslt = createOutput(rdata, record);
		aos_assert_r(rslt, false);
	}

	AosDeltaBeanOpr::E opr = getDeltaBeanOpr(rdata, record);
	switch (opr)
	{
	case AosDeltaBeanOpr::eAdd:
		rslt = procDataByAdd(rdata, input_records);
		break;

	case AosDeltaBeanOpr::eModify:
		rslt = procDataByModify(rdata, input_records);
		break;

	case AosDeltaBeanOpr::eDelete:
		rslt = procDataByDelete(rdata, input_records);
		break;

	default:
		OmnShouldNeverComeHere;
		break;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSyncherStat::procDataByAdd(
		AosRundata *rdata, 
		AosDataRecordObj **input_records)	
{
	bool rslt = false;

	AosDataRecordObj* record = input_records[0];

	bool outofmem;
	AosValueRslt vv;
	for (u32 i = 0; i < mKeys.size(); i++)
	{
		rslt = record->getFieldValue(mKeys[i], vv, true, rdata);
		aos_assert_r(rslt, false);

		rslt = mOutputRecord->setFieldValue(mKeys[i], vv, outofmem, rdata);
		aos_assert_r(rslt, false);
	}

	for (u32 i = 0; i < mMeasures.size(); i++)
	{
		rslt = record->getFieldValue(mMeasures[i].mFieldName, vv, true, rdata);
		aos_assert_r(rslt, false);

		if (mMeasures[i].mAggrType == AosAggregationType::eCount)
		{
			if (vv.isNull())
				vv.setI64(0);
			else
				vv.setI64(1);
		}
		rslt = mOutputRecord->setFieldValue(AosJqlUtil::unescape(mMeasures[i].mEscapeName), vv, outofmem, rdata);
		aos_assert_r(rslt, false);
	}
	// flush record
	return true;
}

bool
AosSyncherStat::procDataByModify(
		AosRundata *rdata, 
		AosDataRecordObj **input_records)	
{
	bool rslt = false;

	AosDataRecordObj* old_record = input_records[0];
	AosDataRecordObj* new_record = input_records[1];

	bool outofmem;
	AosValueRslt old_vv, new_vv;
	for (u32 i = 0; i < mKeys.size(); i++)
	{
		rslt = new_record->getFieldValue(mKeys[i], new_vv, true, rdata);
		aos_assert_r(rslt, false);

		rslt = mOutputRecord->setFieldValue(mKeys[i], new_vv, outofmem, rdata);
		aos_assert_r(rslt, false);
	}

	for (u32 i = 0; i < mMeasures.size(); i++)
	{
		rslt = new_record->getFieldValue(mMeasures[i].mFieldName, new_vv, true, rdata);
		aos_assert_r(rslt, false);

		if (mMeasures[i].mAggrType == AosAggregationType::eSum)
		{
			rslt = old_record->getFieldValue(mMeasures[i].mFieldName, old_vv, true, rdata);
			aos_assert_r(rslt, false);

			AosDataType::E type = AosDataType::autoTypeConvert(old_vv.getType(), new_vv.getType());
			new_vv = AosValueRslt::doArith(ArithOpr::eDiv, type, new_vv, old_vv);
		}
		else if (mMeasures[i].mAggrType == AosAggregationType::eCount)
		{
			if (new_vv.isNull())
				new_vv.setI64(-1);
			else
				new_vv.setI64(0);
		}

		rslt = mOutputRecord->setFieldValue(AosJqlUtil::unescape(mMeasures[i].mEscapeName), new_vv, outofmem, rdata);
		aos_assert_r(rslt, false);
	}
	// flush record
	return true;
}


bool
AosSyncherStat::procDataByDelete(
		AosRundata *rdata, 
		AosDataRecordObj **input_records)	
{
	bool rslt = false;

	AosDataRecordObj* record = input_records[0];

	bool outofmem;
	AosValueRslt vv;
	for (u32 i = 0; i < mKeys.size(); i++)
	{
		rslt = record->getFieldValue(mKeys[i], vv, true, rdata);
		aos_assert_r(rslt, false);

		rslt = mOutputRecord->setFieldValue(mKeys[i], vv, outofmem, rdata);
		aos_assert_r(rslt, false);
	}

	for (u32 i = 0; i < mMeasures.size(); i++)
	{
		rslt = record->getFieldValue(mMeasures[i].mFieldName, vv, true, rdata);
		aos_assert_r(rslt, false);

		if (mMeasures[i].mAggrType == AosAggregationType::eSum)
		{
			AosValueRslt tmp_vv;
			tmp_vv.setI64(-1);
			vv = AosValueRslt::doArith(ArithOpr::eMul, vv.getType(), vv, tmp_vv);
		}
		else if (mMeasures[i].mAggrType == AosAggregationType::eCount)
		{
			if (vv.isNull())
				vv.setI64(0);
			else
				vv.setI64(-1);
		}
		rslt = mOutputRecord->setFieldValue(AosJqlUtil::unescape(mMeasures[i].mEscapeName), vv, outofmem, rdata);
		aos_assert_r(rslt, false);
	}
	// flush record
	return true;
}



bool 	
AosSyncherStat::createOutput(AosRundata *rdata, AosDataRecordObj *input_record)
{
	OmnString record_conf;
	record_conf << "<datarecord type=\"ctnr\" zky_name=\"syncher_stat\">"
		<< 	"<datarecord type=\"buff\" zky_name=\"syncher_stat\">"
		<<	 "<datafields>";

	for (u32 i = 0; i < mKeys.size(); i++)
	{
		int field_idx = input_record->getFieldIdx(mKeys[i], rdata);
		aos_assert_r(field_idx != -1, false);
		AosDataFieldObj *data_field = input_record->getFieldByIdx1(field_idx);
		AosDataFieldType::E type = data_field->getType();
		record_conf << "datafield type=\"" << AosDataFieldType::toString(type) <<"\" zky_name=\"" << mKeys[i] << "\"></datafield>";
	}

	for (u32 i = 0; i < mMeasures.size(); i++)
	{
		//int field_idx = input_record->getFieldIdx(mMeasures[i].mFieldName, rdata);
		//aos_assert_r(field_idx != -1, false);
		//AosDataFieldObj *data_field = input_record->getFieldByIdx1(field_idx);
		//AosDataFieldType type = data_field->getType();
		//record_conf << "datafield type=\"" << AosDataFieldType::toStr(type) <<"\" zky_name=\"" << mMeasures[i].mFieldName << "\"></datafield>"
		record_conf << "datafield type=\"" << AosDataFieldType::toString(dataTypeToDataFieldType(mMeasures[i].mDataType)) <<"\" zky_name=\"" << AosJqlUtil::unescape(mMeasures[i].mEscapeName) << "\"></datafield>";
	}

	if (mTimeField != "")
	{
		record_conf << "<datafield type=\"bin_int64\" zky_name=\"time\"></datafield>";
	}

	record_conf	<<	 "</datafields>"
		<<	"</datarecord>"
		<< "</datarecord>";

	AosXmlTagPtr xml = AosXmlParser::parse(record_conf AosMemoryCheckerArgs);
	mOutputRecord = AosDataRecordObj::createDataRecordStatic(xml, 0, rdata AosMemoryCheckerArgs);
	aos_assert_r(mOutputRecord, false);
	return true;
}


AosDataFieldType::E
AosSyncherStat::dataTypeToDataFieldType(const AosDataType::E type)
{
	switch (type)
	{
		case AosDataType::eInt64:
			return  AosDataFieldType::eBinInt64;
		case AosDataType::eU64: 
			return  AosDataFieldType::eBinU64;
		case AosDataType::eDouble:
			return  AosDataFieldType::eBinDouble;
		case AosDataType::eDateTime:
			return  AosDataFieldType::eBinDateTime;
		default:
			OmnAlarm << "not handle this data field type: " << type << enderr;
			return AosDataFieldType::eInvalid;
	}
	return AosDataFieldType::eInvalid;
}


bool 
AosSyncherStat::flushDeltaBeans(AosRundata *rdata)
{
	mLock->lock();

	mLock->unlock();
	return true;
}


bool 	
AosSyncherStat::proc(
		AosRundata *rdata,
		const AosXmlTagPtr &dsconf, 
		const AosBuffPtr &buff)
{
	//AosTransPtr trans = OmnNew AosGetSerLogicPidTrans(5113, 0);
	//AosBuffPtr resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	//bool rslt = sendTransNeedResp(rdata, trans, resp);
	//u32 logicid = resp->getU32(0);

	//AosXmlTagPtr dsconf = getDatasetConf(rdata);
	//trans = OmnNew AosSendSyncerDataTrans(0, logicid, dsconf->toString(),
	//		statname, databuff);
	//rslt = sendTransNeedResp(rdata, trans, resp);
	//aos_assert_r(rslt, false);

	return true;
}


bool
AosSyncherStat::sendTransNeedResp(
        AosRundata *rdata,
        const AosTransPtr &trans,
        AosBuffPtr &resp)
{
    bool timeout;
    bool rslt = AosSendTrans(rdata, trans, timeout, resp);
    if(!rslt || !resp)
    {
        rdata->setError() << "fail to addLogTrans: " << trans;
        OmnAlarm << rdata->getErrmsg() << enderr;
        return 0;
    }
    if (timeout)
    {
        rdata->setError() << "Timeout";
        OmnAlarm << rdata->getErrmsg() << enderr;
        return 0;
    }

    rslt = resp->getU8(0);
    aos_assert_r(rslt, false);

    return true;
}
