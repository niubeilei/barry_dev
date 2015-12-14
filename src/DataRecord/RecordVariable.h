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
#ifndef Aos_DataRecord_RecordVariable_h
#define Aos_DataRecord_RecordVariable_h

#if 0
#include "DataRecord/DataRecord.h"


class AosRecordVariable : public AosDataRecord
{
	int				mEstimateRecordLen;
	OmnString		mRowDelimiter;
	OmnString		mFieldDelimiter;
	OmnString		mTextQualifier;
	bool			mIsDataParsed;

public:
	AosRecordVariable(const bool flag);
	AosRecordVariable(
			const AosRecordVariable &rhs,
			AosRundata *rdata);
	~AosRecordVariable();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return false;}

	virtual int			getRecordLen() {return mMemLen;}
	virtual int			getEstimateRecordLen() {return mEstimateRecordLen;} 
	virtual int			getDataLen() {return mMemLen;}

	virtual OmnString	getFieldDelimiter() const {return mFieldDelimiter;}
	virtual OmnString	getTextQualifier() const {return mTextQualifier;}

	virtual bool 		setData(
							char *data, 
							const int len, 
							AosBuffData *metaData, 
							const int64_t offset);
	virtual void 		clear();

	virtual AosDataRecordObjPtr clone(AosRundata *rdata AosMemoryCheckDecl) const;
	virtual AosDataRecordObjPtr create(
							const AosXmlTagPtr &def,
							const AosTaskObjPtr &task,
							AosRundata *rdata AosMemoryCheckDecl) const;

	virtual bool		getFieldValue(
							const int idx,
							AosValueRslt &value,
							const bool copy_flag,
							AosRundata* rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
	bool	parseData(AosRundata* rdata);
};

#endif

#endif

