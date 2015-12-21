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
// 2014/05/07 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_DatasetByQueryNest_h
#define Aos_Dataset_DatasetByQueryNest_h

#include "Dataset/Jimos/DatasetByQuery.h"

#include "SEInterfaces/QueryReqObj.h"


class AosDatasetByQueryNest : virtual public AosDatasetByQuery
{
	AosDatasetObjPtr	mScanDataset;	

public:
	AosDatasetByQueryNest(const int version);
	~AosDatasetByQueryNest();
	
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

	virtual bool nextRecordsetNoIndex(const AosRundataPtr &rdata);
	virtual bool nextRecordsetUsingFieldValues(const AosRundataPtr &rdata);

	//Phil 2015-09-03, add input dataset for subquery
	//jimodb-671
	void setScanDataset(const AosDatasetObjPtr &inputDataset)
	{
		mScanDataset = inputDataset;
	}
};

#endif

