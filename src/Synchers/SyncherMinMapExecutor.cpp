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
// 2015/11/03 Created by Andy 
////////////////////////////////////////////////////////////////////////////
#include "Synchers/SyncherMinMapExecutor.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiQ.h"
#include "JimoCall/JimoCall.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DeltaBeanOpr.h"
#include "SEUtil/SeTypes.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Util/ValueRslt.h"
#include "Util/Opr.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSyncherMinMapExecutor(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSyncherMinMapExecutor(version);
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


AosSyncherMinMapExecutor::AosSyncherMinMapExecutor(const int version)
:
AosSyncher(version)
{
}


AosSyncherMinMapExecutor::~AosSyncherMinMapExecutor()
{
}


bool 
AosSyncherMinMapExecutor::proc()
{
	OmnScreen << "Processing Syncher: SyncherMinMapExecutor" << endl;
	aos_assert_r(mRundata, false);
	
	AosDatasetObjPtr ds = getSyncherDataset(mRundata.getPtr(), mDatasetDef, mDeltasBuff);
	aos_assert_r(ds, false);

	return proc(mRundata.getPtr(), ds);
}


AosBuffPtr
AosSyncherMinMapExecutor::serializeToBuff()
{
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	buff->setOmnStr(mKeyIILName);
	buff->setOmnStr(AosDataType::getTypeStr(mDataType));
	buff->setOmnStr(mMapIILName);
	buff->setOmnStr(mDatasetDef->toString());
	buff->setAosBuff(mDeltasBuff);
	
	return buff;
}


bool
AosSyncherMinMapExecutor::serializeFromBuff(const AosBuffPtr &buff)
{
	mKeyIILName = buff->getOmnStr("");
	aos_assert_r(mKeyIILName != "", false);

	OmnString data_type = buff->getOmnStr("");                          
	aos_assert_r(data_type != "", false);
	mDataType = AosDataType::toEnum(data_type);
	aos_assert_r(AosDataType::isValid(mDataType), false);

	mMapIILName = buff->getOmnStr("");
	aos_assert_r(mMapIILName != "", false);

	OmnString conf = buff->getOmnStr("");                          
	aos_assert_r(conf != "", false);                               

	mDatasetDef = AosXmlParser::parse(conf AosMemoryCheckerArgs);
	aos_assert_r(mDatasetDef, false);

	mDeltasBuff = buff->getAosBuff(true AosMemoryCheckerArgs);
	aos_assert_r(mDeltasBuff, false);

	return true;
}


AosJimoPtr 
AosSyncherMinMapExecutor::cloneJimo() const
{
	return OmnNew AosSyncherMinMapExecutor(*this);
}


bool	
AosSyncherMinMapExecutor::proc(
		AosRundata *rdata, 
		const AosDatasetObjPtr &dataset)
{
	AosDataRecordObj *input_record;

	bool rslt = false;
	while (1)
	{
		rslt = dataset->nextRecord(rdata, input_record);
		aos_assert_r(rslt, false);
		if (!input_record) break;

		rslt = procData(rdata, input_record);
		aos_assert_r(rslt, false);
	}
		
	return true;
}


bool
AosSyncherMinMapExecutor::procData(
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

	AosValueRslt old_key, old_value;
	AosDeltaBeanOpr::E new_opr;
	switch (opr)
	{
	case AosDeltaBeanOpr::eAdd:
		 new_opr = pickOprByAdd(rdata, key_v, docid_v, old_key, old_value);
		 rslt = operationIIL(rdata, new_opr, mMapIILName, docid_v.getU64(), key_v, old_key, old_value);
		 aos_assert_r(rslt, false);
		 break;
	case AosDeltaBeanOpr::eModify:
		 OmnShouldNeverComeHere;
		 new_opr = pickOprByModify(rdata, key_v, docid_v, old_key, old_value);
		 break;
	case AosDeltaBeanOpr::eDelete:
		 new_opr = pickOprByDelete(rdata, key_v, docid_v, old_key, old_value);
		 if (new_opr != AosDeltaBeanOpr::eInvalid)
		 {
		 	rslt = operationIIL(rdata, new_opr, mMapIILName, old_value.getU64(), old_key, key_v, docid_v);
		 	aos_assert_r(rslt, false);
		 }
		 break;
	default:
		 OmnShouldNeverComeHere;
	}
	rslt = operationIIL(rdata, opr, mKeyIILName, docid_v.getU64(), key_v, key_v, docid_v);
	aos_assert_r(rslt, false);
	return true;
}


AosDeltaBeanOpr::E
AosSyncherMinMapExecutor::pickOprByAdd(
		AosRundata* rdata,
		const AosValueRslt &map_key,
		const AosValueRslt &map_value,
		AosValueRslt &old_key,
		AosValueRslt &old_value)
{
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(map_key.getStr());
	query_context->setReverse(false);
	query_context->setPageSize(0);
	query_context->setBlockSize(100);

	bool rslt = AosQueryColumn(mKeyIILName, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, AosDeltaBeanOpr::eMax);

	bool finished = false;
	u64 docid;
	OmnString vv;
	while (query_rslt->nextDocidValue(docid, vv, finished, rdata)) 
	{
		if (finished) return AosDeltaBeanOpr::eAdd;
		if (compare(map_value, docid) < 0)
		{
			old_key.setStr(vv);
			old_value.setU64(docid);
			return AosDeltaBeanOpr::eModify;
		}
		return AosDeltaBeanOpr::eInvalid;
	}
	return AosDeltaBeanOpr::eAdd;
}


AosDeltaBeanOpr::E
AosSyncherMinMapExecutor::pickOprByDelete(
		AosRundata* rdata,
		const AosValueRslt &map_key,
		const AosValueRslt &map_value,
		AosValueRslt &old_key,
		AosValueRslt &old_value)
{
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(map_key.getStr());
	query_context->setReverse(false);
	query_context->setPageSize(0);
	query_context->setBlockSize(100);

	bool rslt = AosQueryColumn(mKeyIILName, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, AosDeltaBeanOpr::eMax);

	bool finished = false;
	u64 docid;
	u32 loop = 0;
	OmnString vv;
	while (query_rslt->nextDocidValue(docid, vv, finished, rdata)) 
	{
		aos_assert_r(loop < 3, AosDeltaBeanOpr::eMax);
		int found = compare(map_value, docid);

		if (found > 0)
			return AosDeltaBeanOpr::eInvalid; 

		if ((found == 0 && loop > 0) || found > 0)
		{
			return AosDeltaBeanOpr::eInvalid; 
		}
		if (found < 0)
		{
			old_key.setStr(vv);
			old_value.setU64(docid);
			return AosDeltaBeanOpr::eModify;
		}
		loop++;
	}
	return AosDeltaBeanOpr::eAdd;
}

AosDeltaBeanOpr::E
AosSyncherMinMapExecutor::pickOprByModify(
		AosRundata* rdata,
		const AosValueRslt &map_key,
		const AosValueRslt &map_value,
		AosValueRslt &old_key,
		AosValueRslt &old_value)
{
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(map_key.getStr());
	query_context->setReverse(false);
	query_context->setPageSize(0);
	query_context->setBlockSize(100);

	bool rslt = AosQueryColumn(mKeyIILName, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, AosDeltaBeanOpr::eMax);

	bool finished = false;
	u64 docid;
	OmnString vv;
	while (query_rslt->nextDocidValue(docid, vv, finished, rdata)) 
	{
		if (compare(map_value, docid) >= 0)
		{
			return AosDeltaBeanOpr::eInvalid; 
		}
		else
		{
			old_key.setStr(vv);
			old_value.setU64(docid);
			return AosDeltaBeanOpr::eModify;
		}
	}
	return AosDeltaBeanOpr::eAdd;
}


bool
AosSyncherMinMapExecutor::operationIIL(
		AosRundata *rdata,
		const AosDeltaBeanOpr::E opr,
		const OmnString &iil_name,
		const u64 &docid,
		const AosValueRslt &new_value,
		const AosValueRslt &old_key,
		const AosValueRslt &old_value)
{
	if (opr == AosDeltaBeanOpr::eInvalid)
		return true;

	bool rslt;
	aos_assert_r(AosDeltaBeanOpr::isValid(opr), false);
	AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
	aos_assert_r(iilmgr, false);

	u64 iilid = iilmgr->getIILID(iil_name, true, rdata);
	u32 siteid = rdata->getSiteid();

	AosIILType iiltype = getIILType(new_value.getType());
	AosIILObjPtr iilobj = iilmgr->getIILPublic(iilid, siteid, 0, iiltype, rdata);
	aos_assert_r(iilobj, false);

	switch (opr)
	{
	case AosDeltaBeanOpr::eAdd:
		 // first flase is value_unique, next is docid_unique
		 rslt = iilobj->addDocSafe(new_value.getStr(), docid, false, false, rdata);
		 aos_assert_r(rslt, false);
		 break;
	case AosDeltaBeanOpr::eModify:
		 // first flase is value_unique, next is docid_unique
		 // rslt = iilobj->modifyDocSafe(old_value.getStr(), new_value.getStr(), docid, false, false, rdata);
		 rslt = iilobj->removeDocSafe(old_key.getStr(), old_value.getU64(), rdata);
		 aos_assert_r(rslt, false);
		 rslt = iilobj->addDocSafe(new_value.getStr(), docid, false, false, rdata);
		 //rslt = iilobj->modifyDocSafe(old_value.getStr(), new_value.getStr(), docid, false, false, rdata);
		 aos_assert_r(rslt, false);
		 break;
	case AosDeltaBeanOpr::eDelete:
		 rslt = iilobj->removeDocSafe(new_value.getStr(), docid, rdata);
		 aos_assert_r(rslt, false);
		 break;
	default:
		 OmnShouldNeverComeHere;
		 return false;
	}
	return true;
}

int
AosSyncherMinMapExecutor::compare(
	const AosValueRslt lhs,
	u64	rhs)
{
	AosValueRslt rhs_vv;
	switch (mDataType)
	{
	case AosDataType::eInt64:
		 rhs_vv.setI64(*(i64 *)&rhs);
		 break;
	case AosDataType::eU64:
		 rhs_vv.setU64(*(u64 *)&rhs);
		 break;
	case AosDataType::eDouble:
		 rhs_vv.setDouble(*(double *)&rhs);
		 break;
	default:
		 OmnShouldNeverComeHere;
		 aos_assert_r(false, -1);
		 return -1;
	}

	AosDataType::E type = AosDataType::autoTypeConvert(lhs.getType(), rhs_vv.getType());
	if (lhs < rhs_vv)
		return -1;
	else if (AosValueRslt::doComparison(eAosOpr_eq, type, lhs, rhs_vv))
		return 0;
	else 
		return 1;
}


bool
AosSyncherMinMapExecutor::setValue(
		AosValueRslt &value,
		u64 docid)
{
	switch (mDataType)
	{
	case AosDataType::eInt64:
		 value.setI64(*(i64 *)&docid);
		 break;
	case AosDataType::eU64:
		 value.setU64(*(u64 *)&docid);
		 break;
	case AosDataType::eDouble:
		 value.setDouble(*(double *)&docid);
		 break;
	default:
		 OmnShouldNeverComeHere;
		 aos_assert_r(false, -1);
	}
	return true;
}
