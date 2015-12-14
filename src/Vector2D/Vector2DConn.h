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
#ifndef Aos_Vector2D_Vector2DConn_h
#define Aos_Vector2D_Vector2DConn_h

#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
//#include "SEUtil/FieldInfo.h"
#include "SEInterfaces/RlbFileType.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/BuffArrayVar.h"
#include "Vector2D/Ptrs.h"
#include "Vector2DUtil/MeasureValueMapper.h"
#include "StatUtil/StatTimeArea.h"
#include "Vector2DQryRslt/Ptrs.h"
#include "Vector2DUtil/Vt2dRecord.h"

class AosVectorFile;
class AosAggrFuncObj;

class AosVector2DConn : public OmnRCObject
{
	OmnDefineRCObject;
	
	struct TimeBlockArea
	{
		AosVectorFile			*mVectorFile;
		u32						mTimeBlockId;
		vector<AosStatTimeArea>	mTimeAreas;
	};

	enum
	{
		eDftTimeUnitsPerFile = 2000,
		
		eMetaFileStart = 100,
		eDataFileEntrySize = sizeof(u32) + sizeof(u64)
	};

	typedef hash_map<const u64, AosVectorFilePtr, u64_hash, u64_cmp> map1_t;
	typedef hash_map<const u64, AosVectorFilePtr, u64_hash, u64_cmp>::iterator itr1_t;
	typedef hash_map<const u64, AosVectorFile*, u64_hash, u32_cmp> map2_t;
	typedef hash_map<const u64, AosVectorFile*, u64_hash, u32_cmp>::iterator itr2_t;

	// Ketty 2014/09/09
	//typedef hash_map<const u32, u64, u32_hash, u32_cmp> map3_t;
	//typedef hash_map<const u32, u64, u32_hash, u32_cmp>::iterator itr3_t;
	typedef map<u32, u64> map3_t;
	typedef map<u32, u64>::iterator itr3_t;
	typedef map<u32, u64>::reverse_iterator r_itr3_t;

	//save time areas and vector file mapping
	vector<TimeBlockArea> mTimeBlockAreas;

private:
	u64			mCubeId;
	u64			mMetaFileID;
	u32			mTimeUnitSize;
	u32			mTimeUnitsPerFile;
	AosRlbFileType::E mFileType;
	
	AosReliableFilePtr	mMetaFile;
	u32			mNumDataFiles;
	AosBuffPtr 	mDataFileIDsBuff;
	map1_t		mFileMap;
	map2_t		mFileMapRaw;
	map3_t		mDataFileIDMap;

	//yang
	OmnString mStatCubeKey;
	u64 		mStatId;//yang

//yang
	AosVectorFile* mDistVecFile;
public:
//yang
	OmnString mStatFunc;
	u8 mStatType;
	enum StatType
	{
		Normal,
		Distcnt
	};
	void setStatType(u8 stattype)
	{
		mStatType=stattype;
	}
	u8 getStatType()
	{
		return mStatType;
	}
	void setStatFunc(OmnString statfunc)
	{
		mStatFunc=statfunc;
	}
	OmnString getStatFunc()
	{
		return mStatFunc;
	}
/*	bool 	addDistValue(AosRundata *rdata,
				const u64 sdocid,
				const i64 time_id,
				u64 newval);*/



	u64         mReadTime1;
	u64         mReadNum1;

	u64         mReadTime2;
	u64         mReadNum2;

	u64         mReadTime3;
	u64         mReadNum3;

	u64         mReadTime4;
	u64         mReadNum4;

	u64         mReadTime5;
	u64         mReadNum5;

	u64         mReadTime6;
	u64         mReadNum6;

	u64         mReadTime7;
	u64         mReadNum7;

	u64         mReadTime8;
	u64         mReadNum8;

	u64         mReadTime9;
	u64         mReadNum9;

	u64         mReadTime10;
	u64         mReadNum10;

	u64         mReadTime11;
	u64         mReadNum11;

	u64         mReadTime12;
	u64         mReadNum12;

	u64         mReadTime13;
	u64         mReadNum13;


public:
	AosVector2DConn(
				const u64 cube_id,
				//const u64 conn_meta_fileid,
				const u32 time_unit_size,
				const AosRlbFileType::E file_type);

	AosVector2DConn(
				const u64 statid,
				const OmnString statcubekey,
				//const u64 conn_meta_fileid,
				const u32 time_unit_size,
				const AosRlbFileType::E file_type);

	~AosVector2DConn();

	bool 	config(const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);
	
	bool 	updateRecord(AosRundata *rdata, 
				const u64 sdocid, 
				const i64 time_id,
				char *data, 
				const int data_len,
				AosMeasureValueMapper &value_mapper);
	
	bool	updateFinished(AosRundata *rdata);

	bool 	readRecords(
				AosRundata *rdata,
				vector<u64> &stat_docids,
				vector<AosStatTimeArea> &qry_time_areas,
				AosVt2dQryRslt *qry_rslt,
				AosVt2dQryRsltProc *qry_rslt_proc);
	
	bool	readRecord(
				AosRundata *rdata,
				AosVt2dRecord *rcd,
				u64 sdocid,
				vector<AosStatTimeArea> &qry_time_areas);

	//	not used yet.
	bool readRecord(AosRundata *rdata, 
				const u64 sdocid, 
				const i64 timeid,
				bool &exist,
				char *record, 
				const int record_len);

	bool readRecords(AosRundata *rdata, 
				const u64 start_sdocid, 
				const int num_records,
				const i64 start_timeid,
				const int num_time_units,
				char *data, 
				const int data_size);

	bool appendRecord(AosRundata *rdata, 
				const u64 sdocid, 
				const i64 timeid,
				AosBuff *data);

	void outputCounters();
	void initCounters();  

	bool 	splitTimeAreasByTimeBlock(
				vector<AosStatTimeArea> &qry_time_areas,
				vector<TimeBlockArea> &time_block_areas,
				vector<AosStatTimeArea> &new_time_areas);

private:
	//inline u32 getSdocidBlockId(const u64 sdocid) const
	//{
	//	return sdocid / mRowsPerFile;
	//}

	inline u32 getTimeBlockId(const u64 timeid)
	{
		return timeid / mTimeUnitsPerFile * mTimeUnitsPerFile;
	}

	AosVectorFile * getVectorFile(
						AosRundata *rdata,
						const u64 timeid);

	AosVectorFile * getVectorFileByTimeBlockId(
						AosRundata *rdata,
						const u32 time_block_id); 

	AosVectorFile * createVectorFile(
						AosRundata *rdata, 
						const u32 time_block_id);

	int readOneFile(AosRundata *rdata, 
						const u64 start_sdocid, 
						const int num_records,
						const i64 start_timeid, 
						const int num_time_units_to_read, 
						char *data, 
						const int data_size);

	AosReliableFilePtr openFile(AosRundata *rdata, const u64 file_id);
	bool readMetaFile(AosRundata *rdata);
	bool saveMetaFile(AosRundata *rdata);



	int64_t adjustQryStartTime(const int64_t qry_start_time);
	int64_t adjustQryEndTime(const int64_t qry_end_time);

	bool 	getNextTimeBlockId(
				const int64_t start_time,
				const int64_t end_time,
				u32 &crt_time_block_id,
				int64_t &crt_end_time);

};
#endif



