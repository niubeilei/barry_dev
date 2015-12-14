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
// 07/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordBuff2_h
#define Aos_DataRecord_RecordBuff2_h

#include "DataRecord/DataRecord.h"


class AosRecordBuff2 : public AosDataRecord
{
	int				mRecordLen;
	bool			mIsDataParsed;
	int				mCrtFieldIdx;
	int				mCrtFieldOffset;

public:
	AosRecordBuff2(const bool flag AosMemoryCheckDecl);
	AosRecordBuff2(
			const AosRecordBuff2 &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordBuff2();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return false;}

	virtual int			getRecordLen() {return mRecordLen;}
	virtual int			getEstimateRecordLen() {return mRecordLen;} 

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

	virtual bool 		determineRecordLen(
							char* data,
							const int64_t &len,
							int &record_len,
							int &status);

	virtual void		flush(const bool clean_memory = false);

	//virtual AosDataFieldObj*	getFieldByIdx1(const u32 idx);		// Ketty 2014/05/07

private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
	//bool	parseData(AosRundata *rdata);
		bool  parseData(
		char *data,
		const int64_t &len,
		int &record_len,
		int &status);

public:
	// Chen Ding, 2015/01/24
	virtual bool appendField(AosRundata *rdata, 
						const OmnString &name, 
						const AosDataType::E type);

};

#endif

