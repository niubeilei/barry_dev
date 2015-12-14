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
#ifndef Aos_Vector2D_VectorFile_h
#define Aos_Vector2D_VectorFile_h

#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/RlbFileType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include "Vector2DUtil/MeasureValueMapper.h"
#include "Vector2DUtil/Vt2dRecord.h"
#include "StatUtil/StatTimeArea.h"
#include "Vector2DQryRslt/Ptrs.h"


class AosAggrFuncObj;

class AosVectorFile : public OmnRCObject
{
protected:
	bool		mIsGood;
	u64			mFileId;

public:
	virtual ~AosVectorFile(){}

	inline bool isGood() const {return mIsGood;}
	inline u64 getFileId() const{return mFileId;}
	
	virtual OmnString getFileName() = 0;

	virtual bool updateRecord(AosRundata *rdata, 
						const u64 sdocid, 
						const i64 timeid,
						char *data, 
						const int data_len,
						AosMeasureValueMapper &value_mapper) = 0;

	virtual bool updateFinished(AosRundata *rdata) = 0;

	virtual bool readRecord(AosRundata *rdata, 
						const u64 sdocid, 
						const i64 time_id,
						bool &exist,
						char *record, 
						const int record_len) = 0;

	virtual int readRecords(AosRundata *rdata, 
						const u64 start_sdocid, 
						const int num_records, 
						const i64 start_timeid, 
						const int num_time_units_to_read, 
						char *data, 
						const int data_size) = 0;

	virtual bool writeRecord( AosRundata *rdata, 
	  					const u64 sdocid, 
	 					const i64 timeid,
	  					AosBuff *buff) = 0;
	
	virtual bool readRecords(
						AosRundata *rdata,
						vector<u64> &stat_docids,
						vector<AosStatTimeArea> &qry_time_areas,
						u32 &time_idx,
						AosVt2dQryRslt *qry_rslt,
						AosVt2dQryRsltProc *qry_rslt_proc) = 0;

	virtual bool readIntoVt2dRecord(
			AosRundata *rdata,
			AosVt2dRecord *rcd,
			u64 sdocid,
			vector<AosStatTimeArea> &timeAreas)
	{
			OmnNotImplementedYet;
			return false;
	}

	//yang
	virtual bool appendDistValue(
			AosRundata *rdata,
			u64 docid,
			u32 timeid,
			u8 measure_idx, 
			u64 vid) = 0;
	
	virtual void setMeasureCount(u8 measure_cnt) = 0;
	
	virtual bool deleteDistValue(
			AosRundata *rdata,
			u64 local_sdocid,u32 timeid,
			u8 measure_idx,u64 val) = 0;

	/*
	virtual bool readRecord( AosRundata *rdata, 
						const u64 sdocid, 
						AosBuffPtr &buff, 
						const bool create_flag) = 0;

	virtual bool writeRecord( AosRundata *rdata, 
						const u64 sdocid, 
						AosBuffPtr &buff) = 0;

	virtual bool readRecords( AosRundata *rdata, 
						const u64 *sdocids, 
						const int num_sdocids,
						AosBuffPtr &buff) = 0;

	virtual bool readBlock( AosRundata *rdata, 
						const u64 sdocid, 
						const int num_rows,
						AosBuffPtr &buff) = 0;

	virtual bool writeBlock(AosRundata *rdata, 
						const u64 sdocid, 
						const int num_rows,
						AosBuff *buff) = 0;

	virtual u64 getFileId() const = 0;
	*/
};
#endif



