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
#ifndef Aos_JimoDataProc_DataProcStatJoin_h
#define Aos_JimoDataProc_DataProcStatJoin_h

#include "JimoDataProc/JimoDataProc.h"


#include "SEInterfaces/RecordsetObj.h"
#include "StatUtil/StatIdIDGen.h"

#include "AosConf/DataRecordFixbin.h"
#include "StreamEngine/StreamDataProc.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataAssembler.h"
#include "JSON/JSON.h"
#include "Util/StrSplit.h"

#include <map>
#include <vector>
#include <boost/make_shared.hpp>   
using namespace std;

class AosDataProcStatJoin : public	AosStreamDataProc
{
private:

	enum Opr
	{
		INSERT,
		DELETE
	};

	enum
	{
		eDefRcdSize = 50000
	};

	//dataproc parameters
	JSONValue						mJson;

	vector<OmnString>				mKeyList;
	vector<OmnString>				mMeasureList;
	vector<OmnString>				mIILList;
	OmnString						mTimeField;
	int								mMaxKeyLen;
	u32								mCubeId;

	//yang,2015/08/25
	OmnString						mStatKey;

	AosDataRecordObjPtr mStatKeyOutputRecord;
	AosDataRecordObj* mStatKeyOutputRecordRaw;

	AosDataRecordObjPtr mVt2dOutputRecord;
	AosDataRecordObj* mVt2dOutputRecordRaw;

	AosDataRecordObjPtr* mKeyOutputRecordArray;
	AosDataRecordObj  **mDataRecordObjRight;

	//yang
	bool							mRhsRecordFirstEmpty;

	//use statid Gen to get new statid
	AosStatIdIDGenPtr 				mStatIdGen;
	OmnString						mIILName;

	//output dataset name
	AosDataRecordType::E			mRecordType;

	//output record name
	OmnString						mOutputStatKey;
	OmnString						mOutputKeyPrefix;
	OmnString						mOutputVt2d;

	//arvin
	OmnString						mTmpKeyValue;  
	Opr								mOpr;
	bool 							mIsStatKeyNew;
	bool 							mHasKey;
	OmnString						mStatKeyValue;
	vector<AosValueRslt>			mKeyValueList;//keep all the key_value
	
	//memorisize the previous created statkey
	char* 						mPrevCreatedStatKey;
	int							mPrevCreatedStatKeyLen;

	u64 							mTmpSdocid;
	u64								mSdocid;

	//output record object
	hash_map<OmnString, AosDataRecordObjPtr, Omn_Str_hash, compare_str> mOutputRecordMap;

	//save individual key's type for index data
	hash_map<OmnString, OmnString, Omn_Str_hash, compare_str> mKeyIndexOprMap;  //???

	vector<AosDataFieldType::E> mInputRcdFieldTypes;

	vector<AosDataRecordObjPtr>         mInputRecords;  // ??? which one to use

	vector<OmnString> mMeasureVals;
	vector<OmnString> mAggrOprs;
	vector<AosExprObjPtr> mKeyExprList;	
	map<OmnString,AosExprObjPtr> mInfoFields;

	//yang,2015/09/27
	bool mFirstCallFillOneNewKey;

	//////////////////////////////////////////////////////////////
	//For Streaming dataproc. Modified by Levi and Felicia
	//////////////////////////////////////////////////////////////
	// statkey->record count
	hash_map<OmnString, u32, Omn_Str_hash, compare_str> mStatKeyRecordCountMap;
	u64								mCurStatKeyDocid;
	AosDataProcStatus::E            mDataProcStatus;    
	AosDataRecordObj **				mInput_Records;
	AosDataRecordObjPtr				mIILRecord;				
	u32								mLeftIdx;
	u32								mRightIdx;
	u32								mLeftRecordSize;
	u32								mRightRecordSize;
	bool							mLeftIsFinish;
	bool							mRightIsFinish;
	bool							mHasKeyValue;
	map<OmnString, u64> 			mStatKeyValueMap;
	vector<OmnString>				mStatKeyList;
	vector<u64>						mSdocidList;
	AosQueryContextObjPtr 			mQueryContext;
	u64								mKeyIdx;
	vector<vector<AosValueRslt> >	mRecordsKeyList;
	vector<AosValueRslt>			mRcdKeyList;

public:
	//////////////////////////////////////////////////////////////
	//For Streaming dataproc. Modified by Levi and Felicia
	//////////////////////////////////////////////////////////////
	u64								mStatJoinTime;
	struct MeasrueInfo
	{
		string						name;
		string						opr;
		AosDataFieldType::E			type;
		int							size;
	};

public:
	//constructors/destructors
	AosDataProcStatJoin(const int ver);
	AosDataProcStatJoin(const AosDataProcStatJoin &proc);
	~AosDataProcStatJoin();

	virtual AosJimoPtr cloneJimo() const;  
	virtual AosDataProcObjPtr clone();

	//proc methods
	virtual AosDataProcStatus::E procData(
						AosRundata *rdata,
						const AosRecordsetObjPtr &lhs_recordset,
						const AosRecordsetObjPtr &rhs_recordset,
						AosDataRecordObj **output_records);
	

	virtual AosDataProcStatus::E procData(
						AosRundata *rdata,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);

	//yang,2015/08/25
	bool fillOneNewKeyAndVt2ds(
			AosRundata*& rdata,
			const AosRecordsetObjPtr& lhs_recordset,
			AosDataRecordObj*& lhs_record,
			u64& sdocid,
			bool& isnew);

	bool fillOneNewKey(
			AosRundata*& rdata,
			const char* key,
			const int len);


	//yang,2015/10/09
	//int cmpstr(const char* lk,int lklen,
	//		const char* rk,int rklen);


	//jql methods
	bool 		createByJql(
						AosRundata *rdata,
						const OmnString &dpname,
						const OmnString &jsonstr,
						const AosJimoProgObjPtr &prog);

 virtual void    setInputDataRecords(vector<AosDataRecordObjPtr> &records);

 vector<AosDataRecordObjPtr> getOutputRecords();

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool 			createOutput(AosRundata *rdata);

	bool 			fillRecordStatKey(
						AosRundata *rdata,
						const char* key,
						const int len,
						const u64 sdocid);

	bool 			fillRecordKey(
						AosRundata *rdata,
						const char* key,
						const int len,
						const u64 sdocid);

	bool 			fillRecordVt2d(
						AosRundata *rdata,
						AosDataRecordObj *input_record,
						const u64 sdocid,
						const char isnew);

	//helper methods
	bool			getStatKey(
						AosRundata *rdata,
						AosDataRecordObj* &lhs_record);

	bool 			addDataField(
						boost::shared_ptr<AosConf::DataRecordFixbin> &dr,
						const OmnString &name,
						const OmnString &type,
						const OmnString &shortplc,
						const u32 offset,
						const u32 len);

	bool 			getJSONParams(JSONValue json,AosRundata* rdata);
	

	bool			createStatkeyOutput(AosRundata* rdata);
	bool			createPrefixKeyOutput(AosRundata* rdata);
	bool			createVector2dOutput(AosRundata* rdata);
	
	//////////////////////////////////////////////////////////////
	//For Streaming dataproc. Modified by Levi and Felicia
	//////////////////////////////////////////////////////////////
	//virtual bool procData(AosRundata *rdata, RecordsetMap &rsMap);
	virtual bool procData(AosRundata *rdata, const AosRDDPtr &rdd);

	bool		getIILEntries(
					const AosRundataPtr &rdata,
					const OmnString &keyValue,
					vector<AosDataRecordObj *> &recordList);
	
	bool		getDocidAndFillValue(
					const AosRundataPtr &rdata,
					vector<AosDataRecordObjPtr> &recordList,
					vector<u64> &sizeList,
					vector<OmnString> &keyList,
					AosBuffPtr &buffPtr);

	bool 		isDataInIIL(AosRundataPtr rdata, AosRecordsetObjPtr &rs);

	bool		fillValue(
					const AosRundataPtr &rdata,
					const OmnString &keyValue,
					vector<AosDataRecordObj *> &recordList);

	vector<AosValueRslt> getRecordKeyList(
					AosRundata *rdata,
					AosDataRecordObj* &lhs_record);
	
};

#endif

