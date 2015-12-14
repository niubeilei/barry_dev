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
// 2015/04/28 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Jimos_DatasetByReadDoc_h
#define Aos_Dataset_Jimos_DatasetByReadDoc_h

#include "Dataset/Dataset.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"

#include <queue>
class AosDatasetByReadDoc : public AosDataset
{
	OmnDefineRCObject;

	AosDataScannerObjPtr		mDataScanner;
	AosDatasetObjPtr			mScanDataset;	
	OmnMutexPtr					mLock;
	bool						mSetBuff;

public:
	AosDatasetByReadDoc(
		const OmnString &type, 
		const int version);
	AosDatasetByReadDoc(const int version);
	~AosDatasetByReadDoc();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;
	virtual void	setTaskDocid(const u64 task_docid);
	virtual bool	nextRecordset(
						const AosRundataPtr &rdata, 
						AosRecordsetObjPtr &recordset);
	virtual bool	sendStart(const AosRundataPtr &rdata);
	virtual bool	sendFinish(const AosRundataPtr &rdata);
	virtual bool	config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc);
	virtual bool	getRecord(const OmnString &name, AosDataRecordObjPtr &record);
	virtual bool	getRecords(vector<AosDataRecordObjPtr> &records);
};
#endif

