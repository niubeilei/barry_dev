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
#ifndef Aos_DataRecord_RecordCSV_h
#define Aos_DataRecord_RecordCSV_h

#include "DataRecord/DataRecord.h"
#include "SEInterfaces/Ptrs.h"


class AosRecordCSV : public AosDataRecord
{
	int				mRecordLen;
	int				mEstimateRecordLen;
	OmnString		mRowDelimiter;
	char			mFieldDelimiter;
	char 			mTextQualifier;
	bool			mNoTextQualifier;
	bool			mIsDataParsed;
	bool *			mNeedEscapeQualifier;
	bool			mBackslashEscape;

	int				mCrtRcdLen;				// Ken Lee, 2014/11/25
	int 			mCrtFieldIdx;			// Young, 2015/07/29

	char            mRowDelimiterChar;		// xiafan, 2015/10/15
	int  			mFieldsNum;
public:
	AosRecordCSV(const bool flag AosMemoryCheckDecl);
	AosRecordCSV(
			const AosRecordCSV &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordCSV();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return false;}

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

	virtual OmnString	getFieldDelimiter() const {OmnString str(&mFieldDelimiter, 1); return str;}
	virtual OmnString	getTextQualifier() const {OmnString str(&mTextQualifier, 1); return str;}

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
	
	//virtual bool		determineRecordLen(
	virtual	bool		parseData(
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
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
	//bool	parseData(AosRundata *rdata);
	bool	composeData();
	inline u32 moveToNextField(const u32 field_idx)
	{
		u32 idx = field_idx;
		u32 num_fields = mNumFields;
		while (idx < num_fields)
		{
			if (!mFields[idx]->isVirtualField())
			{
				return idx;
			}
			idx++;
		}
		return idx;
	}

	// jimodb-953
	enum Status
	{
		eFieldBegin,
		eFieldEnd,
		eRecordBegin,
		eRecordEnd,
		eField,		 // abc
		eStrField    // "abc"
	};
};

#endif

