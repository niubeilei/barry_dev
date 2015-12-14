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
// 08/12/2015 Created by Jackie
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoDataProc_DataProcLeftJoin_h
#define Aos_JimoDataProc_DataProcLeftJoin_h

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

class AosDataProcLeftJoin : public	AosJimoDataProc
{
private:
	vector<AosDataRecordObjPtr>			mInputRecords;
	AosDataRecordObjPtr 				mOutputRecord;
	vector<AosExprObjPtr>				mLeftCondExprs;
	vector<AosExprObjPtr>				mRightCondExprs;
	AosExprObjPtr						mCondition;
	OmnString							mPreLeftKey;
	OmnString							mPreRightKey;
	bool								mPreEQ;
	OmnString							mOutputName;
	int 								mLeftCounter;
	int 								mRightCounter;
	int 								mTotal;
	int 								mFieldSize;
	bool								mIsRightEnd;
	bool								mReverse;
	vector<OmnString>					mAlias;

public:
	AosDataProcLeftJoin(const int ver);
	AosDataProcLeftJoin(const AosDataProcLeftJoin &proc);
	~AosDataProcLeftJoin();

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
	int compare(const OmnString &left, const OmnString &right);

	virtual bool finish(const AosRundataPtr &rdata){return true;}                                 
	virtual bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata)
	{
		OmnScreen << "join output  name:" << mOutputName<<endl;
		OmnScreen << "join append total:" << mTotal <<endl;
		OmnScreen << "join read left records:" << mLeftCounter+1 <<endl;
		OmnScreen << "join read right records:" << mRightCounter+1 <<endl;
		OmnScreen << "join last left key :" << mPreLeftKey <<endl;
		OmnScreen << "join last right key :" << mPreRightKey <<endl;
		return true;
	}
};

#endif

