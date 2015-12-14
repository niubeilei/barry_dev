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
#include "SEInterfaces/DatasetObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataScannerObj.h"
#include "Debug/Debug.h"


AosDatasetObj::AosDatasetObj(const u32 version)
:
AosJimo(AosJimoType::eDataset, version)
{
}


AosDatasetObj::~AosDatasetObj()
{
}

	
AosXmlTagPtr
AosDatasetObj::getDatasetConfig(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &dataset_conf)
{
	aos_assert_r(dataset_conf, 0);

	OmnString name = dataset_conf->getAttrStr("zky_dataset_name");
	if (name == "") return dataset_conf;

	vector<OmnString> key_names;
	key_names.push_back("zky_dataset_name");
	key_names.push_back(AOSTAG_OTYPE);

	vector<OmnString> key_values;
	key_values.push_back(name);
	key_values.push_back("dataset");
	
	AosXmlTagPtr def = AosGetDocByKeys(rdata, "", key_names, key_values);
	if (!def)
	{
		//Linda 2014/05/09
		int num = 0;
		while (num < 10)
		{
			OmnSleep(1);

			def = AosGetDocByKeys(rdata, "", key_names, key_values);
			if (def) break;
			num++;
		}
	}

	if (!def)
	{
		AosSetErrorU(rdata, "failed_get_doc:") << name;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	bool rslt = def->removeAttr("zky_dataset_name");
	aos_assert_r(rslt, 0);

	return def;
}

bool 
AosDatasetObj::setData(AosRundata *rdata, const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}

////////////////////////////////////////////////////////
//
//				Document
//
////////////////////////////////////////////////////////
// virtual bool sendStart(const AosRundataPtr &rdata) = 0;
// Description:
//    This function starts the dataset. Most datasets need to
//    retrieve data. Call this function to start retrieving
//    data. Otherwise, this function does nothing.

// virtual bool sendFinish(const AosRundataPtr &rdata) = 0;
// Description:
//    This function informs the dataset that the retrieving
//    data is finished. Normally, this function is used when
//    a dataset uses a data connector to retrieve data and
//    the data connector detects that there are no more data
//    to retrieve. Normal users should not worry about this function.

// virtual bool reset(const AosRundataPtr &rdata) = 0;
// Description:
//    This function clears all the member data. This makes 
//    the dataset empty.

// virtual int64_t getNumEntries() = 0;
// Descriptoin:
//    This function retrieves the number of records the dataset
//    has.

// virtual int64_t getTotalSize() const = 0;
// Description:
//    This function returns the total size of the dataset. 

// virtual void setTask(const AosTaskObjPtr &task) = 0;
// Description:
// 	  If a dataset is used by a task (called Task Dataset),
//    this function sets the task. Most datasets do not need
//    to worry about this function.

// virtual AosRecordsetObjPtr getRecordset() = 0;
// Description:
// 	  Each dataset defines a AosRecordset. Callers should 
//    (normally) retrieve a set of records (called Recordset)
//    from a dataset instead of retrieving records one by one.
//    This function returns the recordset.

// virtual AosRecordsetObjPtr cloneRecordset() = 0;

// virtual vector<OmnString> getErrorRecords() = 0;
//	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc) = 0;

// virtual bool nextRecordset(
//					const AosRundataPtr &rdata, 
//					AosRecordsetObjPtr &recordset) = 0;
// Description:
//    This function retrieves the next recordset. This function
//    will create a new instance of AosRecordsetObj, retrieve
//    the next N records from the dataset and returns it,
//    where 'N' is configurable. If there are no more records 
//    in the dataset, 'recordset' is set to null. 

//	virtual bool addDataConnector(const AosRundataPtr &rdata, 
//	 				const AosDataCubeObjPtr &data_cube) = 0;
// Description:
//    This function 

/*
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

	virtual bool addContents(
					AosRundata *rdata, 
					AosBuff *buff) = 0;

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
	*/

