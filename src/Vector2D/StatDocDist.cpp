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
// 	This file simulates a fixed length array:
// 		[entry]
// 		[entry]
// 		...
// 		[entry]
//
// Modification History:
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "DfmUtil/Ptrs.h"
#include "Vector2D/StatDocDist.h"
#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/AggrFuncObj.h"
#include "Vector2DQryRslt/Vt2dQryRslt.h"
#include "Vector2DQryRslt/Vt2dQryRsltProc.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "DfmUtil/DfmDocNorm.h"
//#include "TransUtil/ModuleId.h"
#include "API/AosApiG.h"
#include "Vector2D/DistStructs.h"

//////////////////////////////////////////////////////
//         Define needed constants
//////////////////////////////////////////////////////
static u32 const sgStatDocShift	= 40;
static u32 const sgStatCubeShift = 24;
static u32 const sgKeyNumPerDoc = 20;
static u32  sgTotalSavings = 0;

////////////////////////////////////////////////////////////////////////
//  Define static vars
/////////////////////////////////////////////////////////////////////////
AosDocFileMgrObjPtr AosStatDocDist::mDfm=NULL;
map<u64,AosDfmDocPtr> AosStatDocDist::mDfmDocs;
map<u64,AosStatZonePtr> AosStatDocDist::mStatZones;
vector<AosTransId>	AosStatDocDist::mTrans;

static AosDfmConfig sgDfmConf(AosDfmDocType::eNormal,
		"Stat", AosModuleId::eStatistics, false, 10, "gzip", false);

////////////////////////////////////////////////////////////////////////
//  Methods staring with Constructor
/////////////////////////////////////////////////////////////////////////
AosStatDocDist::AosStatDocDist(
		AosRundata *rdata,
		const u64 statid,
		const OmnString statcubekey,
		const u64 cube_id,
		const u64 file_id,
		const u32 time_unit_size,
		const int num_time_units,
		const i64 start_timeId,
		//const vector<AosFieldInfo> &field_defs,
		u64 snapid)
:
AosVectorFile(),
mCubeId(cube_id),
mFileSize(0),
mNumRecords(0),
mTimeUnitSize(time_unit_size),
mNumTimeUnits(num_time_units),
mRowSize(time_unit_size * num_time_units),
mFirstModifiedSdocid(-1),
mLastModifiedSdocid(-1),
mCachedRecords(0),
mCacheSize(eCacheSize),
mStartTimeId(start_timeId),
//mFieldDefs(field_defs),
mCrtBuffRaw(0),
mReliableFileRaw(0),
mSnapshotId(snapid),
mStatCubeKey(statcubekey),
mStatId(statid)
{
	if(!mDfm)
	{
		AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
		//aos_assert(vfsMgr);

		int virtual_id = cube_id;
		AosDocFileMgrObjPtr docFileMgr = vfsMgr->retrieveDocFileMgr(
			rdata, virtual_id, sgDfmConf);

		if (!docFileMgr)
		{
			docFileMgr = vfsMgr->createDocFileMgr(rdata, virtual_id, sgDfmConf);
			if (!docFileMgr)
			{
				OmnAlarm << "Failed creating docfilemgr. This is a serious problem!" << enderr;
			}
		}
		mDfm = docFileMgr;
	}

	//Docid layout (8 bytes)
	//    1     |  2  |  3   | 4    |   5   | 6  | 7  | 8  |
	// reserved    statid:2B   cubeid:2B       sdocid:3B
	//mBaseDocid = (mStatId<<sgStatDocShift)/AosGetNumCubes();
	mBaseDocid = (mStatId<<sgStatDocShift) + (AosGetNumCubes()<<sgStatCubeShift);
	mCachedStartSdocid = mBaseDocid;

	mValueOffset = 0;
}

AosStatDocDist::~AosStatDocDist()
{
}


bool
AosStatDocDist::readBlock(
		AosRundata *rdata,
		const u64 localSdocid)
{
	// This function reads a block. If the block does not
	// exist, it will create it.

	u64 localDocid = mBaseDocid + localSdocid;
	AosDfmDocPtr doc = mDfm->readDoc(rdata, localDocid);
	if (!doc)
	{
		// means maybe it's a new iil.
		//mDfmDoc = newDfmDoc(local_iilid);
		doc = OmnNew AosDfmDocNorm(localDocid);
		AosBuffPtr buff=OmnNew AosBuff(20 AosMemoryCheckerArgs);
		doc->setBodyBuff(buff);
	}

	mDfmDocs[localDocid] = doc;
	return true;
}


bool
AosStatDocDist::updateFinished(AosRundata *rdata)
{
	return saveCurrentCache(rdata);
}


bool
AosStatDocDist::isInCache(const u64 sdocid)
{
	return sdocid >= mCachedStartSdocid && sdocid < mCachedStartSdocid + mCacheSize;
}

AosDfmDocPtr
AosStatDocDist::readDfmDoc(
		AosRundata* rdata,
		u64 docid,
		AosStatZonePtr &zone)
{
	// Read the doc. If the doc does not exist, create it.
	AosDfmDocPtr doc;
	map<u64,AosDfmDocPtr>::const_iterator itr = mDfmDocs.find(docid);

	if (itr != mDfmDocs.end())
	{
		doc = itr->second;
		map<u64,AosStatZonePtr>::const_iterator zoneItr = mStatZones.find(docid);
		aos_assert_r(zoneItr != mStatZones.end(), NULL);

		zone = zoneItr->second;
	}
	else
	{
		doc = mDfm->readDoc(rdata, docid);
		if (doc)
		{
			//create relevant zones
			zone = OmnNew AosStatZone(doc->getBodyBuff(), sgKeyNumPerDoc);
		}
		else
		{
			//create the doc and relevant zone
			doc = OmnNew AosDfmDocNorm(docid);
			zone = OmnNew AosStatZone(NULL, sgKeyNumPerDoc);
			doc->setBodyBuff(zone->getBuff());
		}

		mDfmDocs[docid] = doc;
		mStatZones[docid] = zone;
	}

	return doc;
}

bool
AosStatDocDist::appendDistValue(
		AosRundata *rdata,
		u64 localSdocid,
		u32 timeId,
		u8 measureIdx,
		u64 val)
{
	u64 localDocid = localSdocid - 1;
	//u64 kIdx = (localSdocid - 1) % sgKeyNumPerDoc;
	localDocid /= sgKeyNumPerDoc;
	localDocid += mBaseDocid;

	if (!isInCache(localDocid))
	{
		saveCurrentCache(rdata);
	}

	if (mFirstModifiedSdocid < 0)
	{
		mFirstModifiedSdocid = localDocid;
		mLastModifiedSdocid = localDocid;
		mCachedStartSdocid = localDocid;
	}
	else
	{
		if (localDocid < mFirstModifiedSdocid) mFirstModifiedSdocid = localDocid;
		if (localDocid > mLastModifiedSdocid) mLastModifiedSdocid = localDocid;
	}

	// Read the doc. If the doc does not exist, create it.
	AosStatZonePtr statZone;
	AosDfmDocPtr doc = readDfmDoc(rdata, localDocid, statZone);

	bool rslt = statZone->addValue(localSdocid, timeId, val);
	aos_assert_r(rslt, false);
	
	return rslt;
}


bool
AosStatDocDist::deleteDistValue(
		AosRundata 	*rdata,
		u64 		localSdocid,
		u32 		timeId,
		u8 			measureIdx,
		u64 		val)
{
	u64 localDocid = localSdocid;
	u64 kIdx = localSdocid % sgKeyNumPerDoc;
	localDocid /= sgKeyNumPerDoc;
	localDocid += mBaseDocid;

	if (!isInCache(localDocid))
	{
		saveCurrentCache(rdata);
	}

	if (mFirstModifiedSdocid < 0)
	{
		mFirstModifiedSdocid = localDocid;
		mLastModifiedSdocid = localDocid;
		mCachedStartSdocid = localDocid;
	}
	else
	{
		if (localDocid < mFirstModifiedSdocid) mFirstModifiedSdocid = localDocid;
		if (localDocid > mLastModifiedSdocid) mLastModifiedSdocid = localDocid;
	}

	// Read the doc. If the doc does not exist, create it.
	AosStatZonePtr statZone;
	AosDfmDocPtr doc = readDfmDoc(rdata, localDocid, statZone);
	if (!doc)
	{
		doc = OmnNew AosDfmDocNorm(localDocid);
		mDfmDocs[localDocid] = doc;
	}

	return true;
}

bool
AosStatDocDist::appendDistValues(
		AosRundata 	*rdata,
		u64			docid,
		u32 		timeId,
		u8 			measureIdx,
		u64* 		vals)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosStatDocDist::updateRecordPriv(
		AosRundata 	*rdata,
		const u64 	localSdocid,
		const i64 	timeId,
		char *		data,
		const int 	dataLen,
		AosMeasureValueMapper &value_mapper)
{

	return true;
}

bool
AosStatDocDist::saveCurrentCache(AosRundata *rdata)
{
	map<u64,AosDfmDocPtr>::const_iterator itr = mDfmDocs.begin();
	AosDfmDocPtr dfmDoc;
	while (itr != mDfmDocs.end())
	{
		//OmnScreen << "write docid:" <<itr->second->getDocid() << endl;
		dfmDoc = itr->second;
		aos_assert_r(dfmDoc, false);

		mDfm->saveDoc(rdata, mTrans, dfmDoc, true);
		itr++;
	}

	mDfmDocs.clear();
	mStatZones.clear();
	mFirstModifiedSdocid = -1;
	mLastModifiedSdocid = -1;

	sgTotalSavings++;
	OmnScreen << "Total save current counters: " <<
		sgTotalSavings << endl;
	return true;
}

bool
AosStatDocDist::readIntoVt2dRecord(
		AosRundata*	rdata,
		AosVt2dRecord *rcd,
		u64 		localSdocid,
		vector<AosStatTimeArea> &timeAreas)
{
	i64 timeValue;

	//u64 localDocid = localSdocid;
	//u64 kIdx = localSdocid % sgKeyNumPerDoc;
	u64 localDocid = localSdocid - 1;
	//u64 kIdx = (localSdocid - 1) % sgKeyNumPerDoc;
	localDocid /= sgKeyNumPerDoc;
	localDocid += mBaseDocid;

	AosStatZonePtr statZone; 
	AosDfmDocPtr doc = readDfmDoc(rdata, localDocid, statZone);
	aos_assert_rr(doc, rdata, false);

	AosBuffPtr buff = doc->getBodyBuff();
	statZone->setBuff(buff);

	//struct time_idx_entry *timeEntry;
	if(timeAreas.size() !=0)
	{
		for(u32 i = 0; i < timeAreas.size(); i++)
		{
			timeValue = timeAreas[i].start_time;
			for(; timeValue <= timeAreas[i].end_time; timeValue++)
			{
				statZone->readOneTimeEntry(rdata, rcd, localSdocid, timeValue);
			}
		}
	}
	else
	{
		statZone->readOneTimeEntry(rdata, rcd, localSdocid, -1);
	}

	return true;
}

//
//For delta modify
//
bool
AosStatDocDist::updateRecord(
		AosRundata *rdata,
		const u64 sdocid,
		const i64 timeId,
		char *data,
		const int dataLen,
		AosMeasureValueMapper &value_mapper)
{
	// This function updates the record 'sdocid' for the time slot 'timeId'
	if (isInCache(sdocid))
	{
		bool rslt = updateRecordPriv(rdata, sdocid, timeId, data, dataLen, value_mapper);
		aos_assert_rr(rslt, rdata, false);
		return true;
	}

	OmnScreen << "!!!!!!!! updateRecord read block:" << sdocid << "; "
			<< "timeId:" << timeId << "; "
			<< "file_name:" << mReliableFileRaw->getFileName()
			<< endl;

	bool rslt = saveCurrentCache(rdata);
	aos_assert_rr(rslt, rdata, false);

	rslt = readBlock(rdata, sdocid);
	aos_assert_rr(rslt, rdata, false);

	rslt = updateRecordPriv(rdata, sdocid, timeId, data, dataLen, value_mapper);
	aos_assert_rr(rslt, rdata, false);
	return true;
}

//
//For query
//
bool
AosStatDocDist::readRecord(
		AosRundata 	*rdata,
		const u64 	sdocid,
		const i64 	timeId,
		bool 		&exist,
		char 		*record,
		const int 	recordLen)
{
	if (sdocid >= mNumRecords)
	{
		// The record does not exist.
		exist = false;
		return true;
	}

	if (!isInCache(sdocid))
	{
		bool rslt = saveCurrentCache(rdata);
		aos_assert_rr(rslt, rdata, false);

		rslt = readBlock(rdata, sdocid);
		aos_assert_rr(rslt, rdata, false);
	}

	int delta = sdocid - mCachedStartSdocid;
	aos_assert_rr(delta >= 0 && delta < mCacheSize, rdata, false);

	int timeDelta = timeId - mStartTimeId;
	aos_assert_rr(timeDelta >= 0 && (u32)timeDelta < mNumTimeUnits, rdata, false);
	int offset = delta * mRowSize + timeDelta * mTimeUnitSize;

	aos_assert_rr(record, rdata, false);
	aos_assert_rr(recordLen > 0 && (u32)recordLen >= mTimeUnitSize, rdata, false);
	memcpy(record, &mCrtBuffData[offset], mTimeUnitSize);
	return true;
}

int
AosStatDocDist::readRecords(
		AosRundata *rdata,
		const u64 start_sdocid,
		const int num_records,
		const i64 start_timeId,
		const int num_time_units_to_read,
		char *data,
		const int dataSize)
{
	OmnNotImplementedYet;
	return -1;
}

bool
AosStatDocDist::readRecords(
		AosRundata *rdata,
		vector<u64> &stat_docids,
		vector<AosStatTimeArea> &qry_time_areas,
		u32 &timeIdx,
		AosVt2dQryRslt *qry_rslt,
		AosVt2dQryRsltProc *qry_rslt_proc)
{
	// Ketty 2014/08/30
	OmnShouldNeverComeHere;
	bool rslt;
	u64 sdocid = 0;
	i64 time_value;
	u32 crt_time_idx = timeIdx;
	for(u32 stat_doc_idx =0; stat_doc_idx <stat_docids.size(); stat_doc_idx ++)
	{
		crt_time_idx = timeIdx;
		sdocid = stat_docids[stat_doc_idx];
		aos_assert_r(sdocid, false);
		if(!isInCache(sdocid))
		{
			OmnScreen << "!!!!!!!! read block:" << sdocid
				<< "; this:" << this
				<< endl;
			rslt = readBlock(rdata, sdocid);
			aos_assert_rr(rslt, rdata, false);
		}

		for(u32 i=0; i<qry_time_areas.size(); i++)
		{
			time_value = qry_time_areas[i].start_time;
			for(; time_value <= qry_time_areas[i].end_time; time_value++, crt_time_idx++)
			{
				rslt = readRecordPriv(rdata, stat_doc_idx, sdocid,
						crt_time_idx, time_value, qry_rslt, qry_rslt_proc);
				aos_assert_r(rslt, false);
			}
		}
	}

	timeIdx = crt_time_idx;
	//OmnTagFuncInfo << endl;
	return true;
}


void
AosStatDocDist::setMeasureCount(u8 measureCount)
{
		mMeasureCnt = measureCount;
		mValueOffset = 1 + mMeasureCnt * sizeof(measure_idx_entry); //one byte measure count + value index table
}

bool
AosStatDocDist::readRecordPriv(
		AosRundata *rdata,
		const u32 stat_doc_idx,
		const u64 sdocid,
		const u32 timeIdx,
		const i64 timeId,
		AosVt2dQryRslt *qry_rslt,
		AosVt2dQryRsltProc *qry_rslt_proc)
{
	OmnShouldNeverComeHere;
	int delta = sdocid - mCachedStartSdocid;
	int timeDelta = timeId - mStartTimeId;
	aos_assert_rr(timeDelta >= 0, rdata, false);
	aos_assert_rr(delta >= 0 && delta < mCacheSize, rdata, false);
	aos_assert_rr(mCrtBuffData, rdata, false);
	int offset = delta * mRowSize + timeDelta * mTimeUnitSize;
	char *rcd = &mCrtBuffData[offset];

	bool rslt = qry_rslt_proc->appendValue(rdata, qry_rslt,
			stat_doc_idx, sdocid, timeIdx, timeId, rcd, mTimeUnitSize);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosStatDocDist::writeRecord(
		AosRundata *rdata,
		const u64 sdocid,
		const i64 timeId,
		AosBuff *buff)
{
	OmnNotImplementedYet;
	return false;
}

bool AosStatDocDist::mergeSnapshot(
					const u32 virtual_id,
					const u64 &target_snap_id,
					const u64 &merge_snap_id,
					const AosRundataPtr &rdata)
{
	AosTransId trans_id;
	return mDfm->mergeSnapshot(rdata,target_snap_id,merge_snap_id,trans_id);
}

u64 AosStatDocDist::createSnapshot(
					const u32 virtual_id,
					const u64 &task_docid,
					const u64 &u64,
					const AosRundataPtr &rdata)
{
	AosTransId trans_id;
	return mDfm->createSnapshot(rdata,u64,trans_id);
}

bool AosStatDocDist::commitSnapshot(
					const u32 &virtual_id,
					const u64 &snap_id,
					const u64 &task_docid,
					const AosRundataPtr &rdata)
{
	AosTransId trans_id;
	return mDfm->commitSnapshot(rdata,snap_id,trans_id);
}

bool AosStatDocDist::rollBackSnapshot(
					const u32 virtual_id,
					const u64 &snap_id,
					const u64 &task_docid,
					const AosRundataPtr &rdata)
{
	AosTransId trans_id;
	return mDfm->rollbackSnapshot(rdata,snap_id,trans_id);
}

OmnString
AosStatDocDist::getFileName()
{
	OmnShouldNeverComeHere;
	return "";
}
