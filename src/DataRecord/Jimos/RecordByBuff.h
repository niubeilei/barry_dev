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
// 2013/12/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordByBuff_h
#define Aos_DataRecord_RecordByBuff_h

#include "DataRecord/DataRecord.h"
#include "Util/DataTypes.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosRecordByBuff : public AosDataRecord
{
	OmnDefineRCObject;

private:
	AosBuffPtr		mDataBuff;
	int64_t			mDataLen;
	char *			mData;
	bool			mIsDataParsed;
	i64				mStartIdx;

public:
	AosRecordByBuff(const OmnString &version);
	~AosRecordByBuff();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosRecordByBuff interface
	virtual bool		isFixed() const {return false;}
	virtual int			getRecordLen() const;
	virtual int			getDataLen() const {return mDataLen;}

	virtual char *		getData() {return mData;}
	virtual bool 		setData(
							char *data, 
							const int len, 
							const AosBuffDataPtr &metaData, 
							const int64_t offset,
							const bool need_copy);
	virtual void 		clear();

	virtual AosDataRecordObjPtr clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const;
	virtual AosDataRecordObjPtr create(
							const AosXmlTagPtr &def,
							const AosTaskObjPtr &task,
							const AosRundataPtr &rdata AosMemoryCheckDecl) const;

	virtual bool		getFieldValue(
							const int idx,
							AosValueRslt &value,
							const AosRundataPtr &rdata);

	virtual AosValuePtr getFieldValue(
								const AosRundataPtr &rdata, 
								const int field_idx);

private:
	bool	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc);

	bool	parseData(const AosRundataPtr &rdata);
};

#endif

