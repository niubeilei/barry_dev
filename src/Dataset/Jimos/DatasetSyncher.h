////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2015/09/24 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Jimos_DatasetSyncher_h
#define Aos_Dataset_Jimos_DatasetSyncher_h

#include "SEInterfaces/DatasetSyncherObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "Util/DataTypes.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/Ptrs.h"

class AosDatasetSyncher : public AosDatasetSyncherObj
{
	OmnDefineRCObject;

private:
	AosDataRecordObjPtr				mRecord;
	vector<AosDataRecordObjPtr>		mRecords;
	AosBuffPtr 						mBuff;
	i64								mCrtOffset;

protected:
	//AosRecordsetObjPtr			mRecordset;
public:
	AosDatasetSyncher(const OmnString &type, const int version);
	AosDatasetSyncher(const int version);
	~AosDatasetSyncher();

	virtual AosRecordsetObjPtr cloneRecordset(AosRundata *rdata);
	virtual AosJimoPtr cloneJimo()  const;
	//virtual AosRecordsetObjPtr getRecordset() { return mRecordset; }
	//virtual AosRecordsetObjPtr cloneRecordset(){return mRecordset->clone();}

	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);
	bool 		nextRecordset(
					const AosRundataPtr &rdata,
					AosRecordsetObjPtr &recordset);
	bool 		appendRecord( 
						AosRundata *rdata,
						const int index,
						const int64_t offset,
						const int len,
						const AosMetaDataPtr &metaData);
public:
	bool 		appendRecord(AosRundata *rdata, AosDataRecordObj *& record);

	virtual bool nextRecord(AosRundata* rdata, AosDataRecordObj *& record);
	virtual bool setData(AosRundata *rdata, const AosBuffPtr &buff);
	char* 		getData();

private:
	bool 		nextRecordset(           
					AosRundata *rdata, 
					AosRecordsetObjPtr &recordset, 
					const AosBuffDataPtr &buffdata,
					AosDatasetObj *dataset,
					bool &contents_incomplete);
	
};
#endif

