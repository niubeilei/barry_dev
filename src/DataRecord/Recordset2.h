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
#ifndef Aos_DataRecord_Recordset2_h
#define Aos_DataRecord_Recordset2_h

#include "SEInterfaces/RecordsetObj.h"
#include "Rundata/Ptrs.h"

class AosDataRecordObj;

class AosRecordset2 : public AosRecordsetObj 
{
private:

	AosBuffPtr					mDataBuff;
	AosBuff						*mDataBuffRaw;

	AosBuffDataPtr 				mBuffData;
	AosBuffData					*mBuffDataRaw;


	AosMetaDataPtr				mMetaData;
	AosMetaData					*mMetaDataRaw;

	AosSchemaObjPtr				mSchema;
	AosSchemaObj*				mSchemaRaw;

	vector<AosDataRecordObjPtr>	mRecords;
	vector<AosDataRecordObj *>	mRawRecords;

	bool 						mIncomplete;
	u64							mCount;

public:
	AosRecordset2();
	AosRecordset2(const AosRecordset2 &rhs);
	AosRecordset2(
		AosRundata *rdata, 
		const int num_entries);
	~AosRecordset2();

	virtual bool reset();
	virtual bool resetOffset() {return false;}

	virtual int64_t	size() const;

	virtual AosRecordsetObjPtr clone(AosRundata *rdata) const;
	virtual AosDataRecordObjPtr getRecord();
	AosDataRecordObj *	getRawRecord(const int idx);

	virtual vector<AosDataRecordObjPtr> getRecords();
	virtual bool swap(  AosRundata *rdata, const AosDataRecordObjPtr &record) {return false;}
	virtual void resetReadIdx() {}

	virtual AosBuffPtr getDataBuff() { return mDataBuff; }
	virtual AosBuffPtr getEntryBuff() { return 0; }
	// Jozhi, 2015/01/17
	virtual bool hasMore() const {return false;}

	virtual bool nextRecord(
						AosRundata* rdata, 
						AosDataRecordObj *& record);

	virtual bool setData(
						AosRundata *rdata, 
						const AosBuffPtr &buff);

	virtual void setMetaData(AosRundata *rdata,  
						const AosMetaDataPtr &metadata);

	virtual char* getData() {return 0;}


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
				AosBuffDataPtr &metaData) {return false;}

	virtual AosRecordsetObjPtr createRecordset(
						AosRundata *rdata,
						const AosXmlTagPtr &def);

	virtual AosRecordsetObjPtr createRecordset(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record) {return 0;}

	virtual AosRecordsetObjPtr createStreamRecordset(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record) {return 0;}

	//sorting method for downstream dataproc to use the data
	virtual bool sort(const AosRundataPtr &rdata, AosCompareFunPtr &comp) {return false;}

	//serialize  methods for object transportation between 
	//different processes/nodes
	bool serializeTo(AosRundata *rdata, AosBuff *buff) {return false;}
	bool serializeFrom(AosRundata *rdata, AosBuff *buff){return false;}

	// 2015.10.16
	virtual bool setSchema(
			AosRundata *rdata,    
			AosSchemaObjPtr schema);

	bool getIncomplete(){return mIncomplete;}

	bool getBuffData(AosBuffDataPtr &buffdata);


	bool setBuffData(
			AosRundata *rdata,    
			AosBuffDataPtr buffdata);

	//virtual AosRecordsetObjPtr clone(AosRundata *rdata);
	
	bool sort(const AosRundataPtr&, const AosCompareFunPtr&){return false;}

	virtual bool getIsStreamRecordset() {return false;}
	virtual bool replaceRawRecord(AosDataRecordObj* record) {return false;}

	virtual bool setDataBuff(                
			AosRundata *rdata,       
			AosBuffPtr buff) {return false;}
};
#endif

