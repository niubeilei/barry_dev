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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_Recordset_h
#define Aos_DataRecord_Recordset_h

#include "SEInterfaces/RecordsetObj.h"
#include "Rundata/Ptrs.h"
#include <vector>
using namespace std;

class AosDataRecordObj;

class AosRecordset : public AosRecordsetObj 
{

private:
	enum
	{
		eEntrySize = 20,
		ePosOffset = 8,
		eLengthOffset = 12,
		eDftMaxRecordsetSize = 1000*1000*10
	};

	int64_t		mMaxSize;
	int64_t		mNumEntries;
	int64_t		mReadIdx;
	int64_t		mWriteIdx;
	int64_t		mDataLen;
	int64_t		mOffset;
	Entry *		mEntries;
	char *		mData;
	AosBuffPtr	mEntryBuff;
	AosBuffPtr	mDataBuff;
	vector<AosDataRecordObjPtr>	mRecords;

	static int64_t smMaxRecordsetSize;

public:
	AosRecordset();
	AosRecordset(const AosRecordset &rhs);
	AosRecordset(
		const AosRundataPtr &rdata, 
		const int num_entries);
	~AosRecordset();

	virtual bool reset(){return true;}
	virtual int size() const {return mNumEntries;}
	virtual AosDataRecordObj *nextRecord(const AosRundataPtr &rdata);
	virtual bool setData(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);
	virtual bool holdRecord(
						const AosRundataPtr &rdata, 
						const AosDataRecordObjPtr &record);
	virtual bool holdRecords(
						const AosRundataPtr &rdata, 
						const vector<AosDataRecordObjPtr> &records);
	virtual bool appendRecord(
						const AosRundataPtr &rdata, 
						AosDataRecordObj *record, 
						const int64_t offset, 
						const int len);
	virtual int64_t getNumRecordsToAdd();
	virtual int64_t getWriteIdx() const {return mWriteIdx;}
	virtual AosRecordsetObjPtr clone() const;
	virtual AosRecordsetObjPtr createRecordset(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};
#endif

