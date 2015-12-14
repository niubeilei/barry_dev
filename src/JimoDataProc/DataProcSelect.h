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
#ifndef Aos_DataProc_DataProcSelect_h
#define Aos_DataProc_DataProcSelect_h

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataAssembler.h"

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"
#include "StreamEngine/StreamDataProc.h"
#include "SEUtil/SeTypes.h"

#include "JSON/JSON.h"
#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h" 

class AosDataProcSelect : public AosStreamDataProc
{
private:
	enum Status
	{
		eGroupBy,
		eDistinct,
		eNorm,
		eOrderBy,
		eNULL
	};

	struct SelectField
	{
		OmnString 			mName;
		AosExprObjPtr		mValue;
		AosExprObj			*mRawValue;
		//OmnString 		mType;
		AosDataFieldType::E	mType;
		int 				mLen;
		bool				mIsAgrFunc;
		OmnString 			mFuncName;

		SelectField(
				const OmnString &name, 
				const AosExprObjPtr &value,
				const AosDataFieldType::E &type, 
				const int &len,
				bool isAgrFunc = false,
				const OmnString &funcName = "")
		:
		mName(name),
		mValue(value->cloneExpr()),
		mRawValue(mValue.getPtr()),
		mType(type),
		mLen(len),
		mIsAgrFunc(isAgrFunc),
		mFuncName(funcName)
		{
		}

		SelectField(const SelectField &rhs)
		{
			mName = rhs.mName;
			mValue = rhs.mValue->cloneExpr();
			mRawValue = mValue.getPtr();
			mType = rhs.mType;
			mLen = rhs.mLen;
			mIsAgrFunc = rhs.mIsAgrFunc;
			mFuncName = rhs.mFuncName;
		}

		~SelectField(){}

		SelectField* clone() const
		{
			return OmnNew SelectField(*this);
		}
	};

	vector<SelectField>					mFields;
	u64									mFieldsSize;

	vector<AosExprObjPtr>				mGroupByKeys;
	vector<AosExprObj*>					mRawGroupByKeys;
	u64									mGroupByKeysSize;

	vector<AosExprObjPtr>				mKeyFields;
	vector<AosExprObj*>					mRawKeyFields;
	u64									mKeyFieldsSize;
	
	AosExprObjPtr				mInputDocid;
	AosExprObj*					mRawInputDocid;

	AosExprObjPtr						mCondition;
	AosExprObj							*mRawCondition;
	bool								mAssignDocid;

	vector<AosDataRecordObjPtr>			mInputRecords;

	AosDataRecordObjPtr					mOutputRecord;
	AosDataRecordObj					*mRawOutputRecord;

	AosDataRecordObjPtr					mOutputRecordDocid;

	map<OmnString, AosDataFieldObjPtr> 	mInputRecordsMap;
	map<OmnString, int>					mSelectNameMap;
	map<OmnString, OmnString>			mOrderByTypeMap;

	AosDataProcSelect::Status			mStatus;

	//for approach 1
	bool								mConfigDone;
	JSONValue							mJson;
	OmnString							mRecordType;
	vector<OmnString>					keyTypeList;
	vector<OmnString>					keyNameList;
	vector<int>							keyLenList;
	int									mNumDocids;
	u64									mDocids;

public:
	AosDataProcSelect(const int ver);
	AosDataProcSelect(const AosDataProcSelect &proc);
	~AosDataProcSelect();

	virtual AosDataProcStatus::E procData(
				AosRundata *rdata_raw,
				AosDataRecordObj **input_record,
				AosDataRecordObj **output_record);

	AosJimoPtr 		cloneJimo() const;
	virtual AosDataProcObjPtr 		cloneProc();

	bool 			createByJql(
						AosRundata *rdata,
						const OmnString &obj_name,
						const OmnString &jsonstr,
						const AosJimoProgObjPtr &prog);

	virtual bool finish(const AosRundataPtr &rdata);
	virtual bool start(const AosRundataPtr &rdata);
	virtual	bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool			createOutput(
						const OmnString &dpname,
						const JSONValue &json,
						const AosRundataPtr &rdata);

	bool 			createOutputShuffle(
						const JSONValue &json,
						const boost::shared_ptr<Output> &output,
						const AosRundataPtr &rdata);

	bool 			createOutputKey(
						const boost::shared_ptr<Output> &output,
						const AosRundataPtr &rdata);

	virtual void 	setInputDataRecords(vector<AosDataRecordObjPtr> &records);

	bool 			createInputRecordsMap();

	bool 			configSelectFields(
						const JSONValue &json, 
						const AosRundataPtr &rdata);

	bool 			configFields(
						const JSONValue &fields,
						const AosRundataPtr &rdata);

	bool 			configKeyFields(
						const JSONValue &keys,
						const JSONValue &orderby_types,
						const AosRundataPtr &rdata);

	bool 			configDistinctFields(
						const JSONValue &distincts,
						const AosRundataPtr &rdata);

	bool 			buildOrderByTypeMap(
						const JSONValue &orders,
						const JSONValue &orderTypes,
						const AosRundataPtr &rdata);

	bool			resumConfig(AosRundata *rdata);
	
	u64				getNextDocid(AosRundata* rdata);
};

#endif
