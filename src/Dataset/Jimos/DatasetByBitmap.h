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
// 2015/01/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Jimos_DatasetByBitmap_h
#define Aos_Dataset_Jimos_DatasetByBitmap_h

#include "Dataset/Dataset.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"

#include <queue>
class AosDatasetByBitmap : public AosDataset
{
	OmnDefineRCObject;

protected:

public:
	AosDatasetByBitmap(const int version);
	~AosDatasetByBitmap();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;
	
	virtual bool	reset(const AosRundataPtr &rdata);
	virtual int64_t	getNumEntries();
	virtual int64_t	getTotalSize() const;
	virtual void	setTask(const AosTaskObjPtr &t);
	virtual bool	sendStart(const AosRundataPtr &rdata);
	virtual bool	sendFinish(const AosRundataPtr &rdata);
	virtual bool	getRecords(vector<AosDataRecordObjPtr> &records);
	virtual bool	config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc);
	virtual AosRecordsetObjPtr	getRecordset(){return mRecordset;}
	virtual AosRecordsetObjPtr  cloneRecordset(AosRundata *rdata){return mRecordset->clone(rdata);}

	virtual bool	nextRecordset(
						const AosRundataPtr &rdata, 
						AosRecordsetObjPtr &recordset);

	virtual bool	setValueBuff(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);
};
#endif

