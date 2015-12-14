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
// 05/22/2014 Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordStat_h
#define Aos_DataRecord_RecordStat_h

#include "DataRecord/DataRecord.h"


class AosRecordStat : public AosDataRecord
{
	int							mRecordLen;

	int							mSplitFieldIdx;

	map<int, AosValueRslt>		mFieldValues;
	vector<AosDataFieldObjPtr>	mChildFields;
	map<OmnString, int>			mChildFieldIdxs;
	int							mNumFields;

public:
	AosRecordStat(const bool flag AosMemoryCheckDecl);
	AosRecordStat(
			const AosRecordStat &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordStat();

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
							const int64_t offset);
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

	virtual int			getFieldIdx(
							const OmnString &name,
							AosRundata *rdata);
private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);

	bool splitStatKey(const OmnString &keys);
};

#endif

