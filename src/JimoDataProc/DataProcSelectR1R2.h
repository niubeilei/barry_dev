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
#ifndef Aos_DataProc_JimoDataProcSelectR1R2_h
#define Aos_DataProc_JimoDataProcSelectR1R2_h

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/OutputDataset.h"
#include "AosConf/DataAssembler.h"

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "SEUtil/SeTypes.h"

#include "JSON/JSON.h"


class AosJimoDataProcSelectR1R2 : public AosJimoDataProc
{
private:
	vector<AosDataRecordObjPtr>	mInputRecords;
	AosDataRecordObjPtr			mOutputRecord;

	AosDataRecordObjPtr			mDataRecord;
	char *						mData;

public:
	AosJimoDataProcSelectR1R2(const int ver);
	AosJimoDataProcSelectR1R2(const AosJimoDataProcSelectR1R2 &proc);
	~AosJimoDataProcSelectR1R2();

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
	virtual	bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);

private:
	bool	config(const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
	bool 	createOutput(
		const OmnString &dpname,
		const JSONValue &json,
		const AosRundataPtr &rdata);

	int 	getMaxThreads() const {return 1;}
};

#endif
