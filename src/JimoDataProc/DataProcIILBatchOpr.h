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
#ifndef Aos_JimoDataProc_DataProcIILBatchOpr_h
#define Aos_JimoDataProc_DataProcIILBatchOpr_h

#include "AosConf/DataRecord.h"

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "StreamEngine/StreamDataProc.h"
#include "StreamEngine/Service.h"
#include "SEUtil/SeTypes.h"
#include "IILUtil/IILUtil.h"
#include "SEInterfaces/IILOpr.h"

class AosDataProcIILBatchOpr : public AosStreamDataProc
{
private:
	enum
	{
		eMaxBuffSize = 20000000,
		eMaxKeySize = 50
	};

	OmnString						mShuffleType;
	int								mShuffleId;
	OmnString						mIILName;
	OmnString						mLenType;
	AosIILOpr::E					mOpr;
	AosIILType						mIILType;
	AosIILUtil::AosIILIncType 		mIncType;
	JSONValue						mJson;
	u64								mInitdocid;
	bool							mBuildBitmap;
	u64 							mSnapId;
	AosBuffPtr						mBuff;
	AosBuff*						mBuffRaw;
	int								mRecordLen;
	AosDataRecordType::E			mRecordType;

	int								mMaxKeyLen;

	AosDataFieldType::E				mKeyType;
	AosDataFieldType::E				mValueType;
	char *							mKeyStr;
	int								mKeyStrLen;

public:
	AosDataProcIILBatchOpr(const int ver);
	AosDataProcIILBatchOpr(const AosDataProcIILBatchOpr &proc);
	~AosDataProcIILBatchOpr();

	virtual AosDataProcStatus::E procData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_record,
			AosDataRecordObj **output_record);

	virtual AosJimoPtr cloneJimo() const;
	virtual AosDataProcObjPtr cloneProc();

	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);
	virtual bool	start(const AosRundataPtr &rdata);
	virtual bool 	finish(const AosRundataPtr &rdata);
	virtual bool	finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);
	bool 			createByJql(
						AosRundata *rdata,
						const OmnString &obj_name,
						const OmnString &jsonObj,
						const AosJimoProgObjPtr &prog);

	virtual int  getMaxThreads() const { return 1; } // Young, 2015/07/15

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	bool			flush(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);
	bool			createSnapshots(const AosRundataPtr &rdata);
	bool			createSnapshotsStreaming(const AosRundataPtr &rdata);
	bool 			checkBuff(AosRundata *rdata, const AosBuffPtr &buff);
//	bool setShuffleId(const OmnString &key);
};

#endif
