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
// 12/24/2014 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcDocBatchOpr_h
#define Aos_DataProc_DataProcDocBatchOpr_h

#include "AosConf/DataRecord.h"

#include "SEInterfaces/RecordFieldInfo.h"
#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "StreamEngine/StreamDataProc.h"
#include "StreamEngine/Service.h"
#include "SEUtil/SeTypes.h"
#include "StorageEngine/GroupDocOpr.h"

class AosDataProcDocBatchOpr : public AosStreamDataProc
{
	OmnDefineRCObject;
	enum
	{
		eDocidLength = sizeof(u64) + sizeof(int), //docid + length	
		eMaxBuffSize = 2000000 					  //2M
	};

private:
	AosExprObjPtr				mInputDocid;
	AosExprObj					*mRawInputDocid;

	AosDataRecordObjPtr			mOutputRecord;
	AosDataRecordObj			*mRawOutputRecord;

	vector<AosExprObjPtr>		mFields;
	vector<AosExprObj*>			mRawFields;
	u32							mFieldSize;

	AosGroupDocOpr::E			mOpr;
	map<int, u64>				mSnapMaps;
	OmnString					mSchemaName;
	u64							mSchemaDocid;

	OmnSemPtr       			mSem;
	i64							mTotalReqs;
	JSONValue					mJson;
	AosValueRslt				mValue;
	AosBuffPtr					mBuff;
	u64							mDocid;
	u64							mGroupId;
	u64							mGroupDocMaxSize;
	u64							mNumDocidsUsed;

	//barry 2015/11/24
	map<int, AosBuffPtr>		mBuffMap;
	
public:
	AosDataProcDocBatchOpr(const int ver);
	AosDataProcDocBatchOpr(const AosDataProcDocBatchOpr &proc);
	~AosDataProcDocBatchOpr();

	virtual AosDataProcStatus::E procData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_record,
			AosDataRecordObj **output_record);

	virtual AosJimoPtr 		cloneJimo() const;
	virtual AosDataProcObjPtr cloneProc() ;

	virtual bool	start(const AosRundataPtr &rdata);
	virtual bool	finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);
	virtual bool	finish(const AosRundataPtr &rdata);
	virtual bool	getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);
	bool 			createByJql(
						AosRundata *rdata,
						const OmnString &obj_name,
						const OmnString &jsonstr,
						const AosJimoProgObjPtr &prog);

	virtual void callback(const bool svr_death);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	bool			batchInsertDoc(
						AosRundata* rdata_raw,
						AosDataRecordObj* input_record);

	bool			batchUpdateDoc(
						AosRundata* rdata_raw,
						AosDataRecordObj* input_record);

	bool			batchDeleteDoc(
						AosRundata* rdata_raw,
						AosDataRecordObj* input_record);

	bool			flush(
						const int vir_id,
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);

	bool 			createSnapshots(const AosRundataPtr &rdata);

	bool			createSnapshotsStreaming(const AosRundataPtr &rdata);

	AosDataRecordObjPtr createOutputRecord(AosRundata* rdata_raw,
						AosDataRecordObj* input_record);
};

#endif
