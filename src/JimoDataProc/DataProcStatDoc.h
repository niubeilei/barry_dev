////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 04/01/2014 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcStatDoc_h
#define Aos_DataProc_DataProcStatDoc_h

#include "JimoDataProc/JimoDataProc.h"

//#include "DataStructs/Ptrs.h"
//#include "DataStructs/Vector2D.h"
#include "StreamEngine/StreamDataProc.h"
#include "SEUtil/StrValueInfo.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/StatModifyInfo.h"
#include "SEInterfaces/Ptrs.h"
#include "JSON/JSON.h"

#define ISNEW_POS  1
#define DOCID_POS  3
#define STATKEYLEN_POS   12
#define STATKEY_POS		 16

class AosDataProcStatDoc : public AosStreamDataProc
{
public:
	typedef map<OmnString,map<u32,vector<OmnString> > > ktvlist_t;

	enum
	{
		eMaxCachedRecords = 50000
	};

private:

	vector<AosBuffPtr> mBuffs;	// for distinct count

	AosStatisticPtr					mStatistic;
	map<u32, AosXmlTagPtr>			mStatCubeConfMap;
	AosStatModifyInfo				mStatMdfInfo;

	//when record num reach mCachedRecords, will send trans
	int 							mNumRecords;
	int 							mCachedRecords;
	AosBuffPtr 						mRcdBuff;

	AosBuffPtr						mRcdBuff2;	//record buff2

	bool 							mMdfInit;
	bool 							mIsDistCount;
	int 							mCubeId;
	int 							mTimeValuePos;
	
	OmnString						mTimeUnit;

	//yang, for dist_count statistics
	ktvlist_t 						mktvlist;

	int 							mMeasureSize;

	//yang
	OmnMutexPtr					mLock;	// for sending trans serialization

	//yang
	AosXmlTagPtr mRecordBuff2Conf;

	OmnString mOpr;

	//for streaming
	JSONValue 						mJson;
	bool							mIsStream;
	
	//for test ,counter
	u64								mTotalTime;
	u64 							mTotalCounter;
	u64								mCollectInputDataTime;
	u64								mConfigStatMdfTime;
	u64								mConfigStatMdfCounter;
	u64								mSendDocTime;
	u64								mSendDocCounter;
	u64								mMergeBuffTime;
	u64								mRebuildBuffTime;
	u64								mFirstMergeTime;
	u64								mSecondMergeTime;


public:
	//constructors/destructors
	AosDataProcStatDoc(const int version);
	AosDataProcStatDoc(const AosDataProcStatDoc &proc);
	~AosDataProcStatDoc();

	AosJimoPtr cloneJimo() const;
	virtual AosDataProcObjPtr 		cloneProc();

	//proc methods
	virtual AosDataProcStatus::E procData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_records,
			AosDataRecordObj **output_records);

	virtual bool	finish(
			const vector<AosDataProcObjPtr> &proc,
			const AosRundataPtr &rdata);
	
	virtual bool	finish(
			const AosRundataPtr &rdata);

	//JQL methods
	virtual bool createByJql(
					AosRundata *rdata,
					const OmnString &obj_name,
					const OmnString &jsonstr,
					const AosJimoProgObjPtr &prog);

	//arvin 
	bool rebuildBuff(
			const vector<AosBuffPtr>& input_record,
			vector<AosBuffPtr>& newArray,
			const u32 mea_num,
			const AosDataRecordType::E &recordType);
	
	AosBuffPtr mergeRecord(
			const vector<AosBuffPtr>& buffArray,
			const u32 mea_num,
			const AosDataRecordType::E &recordType);

	virtual int getMaxThreads() const
	{
		return 1;;
	}



private:

	void initCounters();

	void outputCounters();
	//configuration methods
	bool 	config(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);

	AosBuffPtr collectInputData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_records);

	bool 	configStatMdfInfo(AosRundata *rdata_raw,
							  AosDataRecordObj *input_record);

	//int 	getCubeIdFromInputData(const AosBuffPtr &input_data);
	int 	getCubeIdFromInputData(char *input_data);

	bool 	sendSaveStatDocTrans(
				AosRundata *rdata_raw,
				const u32 cube_id,
				const AosBuffPtr &input_data);

};

#endif
