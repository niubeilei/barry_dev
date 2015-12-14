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

class AosDataProcJoin : public	AosJimoDataProc
{
private:
	vector<AosDataRecordObjPtr>			mInputRecords;
	AosDataRecordObjPtr 				mOutputRecord;
	AosDataRecordObj					*mRawOutputRecord;

	vector<AosExprObjPtr>				mLeftCondExprs;
	vector<AosExprObj*>					mRawLeftCondExprs;

	vector<AosExprObjPtr>				mRightCondExprs;
	vector<AosExprObj*>					mRawRightCondExprs;

	AosRecordsetObjPtr					mRightSetHoldor;
	AosRecordsetObj						*mRawRightSetHoldor;

	AosExprObjPtr						mCondition;
	AosExprObj							*mRawCondition;

	OmnString							mPreLeftKey;
	OmnString							mPreRightKey;
	vector<OmnString>					mAlias;
	bool								mIsLeftMove;
	AosBuffPtr							mBuff;
	OmnString							mOutputName;
	bool								mIsRightEnd;
	int 								mLeftCounter;
	int 								mRightCounter;
	int 								mTotal;
	OmnString							mLeftInputAlias;
	OmnString							mRightInputAlias;
	
	AosValueRslt						mValue;

	vector<AosValueRslt>				mLeftValues;
	vector<AosValueRslt>				mRightValues;
	u32									mJoinFields;
	int									mCmpRslt;

u64 mGetLeftValue;
u64	mCmp;
u64	mGetRightValue;
u64 mHoldTime;
u64 mAppendRecord;

public:
	AosDataProcJoin(const int ver);
	AosDataProcJoin(const AosDataProcJoin &proc);
	~AosDataProcJoin();

	virtual AosJimoPtr cloneJimo() const;  
	virtual AosDataProcObjPtr cloneProc();

	virtual AosDataProcStatus::E procData(
						AosRundata *rdata,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);

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

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool 			createOutput(
						const AosRundataPtr &rdata,
						const OmnString &dpname,
						const AosDataRecordType::E type);

	virtual void setInputDataRecords(vector<AosDataRecordObjPtr> &records);
	bool 			appendRecord(
						AosDataRecordObj **input_records,
						AosDataRecordObj *l_record,
						AosDataRecordObj *r_record,
						AosRundata *rdata);

	int compare(vector<AosValueRslt> &lhs, vector<AosValueRslt> &rhs);

	virtual bool finish(const AosRundataPtr &rdata){return true;}                                 
	virtual bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata)
	{
		return true;
	}
};

#endif

