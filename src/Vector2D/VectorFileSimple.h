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
#ifndef Aos_Vector2D_VectorFileSimple_h
#define Aos_Vector2D_VectorFileSimple_h

#include "Vector2D/VectorFile.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
//#include "SEUtil/FieldInfo.h"
#include "SEInterfaces/RlbFileType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include "Vector2DUtil/Vt2dRecord.h"

class AosAggrFuncObj;

//
//in this class, record = doc, which can be
//1. statistics key
//2. vt2d record data
//
//A record or a doc is identified by a sdocid
//
class AosVectorFileSimple : public AosVectorFile
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxJumps = 100,
		eMaxBuffSize = 100*1000*1000,
		eCacheSize = 10*1000*1000
	};

	u64					mCubeId;
	u64					mFileSize;
	u32					mNumRecords;  //Total sdoc for a statistics
	u32					mTimeUnitSize;
	u32					mNumTimeUnits;
	u32					mRecordSize;  //it could be a key's size of the
							       //sum of all the statistics values
								   //in a time slot
	u64					mCachedStartSdocid;  //beginning sdocid of the cache

	i64					mFirstModifiedSdocid;  //first modified can be used to
											   //tell if the cache is dirty
											   //or not
	i64					mLastModifiedSdocid;  //last modified can be used with
	                                          //first modified to decide how big
											  //cache to write back to disk

	int					mCachedRecords;  //total sdocs in the cache
	int					mCacheSize;    //how many bytes in the cache	
	i64					mStartTimeId;
	//vector<AosFieldInfo> mFieldDefs;
	
	u64                 mReadTime1;
	u64                 mReadNum1;

	u64                 mReadTime2;
	u64                 mReadNum2;

	u64                 mReadTime3;
	u64                 mReadNum3;

	AosBuffPtr			mCrtBuff;
	AosBuff *			mCrtBuffRaw;
	char *				mCrtBuffData;
	AosRlbFileType::E 	mFileType;
	AosReliableFilePtr	mReliableFile;
	AosReliableFile *  	mReliableFileRaw;


public:
	AosVectorFileSimple(AosRundata *rdata, 
				const u64 cube_id,
				const u64 file_id, 
				const u32 time_unit_size,
				const int num_time_units,
				const i64 start_time_id, 
				//const vector<AosFieldInfo> &field_defs,
				const AosRlbFileType::E file_type);

	~AosVectorFileSimple();
	
	virtual OmnString getFileName();

	virtual bool updateRecord(AosRundata *rdata, 
						const u64 sdocid, 
						const i64 timeid,
						char *data, 
						const int data_len,
						AosMeasureValueMapper &value_mapper);
	
	virtual bool updateFinished(AosRundata *rdata);

	virtual bool readRecord(AosRundata *rdata, 
						const u64 sdocid, 
						const i64 time_id,
						bool &exist,
						char *record, 
						const int record_len);

	virtual int readRecords(AosRundata *rdata, 
						const u64 start_sdocid, 
						const int num_records, 
						const i64 start_timeid, 
						const int num_time_units_to_read, 
						char *data, 
						const int data_size);

	virtual bool writeRecord( AosRundata *rdata, 
	  					const u64 sdocid, 
	 					const i64 timeid,
	  					AosBuff *buff);

	virtual bool readRecords(
						AosRundata *rdata,
						vector<u64> &stat_docids,
						vector<AosStatTimeArea> &qry_time_areas,
						u32 &time_idx,
						AosVt2dQryRslt *qry_rslt,
						AosVt2dQryRsltProc *qry_rslt_proc);


	//yang
	virtual bool appendDistValue(AosRundata *rdata,u64 docid,u32 timeid,u8 measure_idx,u64 vid){ return 0;}
	virtual void setMeasureCount(u8 measure_cnt){}

	virtual bool deleteDistValue(
			AosRundata *rdata,
			u64 local_sdocid,u32 timeid,
			u8 measure_idx,u64 val){return 0;}

	// bool readRecords( AosRundata *rdata, 
	// 					const u64 *sdocids, 
	// 					const int num_sdocids,
	// 					AosBuffPtr &buff);

	// bool writeBlock(AosRundata *rdata, 
	// 					const u64 sdocid, 
	// 					const int num_rows,
	// 					AosBuff *buff);

	void outputCounters();
	void initCounters();

private:
	bool readBlock( AosRundata *rdata, const u64 sdocid); 
	i64 getCellBlockPos(AosRundata *rdata, 
						const u64 sdocid, 
						const bool create_flag);

	bool setToCrtBuff( AosRundata *rdata,
						const u64 sdocid,
						const AosBuffPtr &buff);

	i64 createCellBlock(AosRundata *rdata, const u64 sdocid);
	bool init(AosRundata *rdata);
	bool initializeVectorFileSimple(AosRundata *rdata);
	bool saveCurrentCache(AosRundata *rdata);
	bool createNewBuff(AosRundata *rdata);

	bool updateRecordPriv(AosRundata *rdata, 
						const u64 sdocid, 
						const i64 timeid,
						char *data, 
						const int data_len,
						AosMeasureValueMapper &value_mapper);

	bool isInCache(const u64 sdocid);
	int getCacheOffset(u64 sdocid, i64 timeValue);

	bool 	readRecordPriv(
				AosRundata *rdata,
				const u32 stat_doc_idx,
				const u64 sdocid,
				const u32 time_idx,
				const i64 timeid,
				AosVt2dQryRslt *qry_rslt,
				AosVt2dQryRsltProc *qry_rslt_proc);

	bool readIntoVt2dRecord(
			AosRundata *rdata,
			AosVt2dRecord *rcd,
			u64 sdocid,
			vector<AosStatTimeArea> &timeAreas);
};
#endif



