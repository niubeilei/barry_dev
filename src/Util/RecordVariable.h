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
#if 0
This file is moved to DataRecord directory
#ifndef Aos_Util_RecordVariable_h
#define Aos_Util_RecordVariable_h

#include "Util/DataRecord.h"
#include "Util/DataTypes.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosRecordVariable : public AosDataRecord
{
	OmnDefineRCObject;

private:
	OmnString		mData;
	OmnString		mSep;
	int				mRecordLen;
	int				mDataLen;
	char *			mRawData;
	vector<int>		mStartPos;
	vector<int> 	mFieldLen;
	bool			mIsDataParsed;
	bool			mIsCaseSensitive;

public:
	AosRecordVariable(const bool flag);
	AosRecordVariable(const AosXmlTagPtr &def, AosRundata *rdata);
	~AosRecordVariable();

	// AosRecordVariable interface
	virtual int			getRecordLen() const {return mData.length();}
	virtual bool 		reset(AosRundata *rdata);
	virtual char *		getData() {return mData.getBuffer();}
	virtual const char *getData() const {return mData.data();}
	virtual OmnString	toString() const;
	virtual bool 		setData(char *data, const int len, const bool need_copy);
	virtual AosDataRecordPtr clone(const AosXmlTagPtr &def, AosRundata *) const;

	virtual OmnString	getOmnStr(const int idx, const OmnString &dft);
	virtual const char *getCharStr(const int idx, int &len, const char *dft, const int);
	virtual char		getChar(const int idx, const char dft);
	virtual int			getInt(const int idx, const int dft);
	virtual u32			getU32(const int idx, const u32 dft);
	virtual int64_t		getInt64(const int idx, const int64_t &dft);
	virtual u64			getU64(const int idx, const u64 &dft);
	virtual double		getDouble(const int idx, const double &dft);

	virtual bool		setOmnStr(const int idx, const OmnString &data);
	virtual bool		setCharStr(const int idx, const char *data, const int len);
	virtual bool		setChar(const int idx, const char data);
	virtual bool		setInt(const int idx, const int data);
	virtual bool		setU32(const int idx, const u32 data);
	virtual bool		setInt64(const int idx, const int64_t &data);
	virtual bool		setU64(const int idx, const u64 &data);
	virtual bool		setDouble(const int idx, const double &data);
	virtual bool		setValue(
							const int idx, 
							const AosValueRslt &value, 
							AosRundata *rdata);

private:
	bool config(const AosXmlTagPtr &def, AosRundata *rdata);
	bool parseData();
};

#endif
#endif
