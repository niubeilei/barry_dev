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
// 07/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_RecordBuff_h
#define Aos_DataRecord_RecordBuff_h

#include "DataRecord/DataRecord.h"
#include "Util/DataTypes.h"
#include "Util/Ptrs.h"


class AosRecordBuff : public AosDataRecord
{
	struct Entry
	{
		int				pos;
		int				field_idx;
		AosDataType::E	data_type;
		int				field_len;

		Entry(const int p, const AosDataType::E dt)
		:
		p(pos), 
		data_type(dt),
		field_len(0)
		{
		}

		Entry(const int p, const AosDataType::E dt, const int len)
		:
		p(pos), 
		data_type(dt),
		field_len(len)
		{
		}

		Entry(const int idx)
		:
		p(-1), 
		data_type(AosDataType::eInvalid),
		field_idx(idx),
		field_len(0)
		{
		}
	};

	typedef hash_map<const char *, Entry, char_str_hash, compare_charstr> map_t;
	typedef hash_map<const char *, Entry, char_str_hash, compare_charstr>::iterator map_t;

	OmnString		mInternalData;

	char *			mData;
	int				mDataLen;
	int				mRecordLen;
	int				mBuffIdx;
	map_t			mFieldMap;

public:
	AosRecordBuff();
	~AosRecordBuff();

	// AosDataRecordObj interface
	virtual bool		isFixed() const {return false;}

	virtual int			getRecordLen() {return mRecordLen;}
	virtual int			getEstimateRecordLen() {return mRecordLen;} 
	virtual int			getDataLen() {return mDataLen;}
	virtual void		setDataLen(const int data_len) {mDataLen = data_len;}

	virtual char *		getData(AosRundata *rdata);

	virtual void 		clear();

	virtual AosDataRecordObjPtr clone(AosRundata *rdata AosMemoryCheckDecl) const;

	virtual bool 		setFieldValue(
							const int idx, 
							AosValueRslt &value, 
							bool &outofmem,
							AosRundata* rdata);

	virtual bool 		setFieldValue(
							const OmnString &field_name,
							AosValueRslt &value, 
							bool &outofmem,
							AosRundata* rdata);


private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
};

#endif

