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
// 2013/10/31 Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_StructProc_h
#define Aos_SEInterfaces_StructProc_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/ValueRslt.h"
#include "TaskUtil/Ptrs.h"

//#define AOSSTRUCTPROC_SUM					"sum"
//#define AOSSTRUCTPROC_MAX					"max"
//#define AOSSTRUCTPROC_MIN					"min"

class AosStructProcObj : public AosJimo
{
public:
	AosStructProcObj(const int version);
	~AosStructProcObj();

	static AosStructProcObjPtr  createStructProcStatic(
			AosRundata *rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc);

	static AosStructProcObjPtr  createStructProcStatic(
			AosRundata *rdata, 
			const AosXmlTagPtr &worker_doc);

	virtual void config(const AosXmlTagPtr &conf) = 0;

	virtual bool setFieldValue(
			char *array,
			const int64_t data_len,
			const int pos,
			const int64_t &value,
			AosDataType::E data_type,
			const u64 &docid) = 0;

	virtual vector<AosValueRslt> & getNewValues() = 0;

	virtual vector<AosValueRslt> & getOldValues() = 0;

	virtual void clearValuesRslt() = 0;

	virtual int64_t calculateGeneralValue(const int64_t &new_value, const int64_t &old_value) = 0;

	virtual int getDftValue() = 0;

	virtual AosJimoPtr cloneJimo()  const = 0;
};

#endif

