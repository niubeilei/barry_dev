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

#include "MetaData/MetaData.h"
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
		eDftRecordsetSize = 100000,
		eDftMaxRecordsetSize = 10000*10
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
	AosBuff		*mEntryBuffRaw;

	AosBuffPtr	mDataBuff;
	AosBuff		*mDataBuffRaw;

	AosMetaDataPtr				mMetaData;
	AosMetaData					*mMetaDataRaw;

	vector<AosDataRecordObjPtr>	mRecords;
	vector<AosDataRecordObj *>	mRawRecords;

	static int64_t smMaxRecordsetSize;

	//for sorting purpose
	AosCompareFunPtr mComp;

public:
	AosRecordset();
	AosRecordset(const AosRecordset &rhs, AosRundata *rdata);
	AosRecordset(
		AosRundata *rdata, 
		const int num_entries);
	~AosRecordset();

	virtual bool getIsStreamRecordset() { return false;}
	virtual bool reset();
	virtual bool resetOffset() {return false;}

	virtual int64_t	size() const {return mWriteIdx;}
	//virtual int64_t getNumRecordsToAdd();
	virtual AosRecordsetObjPtr clone(AosRundata *rdata) const;
	virtual AosDataRecordObjPtr getRecord();
	AosDataRecordObj *	getRawRecord(const int idx);
	virtual vector<AosDataRecordObjPtr> getRecords(){return mRecords;}
	virtual bool swap(  AosRundata *rdata, const AosDataRecordObjPtr &record);
	virtual void resetReadIdx() {mReadIdx = 0;}

	virtual AosBuffPtr getDataBuff() { return mDataBuff; }
	virtual AosBuffPtr getEntryBuff() { return mEntryBuff; }
	// Jozhi, 2015/01/17
	virtual bool hasMore() const {return mReadIdx >= mWriteIdx;}

	virtual bool nextRecord(
						AosRundata* rdata, 
						AosDataRecordObj *& record);

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

	virtual bool appendRecord(
						AosRundata *rdata, 
						const int index,
						const int64_t offset, 
						const int len,
						const AosMetaDataPtr &metaData);

	bool appendRecord(
				AosRundata *rdata, 
				AosDataRecordObjPtr &rcd,
				AosBuffDataPtr &metaData);

	virtual AosRecordsetObjPtr createRecordset(
						AosRundata *rdata,
						const AosXmlTagPtr &def);

	virtual AosRecordsetObjPtr createRecordset(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record);

	virtual AosRecordsetObjPtr createStreamRecordset(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record);

	//sorting method for downstream dataproc to use the data
	virtual bool sort(const AosRundataPtr &rdata, const AosCompareFunPtr &comp);

	//serialize  methods for object transportation between 
	//different processes/nodes
	bool serializeTo(AosRundata *rdata, AosBuff *buff);
	bool serializeFrom(AosRundata *rdata, AosBuff *buff);
	virtual bool replaceRawRecord(AosDataRecordObj* record);

	// 2015.10.16
	virtual bool setSchema(
			AosRundata *rdata,    
			AosSchemaObjPtr schema) {return false;}

	bool getIncomplete(){return false;}

	bool getBuffData(AosBuffDataPtr &buffdata) {return false;}

	bool setBuffData(                
			AosRundata *rdata,       
			AosBuffDataPtr buffdata) {return false;};

	virtual bool setDataBuff(                
			AosRundata *rdata,       
			AosBuffPtr buff){return false;}

private:
	bool	config(
				const AosXmlTagPtr &def,
				AosRundata *rdata);
};
#endif

