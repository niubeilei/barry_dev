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
#ifndef Aos_Util_TableFixed_h
#define Aos_Util_TableFixed_h

#include "SEInterfaces/DataRecordObj.h"
#include "Util/Ptrs.h"
#include "Util/DataTable.h"
#include "Util/DataRecord.h"



class AosTableFixed : public AosDataTable
{
	OmnDefineRCObject;

private:
	int					mRecordLength;
	AosBuffPtr			mBuff;
	char *				mData;
	int64_t				mDataLen;
	AosDataRecordObjPtr	mRecord;
	int64_t				mMemCap;
	int64_t				mCursor;

public:
	AosTableFixed(const bool flag);
	// AosTableFixed(const int fieldlen1, 
	// 			  const int fieldlen2, 
	// 			  const int start_idx, 
	// 			  const int psize, 
	// 			  AosRundata *rdata);
	AosTableFixed(const AosXmlTagPtr &def, AosRundata *rdata);
	~AosTableFixed();

	// AosTableFixed interface
	virtual int			getNumRecords() const;
	virtual bool		clear();
	virtual bool		copyData(const char *data, const int len, AosRundata *);
	virtual char *		getRecord(const int data_len, int &record_len) const; 
	virtual AosDataRecordObjPtr getRecord() const;
	virtual AosDataTablePtr	clone(const AosXmlTagPtr &, AosRundata *rdata) const; 
	virtual int64_t		resize(const int64_t &num_records, AosRundata *rdata);
	virtual int64_t		setSize(const int64_t &data_size, AosRundata *rdata);
	virtual int			getFieldLen(const int idx, AosRundata *rdata) const;
	virtual bool		sort();
	virtual bool 		appendRecord(const AosDataRecordObjPtr &, AosRundata *);

private:
	bool config(const AosXmlTagPtr &def, AosRundata *rdata);
	bool expandMemoryPriv();
	bool expandMemoryPriv(const u64 &size);
};

#endif

