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
#include "Synchers/SyncherIndexExecutor.h"

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEUtil/SeTypes.h"
#include "Util/Buff.h"
#include "JSON/JSON.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Dataset/Jimos/DatasetSyncher.h"
#include "SEUtil/SeTypes.h"

#include "SEInterfaces/JimoCaller.h"
#include "JimoCall/Ptrs.h"
#include "JimoCall/JimoCall.h"
#include "JimoAPI/JimoSynchers.h"
#include "Synchers/SyncherNull.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSyncherIndexExecutor(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSyncherIndexExecutor(version);
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


AosSyncherIndexExecutor::AosSyncherIndexExecutor(const int version)
:
AosSyncher(version)
{
}


AosSyncherIndexExecutor::~AosSyncherIndexExecutor()
{
}


bool 
AosSyncherIndexExecutor::proc()
{
	OmnScreen << "Processing Syncher: SyncherIndexExecutor" << endl;
	aos_assert_r(mRundata, false);
	
	AosDatasetObjPtr ds = getSyncherDataset(mRundata.getPtr(), mDatasetDef, mDeltasBuff);
	aos_assert_r(ds, false);

	return proc(mRundata.getPtr(), ds);
}


AosBuffPtr
AosSyncherIndexExecutor::serializeToBuff()
{
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	buff->setOmnStr(mIILName);
	buff->setOmnStr(mDatasetDef->toString());
	buff->setAosBuff(mDeltasBuff);
	
	return buff;
}


bool
AosSyncherIndexExecutor::serializeFromBuff(const AosBuffPtr &buff)
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
AosSyncherIndexExecutor::cloneJimo() const
{
	return OmnNew AosSyncherIndexExecutor(*this);
}


bool 
AosSyncherIndexExecutor::updateIndex(
		AosRundata *rdata, 
		const AosDeltaBeanOpr::E opr, 
		const AosValueRslt &key_v,
		const u64 docid)
{
	AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
	aos_assert_r(iilmgr, false);

	u64 iilid = iilmgr->getIILID(mIILName, true, rdata);
	u32 siteid = rdata->getSiteid();

	AosIILType iiltype = getIILType(key_v.getType());
	AosIILObjPtr iilobj = iilmgr->getIILPublic(iilid, siteid, 0, iiltype, rdata);
	aos_assert_r(iilobj, false);

	AosValueRslt old_vaule;
	bool rslt = false;
	switch (opr)
	{
	case AosDeltaBeanOpr::eAdd:
		 rslt = addIndex(rdata, iilobj, key_v, docid, false, false);
		 break;

	case AosDeltaBeanOpr::eDelete:
		 rslt = deleteIndex(rdata, iilobj, key_v, docid);
		 break;
	
	case AosDeltaBeanOpr::eModify:
		 break;
	
	default:
		 OmnAlarm << enderr;
	}
	aos_assert_r(rslt, false);

	return true;
}


bool	
AosSyncherIndexExecutor::proc(
		AosRundata *rdata, 
		const AosDatasetObjPtr &dataset)
{
	//AosRecordsetObjPtr recordset = dataset->getRecordset(); 
	//aos_assert_r(recordset, false);
	
	AosDataRecordObj *input_record;
	//AosDataRecordObj *output_record;

	bool rslt = false;
	while (1)
	{
		rslt = dataset->nextRecord(rdata, input_record);
		aos_assert_r(rslt, false);
		if (!input_record) break;

		rslt = procData(rdata, input_record);
		aos_assert_r(rslt, false);
	}
		
	// call back 
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	buff->setOmnStr("Young, jimocall testing ....");
	buff->reset();

	AosSyncherObj *sync = OmnNew AosSyncherNull();
	sync->serializeFromBuff(buff);
	Jimo::jimoSendSyncher(rdata, sync);
	return true;
}


bool
AosSyncherIndexExecutor::procData(
		AosRundata *rdata, 
		AosDataRecordObj *record)	
{
	aos_assert_r(record, false);

	AosValueRslt key_v, docid_v;
	// 1. get key value 
	bool rslt = record->getFieldValue("key", key_v, false, rdata);
	aos_assert_r(rslt, false);

	// 2. get docid value
	rslt = record->getFieldValue("docid", docid_v, false, rdata);
	aos_assert_r(rslt, false);

	// 3. get current operator
	AosDeltaBeanOpr::E opr = getDeltaBeanOpr(rdata, record);
	aos_assert_r(AosDeltaBeanOpr::isValid(opr), false); 
		
	// 4. update index
	u64 docid = docid_v.getU64();
	rslt = updateIndex(rdata, opr, key_v, docid);
	aos_assert_r(rslt, false);

	return true;
}


AosIILType 
AosSyncherIndexExecutor::getIILType(const AosDataType::E type)
{
	if (AosDataType::isNumericType(type))
	{
		return eAosIILType_U64;
	}
	else if (AosDataType::isCharacterType(type))
	{
		return eAosIILType_Str;
	}
	else
	{
		OmnAlarm << "type: " << AosDataType::getTypeStr(type) << enderr;
	}
	
	return eAosIILType_Invalid;
}


bool 
AosSyncherIndexExecutor::addIndex(
		AosRundata *rdata, 
		const AosIILObjPtr &iilobj,
		const AosValueRslt &vv, 
		const u64 &docid, 
		const bool value_unique,                          
		const bool docid_unique)
{
	aos_assert_r(iilobj, false);

	AosDataRecordObjPtr output_record;
	if (AosDataType::isNumericType(vv.getType()))
	{
		return iilobj->addDocSafe(vv.getU64(), docid, false, false, rdata);
	}
	else if (AosDataType::isCharacterType(vv.getType()))
	{
		return iilobj->addDocSafe(vv.getStr(), docid, false, false, rdata);
	}
	else 
	{
		OmnAlarm << "type error!" << enderr;
	}
	
	return false;
}


bool
AosSyncherIndexExecutor::deleteIndex(
		AosRundata *rdata, 
		const AosIILObjPtr &iilobj,
		const AosValueRslt &vv,
		const u64 &docid)
{
	aos_assert_r(iilobj, false);

	if (AosDataType::isNumericType(vv.getType()))
	{
		return iilobj->removeDocSafe(vv.getU64(), docid, rdata);
	}
	else if (AosDataType::isCharacterType(vv.getType()))
	{
		return iilobj->removeDocSafe(vv.getStr(), docid, rdata);
	}
	else 
	{
		OmnAlarm << "type error!" << enderr;
	}

	return false;
}


bool 
AosSyncherIndexExecutor::modifyIndex(
		AosRundata *rdata, 
		const AosIILObjPtr &iilobj,
		const AosValueRslt &oldvalue, 
		const AosValueRslt &newvalue, 
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique)
{
	aos_assert_r(iilobj, false);
	
	if (AosDataType::isNumericType(oldvalue.getType()))
	{
		return iilobj->modifyDocSafe(oldvalue.getU64(), 
				newvalue.getU64(), docid, value_unique, docid_unique, rdata);
	}
	else if (AosDataType::isCharacterType(oldvalue.getType()))
	{
		return iilobj->modifyDocSafe(oldvalue.getStr(), 
				newvalue.getStr(), docid, value_unique, docid_unique, rdata);
	}
	else 
	{
		OmnAlarm << "type error!" << enderr;
	}

	return false;
}


bool
AosSyncherIndexExecutor::generatorOutput(
		AosRundata *rdata, 
		const AosDeltaBeanOpr::E opr, 
		const AosValueRslt &oldvalue, 
		const AosValueRslt &newvalue, 
		const u64 &docid)
{
	//AosDataRecordObjPtr output_record;
	//bool rslt, outofmem;
	//AosValueRslt opr_v(AosDeltaBeanOpr::toStr(opr));
	//AosValueRslt iilname_v(mIILName);

	//rslt = output_record->setFieldValue(0, iilname_v, outofmem, rdata);
	//aos_assert_r(rslt && (!outofmem), false);
	//rslt = output_record->setFieldValue(1, opr_v, outofmem, rdata);
	//aos_assert_r(rslt && (!outofmem), false);
	//rslt = output_record->setFieldValue(2, newvalue, outofmem, rdata);
	//aos_assert_r(rslt && (!outofmem), false);
	//rslt = output_record->setFieldValue(3, docid, outofmem, rdata);
	//aos_assert_r(rslt && (!outofmem), false);

	//if (opr == AosDeltaOpr::eModify)
	//{
	//	rslt = output_record->setFieldValue(4, oldvalue, outofmem, rdata);
	//	aos_assert_r(rslt && (!outofmem), false);
	//}

	return false;
}


AosDataRecordObjPtr 
AosSyncherIndexExecutor::generatorDataRecord(
		AosRundata *rdata,
		const AosDeltaBeanOpr::E opr, 
		const AosDataFieldType::E keytype)
{
	return 0;
	//boost::shared_ptr<DataRecordBuff> record = boost::make_shared<DataRecordBuff>();
	//record->setAttribute("zky_name", "output");
	//record->setField("iil_name", mIndexName, 50);
	//record->setField("zky_operator", AosDeltaBeanOpr::toStr(opr), 50);
	//record->setField("key", AosDataFieldType::toString(keytype), 50);
	//record->setField("docid", "bin_int64", sizeof(int64_t));
	//if (opr == AosDeltaBeanOpr::eModify)
	//{
	//	record->setField("oldkey", AosDataFieldType::toString(keytype), 50);
	//}
	//
	//AosXmlTagPtr recorddoc = AosXmlParser::parse(record->getConfig() AosMemoryCheckerArgs);
	//aos_assert_r(recorddoc, 0);

	//return AosDataRecordObj::createDataRecordStatic(
	//		recorddoc, 0, rdata.getPtr() AosMemoryCheckerArgs);
}

