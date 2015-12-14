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
#ifndef Aos_DataRecord_RecordFixBin_h
#define Aos_DataRecord_RecordFixBin_h

#include "DataRecord/DataRecord.h"


class AosRecordFixBin : public AosDataRecord
{
	enum E
	{
		eInvalid,
		eAll,
		eHead,
		eTail
	};
	int				mRecordLen;
	bool			mWithFieldCache;	
	E 				mTrimCondition;  //gavin,2015/05/11

public:
	AosRecordFixBin(const bool flag AosMemoryCheckDecl);
	AosRecordFixBin(
			const AosRecordFixBin &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordFixBin();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return true;}

	virtual int			getRecordLen() {return mRecordLen;}
	virtual int			getEstimateRecordLen() {return mRecordLen;} 
	virtual int			getDataLen() {return mRecordLen;}

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

	virtual AosXmlTagPtr serializeToXmlDoc(
			               	const char *data,
							const int data_len,
							AosRundata* rdata);

	virtual AosBuffPtr	serializeToBuff(
			               	const AosXmlTagPtr &doc,
							AosRundata *rdata);

	virtual bool		createRandomDoc(
							const AosBuffPtr &buff,
							AosRundata *rdata);

	virtual bool		determineRecordLen(
							char* data,
							const int64_t &len,
							int &record_len,
							int &status)
	{
		if (len < mRecordLen)
		{
			record_len = -1;
			status = 0;
			return true;
		}
		record_len = mRecordLen;
		status = 0;
		return true;
	}

	virtual bool		appendField(
							AosRundata *rdata, 
							const AosDataFieldObjPtr &field);


private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
};

#endif

