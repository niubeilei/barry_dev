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
//
// Modification History:
// 2013/12/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataEngine_ScanDataEngine2_h
#define Aos_DataEngine_ScanDataEngine2_h

#include "Actions/TaskAction.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/Ptrs.h"
#include "JobTrans/NotifyTaskMgrCreateDataCol.h"

class AosScanDataEngine2 : public AosTaskAction 
{
private:
	enum Status
	{
		eIdle,
		eStart,
		eRunning,
		eRunFinished,
		eProcFailed,
		eFinished,
		eStop,     		//barry 2015/12/13
	};

	struct StatisticsInfo
	{
		u64		mProcNum;
		u64		mFilterNum;
		u64		mDataErrorNum;
		u64		mProcErrorNum;
		u64	    mEntries;
		
		void clear()
		{
			mProcNum = 0;
			mFilterNum = 0;
			mDataErrorNum = 0;
			mProcErrorNum = 0;
			mEntries = 0;
		}
	};

	struct DataEnginEnv
	{
		AosRecordsetObjPtr recordset;
	};

	OmnMutexPtr						mLock;
	Status							mEngineStatus;
	u32								mStartTime;
	u32								mEndTime;
	StatisticsInfo					mStatistics;
	AosDatasetObjPtr				mDataset;

	map<OmnString, vector< pair<AosDataProcObjPtr, vector<AosDataProcObjPtr> > > >	mProcGroups;
	map<OmnString, vector< pair<AosDataProcObj*, vector<AosDataProcObj*> > > >	mRawProcGroups;

protected:
	vector<AosDataAssemblerObjPtr> 	mAssemblers;
	vector<AosDataAssemblerObj*> 	mRawAssemblers;

public:
	AosScanDataEngine2(const bool flag);
	~AosScanDataEngine2();

public:
	virtual void onThreadInit(const AosRundataPtr &, void **data);
	virtual void onThreadExit(const AosRundataPtr &, void *data);

	virtual bool config(const u64 task_docid,      
			const AosXmlTagPtr &conf,               
			const AosRundataPtr &rdata);            

	virtual bool initAction(                                         
			const AosTaskObjPtr &task,                           
			const AosXmlTagPtr &sdoc,                            
			const AosRundataPtr &rdata);                         
	virtual bool runAction(const AosRundataPtr &rdata, void *data);              
	virtual bool finishedAction(const AosRundataPtr &rdata);         
	// AosTaskAction Interfaces finish.                              
	OmnString   reportStatus();                                     
	virtual bool reset(const AosRundataPtr &rdata); 

	virtual AosActionObjPtr clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata) const ;

private:
	bool 	configDataCollector(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf);

	bool 	configDataset(
			const AosTaskObjPtr &task,
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata);

	bool 	configEngineProc(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf);

	AosScanDataEngine2::Status 	procRecordSet(
									AosRundata *rdata_raw,
									AosRecordsetObj *record_set_raw,
									void *data);

	AosScanDataEngine2::Status 	processRecord(
									AosRundata *rdata_raw,
									AosDataRecordObj *input_record,
									void* data);

	AosDataProcObjPtr createDataProc(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &proc_conf);

	virtual int getMaxThreads(); 

	bool 	notifyAssemblerStart(const AosRundataPtr &rdata);
	bool 	notifyDatasetStart(const AosRundataPtr &rdata);
	bool	notifyDataProcStart(const AosRundataPtr &rdata);

	bool 	notifyAssemblerFinish(const AosRundataPtr &rdata);
	bool 	notifyDatasetFinish(const AosRundataPtr &rdata);
	bool 	notifyDataProcFinish(const AosRundataPtr &rdata);

	bool 	tryFinish();
	bool	clean();
	bool 	procFailed(const char *fname, const int line);
	bool 	runFinished(const AosRundataPtr &rdata);
	bool 	stop(const AosRundataPtr &rdata, AosRecordsetObjPtr &record_set);


	void 	logProcEntry(const u64 num);
	void 	logProcOne(
			AosRundata * rdata_raw,
			AosDataRecordObj *input_record);

	void 	logFiltered(
			AosRundata * rdata_raw,
			AosDataRecordObj *input_record);

	void 	logDataError(
			AosRundata * rdata_raw,
			AosDataRecordObj *input_record);

	void 	logProcError(
			AosRundata * rdata_raw,
			AosDataRecordObj *input_record);

	void 	showDataEngineInfo(
			const char *file,
			const int line,
			const OmnString &action);

};
#endif
