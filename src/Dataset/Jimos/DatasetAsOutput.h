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
// 2015/01/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_DatasetAsOutput_h
#define Aos_Dataset_DatasetAsOutput_h

#include "Dataset/Dataset.h"

#include "QueryUtil/QrUtil.h"
#include "SEInterfaces/ConditionObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Thread/Mutex.h"


class AosDatasetAsOutput : public AosDataset
{
	OmnDefineRCObject;

protected:
	AosDatasetPtr			mDataset;
	AosDataset*				mDatasetRaw;
	AosDataAssemblerObjPtr	mDataAssembler;
	AosDataCollectorObjPtr	mDataCollector;

public:
	AosDatasetAsOutput(OmnString &type, const int version);
	~AosDatasetAsOutput();

	// AosDatasetObj interface
	virtual bool sendStart(const AosRundataPtr &rdata);
	virtual bool sendFinish(const AosRundataPtr &rdata);
	virtual bool reset(const AosRundataPtr &rdata);
	virtual int64_t getNumEntries();
	virtual int64_t getTotalSize() const;
	virtual void setTask(const AosTaskObjPtr &task);
	virtual AosRecordsetObjPtr getRecordset();
	virtual AosRecordsetObjPtr cloneRecordset();
	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record);
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records);

	virtual bool nextRecordset(
					const AosRundataPtr &rdata, 
					AosRecordsetObjPtr &recordset);

	virtual bool addDataCube(const AosRundataPtr &rdata, 
	 				const AosDataCubeObjPtr &data_cube);

	virtual bool addUnrecogContents(
					const AosRundataPtr &rdata,
					const AosBuffDataPtr &buff_data,
					const char *data,
					const int64_t &start_pos,
					const int64_t &length);

	virtual bool addInvalidContents(
					const AosRundataPtr &rdata,
					AosDataRecordObj * record);

	virtual bool addFilterContents(
					const AosRundataPtr &rdata,
					AosDataRecordObj * record);

	virtual bool addContents(
					AosRundata *rdata, 
					AosBuff *buff);

	virtual bool config(const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);

	virtual bool getRsltInfor(
					AosXmlTagPtr &infor,
					const AosRundataPtr &rdata);

	virtual bool setValueBuff(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata);
};
#endif
