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
// 2015/11/04 Created by Andy 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcTransTorturer_h
#define Aos_DataProc_DataProcTransTorturer_h

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "JSON/JSON.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"


class AosDataProcTransTorturer : public AosJimoDataProc
{
private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	double 			mHours;
	u64				mCallbackCount;

public:
	AosDataProcTransTorturer(const int ver);
	AosDataProcTransTorturer(const AosDataProcTransTorturer &proc);
	~AosDataProcTransTorturer();

	virtual AosDataProcStatus::E procData(
					AosRundata *rdata_raw,
					AosDataRecordObj **input_record,
					AosDataRecordObj **output_record);

	virtual bool createByJql(
					AosRundata *rdata,
					const OmnString &obj_name, 
					const OmnString &jsonstr,
					const AosJimoProgObjPtr &prog);

	virtual AosJimoPtr cloneJimo() const;
	virtual AosDataProcObjPtr clone() ;
	virtual vector<AosDataRecordObjPtr> getOutputRecords();

	virtual	bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);
	virtual bool finish(const AosRundataPtr &rdata);
	virtual i64  getProcDataCount() { return mProcDataCount; }
	virtual i64	 getProcOutputCount() { return mOutputCount; }
	virtual void callback(const AosBuffPtr &resp_buff, const bool svr_death);

	// JIMODB-55
	void setInputDataRecords(vector<AosDataRecordObjPtr> &records);
private:
	bool	config(const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	bool 	createOutputDataRecord(
							OmnString &dp_name,
							JSONValue &json,
							const AosRundataPtr &rdata);

	bool 	createOutput(
				const OmnString &dp_name,
				const JSONValue &json_conf,
				const AosRundataPtr &rdata);
	};

#endif
