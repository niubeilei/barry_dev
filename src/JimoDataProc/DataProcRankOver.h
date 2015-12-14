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
// 03/05/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcRankOver_h
#define Aos_DataProc_DataProcRankOver_h

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataAssembler.h"

#include "DataRecord/Ptrs.h"
#include "DataEngine/ScanDataEngine2.h"
#include "JimoDataProc/JimoDataProc.h"
#include "SEUtil/SeTypes.h"
#include "Util/CompareFun.h"

#include "JSON/JSON.h"
#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h" 

class AosDataProcRankOver : public AosJimoDataProc
{
private:
	struct FieldInfo
	{
		OmnString mType;
		int 	  mOffset;

		FieldInfo(const OmnString &type, const int offset)
		:
		mType(type),
		mOffset(offset)
		{
		}

		FieldInfo() {}
		~FieldInfo() {}

	};
	vector<AosExprObjPtr>				mFields;
	vector<AosDataRecordObjPtr>			mInputRecords;
	AosDataRecordObjPtr					mOutputRecord;

	vector<AosExprObjPtr>				mOrderByFields;
	vector<AosExprObjPtr>				mPartitionByFields;

	map<OmnString, AosDataFieldObjPtr> 	mInputRecordsMap;
	map<OmnString, u64> 				mPartitionRankMap;
	map<OmnString, u64> 				mOrderRankMap;
	u64									mTotalRank;

public:
	AosDataProcRankOver(const int ver);
	AosDataProcRankOver(const AosDataProcRankOver &proc);
	~AosDataProcRankOver();

	virtual AosDataProcStatus::E procData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_record,
			AosDataRecordObj **output_record);

	AosJimoPtr 		cloneJimo() const;
	AosDataProcObjPtr cloneProc();

	bool 			createByJql(
						AosRundata *rdata,
						const OmnString &obj_name,
						const OmnString &jsonstr,
						const AosJimoProgObjPtr &prog);

	virtual	bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool 			configOrderByFields(
						const JSONValue &fields,
						const AosRundataPtr &rdata);

	bool 			configPartitionByFields(
						const JSONValue &fields,
						const AosRundataPtr &rdata);

	bool			createOutput(
						const OmnString &dp_name,
						const JSONValue &json,
						const AosRundataPtr &rdata);

	virtual void	 setInputDataRecords(vector<AosDataRecordObjPtr> &records);

	bool 			createInputRecordsMap(const AosRundataPtr &rdata);
};

#endif
