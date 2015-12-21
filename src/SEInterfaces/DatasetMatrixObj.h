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
// 2015/09/11 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DatasetMatrixObj_h
#define Aos_SEInterfaces_DatasetMatrixObj_h

#include "SEInterfaces/DatasetObj.h"
#include "SEInterfaces/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/DatasetId.h"


class AosDatasetMatrixObj : public AosDatasetObj
{
public:
	AosDatasetMatrixObj(
			const OmnString &type, 
			const u32 version)
    :
	AosDatasetObj(version)
	{

	}
	
	~AosDatasetMatrixObj() {};

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
	
	virtual AosRecordsetObjPtr cloneRecordset()
	{
		OmnShouldNeverComeHere;
		return 0;
	}
	
	virtual bool reset(const AosRundataPtr &rdata) { return false;}
	virtual int64_t getNumEntries() { return -1;}
	virtual bool nextRecordset(
					const AosRundataPtr &rdata, 
					AosRecordsetObjPtr &recordset){ return false;}
	
	virtual bool stop(
					const AosRundataPtr &rdata, 
					const AosRecordsetObjPtr &recordset){ return false;}
	
	virtual int64_t getTotalSize() const{return -1;}
	
	virtual void setTask(const AosTaskObjPtr &t){}

	virtual bool addDataConnector(const AosRundataPtr &rdata, 
	 				const AosDataConnectorObjPtr &data_cube){ return false;}
	
	virtual bool addUnrecogContents(
					const AosRundataPtr &rdata,
					const AosBuffDataPtr &buff_data,
					const char *data,
					const int64_t &start_pos,
					const int64_t &length) {return false;}
	
	virtual bool addInvalidContents(
					const AosRundataPtr &rdata,
					AosDataRecordObj * record) {return false;}
	
	virtual bool addFilterContents(
					const AosRundataPtr &rdata,
					AosDataRecordObj * record) { return false;}

	// Chen Ding, 2014/08/13
	virtual bool addContents(
					AosRundata *rdata, 
					AosBuff *data) { return false;}

	virtual bool addContents(
					AosRundata *rdata, 
					AosBuffArrayVar *data) { return false;}

	virtual bool getRecord(const OmnString &name, AosDataRecordObjPtr &record){return false;}
	
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records){return false;}
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const { return 0;}

	virtual bool getRsltInfor(
					AosXmlTagPtr &infor,
					const AosRundataPtr &rdata) {return false;}

	virtual bool data2Str(
				const AosRundataPtr &rdata,
				OmnString &contents) { return false; }

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
	virtual vector<AosDatasetObjPtr> split(AosRundata *rdata) { }

	// Jozhi, 2015/09/11, JIMODB-734
	virtual bool supportMatrix() const
	{
		return true;
	}

	virtual bool nextRecord(AosRundata* rdata, AosDataRecordObj *& record)
	{
		OmnShouldNeverComeHere;
		return false;
	}
	// jimodb-1375
	virtual void    setPageSize(u64 num) {};
};
#endif

