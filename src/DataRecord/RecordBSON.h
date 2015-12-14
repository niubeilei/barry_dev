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
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordBSON_h
#define Aos_DataRecord_RecordBSON_h

#include "DataRecord/DataRecord.h"


class AosRecordBSON : public AosDataRecord
{
	AosBSONPtr		mBSON;
	AosBSON *		mBSONRaw;

public:
	AosRecordBSON(const bool flag AosMemoryCheckDecl);
	AosRecordBSON(
			const AosRecordBSON &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordBSON();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return false;}
	virtual int			getRecordLen();
	virtual int			getEstimateRecordLen() {return getRecordLen();} 
	virtual char *		getData(AosRundata *rdata);
	virtual void 		clear();

	virtual bool 		setData(
							char *data, 
							const int len, 
							AosBuffData *metaData, 
							const int64_t offset);

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

	virtual int	getFieldIdx( const OmnString &name,
							AosRundata *rdata);
	
	virtual AosDataFieldObj*	getFieldByIdx1(const u32 idx);

private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
};

#endif

