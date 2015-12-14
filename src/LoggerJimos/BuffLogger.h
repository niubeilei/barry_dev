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
// 2013/12/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LogJimos_BuffLogger_h
#define Aos_LogJimos_BuffLogger_h

#include "DataRecord/DataRecord.h"


class AosBuffLogger : public AosDataRecord
{
protected:
	bool		mIsDataParsed;
	OmnString	mInternalData;
	char 	  *	mData;
	int			mDataLen;			// Current value actual length
	int		 	mRecordLen;			// The record length
	int			mBuffIdx;
	u64			mSchemaId;
	vector<AosValueRslt>	mValues;

public:
	AosBuffLogger(const OmnString &name, const OmnString &version);
	~AosBuffLogger();

	// AosDataRecord interface
	virtual bool		isFixed() const {return false;}
	virtual int			getRecordLen() const {return mRecordLen;}
	virtual int			getDataLen() const {return mDataLen;}
	virtual char *		getData() {return mData;}
	virtual void 		clear();

	virtual AosDataRecordObjPtr clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const;
	virtual AosDataRecordObjPtr create(
	 							const AosXmlTagPtr &def,
	 							const AosTaskObjPtr &task,
	 							const AosRundataPtr &rdata AosMemoryCheckDecl) const;

	virtual bool 		setData(
							char *data, 
							const int len, 
							const AosBuffDataPtr &metaData, 
							const int64_t offset,
							const bool need_copy);

	virtual bool		getFieldValue(
							const int idx,
							AosValueRslt &value,
							const AosRundataPtr &rdata);

	virtual bool	parseData(const AosRundataPtr &rdata);
};

#endif

