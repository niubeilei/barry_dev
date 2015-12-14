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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcGroupBy_h
#define Aos_DataProc_DataProcGroupBy_h

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "SEUtil/SeTypes.h"

#include "AosConf/DataRecord.h"
#include "AosConf/OutputDataset.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataAssembler.h"
#include "AosConf/CompFun.h"
#include "StreamEngine/StreamDataProc.h"
#include "IILEntryMap/IILEntryMapMgr.h"
#include "JSON/JSON.h"
#include "StatUtil/StatTimeUnit.h"

#define DEFAULT_BUFF_SIZE 	(1000*1000)

class AosDataProcGroupBy : public AosStreamDataProc
{
public:
	enum E
	{
		eInvalid,
		eCountAll,
		eCountNormal,
		eOther
	};
	
	struct MeasrueInfo
	{
		string 					name;
		string					opr;
		AosDataFieldType::E		type;
		int						size;
	};
private:
	u32 						mStatKeysSize;
	u32							mInputMeasuresSize;
	vector<AosExprObjPtr>		mStatKeys;	
	vector<AosExprObjPtr>		mInputMeasures;	
	AosExprObjPtr				mStatInputTime;	
	AosDataRecordObjPtr			mOutputRecord;
	AosDataRecordObj*			mOutputRecordRaw;
	vector<AosExprObjPtr>		mMeasures;

	vector<AosDataFieldType::E> mInputRcdFieldTypes;


	u32							mConfigCalled;

	//yang
	AosExprObjPtr       		mConditionPtr;
	AosExprObj*       			mCondition;
	OmnString 					mTimeField;
	OmnString					mTimeFormat;
	AosStatTimeUnit::E				mTimeUnit;

	int64_t 					mCountValue;

	//for test
	bool 						mIsGenFieldIdx;
	bool 						mHasKey;
	vector<AosDataRecordObjPtr>	mInputRecords;
	vector<OmnString>			mInfoFields;
	vector<AosExprObjPtr>		mInfoFieldExprList;
	u32							mInfoFieldsSize;	
	vector<u32>					mKeyIdxV;
	vector<u32>					mMeasureIdxV;
	vector<E>					mMeasureFuncTypeV; 
	//for streaming
	JSONValue					mJson;
	
	//arvin 2015.11.04
	//for distcount
	u32							mDistCountIdx;
	bool						mIsDistCount;
	AosIILEntryMapPtr			mDistCountIILEntryMap;
	AosIILEntryMapItr 			itr;
	vector<OmnString> 			mDistCountMeasureKeys;
	vector<OmnString> 			mDistCountMeasureValues;
	AosBuffPtr					mTmpBuff;
	//define counters
	u64		mTimeTotal;
	u64		mTimeCondition;
	u64		mTimeConditionNum;
	u64		mRecordClear;
	u64		mTimeStatKey;
	u64		mTimeStatKeyNum;
	u64		mTimeInfoKey;
	u64		mTimeInfoKeyNum;
	u64		mTimeTrans;
	u64		mTimeFieldTime;
	u64		mTimeFieldTimeNum;
	u64		mTimeGenIdx;
	u64		mTimeGenIdxNum;
	u64		mTimeMeasure;
	u64		mTimeMeasureNum;
	u64     mTimeFlush;
	u64     mTimeFlushNum;
	u64     mTimeConfig;
	u64     mTimeOutput;

public:
	AosDataProcGroupBy(const int ver);
	AosDataProcGroupBy(const AosDataProcGroupBy &proc);
	~AosDataProcGroupBy();

	virtual AosDataProcStatus::E procData(
					AosRundata *rdata_raw,
					AosDataRecordObj **input_record,
					AosDataRecordObj **output_record);

	virtual bool createByJql(
					AosRundata *rdata,
					const OmnString &obj_name, 
					const OmnString &jsonstr, 
					const AosJimoProgObjPtr &prog);

	AosJimoPtr cloneJimo() const;
	virtual AosDataProcObjPtr cloneProc() ;

	virtual void 	setInputDataRecords(vector<AosDataRecordObjPtr> &records);

	OmnString convertTimeFormat(const OmnString &tfmt);
private:

 	//for test,test dataprocgroupby performance
	bool	initCounters();

	bool	outputCounters();

	void    clearDistCountValues();
	

	virtual bool 	start(const AosRundataPtr &rdata);
	virtual bool 	finish(const AosRundataPtr &rdata);

	virtual bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);
	
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	bool	createOutputDataRecord(
				OmnString &dp_name,
				JSONValue &json,
				const AosRundataPtr &rdata);

	bool	createOutput(
				OmnString &dp_name,
				JSONValue &json,
				const AosRundataPtr &rdata);
	
	vector<AosDataRecordObjPtr> getOutputRecords();
/*
	void transformTime(
			AosRundata* rdata,
			AosValueRslt& time,
			const OmnString& tfmt,
			const OmnString& tunit);
*/
	//arvin 2015.10.14
	bool genFieldIdx(
			AosRundata* rdata,
			AosDataRecordObj *record);
	
	bool procDistCount(
			AosRundata *data,
			const u32 measure_idx,
			AosDataRecordObj *input_record);

	bool flushRecordset(AosRundata* rdata);
};

#endif
