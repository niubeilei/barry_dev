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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_DataRecord_h
#define Aos_DataRecord_DataRecord_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"

#include <vector>
#include <map>
using namespace std;


class AosDataRecord : public AosDataRecordObj,
					  public AosMemoryCheckerObj
{

protected:
	enum
	{
		eEstimateEachFieldLen = 5,
		eMaxDocids = 500,
		eBuffInitSize = 500,
		eBuffIncSize = 500
	};

	AosDeltaBeanOpr::E 			mOperator; // Add by Young, for syncher
	AosDataRecordType::E		mType;
	OmnString					mTypeName;
	OmnMutexPtr					mLock;

	OmnString					mName;
	OmnString					mObjid;
	AosXmlTagPtr				mRecordDoc;
	u64							mTaskDocid;

	vector<AosDataFieldObjPtr>	mFields;
	AosDataFieldObj**			mFieldsRaw;
	u64							mNumFields;

	map<OmnString, int>			mFieldIdxs;

	u64							mDocid;
	vector<u64>					mDocids;
	AosValueRslt				*mFieldValues;
	bool*           			mFieldValFlags;

	// Chen Ding, 2014/07/24
	// AosBuffDataPtr			mMetaData;
	AosMetaDataPtr				mMetaData;
	AosMetaData					*mMetaDataRaw;
	int64_t						mOffset;		// Chen Ding, 2013/11/29
	
	bool						mIsDirty;

	char *						mMemory;
	int64_t						mMemLen;
	vector<AosDataProcObj*>  	mDataProcs;
	bool 						mIsValid;
	u64							mSchemaDocid;


	//yang
	AosDataFieldObj*		mBitmapField;

public:
	AosDataRecord(
			const OmnString &name,
			const int version AosMemoryCheckDecl);
	AosDataRecord(
			const AosDataRecordType::E type,
			const OmnString &name,
			const bool flag AosMemoryCheckDecl);
	AosDataRecord(
			const AosDataRecord &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosDataRecord();

	static bool					staticInit();

	// AosDataRecord interface
	virtual void setOperator(AosDeltaBeanOpr::E type) {mOperator = type;}
	virtual AosDeltaBeanOpr::E  getOperator() {return mOperator;}
	virtual AosDataRecordType::E getType() const {return mType;}
	virtual AosXmlTagPtr		getRecordDoc() const {return mRecordDoc;}

	virtual int					getMemoryLen() {return mMemLen;}
	virtual char *				getData(AosRundata *rdata) {
		return mMemory;}
	
	virtual void				setMemory(
									char* data,
									const int64_t &len){
	//	if(!mMemory)
	//	{
			//new memory
	//		mMemory = data; mMemLen = len;
	//		memset(mMemory,0,mMemLen);
	//	}
	//	else
	//	{
			mMemory = data; mMemLen = len;
	//	}
	}
	virtual bool 				determineRecordLen(
									char* data,
									const int64_t &len,
									int &record_len,
									int &status){return false;}

	virtual void				clear();
	virtual OmnString 			getFieldDelimiter() const {return "";}
	virtual OmnString			getTextQualifier() const {return "";}
	virtual AosMetaDataPtr		getMetaData() {return mMetaData;}
	virtual u64					getDocid() const {return mDocid;}
	virtual void 				setDocid(const u64 &docid) {mDocid = docid;}
	virtual u64 				getDocidByIdx(const int idx) const
	{
		aos_assert_r(idx >= 0 || (u32)idx < mDocids.size(), 0);
		return mDocids[idx];
	}
	virtual bool 				setDocidByIdx(const int idx, const u64 &docid)
	{
		aos_assert_r(idx >= 0 && idx < eMaxDocids, false);
		if ((u32)idx >= mDocids.size()) mDocids.resize(idx+1);
		mDocids[idx] = docid;
		return true;
	}

	virtual int					getNumFields() const {return mFields.size();}
	virtual u64					getTaskDocid() const {return mTaskDocid;}	// Ketty 2013/12/23
	virtual void				setTaskDocid(const u64 task_docid) {mTaskDocid = task_docid;}
	virtual OmnString			getRecordName() const {return mName;}
	virtual void 				setRecordName(const OmnString &name) {mName = name;}
	virtual OmnString			getRecordObjid() const {return mObjid;}

	virtual int64_t				getOffset() const {return mOffset;}
	virtual bool				isValid(AosRundata *rdata);
	virtual bool				isDirty(){return mIsDirty;}
	virtual void				flush(const bool clean_memory = false) {mIsDirty = false;}
	virtual int	 				getNumRecord() {return 1;}
	virtual bool 				getRecordLens(vector<int> &rcd_lens){return false;}

	virtual AosDataRecordObjPtr	createDataRecord(
									const AosXmlTagPtr &def, 
									const u64 task_docid,
									AosRundata *rdata AosMemoryCheckDecl);
	
	virtual bool				getFieldValue(
									const int idx, 
									AosValueRslt &value, 
									const bool copy_flag,
									AosRundata* rdata);

	virtual bool				getFieldValue(
									const OmnString &field_name,
									AosValueRslt &value, 
									const bool copy_flag,
									AosRundata* rdata);

	virtual bool				setFieldValue(
									const int idx, 
									AosValueRslt &value, 
									bool &outofmem,
									AosRundata* rdata);

	virtual bool				setFieldValue(
									const OmnString &field_name, 
									AosValueRslt &value, 
									bool &outofmem,
									AosRundata* rdata);
	//yang
	virtual bool 				replaceField(
									const int idx,
									const AosDataFieldObjPtr& newfield);

   
	virtual int                 getFieldIdx( 
			                      	const OmnString &name,
									AosRundata *rdata);

	virtual AosDataFieldObj*    getDataField(
									AosRundata *rdata,
									const OmnString &name);

	virtual AosXmlTagPtr		serializeToXmlDoc(
			                		const char *data,
									const int data_len,
									AosRundata* rdata);

	virtual AosBuffPtr			serializeToBuff(
			             			const AosXmlTagPtr &doc,
									AosRundata *rdata);
	
	
	// Chen Ding, 2014/07/24
	// virtual AosDataFieldObjPtr	getFieldByIdx(const u32 idx);		// Ketty 2014/05/07
	virtual AosDataFieldObj*	getFieldByIdx1(const u32 idx);		// Ketty 2014/05/07

	virtual AosXmlTagPtr		getDataFieldConfig(
									const OmnString &name,
									AosRundata *rdata);

	virtual bool				createRandomDoc(
									const AosBuffPtr &buff,
									AosRundata *rdata);

	virtual bool 				getRecordENV(
									const OmnString &name,
									OmnString &record_env,
									AosRundata *rdata);

	virtual OmnString getFieldValue(
			AosRundata *rdata, 
			const OmnString &field_name);

	char   getTextQualifierChar() const {return 0;}
	virtual void				flushRecord(AosRundata *rdata_raw){}
	virtual void				reset(){}
	virtual void				setDataAssembler(AosDataAssemblerObj *assemble) {}
	virtual void                bindDataProcs(vector<AosDataProcObj*> &procs);

	virtual vector<AosDataProcObj*>& getBindDataProcs();

	virtual vector<AosDataFieldObjPtr> getFields() const {return mFields;}
	virtual bool removeFields();

	virtual bool isValid() {return mIsValid;}
	virtual u64	getSchemaDocid() { return mSchemaDocid; }
    
protected:
	bool	registerRecord(
				const AosDataRecordType::E type, 
				const OmnString &name,
				AosDataRecord *record);
	bool 	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
	
public:
	// Chen Ding, 2015/01/24
	virtual bool appendField(AosRundata *rdata, 
							const OmnString &name, 
							const AosDataType::E type,
							const AosStrValueInfo &info);

	virtual bool appendField(AosRundata *rdata, const AosDataFieldObjPtr &field);

	virtual AosDataRecordObjPtr createBuffRecord(AosRundata *rdata AosMemoryCheckDecl);

	//display data record buff chars one by one
	virtual bool dumpData(bool flag, OmnString title);


	//yang,2015/08/13
	virtual bool determineBitmapOffset(
			AosRundata* rdata,
			int &offset){return true;}

	//yang,2015/08/13
	virtual bool buildRecordBitmap(
				AosRundata *rdata)
	{
		return true;
	}

	//Phil,  2015/09/03
	virtual bool compareRecord(
			AosRundata *rdata, 
			AosDataRecordObj* rcd,
			int &cmpRslt);

	virtual bool setData(
			char *data, 
			const int len, 
			AosMetaData *metaData, 
			int &status);

	


};

#endif

