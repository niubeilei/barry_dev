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
// 04/09/2015 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoDataProc_DataProcJoin_h
#define Aos_JimoDataProc_DataProcJoin_h

#include "JimoDataProc/JimoDataProc.h"

#include "SEInterfaces/RecordsetObj.h"

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataAssembler.h"
#include "JSON/JSON.h"
#include "Util/StrSplit.h"

#include <map>
#include <vector>
#include <boost/make_shared.hpp>   
using namespace std;

class AosDataProcMerge : public	AosJimoDataProc
{
private:
	vector<AosDataRecordObjPtr>			mInputRecords;
	AosDataRecordObjPtr 				mOutputRecord;
	AosExprObjPtr						mCondExpr;

public:
	AosDataProcMerge(const int ver);
	AosDataProcMerge(const AosDataProcMerge &proc);
	~AosDataProcMerge();

	virtual AosJimoPtr cloneJimo() const;  
	virtual AosDataProcObjPtr cloneProc();

	virtual AosDataProcStatus::E procData(
						AosRundata *rdata,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);

	bool createByJql(
						AosRundata *rdata,
						const OmnString &dpname,
						const OmnString &jsonstr,
						const AosJimoProgObjPtr &prog);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool 			createOutput(
						const AosRundataPtr &rdata,
						const OmnString &dpname,
						const AosDataRecordType::E type);

	virtual void 	setInputDataRecords(vector<AosDataRecordObjPtr> &records);
	bool 			appendRecord(
						AosDataRecordObj *l_record,
						AosDataRecordObj *r_record,
						AosRundata *rdata);
};

#endif

