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
#ifndef Aos_DataRecord_RecordIIL_h
#define Aos_DataRecord_RecordIIL_h

#include "DataRecord/DataRecord.h"


class AosRecordIIL : public AosDataRecord
{
	int					mRecordLen;
	bool				mIsDataParsed;
	AosDataFieldObjPtr	mValueField;
	AosDataFieldObjPtr	mDocidField;

public:
	AosRecordIIL(const bool flag AosMemoryCheckDecl);
	AosRecordIIL(
			const AosRecordIIL &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordIIL();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return false;}

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

	virtual bool		determineRecordLen(
							char* data,
							const int64_t &len,
							int &record_len,
							int &status);

private:
	bool				config(
							const AosXmlTagPtr &def,
							AosRundata *rdata);

	bool				parseIILKey(AosRundata *rdata,
							char *memory, int idx, int len);

};

#endif

