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
#ifndef Aos_Util_DataRecord_h
#define Aos_Util_DataRecord_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RecordType.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosValueRslt;

class AosDataRecord : virtual public OmnRCObject
{
protected:
	enum
	{
		eMaxDocids = 50
	};

	AosRecordType::E	mType;
	vector<AosDataElemPtr>	mElems;
	int					mNumElems;
	u64					mDocid;
	u64					mDocids[eMaxDocids];	// Chen Ding, 05/30/2012
	u64					mKeyDocid;
	u64					mValueDocid;

public:
	AosDataRecord(const OmnString &name, const AosRecordType::E type, const bool flag);
	AosDataRecord(const AosRecordType::E type, const AosXmlTagPtr &def, AosRundata *rdata);
	~AosDataRecord();

	// AosDataRecord interface
	virtual int			getRecordLen() const = 0;
	virtual bool 		reset(AosRundata *rdata) = 0;
	virtual char *		getData() = 0;
	virtual const char *getData() const = 0;
	virtual OmnString 	toString() const = 0;
	virtual bool 		setData(char *data, const int len, const bool need_copy) = 0;
	virtual AosDataRecordPtr clone(const AosXmlTagPtr &def, AosRundata *) const =0;

	virtual OmnString	getOmnStr(const int idx, const OmnString &dft) = 0;
	virtual const char *getCharStr(const int idx, int &len, const char *dft, const int) = 0;
	virtual char		getChar(const int idx, const char dft) = 0;
	virtual int			getInt(const int idx, const int dft) = 0;
	virtual u32			getU32(const int idx, const u32 dft) = 0;
	virtual int64_t		getInt64(const int idx, const int64_t &dft) = 0;
	virtual u64			getU64(const int idx, const u64 &dft) = 0;
	virtual double		getDouble(const int idx, const double &dft) = 0;

	virtual bool		setOmnStr(const int idx, const OmnString &data) = 0;
	virtual bool		setCharStr(const int idx, const char *data, const int len) = 0;
	virtual bool		setChar(const int idx, const char data) = 0;
	virtual bool		setInt(const int idx, const int data) = 0;
	virtual bool		setU32(const int idx, const u32 data) = 0;
	virtual bool		setInt64(const int idx, const int64_t &data) = 0;
	virtual bool		setU64(const int idx, const u64 &data) = 0;
	virtual bool		setDouble(const int idx, const double &data) = 0;
	virtual bool		setValue(
							const int idx, 
							const AosValueRslt &value, 
							AosRundata *rdata) = 0;

	u64 	getDocid() const {return mDocid;}
	u64 	getKeyDocid() const {return mKeyDocid;}
	u64 	getValueDocid() const {return mValueDocid;}
	void 	setDocid(const u64 &docid) {mDocid = docid;}
	void 	setKeyDocid(const u64 &docid) {mKeyDocid = docid;}
	void 	setValueDocid(const u64 &docid) {mValueDocid = docid;}
	int		getNumFields() const {return mNumElems;}
	bool 	config(const AosXmlTagPtr &def, AosRundata *rdata);
	inline bool setDocid(const int idx, const u64 &docid)
	{
		aos_assert_r(idx >= 0 && idx < eMaxDocids, false);
		mDocids[idx] = docid;
		return true;
	}

	inline u64  getDocid(const int idx) const
	{
		aos_assert_r(idx >= 0 || idx < eMaxDocids, 0);
		return mDocids[idx];
	}
	
	inline bool setData(const OmnString &record, const bool need_copy)
	{
		return setData((char *)record.data(), record.length(), need_copy);
	}

	static AosDataRecordPtr getRecord(const AosXmlTagPtr &def, AosRundata *rdata);
	static bool staticInit();


private:
	bool registerRecord(const OmnString &name, AosDataRecord *record);
};

#endif
#endif
