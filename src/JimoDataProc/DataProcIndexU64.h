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
#ifndef Aos_DataProc_DataProcIndexU64_h
#define Aos_DataProc_DataProcIndexU64_h

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/OutputDataset.h"
#include "AosConf/DataAssembler.h"

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "SEUtil/SeTypes.h"

#include "JSON/JSON.h"


class AosDataProcIndexU64 : public AosJimoDataProc
{
private:
	AosExprObjPtr		 		mInputKey;
	AosExprObjPtr				mInputDocid;
	AosDataRecordObjPtr 		mOutputRecord;
	AosExprObjPtr       		mCondition;


public:
	AosDataProcIndexU64(const int ver);
	AosDataProcIndexU64(const AosDataProcIndexU64 &proc);
	~AosDataProcIndexU64();

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

private:
	bool	config(const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	bool 	createOutput(
				const OmnString &dp_name,
				const JSONValue &json_conf,
				const AosRundataPtr &rdata);
	};

#endif
