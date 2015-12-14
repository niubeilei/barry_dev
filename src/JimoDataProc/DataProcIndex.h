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
#ifndef Aos_DataProc_DataProcIndex_h
#define Aos_DataProc_DataProcIndex_h

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/OutputDataset.h"
#include "AosConf/DataAssembler.h"

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "StreamEngine/StreamDataProc.h"
#include "SEUtil/SeTypes.h"

#include "JSON/JSON.h"


class AosDataProcIndex : public AosStreamDataProc
{
private:
	vector<AosExprObjPtr> 		mInputKeys;
	vector<AosExprObj*> 		mRawInputKeys;

	AosExprObjPtr				mInputDocid;
	AosExprObj*					mRawInputDocid;

	AosDataRecordObjPtr 		mOutputRecord;
	AosDataRecordObj*			mRawOutputRecord;

	AosExprObjPtr       		mCondition;
	AosExprObj*					mRawCondition;
	u32							mNumFields;

	JSONValue					mJson;

	vector<AosDataRecordObjPtr>			mInputRecords;
	vector<AosDataRecordObj*>			mRawInputRecords;
	bool								mAssign;

public:
	AosDataProcIndex(const int ver);
	AosDataProcIndex(const AosDataProcIndex &proc);
	~AosDataProcIndex();

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
	virtual AosDataProcObjPtr cloneProc() ;
	virtual vector<AosDataRecordObjPtr> getOutputRecords();

	virtual	bool start(const AosRundataPtr &rdata);
	virtual	bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);

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
