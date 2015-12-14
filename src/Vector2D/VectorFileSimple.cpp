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
#include "Vector2D/VectorFileSimple.h"

#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/AggrFuncObj.h"
#include "Vector2DQryRslt/Vt2dQryRslt.h"
#include "Vector2DQryRslt/Vt2dQryRsltProc.h"
#include "Debug/Debug.h"

static bool mDebug = false;

AosVectorFileSimple::AosVectorFileSimple(
		AosRundata *rdata, 
		const u64 cube_id,
		const u64 file_id, 
		const u32 time_unit_size,
		const int num_time_units,
		const i64 start_timeid,
		//const vector<AosFieldInfo> &field_defs,
		const AosRlbFileType::E file_type)
:
AosVectorFile(),
mCubeId(cube_id),
mFileSize(0),
mNumRecords(0),
mTimeUnitSize(time_unit_size),
mNumTimeUnits(num_time_units),
mRecordSize(time_unit_size * num_time_units),
mCachedStartSdocid(0),
mFirstModifiedSdocid(-1),
mLastModifiedSdocid(-1),
mCachedRecords(0),
mCacheSize(eCacheSize),
mStartTimeId(start_timeid),
//mFieldDefs(field_defs),
mCrtBuffRaw(0),
mFileType(file_type),
mReliableFileRaw(0)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert(vfsMgr);

	//
	// Set the reliable file for this class. Each reliable
	// file (vctxxx file) has a file_id
	//
	if (file_id)
	{
		mReliableFile = vfsMgr->openRlbFile(file_id, mFileType, rdata);
		aos_assert(mReliableFile);
	}
	else
	{
		//if a new file, create and open it
		mReliableFile = vfsMgr->createRlbFile(
			rdata, mCubeId, "vctfile", 0, mFileType, false);
	}
		
	aos_assert(mReliableFile);
	mReliableFileRaw = mReliableFile.getPtr();
	mFileSize = mReliableFileRaw->getFileCrtSize();
	mFileId = mReliableFile->getFileId();
	mIsGood = init(rdata);
	
	//file size is pre-allocated. 
	mNumRecords = mFileSize / mRecordSize;

	//adjust cache size based on row size
	mCachedRecords = eCacheSize / mRecordSize;
	mCacheSize = mCachedRecords * mRecordSize;

	initCounters();
}


bool 
AosVectorFileSimple::init(AosRundata *rdata)
{
	return true;
}


AosVectorFileSimple::~AosVectorFileSimple()
{
}

//
// This function is normally called when the needed sdocid is not in 
// the cache. Therefore we need to read from the disk file.
//
// But if the needed sdocid is bigger than the biggest sdocid which 
// means it is a newer sdocid entry. We need to have its value in 
// the cache firstly and save into disk later on
//
bool
AosVectorFileSimple::readBlock(
		AosRundata *rdata, 
		const u64 sdocid) 
{	
	//no need to read any block if already in cache
	if (isInCache(sdocid))
		return true;

	// This function reads a block. If the block does not
	// exist, it will create it.
	if (sdocid >= mNumRecords)
	{
		// This is a totally new sdocid, not in disk too
		// Therefore the block does not exist.
		// 
		// 1. save existing cache
		// 2. use the cache for newly created sdoc
		saveCurrentCache(rdata);

		mCrtBuff = OmnNew AosBuff(mCachedRecords * mRecordSize AosMemoryCheckerArgs);
		mCrtBuffRaw = mCrtBuff.getPtr();
		mCrtBuffData = mCrtBuffRaw->data();
		memset(mCrtBuffData, 0, mCachedRecords * mRecordSize);
		
		mCachedStartSdocid = sdocid;
		
		// Phil 2015/01/11
		//not modified yet. shall we comment out the following settings?
		mFirstModifiedSdocid = mCachedStartSdocid;
		mLastModifiedSdocid = mCachedStartSdocid + mCachedRecords - 1;
		return true;
	}

	// It is to read a block that exists in the file.
	// Check whether the block has already been read. 
	if (mCrtBuffRaw)
	{
		//aos_assert_rr(sdocid != mCachedStartSdocid, rdata, false);
		if (mFirstModifiedSdocid >= 0) saveCurrentCache(rdata);
	}

	// It needs to read in the cache.
	mCachedStartSdocid = sdocid;
	int read_size = mCachedRecords * mRecordSize;
	mCrtBuff = OmnNew AosBuff(read_size AosMemoryCheckerArgs);
	mCrtBuffRaw = mCrtBuff.getPtr();
	mCrtBuffData = mCrtBuffRaw->data();
	//arvin 2015.11.25
	//JIMODB-1226
	memset(mCrtBuffData, 0,read_size);

	i64 start_pos = sdocid * mRecordSize; //start pos is for vt2d file
	u64 tStart, tEnd;
	tStart = OmnGetTimestamp();
	bool rslt = mReliableFileRaw->readToBuff(
			start_pos, read_size, mCrtBuffData, rdata);
	aos_assert_rr(rslt, rdata, false);
	tEnd = OmnGetTimestamp();
	mReadTime3 += tEnd - tStart; 
	mReadNum3++;

	mFirstModifiedSdocid = -1;
	mLastModifiedSdocid = -1;
	
	/*OmnScreen << "!!!!!!!! VectorFile read from file. "
			<< "file_name:" << mReliableFileRaw->getFileName()
			<< "size:" << read_size << "; "
			<< endl;*/
	
	if(mDebug)
	{
		OmnScreen << "!!!!!!!! VectorFile read from file. "
				<< "file_name:" << mReliableFileRaw->getFileName()
				<< "size:" << read_size << "; "
				<< endl;
	}

	// modified by barry 2014/08/06
	mCrtBuffRaw->setDataLen(read_size);
	OmnTagFuncInfo << endl;
	return true;
}

//
//update a time slot value for a sdocid.
//If the record is not in the cache, need to
//read it into the cache
//
bool 
AosVectorFileSimple::updateRecord(
		AosRundata *rdata, 
		const u64 sdocid, 
		const i64 timeid,
		char *data, 
		const int data_len,
		AosMeasureValueMapper &value_mapper)
{
	bool rslt;

	// This function updates the record 'sdocid' for the time slot 'timeid'
	if (!isInCache(sdocid))
	{
		//read into cache firstly
		/*OmnScreen << "!!!!!!!! updateRecord read block:" << sdocid << "; "
			<< "time_id:" << timeid << "; "
			<< "file_name:" << mReliableFileRaw->getFileName()
			<< endl;*/
		if(mDebug)
		{
			//read into cache firstly
			OmnScreen << "!!!!!!!! updateRecord read block:" << sdocid << "; "
				<< "time_id:" << timeid << "; "
				<< "file_name:" << mReliableFileRaw->getFileName()
				<< endl;
		}

		rslt = saveCurrentCache(rdata);
		aos_assert_rr(rslt, rdata, false);

		rslt = readBlock(rdata, sdocid);
		aos_assert_rr(rslt, rdata, false);
	}

	rslt = updateRecordPriv(rdata, sdocid, timeid, data, data_len, value_mapper);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosVectorFileSimple::updateFinished(AosRundata *rdata)
{
	return saveCurrentCache(rdata);
}


bool
AosVectorFileSimple::isInCache(const u64 sdocid)
{
	if (!mCrtBuffRaw) return false;
	return sdocid >= mCachedStartSdocid && sdocid < mCachedStartSdocid + mCachedRecords;
}


//
//update the a statistics in the cache
//
bool
AosVectorFileSimple::updateRecordPriv(
		AosRundata *rdata, 
		const u64 sdocid, 
		const i64 timeid,
		char *data, 
		const int data_len,
		AosMeasureValueMapper &value_mapper)
{
	// Each vector file saves data for a number of time units.
	// 	[data_for_one_time_unit] [data_for_one_time_unit] ...
	//
	// The vector file is configured with the record format.
	// When modifying a record, for each field defined in the record, 
	// it locates the field in the db record, it passes on the info
	// to 
	int offset = getCacheOffset(sdocid, timeid);
	char *rcd = &mCrtBuffData[offset];
	
	bool rslt = value_mapper.updateOutputData(rdata,
			data, data_len, rcd, mTimeUnitSize);
	aos_assert_r(rslt, false);

	//
	//if data modified, we need to set dirty flag relevant variables
	//
	if (mFirstModifiedSdocid == -1 || (u64)mFirstModifiedSdocid > sdocid) 
		mFirstModifiedSdocid = sdocid;

	if (mLastModifiedSdocid == -1 || (u64)mLastModifiedSdocid < sdocid) 
		mLastModifiedSdocid = sdocid;

	return true;
}

//
//readRecord doesn't create new record or block
//
bool 
AosVectorFileSimple::readRecord( 
		AosRundata *rdata, 
		const u64 sdocid, 
		const i64 time_id,
		bool &exist,
		char *record, 
		const int record_len) 
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

	aos_assert_rr(record, rdata, false);
	aos_assert_rr(record_len > 0 && (u32)record_len >= mTimeUnitSize, rdata, false);

	int offset = getCacheOffset(sdocid, time_id);
	memcpy(record, &mCrtBuffData[offset], mTimeUnitSize);
	return true;
}


bool
AosVectorFileSimple::saveCurrentCache(AosRundata *rdata)
{
	if (!mCrtBuffRaw) return true;
	if (mFirstModifiedSdocid == -1) return true;
	
	i64 start_sdocid = mFirstModifiedSdocid - mCachedStartSdocid;
	int cache_start_pos = start_sdocid * mRecordSize;
	i64 file_start_pos = mFirstModifiedSdocid * mRecordSize;

	// Ketty 2014/08/28 this mFirstModifiedSdocid never be 0.
	i64 num_records = mLastModifiedSdocid - mFirstModifiedSdocid + 1;

	int update_len = num_records * mRecordSize;
	aos_assert_r(cache_start_pos + update_len <= mCrtBuff->buffLen(), false);
	bool rslt = mReliableFileRaw->write(file_start_pos, 
			&mCrtBuffData[cache_start_pos], update_len, true);
	aos_assert_rr(rslt, rdata, false);
	
	/*OmnScreen << "!!!!!!!! VectorFile Save to file. "
			<< "file_name:" << mReliableFileRaw->getFileName() << "; "
			<< "size:" << update_len << "; "
			<< "num_rcds:" << num_records << "; "
			<< "row_size:" << mRecordSize << "; "
			<< endl;*/
	if(mDebug)
	{
		OmnScreen << "!!!!!!!! VectorFile Save to file. "
				<< "file_name:" << mReliableFileRaw->getFileName() << "; "
				<< "size:" << update_len << "; "
				<< "num_rcds:" << num_records << "; "
				<< "row_size:" << mRecordSize << "; "
				<< endl;
	}

	mCrtBuff = 0;
	mCrtBuffRaw = 0;
	mCrtBuffData = 0;
	mFirstModifiedSdocid = -1;
	mLastModifiedSdocid = -1;

	// Ketty 2014/08/28
	mFileSize = mReliableFileRaw->getFileCrtSize();
	mNumRecords = mFileSize / mRecordSize;

	OmnTagFuncInfo << endl;
	return true;
}


bool 
AosVectorFileSimple::writeRecord( 
		AosRundata *rdata, 
		const u64 sdocid, 
		const i64 timeid,
		AosBuff *buff)
{
	OmnNotImplementedYet;
	return false;
}

int
AosVectorFileSimple::readRecords(
		AosRundata *rdata, 
		const u64 start_sdocid, 
		const int num_records, 
		const i64 start_timeid, 
		const int num_time_units_to_read, 
		char *data, 
		const int data_size)
{
	OmnNotImplementedYet;
	return -1;
}

bool
AosVectorFileSimple::readRecords(
		AosRundata *rdata,
		vector<u64> &stat_docids,
		vector<AosStatTimeArea> &qry_time_areas,
		u32 &time_idx,
		AosVt2dQryRslt *qry_rslt,
		AosVt2dQryRsltProc *qry_rslt_proc)
{
	// Ketty 2014/08/30
	bool rslt;
	u64 sdocid = 0;
	i64 time_value;
	u32 crt_time_idx = time_idx;
	for(u32 stat_doc_idx =0; stat_doc_idx <stat_docids.size(); stat_doc_idx ++)
	{
		crt_time_idx = time_idx;
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
	
	time_idx = crt_time_idx;
	//OmnTagFuncInfo << endl;
	return true;
}


bool
AosVectorFileSimple::readRecordPriv(
		AosRundata *rdata,
		const u32 stat_doc_idx,
		const u64 sdocid,
		const u32 time_idx,
		const i64 timeid,
		AosVt2dQryRslt *qry_rslt,
		AosVt2dQryRsltProc *qry_rslt_proc)
{
	// Ketty 2014/08/30
	int offset = getCacheOffset(sdocid, timeid);
	char *rcd = &mCrtBuffData[offset];
	
	bool rslt = qry_rslt_proc->appendValue(rdata, qry_rslt,
			stat_doc_idx, sdocid, time_idx, timeid, rcd, mTimeUnitSize); 
	aos_assert_r(rslt, false);
	
	return true;
}

void
AosVectorFileSimple::initCounters()
{
	    mReadTime1 = 0;
		mReadTime2 = 0;
		mReadTime3 = 0;

	    mReadNum1 = 0;           
		mReadNum2 = 0;
		mReadNum3 = 0;
}

void
AosVectorFileSimple::outputCounters()
{
	    OmnScreen << "(Statistics counters : VectorFileSimple) readBlock --- Time1 : "
			            << mReadTime1 << ", Num: " << mReadNum1 << endl;

		OmnScreen << "(Statistics counters : VectorFileSimple) for --- Time2 : "
				        << mReadTime2 << ", Num: " << mReadNum2 << endl;

		OmnScreen << "(Statistics counters : VectorFileSimple) for --- Time3 : "
				        << mReadTime3 << ", Num: " << mReadNum3 << endl;
}

bool
AosVectorFileSimple::readIntoVt2dRecord(
		AosRundata *rdata,
		AosVt2dRecord *rcd,
		u64 sdocid,
		vector<AosStatTimeArea> &timeAreas)
{
	// Ketty 2014/08/30
	bool rslt;
	i64 timeValue;
	int offset;
	char *data;

	u64 tStart, tEnd;

	//if not in cache, read into cache
	if(!isInCache(sdocid))
	{
		OmnScreen << "!!!!!!!! read block:" << sdocid << "; " 
			<< "file_name:" << mReliableFileRaw->getFileName()
			<< endl;
		tStart = OmnGetTimestamp();
		rslt = readBlock(rdata, sdocid);
		aos_assert_rr(rslt, rdata, false);
		tEnd = OmnGetTimestamp();
		mReadTime1 += tEnd - tStart;
		mReadNum1++;                
	}

    tStart = OmnGetTimestamp();
	//read the key's statistics values into vt2dRecord
	//with all the available time slots
	for(u32 i=0; i<timeAreas.size(); i++)
	{
		timeValue = timeAreas[i].start_time;
		tStart = OmnGetTimestamp();
		for(; timeValue <= timeAreas[i].end_time; timeValue++)
		{
			offset = getCacheOffset(sdocid, timeValue);
			data = &mCrtBuffData[offset];

			rcd->appendValue(timeValue, &data[0], mTimeUnitSize);
		}
		tEnd = OmnGetTimestamp();
		mReadTime3 += tEnd - tStart;
		mReadNum3++;                
	}

	tEnd = OmnGetTimestamp();
	mReadTime2 += tEnd - tStart;
	mReadNum2++;                
	//outputCounters();
	return true;
}

	
OmnString
AosVectorFileSimple::getFileName()
{ 
	return mReliableFileRaw->getFileName(); 
}

int
AosVectorFileSimple::getCacheOffset(u64 sdocid, 
								    i64 timeValue)
{
	int delta;
	int timeDelta;
	int offset;

	delta = sdocid - mCachedStartSdocid;
	timeDelta = timeValue - mStartTimeId;
	aos_assert_r(timeDelta >= 0, false);
	aos_assert_r(delta >= 0 && delta < mCachedRecords, false);
	aos_assert_r(mCrtBuffData, false);

	offset = delta * mRecordSize + timeDelta * mTimeUnitSize;
	return offset;
}
