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
#include "Synchers/SyncherSumMapExecutor.h"

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

AosJimoPtr AosCreateJimoFunc_AosSyncherSumMapExecutor(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSyncherSumMapExecutor(version);
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


AosSyncherSumMapExecutor::AosSyncherSumMapExecutor(const int version)
:
AosSyncher(version)
{
}


AosSyncherSumMapExecutor::~AosSyncherSumMapExecutor()
{
}


bool 
AosSyncherSumMapExecutor::proc()
{
	OmnScreen << "Processing Syncher: SyncherSumMapExecutor" << endl;
	aos_assert_r(mRundata, false);
	
	AosDatasetObjPtr ds = getSyncherDataset(mRundata.getPtr(), mDatasetDef, mDeltasBuff);
	aos_assert_r(ds, false);

	return proc(mRundata.getPtr(), ds);
}


AosBuffPtr
AosSyncherSumMapExecutor::serializeToBuff()
{
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	buff->setOmnStr(mIILName);
	buff->setOmnStr(mDatasetDef->toString());
	buff->setAosBuff(mDeltasBuff);
	
	return buff;
}


bool
AosSyncherSumMapExecutor::serializeFromBuff(const AosBuffPtr &buff)
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
AosSyncherSumMapExecutor::cloneJimo() const
{
	return OmnNew AosSyncherSumMapExecutor(*this);
}


bool	
AosSyncherSumMapExecutor::proc(
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
AosSyncherSumMapExecutor::procData(
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
	aos_assert_r(AosDeltaBeanOpr::eIncrement, false);
		
	// 4. increment index
	AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
	aos_assert_r(iilmgr, false);
	u64 iilid = iilmgr->getIILID(mIILName, true, rdata);
	u32 siteid = rdata->getSiteid();

	AosIILType iiltype = getIILType(key_v.getType());
	AosIILObjPtr iilobj = iilmgr->getIILPublic(iilid, siteid, 0, iiltype, rdata);
	aos_assert_r(iilobj, false);

	u64 return_value = 0;
	u64 init_value = 0;
	return iilobj->incrementDocidSafe(key_v.getStr(),
			return_value, docid_v.getU64(), init_value, true, rdata);
}

