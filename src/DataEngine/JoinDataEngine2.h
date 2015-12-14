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
// 2014/03/14 Created by Linda 
// 2015/01/16 Copied by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataEngine_JoinDataEngine2_h
#define Aos_DataEngine_JoinDataEngine2_h

#include "Actions/TaskAction.h"
#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "JobTrans/NotifyTaskMgrCreateDataCol.h"


#include "Util/File.h"



class AosJoinDataEngine2 : public AosTaskAction 
{
	enum JoinType
	{
		eInvalid = 0,
		eLeftJoin = 1,
		eRightJoin = 2
	};

private:
	struct Input 
	{
		bool 				finished;
		AosRecordsetObjPtr 	record_set; 
		AosRecordsetObjPtr 	record_group; 
		AosDataRecordObj*	record;
		AosValueRslt		crt_key;
		AosBuffPtr			buff;

		Input()
		:
		finished(false),
		record_set(0),
		record_group(0),
		record(0),
		buff(0)
		{
		}

		bool hasMore() const
		{
			if (!record_set) return false;
			return !record_set->hasMore();
		}
	};

	struct DataEnginEnv
	{
	    AosRecordsetObjPtr l_recordset;
	    AosRecordsetObjPtr r_recordset;
	};

	enum Status
	{
		eIdle,
		eStart,
		eRunning,
		eRunFinished,
		eProcFailed,
		eFinished,
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


	OmnMutexPtr									mLock;
	AosDatasetObjPtr							mLhsDataset;
	AosDatasetObjPtr							mRhsDataset;


	vector<AosDataAssemblerObjPtr>  			mAssemblers;
	AosDataProcObjPtr							mProc;

	AosExprObjPtr								mLhsFields;
	AosExprObjPtr								mRhsFields;

	Status										mEngineStatus;	  
	u32											mStartTime;
	u32											mEndTime;
	StatisticsInfo								mStatistics;

	AosDataProcStatus::E 						mDataProcStatus;

	char* mDsData;
	int64_t mDsTotalLen;

	AosDataRecordObj** 							mInput_records;
	OmnString 									mModelType;

	//yang
	OmnFilePtr mLogLhsDataset;
	OmnFilePtr mLogRhsDataset;

	vector<AosExprObjPtr>                                           mKeyExpr;

public:
	AosJoinDataEngine2(const bool flag);
	~AosJoinDataEngine2();

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
	virtual bool clean();
	virtual AosActionObjPtr clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata) const ;     	

	bool    configDataCollector(                    
			const AosRundataPtr &rdata,             
			const AosXmlTagPtr &conf);              

	bool    configDataset(                          
			const AosTaskObjPtr &task,              
			const AosXmlTagPtr &conf,               
			const AosRundataPtr &rdata);            
													
	bool    configEngineProc(                       
			const AosRundataPtr &rdata,             
			const AosXmlTagPtr &conf);              

	bool    notifyAssemblerStart(const AosRundataPtr &rdata);     
	bool    notifyDatasetStart(const AosRundataPtr &rdata);       
	bool    notifyDataProcStart(const AosRundataPtr &rdata);      
																  
	bool    notifyAssemblerFinish(const AosRundataPtr &rdata);    
	bool    notifyDatasetFinish(const AosRundataPtr &rdata);      
	bool    notifyDataProcFinish(const AosRundataPtr &rdata);     

private:
	bool 	proc(AosRundata* rdata, void *data);
	bool	procSimple(AosRundata *rdata, void *data);

	bool	procData(
				Input &rhs_input,
				Input &lhs_input,
				AosRundata* rdata);
	bool	procDataSimple(
				Input &rhs_input,
				Input &lhs_input,
				AosRundata* rdata);
	bool	getNextRecord(
					AosRundata* rdata,
					Input &input,
					const AosDatasetObjPtr &dataset);
	bool	getRecordset(
					AosRundata* rdata,
					Input &input,
					const AosDatasetObjPtr &dataset,
					std::string type);
	bool    procFailed(const char *fname, const int line);
	bool    runFinished(const AosRundataPtr &rdata);
	AosDataProcObjPtr createDataProc(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &proc_conf);
	virtual int getMaxThreads() {return 1;}

	JoinType toEnum(const OmnString &str)
	{
		if (str == "left") return eLeftJoin;
		if (str == "right") return eRightJoin;
		return eInvalid;
	}
	bool    tryFinish();
	void 	showDataEngineInfo(
			const char *file,
			const int line,
			const OmnString &action);
};
#endif

