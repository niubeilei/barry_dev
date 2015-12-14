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
//
// Modification History:
// 04/21/2014	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Vector2D.h"

#include "CounterUtil/CounterUtil.h"
#include "DataStructs/StructProc.h"
#include "DataStructs/StatIdExtIDGen.h"
#include "DataStructs/RootStatDoc.h"
#include "DataStructs/StatBatchReader.h"
#include "DataStructs/LocalDistBlockMgr.h"
#include "Util/DataTypes.h"
#include "DocServer/DocSvr.h"
//#include "StatUtil/StatGroupBy.h"
#include "StatUtil/AggregationFunc.h"

static u64 grpby_key_time = 0;
static u64 grpby_time_time = 0;
static u64 vt2d_time = 0;
static u64 decompose_time = 0;

static u64 sg_time1 = 0;
static u64 sg_time2 = 0;
static u64 sg_time3 = 0;

static int64_t sTotalValue = 0;

// for test.
static u64 sg_cal_total_sum = 0;
static u64 sg_cal_total_t_sum = 0;

bool
AosVector2D::splitTimeSlots(
		vector<AosStatTimeArea> &orig_time_conds,
		vector<AosStatTimeCond*> &time_conds,
		const AosStatTimeUnit::E grpby_time_unit)
{
	bool rslt;
	map<int64_t, u32> time_index;
	for (u32 i=0; i<orig_time_conds.size(); i++)
	{
		rslt = splitTimeSlotsPriv(orig_time_conds[i], time_conds, time_index);
		aos_assert_r(rslt, false);
	}
	
	for(u32 i=0; i<time_conds.size(); i++)
	{
		vector<AosStatTimeArea> &time_areas = time_conds[i]->getTimeAreas();
		
		for(u32 j=0; j<time_areas.size(); j++)
		{
			rslt = splitTimeAreaByGroupTimeUnit(time_conds[i], time_areas[j], grpby_time_unit);
			aos_assert_r(rslt, false);
		}
	}
	
	return true;
}


bool
AosVector2D::splitTimeSlotsPriv(
		AosStatTimeArea &orig_time_cond,
		vector<AosStatTimeCond*> &time_conds,
		map<int64_t, u32> &time_index)
{
	int64_t start_time = AosStatTimeUnit::parseTimeValue(
			orig_time_cond.start_time, orig_time_cond.time_unit, mTimeUnit);
	aos_assert_r(start_time >=0, false);

	int64_t end_time = AosStatTimeUnit::parseTimeValue(
			orig_time_cond.end_time, orig_time_cond.time_unit, mTimeUnit);
	aos_assert_r(end_time >=0, false);
		
	OmnScreen << "orig time area: start:" << start_time
			<< "; end:" << end_time << endl;

	aos_assert_r(start_time >= 0, false);
	int64_t crt_time = ((u64)start_time < mStartTimeSlot) ? mStartTimeSlot : start_time;
	int64_t crt_slot;
	int64_t crt_seqno;
	int64_t expect_end_time;
	AosStatTimeCond* stat_cond;
	u64 weight = mParm.getTimeBucketWeight();
	while(1)
	{
		crt_slot = crt_time - mStartTimeSlot;
		crt_seqno = crt_slot / weight;
		
		stat_cond = findStatTimeCondPriv(time_conds, time_index, crt_seqno, crt_slot);
		aos_assert_r(stat_cond, false);

		expect_end_time = crt_time + weight - 1;
		if(expect_end_time <= end_time)
		{
			stat_cond->appendTimeArea(crt_time, expect_end_time);
			
			crt_time = expect_end_time + 1;
			continue;
		}
	
		stat_cond->appendTimeArea(crt_time, end_time);
		break;
	}
	
	return true;
}


AosStatTimeCond *
AosVector2D::findStatTimeCondPriv(
		vector<AosStatTimeCond*> &time_conds,
		map<int64_t, u32> &time_index, 
		const int64_t crt_idx,
		const int64_t crt_slot)
{
	AosStatTimeCond * stat_cond;
	map<int64_t, u32>::iterator itr = time_index.find(crt_idx);
	if(itr == time_index.end())
	{
		stat_cond = OmnNew AosStatTimeCond(crt_slot);
		
		u32 idx = time_conds.size();
		time_conds.push_back(stat_cond);
		time_index.insert(make_pair(crt_idx, idx));
	}
	else
	{
		u32 idx = itr->second;
		stat_cond = time_conds[idx];
	}
	return stat_cond;
}

bool
AosVector2D::splitTimeAreaByGroupTimeUnit(
		AosStatTimeCond * time_cond,
		AosStatTimeArea &time_area,
		const AosStatTimeUnit::E grpby_time_unit)
{
	//OmnScreen << "proc time area: start:" << time_area.start_time
	//	<< "; end:" << time_area.end_time << endl;

	int64_t crt_grpby_time = -1, front_grpby_time = -1;
	
	int64_t grpby_time_start = time_area.start_time;
	aos_assert_r(time_area.end_time >= 0, false);
	u64 crt_time = 0;
	for(crt_time = time_area.start_time; crt_time <= (u64)time_area.end_time; crt_time++)
	{
		crt_grpby_time = AosStatTimeUnit::parseTimeValue(crt_time, mTimeUnit, grpby_time_unit);
		aos_assert_r(crt_grpby_time >=0, false);
	
		if(crt_grpby_time == front_grpby_time)	continue;

		if(front_grpby_time == -1)
		{
			if(front_grpby_time == -1)	front_grpby_time = crt_grpby_time;
			continue;	
		}

		time_cond->appendGroupbyTimeArea(front_grpby_time, 
				grpby_time_start - mStartTimeSlot, crt_time -1 - mStartTimeSlot);
		front_grpby_time = crt_grpby_time;
		grpby_time_start = crt_time;
	}

	if(crt_grpby_time != -1)
	{
		time_cond->appendGroupbyTimeArea(crt_grpby_time, grpby_time_start - mStartTimeSlot, crt_time -1 - mStartTimeSlot);
	}
	
	//for (u32 j = 0; j < time_cond->getGroupbyAreaNum(); j++)
	//{
	//	u64 start, end;
	//	u64 agr_time_value;
	//	bool rslt = time_cond->getGroupbyArea(j, start, end, agr_time_value);
	//	aos_assert_r(rslt, false);
	
	//	OmnScreen << "grpby time area:" << agr_time_value
	//		<< "; start:" << start + mStartTimeSlot
	//		<< "; end:" << end + mStartTimeSlot
	//		<< "; start1:" << start
	//		<< "; end1:" << end
	//		<< endl;
	//}
	return true;
}
		

bool
AosVector2D::retrieveStatByDocidLocal(
		const AosRundataPtr &rdata,
		vector<u64> &dist_block_docid,
		vector<u64> &stat_ids,
		vector<AosStatTimeArea> &orig_time_conds,
		const AosStatTimeUnit::E grpby_time_unit,
		const AosVector2DGrpbyRsltPtr &grpby_rslt)
{
	AosVector2DPtr thisptr(this, false);                        
	return mBatchReader->shuffle(
			rdata,
			thisptr,
			dist_block_docid,
			stat_ids,
			orig_time_conds,
			grpby_time_unit,
			grpby_rslt);
}


bool
AosVector2D::retrieveStatByDocid(
		const AosRundataPtr &rdata,
		const u64 dist_block_docid,
		vector<u64> &stat_ids,
		vector<AosStatTimeCond *> &time_conds,
		const AosStatTimeUnit::E grpby_time_unit,
		const AosVector2DGrpbyRsltPtr &grpby_rslt)
{
	bool rslt;
	
	if(time_conds.size() !=0 )
	{
		rslt = retrieveWithTimeConds(rdata, dist_block_docid,
				stat_ids, time_conds, grpby_time_unit, grpby_rslt.getPtr());
		aos_assert_r(rslt, false);
		return true;
	}
	
	if(grpby_time_unit != AosStatTimeUnit::eAllTime)
	{
		rslt = retrieveWithEachTime(rdata, dist_block_docid,
				stat_ids, time_conds, grpby_time_unit, grpby_rslt.getPtr());
		aos_assert_r(rslt, false);
		return true;
	}
	
	rslt = retrieveWithGeneral(rdata, dist_block_docid,
			stat_ids, time_conds, grpby_rslt.getPtr());
	aos_assert_r(rslt, false);
//	OmnScreen << "CCCCCCCCCCc;sTotalValue:" <<  sTotalValue << ";"<< endl;
	return true;
}


bool
AosVector2D::retrieveWithTimeConds(
		const AosRundataPtr &rdata,
		const u64 dist_block_docid,
		vector<u64> &stat_ids,
		vector<AosStatTimeCond *> &time_conds,
		const AosStatTimeUnit::E grpby_time_unit,
		AosVector2DGrpbyRslt *grpby_rslt)
{
	bool rslt;
	AosDistBlock dist_block(mRootStatDocSize, mParm.getDftValue());
	vector<AosStatImRslt> stat_im_rslts;
	
	vector<AosVector2DRecord *> records;
	for (u32 i = 0; i < stat_ids.size(); i++)
	{
		AosVector2DRecord * record = OmnNew AosVector2DRecord();
		
		record->init(stat_ids[i], grpby_time_unit, mDataType);	

		rslt =  retrieveRootBucketPriv(
				rdata,
				dist_block_docid,
				dist_block,
				stat_ids[i],
				time_conds,
				stat_im_rslts,
				record);
		aos_assert_r(rslt, false);

		records.push_back(record);
	}

	std::sort(stat_im_rslts.begin(), stat_im_rslts.end());
	rslt = retrieveExtBucketPriv(rdata, time_conds, stat_im_rslts);
	aos_assert_r(rslt, false);
	
	for (u32 i = 0; i< records.size(); i++)
	{
		//u64 t2 = OmnGetTimestamp();
	
		rslt = grpby_rslt->grpByKeys(rdata, records[i]);
		aos_assert_r(rslt, false);
	
		//grpby_key_time += OmnGetTimestamp() - t2;

		delete records[i];
	}
	
	return true;
}


bool
AosVector2D::retrieveRootBucketPriv(
		const AosRundataPtr &rdata,
		const u64 &dist_block_docid,
		AosDistBlock &dist_block,
		const u64 &stat_id,
		vector<AosStatTimeCond *> &time_conds,
		vector<AosStatImRslt> &stat_im_rslts,
		AosVector2DRecord * record)
{
	char *stat_doc = 0;
	int64_t stat_doc_len = 0; 
	
	bool rslt = AosLocalDistBlockMgr::getStatDocByDocid(
			rdata, 
			dist_block, 
			dist_block_docid, 
			stat_id, 
			stat_doc, 
			stat_doc_len);
	aos_assert_r(rslt, false);
	aos_assert_r(stat_doc && stat_doc_len > 0, false);

	AosRootStatDoc root_stat_doc(stat_doc, stat_doc_len, mParm);
	OmnString keys = root_stat_doc.getStatKey();
	rslt = decomposeStatKey(keys, record);
	aos_assert_r(rslt, false);
	
	u64 start_idx = 0;
	u64 ext_docid = 0;
	AosBuffPtr bucket = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	for (u32 i = 0; i< time_conds.size(); i++)
	{
		start_idx = ext_docid = 0;
		bucket->resetMemory(10);
		rslt = root_stat_doc.getStatDocIndex(
				rdata,
				bucket,
				time_conds[i]->getSlot(), 
				ext_docid, 
				start_idx); 

		if (rslt && ext_docid == 0)
		{
			aos_assert_r(mExtensionStatDocSize <= stat_doc_len 
					&& bucket->dataLen () == mExtensionStatDocSize, false);

			getValuesByBucket(rdata, record, bucket.getPtr(), start_idx, time_conds[i]);
		}
	
		if (rslt && ext_docid > 0 && ext_docid != AosRootStatDoc::eInvExtStatId)
		{
			AosStatImRslt im_rslt(record, stat_id, ext_docid, start_idx, i);
			stat_im_rslts.push_back(im_rslt);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


bool
AosVector2D::retrieveExtBucketPriv(
		const AosRundataPtr &rdata,
		vector<AosStatTimeCond *> &time_conds,
		vector<AosStatImRslt> &stat_im_rslts)
{
	AosBuffPtr bucket; 
	for (u32 i = 0; i < stat_im_rslts.size(); i++)
	{
		bucket = AosLocalDistBlockMgr::retrieveBinaryDoc(
				stat_im_rslts[i].getExtDistBlockDocid(), rdata);
		aos_assert_r(bucket && bucket->dataLen () == mExtensionStatDocSize, false);

		int idx = stat_im_rslts[i].getTimeCondIndex();
		getValuesByBucket(
				rdata, 
				stat_im_rslts[i].getRecord(), 
				bucket.getPtr(), 
				stat_im_rslts[i].getExtStartIdx(), 
				time_conds[idx]);
	}
	return true;
}


bool
AosVector2D::getValuesByBucket(
		const AosRundataPtr &rdata,
		AosVector2DRecord * record,
		AosBuff *bucket,
		const u64 &start_idx,
		const AosStatTimeCond * time_cond)
{
	//u64 t0 = OmnGetTimestamp();
	
	bool rslt = false;
	
	vector<u64> all_time_value;
	AosBuffPtr all_stat_value = OmnNew AosBuff(bucket->dataLen() AosMemoryCheckerArgs);
	AosBuff * all_stat_value_raw = all_stat_value.getPtr();
	
	u64 agr_time_value = 0;
	char * agr_stat_value = 0; 
	for (u32 j = 0; j < time_cond->getGroupbyAreaNum(); j++)
	{
		u64 start, end;
		rslt = time_cond->getGroupbyArea(j, start, end, agr_time_value);
		aos_assert_r(rslt, false);

		rslt = getValuesGroupbyTime(rdata, bucket, 
					start_idx, start, end, agr_stat_value);
		aos_assert_r(rslt, false);
		if(agr_stat_value == 0)	continue;

		all_time_value.push_back(agr_time_value);
		all_stat_value_raw->setBuff(agr_stat_value, mValueSize);
	}

	rslt = addStatValuesToRecord(rdata, record, all_time_value, all_stat_value_raw);
	aos_assert_r(rslt, false);
	
	//grpby_time_time += OmnGetTimestamp() - t0;	
	return true;
}


bool
AosVector2D::getValuesGroupbyTime(
		const AosRundataPtr &rdata,
		AosBuff	*bucket,
		const u64 &start_idx,
		const u64 &start,
		const u64 &end,
		char * &agr_stat_vv)
{
	// Ketty 2014/04/30
	aos_assert_r(start <= end, false);
	bool rslt;

	agr_stat_vv = 0;
	char * crt_stat_vv = 0; 
	for(u64 idx = start; idx <= end; idx++)
	{
		int pos = (idx - start_idx) * mValueSize;
		aos_assert_r(pos >=0 && pos + mValueSize <= bucket->dataLen(), false);
		crt_stat_vv = bucket->data() + pos;

		if(memcmp(crt_stat_vv, mInvalidStatValue, mValueSize) == 0)	continue;
		
		//sTotalValue += *(int64_t*)crt_stat_vv;
		
		if(agr_stat_vv == 0)
		{
			agr_stat_vv = crt_stat_vv; 
			continue;
		}
		
		rslt = mAgrFuncRaw->calculate(rdata, mDataType, agr_stat_vv, agr_stat_vv, crt_stat_vv);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosVector2D::addStatValuesToRecord(
		const AosRundataPtr &rdata,
		AosVector2DRecord * record,
		vector<u64> &new_time_value,
		AosBuff *new_stat_value)
{
	// Ketty 2014/04/30
	if(new_time_value.size() == 0)	return true;
	aos_assert_r(new_stat_value, false);

	u32 crt_num = record->getStatValueNum();
	if(crt_num == 0)
	{
		record->appendStatValues(new_time_value, new_stat_value);
		return true;
	}

	u32 crt_end_idx = crt_num - 1;	
	int crt_end_time_vv = record->getTimeValue(crt_end_idx);
		
	aos_assert_r(crt_end_time_vv >=0 && (u32)crt_end_time_vv <= new_time_value[0], false);
	if((u32)crt_end_time_vv < new_time_value[0])
	{
		record->appendStatValues(new_time_value, new_stat_value);
		return true;
	}
	
	// need groupby this end_stat_vv and new_beg_stat_vv.
	
	char * crt_end_stat_vv = 0;
	int data_len;
	bool rslt = record->getStatValue(crt_end_idx, &crt_end_stat_vv, data_len);
	aos_assert_r(rslt && (u32)data_len == mValueSize, false);

	char * new_beg_stat_vv = new_stat_value->data();
	aos_assert_r(new_stat_value->dataLen() >= mValueSize, false);

	rslt = mAgrFuncRaw->calculate(rdata, mDataType, crt_end_stat_vv, crt_end_stat_vv, new_beg_stat_vv);
	aos_assert_r(rslt, false);

	record->appendStatValues(new_time_value, new_stat_value, 1);
	return true;
}


bool
AosVector2D::retrieveWithEachTime(
		const AosRundataPtr &rdata,
		const u64 dist_block_docid,
		vector<u64> &stat_ids,
		vector<AosStatTimeCond *> &time_conds,
		const AosStatTimeUnit::E grpby_time_unit,
		AosVector2DGrpbyRslt *grpby_rslt)
{
	bool rslt;
	AosDistBlock dist_block(mRootStatDocSize, mParm.getDftValue());
	vector<AosStatImRslt> stat_im_rslts;
	
	vector<AosVector2DRecord *> records;
	for (u32 i = 0; i < stat_ids.size(); i++)
	{
		AosVector2DRecord * record = OmnNew AosVector2DRecord();
		
		record->init(stat_ids[i], grpby_time_unit, mDataType);	
	
		rslt = retrieveWithEachTimePriv(
				rdata, 
				dist_block_docid,
				dist_block, 
				stat_ids[i], 
				record, 
				grpby_time_unit,
				stat_im_rslts);
		aos_assert_r(rslt, false);

		records.push_back(record);
	}
	
	std::sort(stat_im_rslts.begin(), stat_im_rslts.end());
	rslt = retrieveExtWithEachTimePriv(rdata, grpby_time_unit, stat_im_rslts);
	aos_assert_r(rslt, false);
	
	for (u32 i = 0; i< records.size(); i++)
	{
		//u64 t2 = OmnGetTimestamp();
		
		rslt = grpby_rslt->grpByKeys(rdata, records[i]);
		aos_assert_r(rslt, false);
		
		//grpby_key_time += OmnGetTimestamp() - t2;
		
		delete records[i];
	}
	
	return true;
}


bool
AosVector2D::retrieveWithEachTimePriv(
		const AosRundataPtr &rdata,
		const u64 &dist_block_docid,
		AosDistBlock &dist_block,
		const u64 &stat_id,
		AosVector2DRecord * record,
		const AosStatTimeUnit::E grpby_time_unit,
		vector<AosStatImRslt> &stat_im_rslts)
{
	char *stat_doc = 0;
	int64_t stat_doc_len = 0; 
	
	bool rslt = AosLocalDistBlockMgr::getStatDocByDocid(rdata, 
			dist_block, dist_block_docid, stat_id, stat_doc, 
			stat_doc_len);
	aos_assert_r(rslt, false);

	aos_assert_r(stat_doc && stat_doc_len > 0, false);

	AosRootStatDoc root_stat_doc(stat_doc, stat_doc_len, mParm);
	OmnString keys = root_stat_doc.getStatKey();
	rslt = decomposeStatKey(keys, record);
	aos_assert_r(rslt, false);
	
	vector<u64> ext_docids, start_idxs;
	rslt = root_stat_doc.getAllStatDocIndex(rdata, ext_docids, start_idxs);
	aos_assert_r(rslt, false);
	aos_assert_r(ext_docids.size() == start_idxs.size(), false);

	AosStatTimeCond * stat_cond = OmnNew AosStatTimeCond(0);
	for (u32 i = 0; i < ext_docids.size(); i++)
	{
		AosBuffPtr bucket = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		u64 ext_docid = ext_docids[i];
		u64 start_idx = start_idxs[i];

		if (ext_docid == 0)
		{
			rslt = root_stat_doc.getRootStatDoc(bucket);
			aos_assert_r(rslt, false);

			u64 start_time_slot = start_idx + mStartTimeSlot;
			u64 end_time_slot = start_idx + mParm.getTimeBucketWeight() -1 + mStartTimeSlot;
				
			// Ketty 2014/05/14
			rslt = initTimeCond(stat_cond, start_time_slot, end_time_slot, grpby_time_unit);
			aos_assert_r(rslt, false);
			
			rslt = getValuesByBucket(rdata, record, bucket.getPtr(), 
					start_idx, stat_cond);
			aos_assert_r(rslt, false);
		}

		if (ext_docid > 0 && ext_docid != AosRootStatDoc::eInvExtStatId)
		{
			AosStatImRslt im_rslt(record, stat_id, ext_docid, start_idx);
			stat_im_rslts.push_back(im_rslt);
			aos_assert_r(rslt, false);
		}
	}
	delete stat_cond;
	
	return true;
}


bool
AosVector2D::initTimeCond(
		AosStatTimeCond * stat_cond,
		const u64 start_time_slot, 
		const u64 end_time_slot,
		const AosStatTimeUnit::E grpby_time_unit)
{
	stat_cond->clear();
	stat_cond->appendTimeArea(start_time_slot, end_time_slot);
	
	vector<AosStatTimeArea> &time_areas = stat_cond->getTimeAreas();
	bool rslt = splitTimeAreaByGroupTimeUnit(stat_cond, time_areas[0], grpby_time_unit);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosVector2D::retrieveExtWithEachTimePriv(
		const AosRundataPtr &rdata,
		const AosStatTimeUnit::E grpby_time_unit,
		vector<AosStatImRslt> &stat_im_rslts)
{
	bool rslt = false;
	AosBuffPtr bucket; 
	AosStatTimeCond * stat_cond = OmnNew AosStatTimeCond(0);
	for (u32 i = 0; i < stat_im_rslts.size(); i++)
	{
		bucket = AosLocalDistBlockMgr::retrieveBinaryDoc(
				stat_im_rslts[i].getExtDistBlockDocid(),
				rdata);
		aos_assert_r(bucket && bucket->dataLen () == mExtensionStatDocSize, false);

		u64 start_idx = stat_im_rslts[i].getExtStartIdx();
		u64 start_time_slot = start_idx + mStartTimeSlot;
		u64 end_time_slot = start_idx + mParm.getTimeBucketWeight() -1 + mStartTimeSlot;
			
		// Ketty 2014/05/14
		rslt = initTimeCond(stat_cond, start_time_slot, end_time_slot, grpby_time_unit);
		aos_assert_r(rslt, false);
			
		getValuesByBucket(
				rdata, 
				stat_im_rslts[i].getRecord(), 
				bucket.getPtr(), 
				start_idx,
				stat_cond);
	}
	delete stat_cond;
	return true;
}


bool
AosVector2D::retrieveWithGeneral(
		const AosRundataPtr &rdata,
		const u64 dist_block_docid,
		vector<u64> &stat_ids,
		vector<AosStatTimeCond *> &time_conds,
		AosVector2DGrpbyRslt *grpby_rslt)
{
	bool rslt;
	AosDistBlock dist_block(mRootStatDocSize, mParm.getDftValue());

	AosVector2DRecord * record = OmnNew AosVector2DRecord();
	AosBuffPtr stat_value = OmnNew AosBuff(mValueSize AosMemoryCheckerArgs);
	for (u32 i = 0; i < stat_ids.size(); i++)
	{
		record->reset();
		record->init(stat_ids[i], AosStatTimeUnit::eAllTime, mDataType);	
		
		//u64 t1 = OmnGetTimestamp();
		rslt = retrieveStatGeneralPriv(
				rdata, 
				dist_block_docid,
				dist_block, 
				stat_ids[i], 
				record,
				stat_value);
		aos_assert_r(rslt, false);
		//u64 t2 = OmnGetTimestamp();
		
		//vt2d_time += t2 - t1;

		rslt = grpby_rslt->grpByKeys(rdata, record);
		aos_assert_r(rslt, false);
		
		//grpby_key_time += OmnGetTimestamp() - t2;
	}
	
	delete record;
	return true;
}


bool
AosVector2D::retrieveStatGeneralPriv(
		const AosRundataPtr &rdata,
		const u64 &dist_block_docid,
		AosDistBlock &dist_block,
		const u64 &stat_id,
		AosVector2DRecord * record,
		const AosBuffPtr &stat_value)
{
	char *stat_doc = 0;
	int64_t stat_doc_len = 0; 
	
	//u64 t0 = OmnGetTimestamp();
	bool rslt = AosLocalDistBlockMgr::getStatDocByDocid(rdata, 
			dist_block, dist_block_docid, stat_id, stat_doc, 
			stat_doc_len);
	aos_assert_r(rslt, false);

	aos_assert_r(stat_doc && stat_doc_len > 0, false);
	
	//u64 t1 = OmnGetTimestamp();

	AosRootStatDoc root_stat_doc(stat_doc, stat_doc_len, mParm);
	//u64 t2 = OmnGetTimestamp();
	
	OmnString keys = root_stat_doc.getStatKey();
	rslt = decomposeStatKey(keys, record);
	aos_assert_r(rslt, false);
	

	//u64 t4 = OmnGetTimestamp();
	stat_value->reset();
	stat_value->setDataLen(0);
	AosStatUtil::setInt64ToBuff(rdata.getPtr(), root_stat_doc.getStatGeneral(), mDataType, stat_value);
	record->appendStatValue(0, stat_value);
//OmnScreen << "EEEEEE dist_block_docid:" << dist_block_docid << ";stat_id:" << stat_id  << ";keys:" << keys << ";value:" << root_stat_doc.getStatGeneral() << ";"<< endl;
//sTotalValue += root_stat_doc.getStatGeneral();

	//u64 t3 = OmnGetTimestamp();
	
	//sg_time1 += t1 - t0;
	//sg_time2 += t2 - t1;
	//sg_time3 += t3 - t4;
	return true;
}


bool
AosVector2D::decomposeStatKey(
		const OmnString &keys,
		AosVector2DRecord * record)
{
	//u64 t0 = OmnGetTimestamp();
	
	aos_assert_r(keys != "", false);
	//aos_assert_r(keys.length() == strlen(keys.data()), false);

	char * key_data = (char *)keys.data();
	char *save_ptr;
	char * pch = strtok_r(key_data, AOS_COUNTER_SEGMENT_SEP2, &save_ptr);
	//u32 total_len = keys.length();

	while(pch != NULL)
	{
		int len = strlen(pch);
		aos_assert_r(len >=0, false);

		OmnString key(pch, len);
		record->addKeyValue(key);
	
		pch = strtok_r(NULL, AOS_COUNTER_SEGMENT_SEP2, &save_ptr);	
	}
	//u64 t1 = OmnGetTimestamp();
	//decompose_time += t1 - t0;

	return true;
}

void
AosVector2D::KettyInitPrintTime()
{
	grpby_key_time = 0;
	grpby_time_time = 0;
	vt2d_time = 0;
	decompose_time = 0;
	
	sg_time1 = 0;
	sg_time2 = 0;
	sg_time3 = 0;

	sTotalValue = 0;
}

void
AosVector2D::KettyPrintTime()
{
	//cout << "Times: "
	OmnScreen << "Times: "
		<< "grpby_key_time = " << grpby_key_time << "; " 
		<< "grpby_time_time = " << grpby_time_time << "; " 
		<< "vt2d_time = " << vt2d_time << "; " 
		<< "decompose_time = " << decompose_time << "; " 
		<< "sg_time1 = " << sg_time1 << "; " 
		<< "sg_time2 = " << sg_time2 << "; " 
		<< "sg_time3 = " << sg_time3 << "; " 
		//<< "TotalValue= " << sTotalValue << "; "
		<< endl;
}


bool
AosVector2D::test()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	u64 docid = 7248;
	AosXmlTagPtr stat_def_xml = AosDocSvr::getSelf()->getDoc(docid, rdata);
	aos_assert_r(stat_def_xml, false);

	AosXmlTagPtr def = stat_def_xml->getFirstChild("vector2d");
	aos_assert_r(def, false);
	int stat_doc_size = def->getAttrInt("root_stat_doc_size", -1);

	u64 root_did = 7244;
	AosBuffPtr measure_root_buff;
	AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(root_did, rdata);
	aos_assert_r(doc, 0);

	AosDocSvr::getSelf()->retrieveBinaryDoc(doc, measure_root_buff, rdata);
	aos_assert_r(measure_root_buff, false);

	u64 ext_did = 7239;
	AosBuffPtr measure_ext_buff;
	 doc = AosDocSvr::getSelf()->getDoc(ext_did, rdata);
	aos_assert_r(doc, 0);

	AosDocSvr::getSelf()->retrieveBinaryDoc(doc, measure_ext_buff, rdata);
	//aos_assert_r(measure_ext_buff, false);
	
	AosXmlTagPtr internal_statistics_tag = stat_def_xml->getFirstChild("internal_statistics");
	aos_assert_r(internal_statistics_tag, false);

	AosXmlTagPtr statistic_tag = internal_statistics_tag->getFirstChild("statistic");
	aos_assert_r(statistic_tag, false);

	AosXmlTagPtr stat_measure_conf = statistic_tag->getFirstChild("measure");
	aos_assert_r(stat_measure_conf, false);
	stat_measure_conf->setAttr("time_unit", "_day");

	AosVector2DPtr vt_2d = OmnNew AosVector2D(rdata, def, stat_measure_conf);
	aos_assert_r(vt_2d, false);

	u64 num_entries = measure_root_buff->getI64(-1);
	u64 * entries = (u64 *)(measure_root_buff->data()+ sizeof(int64_t));

	int docs_per_distblock = AosDistBlock::calculateDocPerDistBlock(stat_doc_size);

	vector<u64> dist_block_docids;
	vector<u64> stat_ids;
	for (u64 stat_id = 1; stat_id <= 100000; stat_id ++)
	{
		u32 idx = stat_id / docs_per_distblock;
		aos_assert_r(idx < num_entries, false);
		u64 did = entries[idx];
		dist_block_docids.push_back(did);
		stat_ids.push_back(stat_id);
	}

	vt_2d->test1(rdata, dist_block_docids, stat_ids);
	//vt_2d->test2(rdata, dist_block_docids, stat_ids);

	return true;
	
}


bool
AosVector2D::test1(
		const AosRundataPtr &rdata,
		vector<u64> &dist_block_docids,
		vector<u64> &stat_ids)
{
	sg_cal_total_sum = 0;
	sg_cal_total_t_sum = 0;
	AosStatTimeUnit::E grpby_time_unit = AosStatTimeUnit::eAllTime;

	vector<AosStatImRslt> stat_im_rslts;
	vector<AosVector2DRecord *> records;

	vector<AosStatTimeArea> orig_time_conds;
	AosStatTimeArea area;
	area.start_time = 14610;
	area.end_time = 14610 + 1096;
	area.time_unit = AosStatTimeUnit::eEpochDay;
	orig_time_conds.push_back(area);

	vector<AosStatTimeCond *> time_conds;
	splitTimeSlots(orig_time_conds, time_conds, grpby_time_unit);

//	AosVector2DGrpbyRsltPtr grpby_rslt = OmnNew AosVector2DGrpbyRslt();
//	grpby_rslt->setGroupByAll();
//
//	retrieveStatByDocidLocal(rdata, dist_block_docids, stat_ids, orig_time_conds, grpby_time_unit, grpby_rslt);
//
//	OmnScreen << "total:" << sg_cal_total_sum << "; t_total:" << sg_cal_total_t_sum << ";"<< endl;
//	return true;
	
	AosDistBlock dist_block(mRootStatDocSize, mParm.getDftValue());
	for (u64 i = 0; i < stat_ids.size(); i++)
	{
		AosVector2DRecord * record = OmnNew AosVector2DRecord();
		u64 dist_block_docid = dist_block_docids[i];
		bool rslt =  retrieveRootBucketPriv(
					rdata,
					dist_block_docid,
					dist_block,
					stat_ids[i],
					time_conds,
					stat_im_rslts,
					record);
		aos_assert_r(rslt, false);
		records.push_back(record);
	}

	std::sort(stat_im_rslts.begin(), stat_im_rslts.end());

	bool rslt = retrieveExtBucketPriv(rdata, time_conds, stat_im_rslts);
	aos_assert_r(rslt, false);
	return true;
}



bool
AosVector2D::test2(
		const AosRundataPtr &rdata,
		vector<u64> &dist_block_docids,
		vector<u64> &stat_ids)
{
	sg_cal_total_sum = 0;
	sg_cal_total_t_sum = 0;
	AosStatTimeUnit::E grpby_time_unit = AosStatTimeUnit::eEpochDay;
	vector<AosStatImRslt> stat_im_rslts;
	vector<AosVector2DRecord *> records;
	AosDistBlock dist_block(mRootStatDocSize, mParm.getDftValue());
	for (u64 i = 0; i < stat_ids.size(); i++)
	{
		AosVector2DRecord * record = OmnNew AosVector2DRecord();
		u64 stat_id = stat_ids[i];
		u64 dist_block_docid = dist_block_docids[i];

		retrieveWithEachTimePriv(rdata, dist_block_docid,
				dist_block, stat_id, record, 
				grpby_time_unit, stat_im_rslts);
		records.push_back(record);
	}

	std::sort(stat_im_rslts.begin(), stat_im_rslts.end());
	bool rslt = retrieveExtWithEachTimePriv(rdata, grpby_time_unit, stat_im_rslts);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVector2D::printBucket(const AosBuffPtr &bucket)
{
	OmnString ss;
	char *data = bucket->data();
	int64_t crt_idx = 0;
	while(crt_idx < bucket->dataLen())
	{
		int64_t v1 = *(int64_t *)&data[crt_idx];
		if (v1 != 0) ss << "," << v1;
		crt_idx += AosDataType::getValueSize(AosDataType::eInt64);
	}
	OmnScreen << "vv:"<< ss << ";"<< endl;
	return true;
}


bool
AosVector2D::calculateSum(
		AosVector2DRecord * record,
		const AosBuffPtr &bucket,
		const u64 &start_time_slot,
		const u64 &end_time_slot)
{
	char *data = bucket->data();
	for (u64 idx = start_time_slot; idx <= end_time_slot; idx ++)
	{
		int64_t slot = idx - mStartTimeSlot;
		int pos = slot * AosDataType::getValueSize(AosDataType::eInt64);
		int64_t v1 = *(int64_t *)&data[pos];
		if (v1 != 0) 
		{
			sg_cal_total_t_sum += v1;
		}
	}
	return true;
}



bool
AosVector2D::calculateSum(
		AosVector2DRecord * record,
		const AosBuffPtr &bucket,
		const u64 &start_idx,
		const AosStatTimeCond * time_cond)
{
	char *data = bucket->data();
	for (u32 j = 0; j < time_cond->getGroupbyAreaNum(); j++)
	{
		u64 start, end;
		u64 agr_time_value = 0;
		bool rslt = time_cond->getGroupbyArea(j, start, end, agr_time_value);
		aos_assert_r(rslt, false);

		for (u64 idx = start; idx <= end; idx ++)
		{
			aos_assert_r(idx - start_idx < mParm.getTimeBucketWeight(), false);
			int pos = (idx - start_idx) * AosDataType::getValueSize(AosDataType::eInt64);
			int64_t v1 = *(int64_t *)&data[pos];
			if (v1 != 0) 
			{
				sg_cal_total_t_sum += v1;
			}
		}
	}
		
	return true;
}

bool
AosVector2D::kettyTest()
{
	AosStatTimeUnit::KettyInitPrintTime();
	AosVector2D::KettyInitPrintTime();
	
	AosStatTimeUnit::init();
	mDataType = AosDataType::eInt64;
	mTimeUnit = AosStatTimeUnit::eEpochDay;
	
	u64 t1 = OmnGetTimestamp();
	int i= 100000;
	AosStatTimeCond * stat_cond = OmnNew AosStatTimeCond(0);
	while(i--)
	{
		initTimeCond(stat_cond, 14610, 15609, AosStatTimeUnit::eEpochYear);
	}
	u64 t2 = OmnGetTimestamp();
	
	cout << "!!!!!!!! init Time Cond"
		<< "; grpby_times:" << (t2 - t1)
		<< endl; 
	
	AosStatTimeUnit::KettyPrintTime();
	AosVector2D::KettyPrintTime();
	return true;
	/*
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	AosVector2DRecord * record = OmnNew AosVector2DRecord();
	
	AosStatTimeUnit::E grpby_time_unit = AosStatTimeUnit::eAllTime;
	vector<u64> time_slots;
	testParserTimeSlots(rdata, grpby_time_unit, time_slots);
	
	AosBuffPtr bucket = OmnNew AosBuff(mExtensionStatDocSize AosMemoryCheckerArgs);
	//memset(bucket->data(), 0, mExtensionStatDocSize);
	memset(bucket->data(), mParm.getDftValue(), mExtensionStatDocSize);
	bucket->setDataLen(mExtensionStatDocSize);

	AosBuff * raw_buff = bucket.getPtr();
	u64 t1 = OmnGetTimestamp();

	int i= 100000;
	while(i--)
	{
		record->reset();
		getValuesByBucket(rdata, record, raw_buff, 0, time_slots,
			0, time_slots.size() - 1, grpby_time_unit);
	}
	u64 t2 = OmnGetTimestamp();
	
	cout << "!!!!!!!! 2d vector query time"
		<< "; grpby_times:" << (t2 - t1)
		<< endl; 
	return true;
	*/
}


/*
bool
AosVector2D::testParserTimeSlots(
		const AosRundataPtr &rdata,
		const AosStatTimeUnit::E to_time_unit,
		vector<u64>	&time_slots)
{
	// to Guarantee the time from small to big.
	mDataType = AosDataType::eInt64;
	mTimeUnit = AosStatTimeUnit::eEpochDay;
	mStartTimeSlot = 14611;
	mAgrFunc = AosAggregationFunc::getAggregation(AosAggregationType::eSum);
	mAgrFuncRaw = mAgrFunc.getPtr();
	
	AosStatTimeUnit::init();

	AosStatQueryCondInfo time_cond;
	time_cond.start_time = 14611;
	time_cond.end_time = 15705;
	time_cond.time_unit = AosStatTimeUnit::eEpochDay; 

	set<u32> s_times;
	int crt_idx = 0;
	while(1)
	{
		int epoch_time = time_cond.getNextEpochTime(crt_idx);
		if(epoch_time == -1)	break;
	
		s_times.insert(epoch_time);
	}

	set<u32>::iterator itr = s_times.begin();
	for(; itr != s_times.end(); itr++)
	{
		time_slots.push_back(*itr);	
	}
	
	int stat_vv_len = AosDataType::getValueSize(mDataType);
	mExtensionStatDocSize = time_slots.size() * stat_vv_len + 100; 

	return true;
}


*/
