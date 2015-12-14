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
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StreamEngine_DataProcIILJoin_h
#define Aos_StreamEngine_DataProcIILJoin_h

#include "JimoDataProc/JimoDataProc.h"

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"

#include "SEInterfaces/RecordsetObj.h"
#include "AosConf/DataRecordFixbin.h"
#include "StreamEngine/StreamDataProc.h"
#include "JSON/JSON.h"
#include "Util/StrSplit.h"

#include <map>
#include <vector>
#include <boost/make_shared.hpp>   
using namespace std;

class AosDataProcIILJoin : public	AosStreamDataProc
{
private:
	//dataproc parameters
	JSONValue						mJson;
	vector<OmnString>				mLeftKeyList;
	vector<OmnString>				mRightKeyList;
	vector<OmnString>				mLeftValList;
	vector<OmnString>				mRightValList;
	vector<OmnString>				mLeftList;
	vector<OmnString>				mRightList;
	OmnString						mIILName;
	int								mMaxKeyLen;
	AosRecordsetObjPtr 				mRecordset;
	AosRecordsetObjPtr 				mRhs_recordset;

	//for lock
	OmnMutexPtr						mLock;

	//output dataset name
	OmnString						mOutput;
	OmnString						mRecordType;

	//output record object
	hash_map<OmnString, AosDataRecordObjPtr, Omn_Str_hash, compare_str> mOutputRecordMap;

	AosDataRecordObjPtr				mLeftRecord;
	AosDataRecordObjPtr				mRightRecord;

public:
	//constructors/destructors
	AosDataProcIILJoin(const int ver);
	AosDataProcIILJoin(const AosDataProcIILJoin &proc);
	~AosDataProcIILJoin();
	AosJimoPtr cloneJimo() const;  

	//proc methods
	virtual AosDataProcStatus::E procData(
						AosRundata *rdata,
						const AosRecordsetObjPtr &lhs_recordset,
						const AosRecordsetObjPtr &rhs_recordset,
						AosDataRecordObj **output_records);

	//virtual bool procData(AosRundata *rdata, RecordsetMap &rsMap);
	virtual bool procData(AosRundata *rdata, const AosRDDPtr &rdd);
	//jql methods
	bool createByJql(
						AosRundata *rdata,
						const OmnString &dpname,
						const OmnString &jsonstr,
						const AosJimoProgObjPtr &prog);

	vector<AosDataRecordObjPtr> getOutputRecords();

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool 			createOutputRecords(AosRundata *rdata);

	//output record methods
	OmnString 		defineRecord(AosRundata *rdata);
	bool 			fillRecord(
						AosRundata *rdata,
						AosDataRecordObj *lhs_record,
						AosDataRecordObj *rhs_record);

	//helper methods
	bool 			getJSONParams(JSONValue json);

	OmnString		getCombinedKeyValues(
								AosDataRecordObj* &rcd,
								vector<OmnString> &KeyList);


	bool			getIILEntries(
							    const AosRundataPtr &rdata,
								const OmnString &keyValue,
								vector<AosDataRecordObj *> &recordList);

	bool 			setOutputField(AosRundata *rdata,
								AosXmlTagPtr xml,
								OmnString &field_name, 
								boost::shared_ptr<Output> out_put);

	bool 			fillRecordValue(AosRundata* rdata,
								vector<AosDataRecordObj *> &record_list,
								vector<OmnString> &field_list);
};

#endif

