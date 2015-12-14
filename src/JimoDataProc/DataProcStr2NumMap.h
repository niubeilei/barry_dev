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
#ifndef Aos_DataProc_DataProcStr2NumMap_h
#define Aos_DataProc_DataProcStr2NumMap_h

#include "AosConf/OutputDataset.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataAssembler.h"

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "StreamEngine/StreamDataProc.h"
#include "SEUtil/SeTypes.h"

#include "JSON/JSON.h"

class AosDataProcStr2NumMap : public AosStreamDataProc
{
private:
	vector<AosExprObjPtr> 		mInputKeys;
	AosExprObjPtr				mInputValue;
	AosExprObjPtr       		mCondition;
	AosDataRecordObjPtr			mOutputRecord;

	JSONValue					mJson;
	bool						mIsStream;

public:
	AosDataProcStr2NumMap(const int ver);
	AosDataProcStr2NumMap(const AosDataProcStr2NumMap &proc);
	~AosDataProcStr2NumMap();


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
	virtual AosDataProcObjPtr 		cloneProc();

	virtual	bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);
private:
	bool	config(const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);

	bool	createOutput(OmnString &dp_name,
					JSONValue &json,
					const AosRundataPtr &rdata);
};

#endif
