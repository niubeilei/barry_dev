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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DatasetObj_h
#define Aos_SEInterfaces_DatasetObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DatasetId.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Util/ValueRslt.h"

#define AOSDATASET_SCANORDER_RANDOM				"random"
#define AOSDATASET_SCANORDER_ROUNDROBIN			"roundrobin"
#define AOSDATASET_SCANORDER_SEQUENTIAL			"sequential"


class AosDatasetObj : public AosJimo, public AosTaskReporter
{
public:
	// Chen Ding, moved to AosDataset, 2015/01/28
	// vector<OmnString>           mErrorRecords;
	// vector<OmnString>           mFilterRecords;   
	// vector<OmnString>           mInvalidRecords;   

	AosDatasetObj(const u32 version);
	~AosDatasetObj();

	virtual bool sendStart(const AosRundataPtr &rdata) = 0;
	virtual bool sendFinish(const AosRundataPtr &rdata) = 0;
	virtual bool reset(const AosRundataPtr &rdata) = 0;
	virtual int64_t getNumEntries() = 0;
	virtual int64_t getTotalSize() const = 0;
	virtual void setTaskDocid(const u64 task_docid) = 0;
	virtual AosRecordsetObjPtr getRecordset() = 0;
	virtual AosRecordsetObjPtr cloneRecordset(AosRundata *rdata) = 0;

	// Called by AosScanDataEngine
	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record) = 0;

	// Called by AosJoinDataEngine
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records) = 0;

	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc) = 0;

	virtual bool nextRecordset(
					const AosRundataPtr &rdata, 
					AosRecordsetObjPtr &recordset) = 0;

	virtual bool stop(
					const AosRundataPtr &rdata, 
					const AosRecordsetObjPtr &recordset) = 0;

	virtual bool addDataConnector(const AosRundataPtr &rdata, 
	 				const AosDataConnectorObjPtr &data_connector) = 0;

	virtual bool addUnrecogContents(
					const AosRundataPtr &rdata,
					const AosBuffDataPtr &buff_data,
					const char *data,
					const int64_t &start_pos,
					const int64_t &length) = 0;

	virtual bool addInvalidContents(
					const AosRundataPtr &rdata,
					AosDataRecordObj * record) = 0;

	virtual bool addFilterContents(
					const AosRundataPtr &rdata,
					AosDataRecordObj * record) = 0;

	// virtual bool addContents(
	// 				AosRundata *rdata, 
	// 				AosBuff *buff) = 0;

	virtual bool getRsltInfor(
					AosXmlTagPtr &info,
					const AosRundataPtr &rdata) = 0;

	virtual bool setValueBuff(
	 				const AosBuffPtr &buff,
	 				const AosRundataPtr &rdata) = 0;

	static AosDatasetObjPtr createDatasetStatic(
					const AosRundataPtr &rdata,
					const AosTaskObjPtr &task,
					const AosXmlTagPtr &worker_doc);

	static AosDatasetObjPtr createDatasetStatic(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &def);

	static AosXmlTagPtr getDatasetConfig(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &dataset_conf);

	// Chen Ding, 2015/01/28
	virtual vector<AosDatasetObjPtr> split(AosRundata *rdata) = 0;

	// Phil, 2015/09/03
	virtual bool data2Str(
				const AosRundataPtr &rdata,
				OmnString &contents) = 0;

	virtual bool setCellValue(
			AosRundata *rdata,
			const u32 &curRowsNum,
			const u32 &curColumnsNum,  
			const AosValueRslt &value) = 0;

	// Jozhi, 2015/09/11, JIMODB-734
	virtual bool supportMatrix() const = 0;

	// Young, 2015/10/10
	virtual bool nextRecord(AosRundata* rdata, AosDataRecordObj *& record) = 0;
	virtual bool setData(AosRundata *rdata, const AosBuffPtr &buff);

	// jimodb-1375
	virtual void    setPageSize(u64 num) = 0;
};
#endif

