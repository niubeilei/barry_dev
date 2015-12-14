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
// 2015/05/05 Created By Andy Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordMulti_h
#define Aos_DataRecord_RecordMulti_h

#include "DataRecord/DataRecord.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ExprObj.h"
#include <map>


class AosRecordMulti : public AosDataRecord
{
private:

private:
	AosRundata*					mRundata;
	int							mRecordLen;
	int							mEstimateRecordLen;

	vector<vector<int> >		mVirFields;
	map<OmnString, int>			mRecordsMap;
	map<OmnString, int>			mSchemaIdMap;
	vector<AosDataRecordObjPtr>	mRecords;
	vector<AosDataRecordObj*>	mRecordsRaw;

	AosExprObjPtr				mPicekExpr;
	AosExprObj*					mPicekExprRaw;
	int							mRecordIdx;

	OmnString					mRowDelimiter;

	bool						mWithFieldCache;

public:
	AosRecordMulti(const bool flag AosMemoryCheckDecl);
	AosRecordMulti(const AosRecordMulti &rhs, AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordMulti();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return true;}

	virtual int			getRecordLen();
	virtual int			getEstimateRecordLen() {return mEstimateRecordLen;} 
	virtual char *		getData(AosRundata *rdata);

	virtual bool 		setData(
							char *data, 
							const int len, 
							AosMetaData *metaData, 
							int &status);
							//const int64_t offset);
	virtual void 		clear();

	virtual AosDataRecordObjPtr clone(AosRundata *rdata AosMemoryCheckDecl) const;
	virtual AosDataRecordObjPtr create(
							const AosXmlTagPtr &def,
							const u64 task_docid,
							AosRundata *rdata AosMemoryCheckDecl) const;

	virtual bool		getFieldValue(
							const int idx,
							AosValueRslt &value,
							const bool copy_flag,
							AosRundata* rdata);

	virtual bool		setFieldValue(
							const int idx, 
							AosValueRslt &value, 
							bool &outofmem,
							AosRundata* rdata);

	virtual void		flush(const bool clean_memory = false);

	virtual AosXmlTagPtr serializeToXmlDoc(
			               	const char *data,
							const int data_len,
							AosRundata* rdata);
	
	virtual bool		determineRecordLen(
							char* data,
							const int64_t &len,
							int &record_len,
							int &status);

	virtual bool		removeFields();

	virtual bool		appendField(
							AosRundata *rdata, 
							const OmnString &name,
							const AosDataType::E type,
							const AosStrValueInfo &info);

	virtual bool		appendField(
							AosRundata *rdata,
							const AosDataFieldObjPtr &field);

private:
	bool	config(const AosXmlTagPtr &def, AosRundata *rdata);
	bool 	configDataFields(const AosXmlTagPtr &def, AosRundata *rdata);
	bool 	configDataRecords(const AosXmlTagPtr &def, AosRundata *rdata);

	OmnString getRecordNameByPicker(AosRundata *rdata);

};

#endif

