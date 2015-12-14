
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
#ifndef Aos_Vector2D_StatDocDist_h
#define Aos_Vector2D_StatDocDist_h

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
#include "Vector2D/StatZone.h"
#include "Vector2D/Ptrs.h"

#include <map>

class AosAggrFuncObj;
class AosStatDocDist : public AosVectorFile
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxJumps = 100,
		eMaxBuffSize = 100*1000*1000,
		eCacheSize = 1000
	};

	u64		mStatId;	//yang
	u64 	mBaseDocid;//yang

	u64					mCubeId;
	u64					mFileSize;
	u32					mNumRecords;
	u32					mNumDocs;
	u32					mTimeUnitSize;
	u32					mNumTimeUnits;
	u32					mRowSize;
	u64					mCachedStartSdocid;
	i64					mFirstModifiedSdocid;
	i64					mLastModifiedSdocid;
	int					mCachedRecords;
	int					mCacheSize;
	i64					mStartTimeId;
	//vector<AosFieldInfo> mFieldDefs;

	AosBuff *			mCrtBuffRaw;
	char *				mCrtBuffData;
	AosRlbFileType::E 	mFileType;
	AosReliableFilePtr	mReliableFile;
	AosReliableFile *  	mReliableFileRaw;
	

	u64 mSnapshotId;

	OmnString mStatCubeKey;

	//static map<OmnString,vector<u64> > mStatDocs;//map stat cube key to docids
	//static u64 mLastStatLastDocid;
	static AosDocFileMgrObjPtr mDfm;
	static map<u64,AosDfmDocPtr> mDfmDocs;
	static map<u64,AosStatZonePtr> mStatZones;
	static map<u64,u64> mDocDataLens;
	static vector<AosTransId>	mTrans;

	u8 mMeasureCnt;
	u64 mValueOffset;

public:

	enum eopr
	{
		INSERT,
		DELETE
	};


	AosStatDocDist(AosRundata *rdata,
				const u64 statid,
				const OmnString statcubekey,
				const u64 cube_id,
				const u64 file_id,
				const u32 time_unit_size,
				const int num_time_units,
				const i64 start_time_id,
				//const vector<AosFieldInfo> &field_defs,

				u64 snapid=0);

	~AosStatDocDist();


	static u64 getStatFirstDocId(OmnString statcubekey);
	static u64 nextStatDocid(OmnString statcubekey);

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
	virtual bool appendDistValue(
			AosRundata *rdata,
			u64 docid,
			u32 timeid,
			u8 measure_idx,
			u64 val);

	virtual bool appendDistValues(
			AosRundata *rdata,
			u64 docid,
			u32 timeid,
			u8 measure_idx,
			u64* vals);

	virtual bool deleteDistValue(
			AosRundata *rdata,
			u64 local_sdocid,u32 timeid,
			u8 measure_idx,u64 val);


	virtual void setMeasureCount(u8 measureCount);


	//yang
	virtual bool    mergeSnapshot(
						const u32 virtual_id,
						const u64 &target_snap_id,
						const u64 &merge_snap_id,
						const AosRundataPtr &rdata);
	virtual u64		createSnapshot(
						const u32 virtual_id,
						const u64 &task_docid,
						const u64 &u64,
						const AosRundataPtr &rata);
	virtual bool	commitSnapshot(
						const u32 &virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);
	virtual bool	rollBackSnapshot(
						const u32 virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);



private:
	bool readBlock( AosRundata *rdata, const u64 sdocid);
	bool saveCurrentCache(AosRundata *rdata);

	bool updateRecordPriv(AosRundata *rdata,
						const u64 sdocid,
						const i64 timeid,
						char *data,
						const int data_len,
						AosMeasureValueMapper &value_mapper);

	bool isInCache(const u64 sdocid);
	//char * getFieldData(AosRundata *rdata,
	//					char *rcd,
	//					const int field_id,
	//					int &field_len);

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

	AosDfmDocPtr readDfmDoc(
			AosRundata *rdata,
			u64 docid,
			AosStatZonePtr &zone);

};


#endif /* StatDocDist_H_ */
