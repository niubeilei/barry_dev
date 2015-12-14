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
// 10/03/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoDataProc_DataProcMinus_h
#define Aos_JimoDataProc_DataProcMinus_h

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

class AosDataProcMinus : public	AosJimoDataProc
{
private:
	vector<AosDataRecordObjPtr>			mInputRecords;
	AosDataRecordObjPtr 				mOutputRecord;

public:
	AosDataProcMinus(const int ver);
	AosDataProcMinus(const AosDataProcMinus &proc);
	~AosDataProcMinus();

	virtual AosJimoPtr cloneJimo() const;  
	virtual AosDataProcObjPtr cloneProc();

	virtual AosDataProcStatus::E procData(
						AosRundata *rdata,
						const AosRecordsetObjPtr &lhs_recordset,
						const AosRecordsetObjPtr &rhs_recordset,
						AosDataRecordObj **output_records);

	bool createByJql(
						AosRundata *rdata,
						const OmnString &dpname,
						const OmnString &jsonstr,
						const AosJimoProgObjPtr &prog);

	virtual	bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);
private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool 			createOutput(
						const OmnString &dpname,
						const JSONValue &json,
						const AosRundataPtr &rdata);

	virtual void setInputDataRecords(vector<AosDataRecordObjPtr> &records);
	bool 			appendRecord(
						AosDataRecordObj *record,
						AosRundata *rdata);
};

#endif

