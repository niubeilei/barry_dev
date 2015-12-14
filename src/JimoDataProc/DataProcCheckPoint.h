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
// 09/21/2015 Created by Bryant 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoDataProc_DataProcCheckPoint_h
#define Aos_JimoDataProc_DataProcCheckPoint_h

#define PI				3.1415926
#define EARTH_RADIUS	6378.137

#include "JimoDataProc/JimoDataProc.h"

#include "SEInterfaces/RecordsetObj.h"

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataAssembler.h"
#include "JSON/JSON.h"
#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h" 
#include "Util/StrSplit.h"
#include "Util/File.h"

#include <map>
#include <vector>
#include <boost/make_shared.hpp>   
using namespace std;

class AosDataProcCheckPoint : public	AosJimoDataProc
{
private:
//	struct CheckPointField
//	{
//		OmnString 			mName;
//		AosExprObjPtr		mValue;
//		//OmnString 		mType;
//		AosDataFieldType::E	mType;
//		int 				mLen;
//		bool				mIsAgrFunc;
//		OmnString 			mFuncName;
//
//		CheckPointField(
//				const OmnString &name, 
//				const AosExprObjPtr &value,
//				const AosDataFieldType::E &type, 
//				const int &len,
//				bool isAgrFunc = false,
//				const OmnString &funcName = "")
//		:
//		mName(name),
//		mValue(value),
//		mType(type),
//		mLen(len),
//		mIsAgrFunc(isAgrFunc),
//		mFuncName(funcName)
//		{
//		}
//
//		CheckPointField(const CheckPointField &rhs)
//		{
//			mName = rhs.mName;
//			mValue = rhs.mValue->cloneExpr();
//			mType = rhs.mType;
//			mLen = rhs.mLen;
//			mIsAgrFunc = rhs.mIsAgrFunc;
//			mFuncName = rhs.mFuncName;
//		}
//
//		~CheckPointField(){}
//
//		CheckPointField* clone() const
//		{
//			return OmnNew CheckPointField(*this);
//		}
//	};
//
	vector<AosExprObjPtr>				mFields;
	vector<AosDataRecordObjPtr>			mInputRecords;
	vector<AosDataRecordObjPtr>			mRecords;
	AosDataRecordObjPtr 				mOutputRecord;
	OmnString							mOutputName;
	map<OmnString, AosDataFieldObjPtr> 	mInputRecordsMap;
	AosBuffPtr 							mBuff;
	AosRecordsetObjPtr					mSetHoldor;
	double								mSpeed;
	char *								mData;
	int									mIndex;
public:
	AosDataProcCheckPoint(const int ver);
	AosDataProcCheckPoint(const AosDataProcCheckPoint &proc);
	~AosDataProcCheckPoint();

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

	bool 			configFields(
						const JSONValue &json,
						const AosRundataPtr &rdata);

	virtual void setInputDataRecords(vector<AosDataRecordObjPtr> &records);
	
	bool			createInputRecordsMap();


	virtual bool finish(const AosRundataPtr &rdata){return true;}                                 
	virtual bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata)
	{
		return true;
	}
};

#endif

