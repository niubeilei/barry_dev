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
// An instance of this class is created for each table (as needed).
// A table may define multiple IILs. This class will create one 
// AosBuff for each IIL. 
//
// When adding, deleting, or modifying a record, this class will 
// generate an IIL entry for each IIL.
//
// The syncher uses a timer. When the timer expires or the buffer
// is big enough, it will send the buff to the Syncer executor
// to actually update the IILs. 
//	
// Modification History:
// 2015/09/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Synchers/SyncherTable.h"

#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/RecordsetObj.h"
#include "DataRecord/RecordBuff2.h"
#include "JQLStatement/JqlStatement.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "AppMgr/App.h"
#include "Util1/Timer.h"
#include "Synchers/SyncherIndex.h"
#include "Synchers/SyncherMap.h"
#include "Synchers/SyncherJoin.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSyncherTable(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSyncherTable(version);
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


AosSyncherTable::AosSyncherTable(const int version)
:
AosSyncher(version),
mLock(OmnNew OmnMutex()),    
mCondVar(OmnNew OmnCondVar),
mBigEnough(false),
mTimerSec(1),
mTimerUsec(5000)
{
	OmnTimerObjPtr thisptr(this, false);
	OmnTimer::getSelf()->startTimer("sync_table", mTimerSec, mTimerUsec, thisptr, 0);

	if (!mThread)
	{
		OmnThreadedObjPtr thisptr(this, false);
		mThread = OmnNew OmnThread(thisptr, "sync_table", 0, false, true, __FILE__, __LINE__);
		mThread->start();
	}
}


AosSyncherTable::~AosSyncherTable()
{
}


bool 
AosSyncherTable::init(AosRundata *rdata)
{
	// 1. get table doc
	aos_assert_r(mTableObjid != "", false);
	AosXmlTagPtr tbl_doc = AosGetDocByObjid(mTableObjid, rdata);
	aos_assert_r(tbl_doc, false);
	
	// 2. index 
	AosXmlTagPtr indexsNode = tbl_doc->getFirstChild("indexes");
	if (indexsNode)
	{
		AosXmlTagPtr indexNode = indexsNode->getFirstChild("index");
		while (indexNode)
		{
			if (indexNode->getAttrBool("synched", true))
			{
				// syncher index
				OmnString idx_name = indexNode->getAttrStr("zky_index_objid");
				aos_assert_r(idx_name != "", false);
				AosXmlTagPtr conf = AosGetDocByObjid(idx_name, rdata);
				aos_assert_r(conf, false);

				AosSyncherObj *sy = OmnNew AosSyncherIndex(0);
				aos_assert_r(sy, false);
				sy->config(rdata, conf);

				mSynchers.push_back(sy);
			}
			indexNode = indexsNode->getNextChild("index");
		}
	}

	// 3. map node
	AosXmlTagPtr mapsNode = tbl_doc->getFirstChild("maps");
	if (mapsNode)
	{
		AosXmlTagPtr mapNode = mapsNode->getFirstChild("map");
		while (mapNode)
		{
			AosSyncherObjPtr sync = OmnNew AosSyncherMap(0);
			aos_assert_r(sync, false);
			sync->config(rdata, mapNode);
			mSynchers.push_back(sync);

			mapNode = mapsNode->getNextChild("map");
		}
	}

	// 4. join node
	AosXmlTagPtr joinNodes = tbl_doc->getFirstChild("joins");
	if (joinNodes)
	{
		AosXmlTagPtr joinNode = joinNodes->getFirstChild("join");
		while (joinNode)
		{
			AosSyncherObjPtr sync = OmnNew AosSyncherJoin(0);
			aos_assert_r(sync, false);
			joinNode->setAttr("zky_table_objid", mTableObjid);
			sync->config(rdata, joinNode);
			mSynchers.push_back(sync);

			joinNode = joinNodes->getNextChild("join");
		}
	}

	return true;
}


bool 
AosSyncherTable::proc()
{
	return true;


	OmnScreen << "Processing Syncher: SyncherTable" << endl;
	aos_assert_r(mRundata, false);

	AosDatasetObjPtr ds = getSyncherDataset(mRundata.getPtr(), mDatasetDef, mDeltasBuff);
	return proc(mRundata.getPtr(), ds);
}


AosBuffPtr
AosSyncherTable::serializeToBuff()
{
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	buff->setOmnStr(mTableObjid);
	return buff;


	buff->setOmnStr(mDatasetDef->toString());
	buff->setAosBuff(mDeltasBuff);
	return buff;
}


bool
AosSyncherTable::serializeFromBuff(const AosBuffPtr &buff)
{
	mTableObjid = buff->getOmnStr("");
	aos_assert_r(mTableObjid != "", false);
	aos_assert_r(mRundata, false);
	init(mRundata.getPtr());
	return true;


	mDatasetDef = AosXmlParser::parse(buff->getOmnStr("") AosMemoryCheckerArgs);
	aos_assert_r(mDatasetDef, false);
	mDeltasBuff = buff->getAosBuff(true AosMemoryCheckerArgs);
	aos_assert_r(mDeltasBuff, false);

	return true;
}


AosJimoPtr 
AosSyncherTable::cloneJimo() const
{
	return OmnNew AosSyncherTable(*this);
}


bool 		
AosSyncherTable::proc(
		AosRundata* rdata, 
		const AosDatasetObjPtr &dataset)
{
	//aos_assert_r(dataset, false);	

	//vector<AosDataRecordObjPtr> input_records;
	//AosDataRecordObj *input_record;
	//AosDataRecordObj *output_record;

	//vector<AosSyncDataProcIndex*> dataprocs;

	//bool rslt = false;
	//while (1)
	//{
	//	rslt = dataset->nextRecord(rdata, input_record);
	//	aos_assert_r(rslt, false);

	//	if (!input_record) break;
	//	input_records.push_back(input_record);
	//}

	//// 1. config dataprocs
	////if (mDataProcs.size() <= 0)
	//{
	//	for (size_t i=0; i<mIndexs.size(); i++)
	//	{
	//		// generator syncdataproc's config
	//		AosXmlTagPtr conf = generatorDPIndexConf(rdata, mIndexs[i]);
	//		aos_assert_r(conf, false);

	//		AosSyncDataProcIndex* dpindex = OmnNew AosSyncDataProcIndex(0, mIndexs[i]);
	//		aos_assert_r(dpindex, false);
	//		dpindex->setInputDataRecords(input_records);
	//		dpindex->config(rdata, conf);
	//		//mDataProcs.push_back(dpindex);
	//	}
	//}

	//for (size_t i=0; i<input_records.size(); i++)
	//{
	//	input_record = input_records[i].getPtr();
	//	for (size_t j=0; j<dataprocs.size(); j++)
	//	{
	//		AosDataProcStatus::E status = dataprocs[j]->procData(rdata, 
	//				&input_record, &output_record);
	//		aos_assert_r(status != AosDataProcStatus::eError, false);
	//	}
	//}

	return true;
}


AosXmlTagPtr 
AosSyncherTable::generatorIndexDatasetConf(AosRundata *rdata)
{
	OmnString str;
	str << "<dataset jimo_objid=\"dataset_syncher_jimodoc_v0\" zky_name=\"output\" >"
		<<  "<datarecord type=\"buff\" zky_name=\"output\">"
		<<   "<datafields>"
		<<    "<datafield type=\"str\" zky_name=\"zky_operator\"/>"
		<<    "<datafield type=\"str\" zky_name=\"key\"/>"
		<<    "<datafield type=\"bin_int64\" zky_name=\"docid\"/>"
		<< 	 "</datafields>"
		<<  "</datarecord>"
		<< "</dataset>";
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}


bool 	
AosSyncherTable::appendRecord(
		AosRundata *rdata,
		AosDataRecordObj *input_record)
{
	aos_assert_r(input_record, false);

	mLock->lock();
	for (size_t i=0; i<mSynchers.size(); i++)
	{
		bool rslt = mSynchers[i]->procData(rdata, input_record);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();

	return true;
}


bool	
AosSyncherTable::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	mBigEnough = false;
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock(); 
		if (mBigEnough)
		{
			flushDeltaBeans(mRundata.getPtr());
			mBigEnough = false;
		}
		else
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		mLock->unlock(); 
	}
	return true;
}


bool
AosSyncherTable::signal(const int threadLogicId)
{
	return true;
}


void 
AosSyncherTable::timeout(
		const int timerId, 
		const OmnString &timerName,
		void *parm)
{
	mLock->lock();
	mBigEnough = true;
	mCondVar->signal();
	mLock->unlock();

	OmnTimerObjPtr thisptr(this, false);
	OmnTimer::getSelf()->startTimer("syncher_table",
			mTimerSec, mTimerUsec, thisptr, 0);
}


bool 
AosSyncherTable::flushDeltaBeans(AosRundata *rdata)
{
	for (size_t i=0; i<mSynchers.size(); i++)
	{
		bool rslt = mSynchers[i]->flushDeltaBeans(rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}

