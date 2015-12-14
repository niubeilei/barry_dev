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
// 2015/01/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordValue_h
#define Aos_DataRecord_RecordValue_h

#include "DataRecord/DataRecord.h"
#include "Util/DataTypes.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosRecordValue : public AosDataRecord
{
	OmnDefineRCObject;

private:
	OmnString	mFieldName;
	AosValue	mValue;
	char *		mMemory;

public:
	AosRecordValue(const int &version);
	~AosRecordValue();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosRecordValue interface
	virtual bool		isFixed() const {return mValue.isFixedLength();}
	virtual int			getRecordLen() const;
	virtual int			getDataLen() const {return getRecordLen();}

	virtual char *		getData() {return mMemory;}
	virtual bool 		setData(
							char *data, 
							const int len, 
							const AosBuffDataPtr &metaData, 
							const int64_t offset,
							const bool need_copy);
	virtual void 		clear();

	virtual AosDataRecordObjPtr clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const;
	virtual bool		getFieldValue(
							const int idx,
							AosValueRslt &value,
							const AosRundataPtr &rdata);
};
#endif

