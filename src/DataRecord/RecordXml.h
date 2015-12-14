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
// 2014/03/31 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordXml_h
#define Aos_DataRecord_RecordXml_h

#include "DataRecord/DataRecord.h"


class AosRecordXml : public AosDataRecord
{
	AosXmlTagPtr	mXml;

public:
	AosRecordXml(const bool flag AosMemoryCheckDecl);
	AosRecordXml(
			const AosRecordXml &rhs,
			AosRundata *rdata AosMemoryCheckDecl);
	~AosRecordXml();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return false;}

	virtual int			getRecordLen();
	virtual int			getEstimateRecordLen() {return 0;} 

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
							const OmnString &field_name,
							AosValueRslt &value,
							const bool copy_flag,
							AosRundata* rdata);
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

	OmnString			xpathQuery(
							const OmnString &xpath,
							AosRundata *rdata);
	bool				xpathSetAttr(
							const OmnString &xpath,
							const OmnString &value,
							AosRundata *rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
	void	createEmptyXml();
};

#endif

