////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/21/2014 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/StatBatchReader.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStructs/Vector2D.h"
#include "DataStructs/LocalDistBlockMgr.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadPool.h"
//#include "StatUtil/StatGroupBy.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("doc_reader", __FILE__, __LINE__);

AosStatBatchReader::AosStatBatchReader()
{
}


AosStatBatchReader::~AosStatBatchReader()
{
}

bool
AosStatBatchReader::shuffle(
		const AosRundataPtr &rdata,
		const AosVector2DPtr &vector2d,
		vector<u64> &dist_block_docid,
		vector<u64> &stat_ids,
		vector<AosStatTimeArea> &orig_time_conds,
		//vector<u64> &time_slots,
		const AosStatTimeUnit::E grpby_time_unit,
		const AosVector2DGrpbyRsltPtr &grpby_rslt)
{
	//AosStatGroupBy::KettyInitPrintTime();
	AosVector2D::KettyInitPrintTime();
	AosLocalDistBlockMgr::KettyInitPrintTime();
	
	vector<AosStatTimeCond *> time_conds;
	//vector2d->splitTimeSlots(time_slots, time_conds);
	vector2d->splitTimeSlots(orig_time_conds, time_conds, grpby_time_unit);
	
	AosStatReaderRequestPtr  request;
	map<u64, AosStatReaderRequestPtr> requests;
	for (u32 i = 0; i < dist_block_docid.size(); i++)
	{
		u64 dist_block_did = dist_block_docid[i];
		if (requests.count(dist_block_did) == 0)
		{
			// Ketty 2014/04/23
			//request = OmnNew AosStatReaderRequest(
			//		dist_block_did,
			//		time_grpby, 
			//		key_grpby);
			
			AosVector2DGrpbyRsltPtr each_grpby_rslt = grpby_rslt->copy();
			request = OmnNew AosStatReaderRequest(dist_block_did,
					time_conds, grpby_time_unit, each_grpby_rslt);

			request->appendEntry(stat_ids[i]);
			requests.insert(make_pair(dist_block_did, request));
		}
		else
		{
			requests[dist_block_did]->appendEntry(stat_ids[i]);
		}
	}

	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;
	map<u64, AosStatReaderRequestPtr>::iterator itr = requests.begin();
	while(itr != requests.end())
	{
		int i = 0;
		while( i < eNumThrds && itr != requests.end())
		{
			runner = OmnNew AosStatReaderThrd(
					itr->second,
					vector2d, 
					rdata);

			runners.push_back(runner);
			i++;
			++itr;
		}

		aos_assert_r(sgThreadPool, false);
		sgThreadPool->procSync(runners);
		runners.clear();
	}

	// Ketty 2014/04/25
	
	u64 t1 = OmnGetTimestamp();
	bool rslt;
	AosVector2DGrpbyRsltPtr each_grpby_rslt;
	for(itr = requests.begin(); itr != requests.end(); itr++)
	{
		each_grpby_rslt = (itr->second)->mGrpByRslt;
		vector<AosVector2DRecord *>& total_records = each_grpby_rslt->getRecords();
		
		for(u32 i=0; i<total_records.size(); i++)
		{
			rslt = grpby_rslt->grpByKeys(rdata, total_records[i]);
			aos_assert_r(rslt, false);
		}

		//grpby_rslt->addRecords(total_records);
	}
		
	for(u32 i=0; i<time_conds.size(); i++)
	{
		delete time_conds[i];	
	}
	
	u64 t2 = OmnGetTimestamp();
	
	//AosStatGroupBy::KettyPrintTime();
	AosVector2D::KettyPrintTime();
	AosLocalDistBlockMgr::KettyPrintTime();
	OmnScreen << "Second grpby_key_time:" << t2 - t1 << endl;
	return true;
}


bool
AosStatBatchReader::AosStatReaderThrd::run()
{
//	mVector2d->retrieveStatByDocidLocal(
	mVector2d->retrieveStatByDocid(
			mRundata, 
			mRequest->mDistBlockDocid,
			mRequest->mStatIds, 
			mRequest->mTimeConds,
			mRequest->mGrpbyTimeUnit,
			mRequest->mGrpByRslt);
			//mRequest->mTimeGroupBy, 
			//mRequest->mKeyGroupBy);
	return true;
}


bool
AosStatBatchReader::AosStatReaderThrd::procFinished()
{
	return true;
}
