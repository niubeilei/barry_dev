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
#ifndef Aos_Dataset_Dataset_h
#define Aos_Dataset_Dataset_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/DatasetObj.h"
#include "SEInterfaces/DatasetId.h"
#include "SEInterfaces/RecordsetObj.h"


class AosDataset : public AosDatasetObj
{
protected:
	AosDatasetId::E		mId;
	OmnString			mJimoSubType;
	u64 				mTaskDocid;
	// vector<OmnString>           mErrorRecords;
	// vector<OmnString>           mFilterRecords;   
	// vector<OmnString>           mInvalidRecords;   

public:
	AosDataset(const OmnString &type, const u32 version);
	~AosDataset();
	
	virtual bool sendStart(const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual bool sendFinish(const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual AosRecordsetObjPtr getRecordset()
	{
		OmnShouldNeverComeHere;
		return 0;
	}
	
	virtual AosRecordsetObjPtr cloneRecordset(AosRundata *rdata)
	{
		OmnShouldNeverComeHere;
		return 0;
	}
	
	virtual bool reset(const AosRundataPtr &rdata);
	virtual int64_t getNumEntries();
	virtual bool nextRecordset(
					const AosRundataPtr &rdata, 
					AosRecordsetObjPtr &recordset);
	
	virtual bool stop(
					const AosRundataPtr &rdata, 
					const AosRecordsetObjPtr &recordset);
	
	virtual int64_t getTotalSize() const{return -1;}
	
	virtual void setTaskDocid(const u64 task_docid){mTaskDocid = task_docid;}

	virtual bool addDataConnector(const AosRundataPtr &rdata, 
	 				const AosDataConnectorObjPtr &data_cube);
	
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

	// Chen Ding, 2014/08/13
	virtual bool addContents(
					AosRundata *rdata, 
					AosBuff *data);

	virtual bool addContents(
					AosRundata *rdata, 
					AosBuffArrayVar *data);

	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record){return false;}
	
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records){return false;}
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;

	virtual bool getRsltInfor(
					AosXmlTagPtr &infor,
					const AosRundataPtr &rdata) {return false;}

	virtual bool data2Str(
				const AosRundataPtr &rdata,
				OmnString &contents);

	virtual bool setValueBuff(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata) {return false;}

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc) {return true;}

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc) {return true;}

	// Chen Ding, 2015/01/28
	virtual vector<AosDatasetObjPtr> split(AosRundata *rdata);

	virtual bool setCellValue(
			AosRundata *rdata,
			const u32 &curRowsNum,
			const u32 &curColumnsNum,  
			const AosValueRslt &value){ return false; }

	static bool				convert(
							AosRundata* rdata_raw,
							AosDatasetObjPtr &output_dataset,
							const AosXmlTagPtr &def,
							const AosDatasetObjPtr &dataset);

	// Jozhi, 2015/09/11, JIMODB-734
	virtual bool supportMatrix() const
	{
		return false;
	}

	virtual bool nextRecord(AosRundata* rdata, AosDataRecordObj *& record)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	// jimodb-1375
	void    setPageSize(u64 num) {}
};
#endif



