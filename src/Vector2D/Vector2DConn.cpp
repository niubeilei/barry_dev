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
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Vector2D/Vector2DConn.h"


#include "API/AosApi.h"
#include "Vector2D/Ptrs.h"
#include "Vector2D/VectorFile.h"
#include "Vector2D/StatDocDist.h"
#include "Vector2D/VectorFileSimple.h"
#include "ReliableFile/ReliableFile.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "Vector2DQryRslt/Vt2dQryRslt.h"
#include "Vector2DQryRslt/Vt2dQryRsltProc.h"
#include "Debug/Debug.h"

//define constants
static int64_t sgTimeOutOfRange = 0x1fffffffffffffff;

AosVector2DConn::AosVector2DConn(
		const u64 cube_id,
		const u32 time_unit_size,
		//const u64 conn_meta_fileid,
		const AosRlbFileType::E file_type)
:
mCubeId(cube_id),
mMetaFileID(0),
mTimeUnitSize(time_unit_size),
mTimeUnitsPerFile(0),
mFileType(file_type),
mNumDataFiles(0),
mDistVecFile(NULL)//yang
{
	initCounters();
	mTimeBlockAreas.clear();
}


//yang
//for dist count
AosVector2DConn::AosVector2DConn(
		const u64 statid,
		const OmnString statcubekey,
		const u32 time_unit_size,
		//const u64 conn_meta_fileid,
		const AosRlbFileType::E file_type)
:
mStatCubeKey(statcubekey),
mMetaFileID(0),
mTimeUnitSize(time_unit_size),
mTimeUnitsPerFile(0),
mFileType(file_type),
mNumDataFiles(0),
mDistVecFile(NULL),
mStatId(statid)
{
	mTimeBlockAreas.clear();
}


AosVector2DConn::~AosVector2DConn()
{
}


bool
AosVector2DConn::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	//	<vt2d_conn time_bucket_weight="1000" start_time_slot="14610" meta_fileid="xx"/>
	aos_assert_r(conf, false);
	bool rslt;

	mTimeUnitsPerFile = conf->getAttrU64("time_bucket_weight", eDftTimeUnitsPerFile);
	//u64 start_time_slot = conf->getAttrU64("start_time_slot", 0);

	mMetaFileID = conf->getAttrU64("meta_fileid", 0);
	aos_assert_r(mMetaFileID, false);

	mMetaFile = openFile(rdata.getPtr(), mMetaFileID);
	aos_assert_rr(mMetaFile, rdata, false);

	if(mMetaFile->getLength() > eMetaFileStart)
	{
		rslt = readMetaFile(rdata.getPtr());
		aos_assert_r(rslt, false);
	}
	else
	{
		mNumDataFiles = 0;
		mDataFileIDsBuff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	}

	OmnTagFuncInfo << endl;

	return true;
}

void
AosVector2DConn::initCounters()
{
	    mReadTime1 = 0;
		mReadTime2 = 0;
		mReadTime3 = 0;
		mReadTime4 = 0;
		mReadTime5 = 0;
		mReadTime6 = 0;
		mReadTime7 = 0;
		mReadTime8 = 0;
		mReadTime9 = 0;
		mReadTime10 = 0;
		mReadTime11 = 0;
		mReadTime12 = 0;
		mReadTime13 = 0;

		mReadNum1 = 0;          
	    mReadNum2 = 0;
		mReadNum3 = 0;
		mReadNum4 = 0;
		mReadNum5 = 0;
		mReadNum6 = 0;
		mReadNum7 = 0;
		mReadNum8 = 0;
		mReadNum9 = 0;
		mReadNum10 = 0;
		mReadNum11 = 0;
		mReadNum12 = 0;
		mReadNum13 = 0;
}

void
AosVector2DConn::outputCounters()
{
	    OmnScreen << "(Statistics counters : Vector2DConn ) splitTimeAreasByTimeBlock --- Time1 : "
			            << mReadTime1 << ", Num: " << mReadNum1 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) getVectorFileByTimeBlockId --- Time2 : "
				        << mReadTime2 << ", Num: " << mReadNum2 << endl;  

		OmnScreen << "(Statistics counters : Vector2DConn ) readIntoVt2dRecord --- Time3 : "
			            << mReadTime3 << ", Num: " << mReadNum3 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) adjustQryStartTime --- Time4 : "      
			            << mReadTime4 << ", Num: " << mReadNum4 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) adjustQryEndTime --- Time5 : "      
			            << mReadTime5 << ", Num: " << mReadNum5 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) getNextTimeBlockId --- Time6 : "      
			            << mReadTime6 << ", Num: " << mReadNum6 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) block_idx_set.find --- Time7 : "      
			            << mReadTime7 << ", Num: " << mReadNum7 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) while --- Time8 : "      
			            << mReadTime8 << ", Num: " << mReadNum8 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) block_area.mTimeAreas.clear --- Time9 : "      
			            << mReadTime9 << ", Num: " << mReadNum9 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) time_block_areas.push_back --- Time10 : "      
			            << mReadTime10 << ", Num: " << mReadNum10 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) block_idx_set.insert --- Time11 : "      
			            << mReadTime11 << ", Num: " << mReadNum11 << endl;

		OmnScreen << "(Statistics counters : Vector2DConn ) time_block_areas[block_idx].mTimeAreas.push_back --- Time12 : "      
			            << mReadTime12 << ", Num: " << mReadNum12 << endl;
		                                                                          
		OmnScreen << "(Statistics counters : Vector2DConn ) new_time_areas.push_back --- Time13 : "      
			            << mReadTime13 << ", Num: " << mReadNum13 << endl;
		                                                                          
		                                                                          
		                                                                          

		//output vector file counters
		AosVectorFileSimple *vfile;
		itr2_t itr;
		itr = mFileMapRaw.begin();
		while (itr != mFileMapRaw.end())
		{
			vfile = (AosVectorFileSimple *)itr->second;
			vfile->outputCounters();
			itr++;
		}
}


bool
AosVector2DConn::readMetaFile(AosRundata *rdata)
{
	// Data File IDs are saved as follows:
	// 		[time_block_id, file_id]
	// 		[time_block_id, file_id]
	// 		...
	// 		[time_block_id, file_id]
	
	u64 offset = eMetaFileStart;
	mNumDataFiles = mMetaFile->readInt(offset, -1, rdata);
	aos_assert_rr(mNumDataFiles >= 0, rdata, false);
	u32 size_to_read = (u32)mNumDataFiles * eDataFileEntrySize;
	mDataFileIDsBuff = OmnNew AosBuff(size_to_read AosMemoryCheckerArgs);
	offset += sizeof(int);
	char *data = (char *)mDataFileIDsBuff->data();
	memset(data, 0, size_to_read);
	bool rslt = mMetaFile->readToBuff(offset, size_to_read, data, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	mDataFileIDsBuff->setDataLen(size_to_read);

	// Builds the map
	int idx = 0;
	for (u32 i=0; i<mNumDataFiles; i++)
	{
		u32 time_block_id = *(u32*)&data[idx];
		u64 file_id = *(u64*)&data[idx+sizeof(u32)];
		aos_assert_r(file_id, false);
		idx += sizeof(u32) + sizeof(u64);
		mDataFileIDMap[time_block_id] = file_id;
	}

	OmnTagFuncInfo << endl;
	return true;
}


bool 
AosVector2DConn::appendRecord(
		AosRundata *rdata, 
		const u64 sdocid, 
		const i64 timeid,
		AosBuff *data)
{
	AosVectorFile *vector_file = getVectorFile(rdata, timeid);
	aos_assert_rr(vector_file, rdata, false);

	OmnTagFuncInfo << endl;
	return vector_file->writeRecord(rdata, sdocid, timeid, data);
}


AosVectorFile *
AosVector2DConn::getVectorFile(
		AosRundata *rdata,
		const u64 timeid) 
{
	// Given [timeid], this function retrieves the vector file
	// for the time id. If the file does not exist yet, it will
	// create it.
	 
	u32 time_block_id = getTimeBlockId(timeid);
	OmnTagFuncInfo << endl;
	return getVectorFileByTimeBlockId(rdata, time_block_id);
}


AosVectorFile *
AosVector2DConn::getVectorFileByTimeBlockId(
		AosRundata *rdata,
		const u32 time_block_id) 
{
	itr3_t tt = mDataFileIDMap.find(time_block_id);
	u64 file_id = 0;
	if (tt != mDataFileIDMap.end())
	{
		file_id = tt->second;
	}

	if (file_id == 0)
	{
		// Did not find it. 
		return 0;
	}

	itr2_t itr = mFileMapRaw.find(file_id);
	if (itr != mFileMapRaw.end()) 
	{
		if (itr->second->isGood()) return itr->second;
		return 0;
	}

	
	AosVectorFilePtr vector_file;

	// It is not in the map yet. Open it.
	vector_file = OmnNew AosVectorFileSimple(
			rdata, mCubeId, file_id, mTimeUnitSize, mTimeUnitsPerFile,
			time_block_id, mFileType);

	// Ketty 2014/09/09
	mFileMap[file_id] = vector_file;
	mFileMapRaw[file_id] = vector_file.getPtr();
	
	OmnTagFuncInfo << endl;
	return vector_file.getPtr();
	//if (vector_file->isGood()) return vector_file.getPtr();
	//return 0;
}


AosVectorFile *
AosVector2DConn::createVectorFile(
		AosRundata *rdata, 
		const u32 time_block_id)
{
	// This function creates a vector file for the time block id. 
	// 1. Create the file
	AosVectorFilePtr vector_file;


	vector_file = OmnNew AosVectorFileSimple(
			rdata, mCubeId, 0, mTimeUnitSize, mTimeUnitsPerFile,
			time_block_id, mFileType);

	aos_assert_rr(vector_file, rdata, false);

	u64 file_id = vector_file->getFileId();

	mFileMap[file_id] = vector_file;
	mFileMapRaw[file_id] = vector_file.getPtr();
	mDataFileIDMap[time_block_id] = file_id;

	mNumDataFiles++;
	mDataFileIDsBuff->gotoEnd();
	mDataFileIDsBuff->setU32(time_block_id);
	mDataFileIDsBuff->setU64(file_id);
	OmnScreen << "Vector2DConn add new file:"
		<< "num_data_files:" << mNumDataFiles << "; "
		<< "time_block_id:" << time_block_id << "; "
		<< "file_id:" << file_id << "; "
		<< "mDataFileIDsBuff_len:" << mDataFileIDsBuff->dataLen() << "; "
		<< endl;

	bool rslt = saveMetaFile(rdata);
	aos_assert_rr(rslt, rdata, 0);

	OmnTagFuncInfo << endl;
	return vector_file.getPtr();
}


bool
AosVector2DConn::saveMetaFile(AosRundata *rdata)
{
	aos_assert_rr(mMetaFile, rdata, false);

	// Save 'mNumDataFiles'
	u64 offset = eMetaFileStart;
	mMetaFile->setInt(offset, mNumDataFiles, false, rdata);

	// Save 'mDataFileIDsBuff'.
	offset += sizeof(int);
	const char *data = mDataFileIDsBuff->data();
	i64 data_len = mDataFileIDsBuff->dataLen();
	aos_assert_rr(data_len == mNumDataFiles * eDataFileEntrySize, rdata, false);
	mMetaFile->write(offset, data, data_len, true);

	OmnScreen << "Vector2DConn modify MetaFile:"
		<< "meta_fname:" << mMetaFile->getFileName() << "; "
		<< "offset:" << offset << "; "
		<< "data_len:" << data_len << "; "
		<< endl;

	OmnTagFuncInfo << endl;
	return true;
}


AosReliableFilePtr
AosVector2DConn::openFile(
		AosRundata *rdata, 
		const u64 file_id)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_rr(vfsMgr, rdata, 0);
	return vfsMgr->openRlbFile(file_id, mFileType, rdata);
}


bool 
AosVector2DConn::readRecord( 
		AosRundata *rdata, 
		const u64 sdocid, 
		const i64 timeid,
		bool &exist,
		char *record, 
		const int record_len)
{
	AosVectorFile *vector_file = getVectorFile(rdata, timeid);
	if (!vector_file)
	{
		exist = false;
		return true;
	}
	return vector_file->readRecord(rdata, sdocid, timeid, exist, record, record_len);
}


bool 
AosVector2DConn::readRecords(
		AosRundata *rdata, 
		const u64 start_sdocid, 
		const int num_records,
		const i64 start_timeid,
		const int num_time_units,
		char *data, 
		const int data_size)
{
	// This function reads in records starting from 'start_sdocid'
	// for 'num_records'. For each record, it will read 'start_timeid'
	// for 'num_time_units'.
	// The caller should have allocated enough space. The space is
	// arranged as:
	// 		[time_unit_size * num_time_units]
	// 		[time_unit_size * num_time_units]
	// 		...
	aos_assert_rr(data, rdata, false);

	// Read the first file.
	i64 time_block_id = getTimeBlockId(start_timeid);
	int time_units_read = readOneFile(rdata, start_sdocid, num_records, 
			start_timeid, num_time_units, data, data_size);
	aos_assert_rr(time_units_read >= 0, rdata, false);

	// Read all the 'whole file'. 
	int time_units_left = num_time_units - (mTimeUnitsPerFile - start_timeid % mTimeUnitsPerFile);
	int num_whole_files = time_units_left / mTimeUnitsPerFile;
	time_block_id += mTimeUnitsPerFile;
	if (num_whole_files > 0)
	{
		for (int i=0; i<num_whole_files; i++)
		{
			time_units_read = readOneFile(rdata, start_sdocid, num_records, 
				time_block_id, time_units_left, data, data_size);
			if (time_units_read > 0)
			{
				aos_assert_rr((u32)time_units_read == mTimeUnitsPerFile, rdata, false);
			}
			time_block_id += mTimeUnitsPerFile;
		}
	}

	// Read the last 'partial' file
	i64 tt1 = (start_timeid + num_time_units) / mTimeUnitsPerFile * mTimeUnitsPerFile;
	i64 tt2 = (start_timeid + num_time_units) % mTimeUnitsPerFile;
	aos_assert_rr(tt1 == time_block_id, rdata, false);
	aos_assert_rr(tt2 == time_units_left, rdata, false);
	if (time_units_left > 0)
	{
		time_units_read = readOneFile(rdata, start_sdocid, num_records,
				time_block_id, time_units_left, data, data_size);
	}

	OmnTagFuncInfo << endl;
	return true;
}


int 
AosVector2DConn::readOneFile(
		AosRundata *rdata, 
		const u64 start_sdocid, 
		const int num_records,
		const i64 start_timeid, 
		const int num_time_units_to_read, 
		char *data, 
		const int data_size)
{
	// This function reads data for records [sdocid, sdocid+num_records]. 
	// The time starts at 'start_timeid'. It reads up to either the
	// largest timeid the file can hold or the requested number of time
	// units. Upon success, it returns the number of time units it read.
	// If error occurs, it returns -1.
	AosVectorFile *file = getVectorFile(rdata, start_timeid);
	if (!file)
	{
		// The file does not exist. Simply return.
		return 0;
	}

	return file->readRecords(rdata, start_sdocid, num_records, start_timeid, 
			num_time_units_to_read, data, data_size);
}

/*
//yang,now useless
bool AosVector2DConn::addDistValue(AosRundata *rdata,
			const u64 sdocid,
			const i64 timeid,
			u64 newval)
{
	if(!mDistVecFile)
	{
		u32 time_block_id = getTimeBlockId(timeid);
		mDistVecFile=createVectorFile(rdata, time_block_id);
		aos_assert_rr(mDistVecFile, rdata, false);
	}

	return mDistVecFile->appendDistValue(rdata, sdocid, timeid,newval);
}
*/

bool 
AosVector2DConn::updateRecord(
		AosRundata *rdata,
		const u64 sdocid,
		const i64 timeid,
		char *data,
		const int data_len,
		AosMeasureValueMapper &value_mapper)
{
	AosVectorFile *vector_file = getVectorFile(rdata, timeid);
	if (!vector_file)
	{
		u32 time_block_id = getTimeBlockId(timeid);
		vector_file = createVectorFile(rdata, time_block_id);
		aos_assert_rr(vector_file, rdata, false);	
		
		OmnScreen << "Ketty Stat Print. create new vt2d_file, "
			<< "file_name:" << vector_file->getFileName() << "; "
			<< "time_id:" << timeid << "; "
			<< "time_block_id:" << time_block_id << "; "
			<< endl;
	}

	//OmnScreen << "save to vt2d File:"
	//	<< "; sdocid:" << sdocid
	//	<< "; timeid:" << timeid
	//	<< "; vt2d_file:" << getTimeBlockId(timeid)
	//	<< endl; 
	return vector_file->updateRecord(rdata, sdocid, timeid,
			data, data_len, value_mapper); 
}


bool
AosVector2DConn::updateFinished(AosRundata *rdata)
{
	itr2_t tt = mFileMapRaw.begin();
	for(; tt != mFileMapRaw.end(); tt++)
	{
		(tt->second)->updateFinished(rdata);	
	}
	
	return true;
}


bool
AosVector2DConn::readRecords(
		AosRundata *rdata,
		vector<u64> &stat_docids,
		vector<AosStatTimeArea> &qry_time_areas,
		AosVt2dQryRslt *qry_rslt,
		AosVt2dQryRsltProc *qry_rslt_proc)
{
	// Ketty 2014/08/30
	aos_assert_r(qry_rslt && qry_rslt_proc, false);	
	
	vector<TimeBlockArea> time_block_areas;
	vector<AosStatTimeArea> new_time_areas;
	bool rslt = splitTimeAreasByTimeBlock(qry_time_areas, time_block_areas, new_time_areas);
	//aos_assert_r(new_time_areas.size(), false);
	// maybe new_time_areas.size() == 0. use query not exist time_areas.

	rslt = qry_rslt_proc->appendVt2dRecords(rdata,
			qry_rslt, stat_docids, new_time_areas);
	aos_assert_r(rslt, false);
	
	AosVectorFile * vector_file;
	u32 time_idx = 0;
	for(u32 i=0; i<time_block_areas.size(); i++)
	{
		vector_file = getVectorFileByTimeBlockId(rdata, time_block_areas[i].mTimeBlockId);	
		aos_assert_rr(vector_file, rdata, false);

		rslt = vector_file->readRecords(rdata, stat_docids,
				time_block_areas[i].mTimeAreas, time_idx, qry_rslt, qry_rslt_proc);
		aos_assert_r(rslt, false);
	}
	
	OmnTagFuncInfo << endl;
	return true;
}

bool
AosVector2DConn::readRecord(
		AosRundata *rdata,
		AosVt2dRecord *rcd,
		u64 sdocid,
		vector<AosStatTimeArea> &qry_time_areas)
{
	// Ketty 2014/08/30
	bool rslt;
	AosVectorFile *vector_file;
	
	mTimeBlockAreas.clear();
	//if (mTimeBlockAreas.size() == 0)
	//{
		vector<TimeBlockArea> time_block_areas;
		vector<AosStatTimeArea> new_time_areas;

		rslt = splitTimeAreasByTimeBlock(qry_time_areas, time_block_areas, new_time_areas);
		for(u32 i=0; i<time_block_areas.size(); i++)
		{
			vector_file = getVectorFileByTimeBlockId(rdata, time_block_areas[i].mTimeBlockId);
			if(vector_file)
			{
				time_block_areas[i].mVectorFile = vector_file;
				mTimeBlockAreas.push_back(time_block_areas[i]);
			}
		}
	//}

	for(u32 i=0; i<mTimeBlockAreas.size(); i++)
	{
		vector_file = mTimeBlockAreas[i].mVectorFile;
		aos_assert_rr(vector_file, rdata, false);

		// Ketty 2014/10/20
		//rslt = vector_file->readIntoVt2dRecord(rdata, rcd, sdocid, qry_time_areas);
		rslt = vector_file->readIntoVt2dRecord(rdata, rcd, sdocid,
				mTimeBlockAreas[i].mTimeAreas);
		aos_assert_r(rslt, false);
	}
	
	OmnTagFuncInfo << endl;
	//outputCounters();
	return true;
}

bool
AosVector2DConn::splitTimeAreasByTimeBlock(
		vector<AosStatTimeArea> &qry_time_areas,
		vector<TimeBlockArea> &time_block_areas,
		vector<AosStatTimeArea> &new_time_areas)
{
	bool rslt;
	set<u32> block_idx_set;
	set<u32>::iterator s_itr;
	TimeBlockArea block_area;
	AosStatTimeArea t_area;
	u32 block_idx;

	u32 crt_time_block_id = 0;
	int64_t crt_start_time = -1, crt_end_time = -1, qry_end_time = -1;
	for(u32 i=0; i<qry_time_areas.size(); i++)
	{
		crt_start_time = adjustQryStartTime(qry_time_areas[i].start_time);
		if (crt_start_time == sgTimeOutOfRange)
			continue;

		qry_end_time = adjustQryEndTime(qry_time_areas[i].end_time);
		if (qry_end_time == sgTimeOutOfRange)
			continue;

		if(crt_start_time > qry_end_time)	continue;

		crt_end_time = -1;
		while(crt_end_time != qry_end_time)
		{
			rslt = getNextTimeBlockId(crt_start_time, qry_end_time, 
					crt_time_block_id, crt_end_time); 
			aos_assert_r(rslt, false);
			
			s_itr = block_idx_set.find(crt_time_block_id);
			if(s_itr == block_idx_set.end())
			{
				block_area.mTimeBlockId = crt_time_block_id;
				block_area.mTimeAreas.clear();

				time_block_areas.push_back(block_area);
				block_idx = time_block_areas.size() - 1;
				block_idx_set.insert(block_idx);
			}
			else
			{
				block_idx = *s_itr;
			}
			aos_assert_r(block_idx < time_block_areas.size(), false);
			
			t_area.start_time = crt_start_time; 
			t_area.end_time = crt_end_time; 
			t_area.time_unit = qry_time_areas[i].time_unit; 

			time_block_areas[block_idx].mTimeAreas.push_back(t_area);	
			new_time_areas.push_back(t_area);
			crt_start_time = crt_end_time + 1;
		}
	}
	
	return true;
}


int64_t
AosVector2DConn::adjustQryStartTime(const int64_t qry_start_time)
{
	int64_t crt_start_time = -1;
	itr3_t itr = mDataFileIDMap.begin();
	u32 crt_time_block_id;
	if(itr != mDataFileIDMap.end())
	{
		crt_time_block_id = itr->first;
		crt_start_time = crt_time_block_id;
	}

	if(qry_start_time == -1)	return crt_start_time;
	if(qry_start_time < crt_start_time)	return crt_start_time;

	int64_t crt_end_time = -1;
	r_itr3_t r_itr = mDataFileIDMap.rbegin();
	if(r_itr != mDataFileIDMap.rend())
	{
		crt_time_block_id = r_itr->first;
		crt_end_time = crt_time_block_id + mTimeUnitsPerFile - 1;
	}
	
	//if(qry_start_time > crt_end_time)	return crt_end_time;
	//wumeng 2015-11-25 bug jimodb-1252
	if(qry_start_time > crt_end_time) return sgTimeOutOfRange;
	return qry_start_time;
}


int64_t
AosVector2DConn::adjustQryEndTime(const int64_t qry_end_time)
{
	int64_t crt_end_time = -1;
	r_itr3_t r_itr = mDataFileIDMap.rbegin();
	u32 crt_time_block_id;
	if(r_itr != mDataFileIDMap.rend())
	{
		crt_time_block_id = r_itr->first;
		crt_end_time = crt_time_block_id + mTimeUnitsPerFile - 1;
	}

	if(qry_end_time == -1)	return crt_end_time;
	if(qry_end_time > crt_end_time)	return crt_end_time;
	
	int64_t crt_start_time = -1;
	itr3_t itr = mDataFileIDMap.begin();
	if(itr != mDataFileIDMap.end())
	{
		crt_time_block_id = itr->first;
		crt_start_time = crt_time_block_id;
	}
	
	//if(qry_end_time < crt_start_time)	return crt_start_time;
	//wumeng 2015-11-25 bug jimodb-1252
	if(qry_end_time < crt_start_time)	return sgTimeOutOfRange;
	return qry_end_time;
}


bool
AosVector2DConn::getNextTimeBlockId(
		const int64_t start_time,
		const int64_t end_time,
		u32 &crt_time_block_id,
		int64_t &crt_end_time)
{
	crt_time_block_id = getTimeBlockId(start_time);
	
	int end_time_block_id = getTimeBlockId(end_time);
	if(crt_time_block_id == (u32)end_time_block_id)
	{
		crt_end_time = end_time;
	}
	else
	{
		crt_end_time = crt_time_block_id + mTimeUnitsPerFile - 1;
	}
	OmnTagFuncInfo << endl;
	return true;
}


