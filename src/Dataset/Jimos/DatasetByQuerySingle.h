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
// 2014/04/29 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_DatasetByQuerySingle_h
#define Aos_Dataset_DatasetByQuerySingle_h

#include "Dataset/Jimos/DatasetByQuery.h"

#include "SEInterfaces/QueryReqObj.h"


class AosDatasetByQuerySingle : virtual public AosDatasetByQuery
{
private:
	AosQueryReqObjPtr	mQueryReq;
	AosDatasetObjPtr	mScanDataset;	
	bool				mSetScanData;	

	bool				mIsCountAll;
	bool				mPickDocidsOnly;

	OmnString			mTableObjid;

public:
	AosDatasetByQuerySingle(const int version);
	~AosDatasetByQuerySingle();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;

	virtual bool sendStart(const AosRundataPtr &rdata);
	virtual bool sendFinish(const AosRundataPtr &rdata);

	virtual bool nextRecordset(
					const AosRundataPtr &rdata, 
					AosRecordsetObjPtr &recordset);

	virtual bool getRsltInfor(
					AosXmlTagPtr &infor,
					const AosRundataPtr &rdata);

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc);

private:
	virtual bool checkIsIndexQuery(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &def);

	virtual bool generateScanData(const AosRundataPtr &rdata);

	virtual bool nextRecordsetIndex(
					const AosDataRecordObjPtr &record,
					const AosRundataPtr &rdata);
	virtual bool nextRecordsetNoIndex(
					const AosDataRecordObjPtr &record,
					const AosRundataPtr &rdata);
	virtual bool nextRecordsetUsingFieldValues(
					const AosDataRecordObjPtr &record,
					const AosRundataPtr &rdata);
};

#endif

