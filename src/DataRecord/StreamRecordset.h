////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// 2015/08/06 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataRecord_StreamRecordset_h
#define Aos_DataRecord_StreamRecordset_h

#include "MetaData/MetaData.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/DataRecordType.h"
#include "Rundata/Ptrs.h"
#include <vector>
using namespace std;

class AosDataRecordObj;

class AosStreamRecordset : public AosRecordsetObj 
{
private:
	OmnMutexPtr						mLock;
	AosDataRecordObjPtr				mRecord;
	AosDataRecordObj*				mRecordRaw;
	vector<AosDataRecordObjPtr>		mRecords;
	AosBuffPtr 						mBuff;
	AosBuff*						mBuffRaw;
	AosDataRecordType::E			mRecordType;
	i64								mCrtOffset;
	int								mRecordLen;
	int								mSize;
	i64								mWriteIdx;

public:
	AosStreamRecordset();
	AosStreamRecordset(const AosStreamRecordset &rhs, AosRundata *rdata);
	~AosStreamRecordset();
	virtual bool getIsStreamRecordset() { return true;}
	virtual bool reset();
	virtual bool resetOffset();
	virtual int64_t	size() const { return mSize; }
	virtual AosRecordsetObjPtr clone(AosRundata *rdata) const;
	virtual AosDataRecordObjPtr getRecord();
	virtual AosDataRecordObj *	getRawRecord(const int idx);
	virtual vector<AosDataRecordObjPtr> getRecords();
	virtual bool swap(  AosRundata *rdata, const AosDataRecordObjPtr &record);
	virtual void resetReadIdx();

	virtual AosBuffPtr getDataBuff() { return mBuff; }
	virtual AosBuffPtr getEntryBuff() { return mBuff; }

	virtual bool hasMore() const {return false;}

	virtual bool setData(
						AosRundata *rdata, 
						const AosBuffPtr &buff);

	virtual void setMetaData(AosRundata *rdata,  
						const AosMetaDataPtr &metadata);

	virtual char* getData();
	

	virtual bool holdRecord(
						AosRundata *rdata, 
						const AosDataRecordObjPtr &record);

	virtual bool holdRecords(
						AosRundata *rdata, 
						const vector<AosDataRecordObjPtr> &records);

	virtual AosRecordsetObjPtr createRecordset(
						AosRundata *rdata,
						const AosXmlTagPtr &def);

	virtual AosRecordsetObjPtr createStreamRecordset(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record);
	
	virtual AosRecordsetObjPtr createRecordset(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record);

	bool appendRecord(
				AosRundata *rdata, 
				AosDataRecordObjPtr &rcd,
				AosBuffDataPtr &metaData);

	virtual bool appendRecord(
						AosRundata *rdata, 
						const int index,
						const int64_t offset, 
						const int len,
						const AosMetaDataPtr &metaData);
	virtual bool nextRecord(AosRundata* rdata, AosDataRecordObj *& record);
	virtual bool sort(const AosRundataPtr &rdata, const AosCompareFunPtr &comp);
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff);
	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff);
	virtual bool replaceRawRecord(AosDataRecordObj* record);

	// 2015.10.19, xiafan
	virtual bool setSchema(
			AosRundata *rdata,    
			AosSchemaObjPtr schema) {return false;}

	bool setBuffData(
			AosRundata *rdata,    
			AosBuffDataPtr buffdata){return false;}

	bool setDataBuff(AosRundata*, AosBuffPtr);

private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
};
#endif

