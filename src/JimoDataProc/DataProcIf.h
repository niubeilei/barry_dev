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
#ifndef Aos_DataProc_JimoDataProcIf_h
#define Aos_DataProc_JimoDataProcIf_h

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/OutputDataset.h"
#include "AosConf/DataAssembler.h"

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "SEUtil/SeTypes.h"

#include "JSON/JSON.h"


class AosJimoDataProcIf : public AosJimoDataProc
{
private:
	AosExprObjPtr       		mCondition;
	vector<AosDataProcObjPtr>	mTrueProcs;
	vector<AosDataProcObjPtr>	mFalseProcs;

	vector<AosDataRecordObjPtr>	mInputRecords;

public:
	AosJimoDataProcIf(const int ver);
	AosJimoDataProcIf(const AosJimoDataProcIf &proc);
	~AosJimoDataProcIf();

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
	virtual void setInputDataRecords(vector<AosDataRecordObjPtr> &records);
	//bool 	finish(const AosRundataPtr &rdata);
	bool 	start(const AosRundataPtr &rdata);
	bool 	finish(const vector<AosDataProcObjPtr> &procs,
				const AosRundataPtr &rdata);
	int 	getMaxThreads() const;

private:
	bool	config(const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	bool 	getTrueProcs(const JSONValue &json,
				const AosRundataPtr &rdata);
	bool 	getFalseProcs(const JSONValue &json,
				const AosRundataPtr &rdata);
};

#endif
