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
// 2015/09/20 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "Synchers/SyncherMap.h"

#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/JqlUtil.h"
#include "Util/Buff.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataRecordBuff.h"
#include "Rundata/Rundata.h"
#include "SEUtil/SeTypes.h"
#include "JimoCall/Ptrs.h"
#include "JimoCall/JimoCall.h"
#include "JimoAPI/JimoSynchers.h"

#include "Synchers/SyncherSumMapExecutor.h"
#include "Synchers/SyncherCountMapExecutor.h"
#include "Synchers/SyncherMaxMapExecutor.h"
#include "Synchers/SyncherMinMapExecutor.h"
#include "Synchers/SyncherIndexExecutor.h"

#include <boost/make_shared.hpp>

using AosConf::DataRecordCtnr;
using AosConf::DataRecordBuff;
using boost::shared_ptr;
using boost::make_shared;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSyncherMap(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSyncherMap(version);
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


AosSyncherMap::AosSyncherMap(const int version)
:
AosSyncher(version), 
mLock(OmnNew OmnMutex()),
mDeltaBeansBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin))
{
}


AosSyncherMap::~AosSyncherMap()
{
}


bool
AosSyncherMap::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	//<map zky_type="iilmap" zky_name="map_01" zky_iilname="_zt44_map_t_idx_map_01" datatype="string" zky_needsplit="true" zky_sep="0x02" zky_needswap="false" zky_use_key_as_value="false" zky_public_doc="true" zky_public_ctnr="true" zky_pctrs="zky__zt_map" zky_objid="_zt4g_map_map_01">
	//	<keys>
	//		<key>key_field1</key>
	//	</keys>
	//	<values>
	//		<value agrtype="sum" max_len="50" type="expr"><![CDATA[key_field3]]></value>
	//	</values>
	//</map>
	aos_assert_r(def, false);
	mIILName = def->getAttrStr("zky_iilname");
	aos_assert_r(mIILName != "", false);
	
	mMapKeyIILName = def->getAttrStr("zky_key_iil_name");

	mMapValueType = AosDataType::toEnum(def->getAttrStr("datatype"));
	aos_assert_r(mMapValueType != AosDataType::eInvalid, false);

	// iilmap keys
	OmnString msg = "";
	AosXmlTagPtr keysNode = def->getFirstChild("keys");
	aos_assert_r(keysNode, false);
	AosXmlTagPtr keyNode = keysNode->getFirstChild("key");
	OmnString keytype = "";
	while (keyNode)
	{
		keytype = keyNode->getAttrStr("type");
		OmnString key = keyNode->getNodeText();
		aos_assert_r(key != "", false);

		key << ";";
		AosExprObjPtr expr = AosParseExpr(key, msg, rdata);
		aos_assert_r(expr, false);
		mInputKeys.push_back(expr);

		keyNode = keysNode->getNextChild("key");
	}
	mIILMapKeyType = AosDataFieldType::eStr;
	AosXmlTagPtr valuesNode = def->getFirstChild("values");
	aos_assert_r(valuesNode, false);
	AosXmlTagPtr valueNode = valuesNode->getFirstChild("value");
	aos_assert_r(valueNode, false);
	
	mIILMapValueAgrType = AosFieldOpr::toEnum(valueNode->getAttrStr("agrtype"));
	OmnString valuetype = valueNode->getAttrStr("type");
	OmnString value = valueNode->getNodeText();
	value << ";";
	mInputValue = AosParseExpr(value, msg, rdata);
	aos_assert_r(mInputValue, false);

	//condition
	AosXmlTagPtr condNode = def->getFirstChild("cond");
	if (condNode)
	{
		OmnString cond = condNode->getNodeText();
		cond << ";";
		mCondition = AosParseExpr(cond, msg, rdata);
		aos_assert_r(mCondition, false);
	}

	return createOutput(mIILName, rdata);
}


bool
AosSyncherMap::createOutput(const OmnString &dpname, AosRundata *rdata)
{
	boost::shared_ptr<DataRecordCtnr> ctnr = boost::make_shared<DataRecordCtnr>();
	ctnr->setAttribute("zky_name", "output");
	boost::shared_ptr<DataRecordBuff> dr = boost::make_shared<DataRecordBuff>();
	dr->setAttribute("zky_name", dpname);
	dr->setField("zky_operator", "str", 50);
	dr->setField("key", "str", 50);
	dr->setField("docid", "bin_u64", sizeof(u64));
	ctnr->setRecord(dr);

	AosXmlTagPtr rcd_xml = AosXmlParser::parse(ctnr->getConfig() AosMemoryCheckerArgs);
	aos_assert_r(rcd_xml, false);
	mOutputRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata AosMemoryCheckerArgs);
	aos_assert_r(mOutputRecord, false);

	return true;
}


bool 
AosSyncherMap::proc()
{
	OmnScreen << "Processing Syncher: SyncherMap" << endl;
	aos_assert_r(mRundata, false);
	
	AosDatasetObjPtr ds = getSyncherDataset(mRundata.getPtr(), mDatasetDef, mDeltasBuff);
	aos_assert_r(ds, false);

	return proc(mRundata.getPtr(), ds);
}


AosBuffPtr
AosSyncherMap::serializeToBuff()
{
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	buff->setOmnStr(mIILName);
	buff->setOmnStr(mDatasetDef->toString());
	buff->setAosBuff(mDeltasBuff);
	
	return buff;
}


bool
AosSyncherMap::serializeFromBuff(const AosBuffPtr &buff)
{
	mIILName = buff->getOmnStr("");
	aos_assert_r(mIILName != "", false);

	OmnString conf = buff->getOmnStr("");                          
	aos_assert_r(conf != "", false);                               
	mDatasetDef = AosXmlParser::parse(conf AosMemoryCheckerArgs);
	aos_assert_r(mDatasetDef, false);

	mDeltasBuff = buff->getAosBuff(true AosMemoryCheckerArgs);
	aos_assert_r(mDeltasBuff, false);

	return true;
}


AosJimoPtr 
AosSyncherMap::cloneJimo() const
{
	return OmnNew AosSyncherMap(*this);
}


bool	
AosSyncherMap::proc(
		AosRundata *rdata, 
		const AosDatasetObjPtr &dataset)
{
	AosDataRecordObj *input_record;
	while (1)
	{
		bool rslt = dataset->nextRecord(rdata, input_record);
		aos_assert_r(rslt, false);
		if (!input_record) break;

		rslt = procData(rdata, input_record);
		aos_assert_r(rslt, false);
	}
		
	return true;
}


bool
AosSyncherMap::procData(
		AosRundata *rdata, 
		AosDataRecordObj *input_record)	
{
	bool rslt, outofmem;
	AosValueRslt key_rslt, docid_rslt;

	// 1. condition
	if (mCondition)
	{
		AosValueRslt value_rslt;
		mCondition->getValue(rdata, input_record, value_rslt);
		rslt = value_rslt.getBool();
		if (!rslt) return true;
	}

	// 2. get operator type 
	AosDeltaBeanOpr::E opr = input_record->getOperator();
	aos_assert_r(AosDeltaBeanOpr::isValid(opr), false);
	AosValueRslt opr_v(AosDeltaBeanOpr::toStr(opr));

	// 3. get key_values
	OmnString key_values = "";
	for (u32 i = 0; i < mInputKeys.size(); i++)
	{
		mInputKeys[i]->setIsParsedFieldIdx(false);
		rslt = mInputKeys[i]->getValue(rdata, input_record, key_rslt);
		aos_assert_r(rslt, false);
		
		if (i > 0) key_values << char(0x01);

		if (key_rslt.isNull())
		{
			//return AosDataProcStatus::eContinue;
			key_values << '\b';		//key1\0x01\0x08
		}
		else 
		{
			key_values << key_rslt.getStr();		//key1\0x01key2
		}
	}
	if (key_values == "") return true;

	// 4. get value
	AosValueRslt value_rslt;
	rslt = mInputValue->getValue(rdata, input_record, value_rslt);
	aos_assert_r(rslt, false);

	// 5. generate delta bean 
	mOutputRecord->clear();
	AosDataRecordObj *output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, false);

	if (!AosFieldOpr::isValid(mIILMapValueAgrType))
	{
		if (value_rslt.isNull()) return true; 		
		if (AosJqlUtil::isStr2StrMap(value_rslt.getType()))
		{
			key_values << char(0x02) << value_rslt.getStr();
			value_rslt.setU64(0);
		}
	}
	else if (AosFieldOpr::eSum == mIILMapValueAgrType)
	{
		if (!AosDataType::isNumericType(value_rslt.getType())) return true;
		rslt = repairValue(rdata, opr,	value_rslt);
		aos_assert_r(rslt, false);
		opr_v.setStr(AosDeltaBeanOpr::toStr(AosDeltaBeanOpr::eIncrement));
	}
	else if (AosFieldOpr::eCount == mIILMapValueAgrType)
	{
		opr_v.setStr(AosDeltaBeanOpr::toStr(AosDeltaBeanOpr::eIncrement));
		if (mInputValue->isConstant())	// count *
		{
			value_rslt.setU64(1);
		}
		else // count one field
		{
			//rslt = mInputValue->getValue(rdata, input_record, value_rslt);
			//aos_assert_r(rslt, false);
			if (value_rslt.isNull()) return true;
			value_rslt.setU64(1);
		}
	}
	else if (AosFieldOpr::eMaximum == mIILMapValueAgrType 
			|| AosFieldOpr::eMinimum == mIILMapValueAgrType)
	{
		if (!AosDataType::isNumericType(value_rslt.getType())) return true;
		//rslt = repairValue(rdata, opr,	value_rslt);
		aos_assert_r(rslt, false);
	}

	rslt = output_record->setFieldValue(0, opr_v, outofmem, rdata);
	aos_assert_r(rslt && (!outofmem), false);

	AosValueRslt k_v(key_values);
	rslt = output_record->setFieldValue(1, k_v, outofmem, rdata);
	aos_assert_r(rslt && (!outofmem), false);

	rslt = output_record->setFieldValue(2, value_rslt, outofmem, rdata);
	aos_assert_r(rslt && (!outofmem), false);

	appendDeltaBean(rdata, output_record);
	return true;
}


bool 
AosSyncherMap::flushDeltaBeans(AosRundata *rdata)
{
	mLock->lock();

	if (mDeltaBeansBuff->dataLen() <= 0)
	{
		mLock->unlock();
		return true;
	}

	AosSyncherObjPtr sync = getExecutor(rdata);
	Jimo::jimoSendSyncher(rdata, sync.getPtr());
	mDeltaBeansBuff->clear();

	mLock->unlock();

	return true;
}


bool 
AosSyncherMap::appendDeltaBean(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	aos_assert_r(record, false);

	int recordlen = record->getRecordLen();
	AosBuff::encodeRecordBuffLength(recordlen);

	mLock->lock();
	mDeltaBeansBuff->setInt(recordlen);
	mDeltaBeansBuff->setBuff(record->getData(rdata), record->getRecordLen());
	mLock->unlock();

	return true;
}


AosXmlTagPtr
AosSyncherMap::generatorIndexDatasetConf(AosRundata *rdata)
{
	OmnString str;
	str << "<dataset jimo_objid=\"dataset_syncher_jimodoc_v0\" zky_name=\"output\" >"
		<<  "<datarecord type=\"buff\" zky_name=\"output\">"
		<<   "<datafields>"
		<<    "<datafield type=\"str\" zky_name=\"zky_operator\"/>"
		<<    "<datafield type=\"str\" zky_name=\"key\"/>"
		<<    "<datafield type=\"bin_int64\" zky_name=\"docid\"/>"
		<<   "</datafields>"
		<<  "</datarecord>"
		<< "</dataset>";
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}


bool
AosSyncherMap::repairValue(
		AosRundata *rdata, 
		const AosDeltaBeanOpr::E &opr,
		AosValueRslt &value)
{
	aos_assert_r(mMapValueType != AosDataType::eInvalid, false);
	if (opr == AosDeltaBeanOpr::eAdd)
	{
		return true;
	}
	else if (opr == AosDeltaBeanOpr::eDelete)
	{
		value.setDouble(value.getDouble() * (-1));
		return true;
	}
	else if (opr == AosDeltaBeanOpr::eModify)
	{
		OmnNotImplementedYet;	
	}
	else 
	{
		OmnAlarm << enderr;
	}

	return true;
}


AosSyncherObjPtr
AosSyncherMap::getExecutor(AosRundata *rdata)
{
	AosXmlTagPtr dsconf = generatorIndexDatasetConf(rdata);
	aos_assert_r(dsconf, 0);
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	AosSyncherObjPtr sync;
	switch (mIILMapValueAgrType)	
	{
	case AosFieldOpr::eInvalid:
		sync = OmnNew AosSyncherIndexExecutor(0);
		break;

	case AosFieldOpr::eSum:
		sync = OmnNew AosSyncherSumMapExecutor(0);
		break;

	case AosFieldOpr::eCount:
		sync = OmnNew AosSyncherCountMapExecutor(0);
		break;

	case AosFieldOpr::eMaximum:
		sync = OmnNew AosSyncherMaxMapExecutor(0);
		buff->setOmnStr(mMapKeyIILName);
		buff->setOmnStr(AosDataType::getTypeStr(mMapValueType));
		break;

	case AosFieldOpr::eMinimum:
		sync = OmnNew AosSyncherMinMapExecutor(0);
		buff->setOmnStr(mMapKeyIILName);
		buff->setOmnStr(AosDataType::getTypeStr(mMapValueType));
		break;

	default:
		OmnAlarm << enderr;
		break;
	}
	aos_assert_r(sync, 0);

	buff->setOmnStr(mIILName);
	buff->setOmnStr(dsconf->toString());
	buff->setAosBuff(mDeltaBeansBuff);
	buff->reset();
	sync->serializeFromBuff(buff);

	return sync;
}

