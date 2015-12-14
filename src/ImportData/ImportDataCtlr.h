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
// 03/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ImportData_ImportDataCtlr_h
#define Aos_ImportData_ImportDataCtlr_h

#include "DataAssembler/DataAssembler.h"
#include "DataAssembler/Ptrs.h"
#include "ImportData/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
#include <set>
using namespace std;


class AosImportDataCtlr : public OmnRCObject
{
	OmnDefineRCObject;
	
	class DataAsmFinishThrd : public OmnThrdShellProc
	{
		friend class AosImportDataCtlr;

		OmnDefineRCObject;

		AosImportDataCtlr * mCtlr;
		AosDataAssemblerObjPtr	mAsm;
		AosRundataPtr		mRundata;

	public:
		DataAsmFinishThrd(
			AosImportDataCtlr * ctlr,
			const AosDataAssemblerObjPtr &assm,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("DataAsmFinishThrd"),
		mCtlr(ctlr),
		mAsm(assm),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(); 
	};

protected:
	u32									mStartTime;
	u32									mEndTime;
	int									mProgress;
	int									mPrevProgress;
	bool								mIsFinished;
	bool								mAsmFinishError;
	
	OmnMutexPtr							mLock;
	OmnMutexPtr							mCondLock;
	OmnCondVarPtr						mCondVar;
	AosImportListenerPtr				mListener;
	AosTaskObjPtr						mTask;
	AosDataCacherObjPtr					mDataCacher;
	vector<AosImportDataThrdPtr>		mImportDataThrds;
	map<OmnString, AosDataAssemblerObjPtr>	mIILAssemblers;

public:
	AosImportDataCtlr(
			const AosXmlTagPtr &def, 
			const AosImportListenerPtr &listener,
			const AosTaskObjPtr &task,
			const AosRundataPtr &rdata);
	virtual ~AosImportDataCtlr();
	
	virtual bool start(const AosRundataPtr &rdata) = 0;
	virtual bool finish(
				const bool all_success,
				const OmnString &status_records,
				const AosRundataPtr &rdata) = 0;
	virtual bool thrdFinished(
				const AosImportDataThrdPtr &thrd,
				const AosRundataPtr &rdata) = 0;
	virtual bool setFileInfo(
				const AosTaskDataObjPtr &task_data,
				const AosRundataPtr &rdata) = 0;
	virtual bool updateTaskProgress(const AosRundataPtr &rdata) = 0;

	virtual bool setInputError();

	static AosXmlTagPtr resolveDataProcConfig(
				const AosXmlTagPtr &proc_def,
				const AosXmlTagPtr extension_config,
				const AosRundataPtr &rdata);

	static AosImportDataCtlrPtr createImportDataCtlrStatic(
				const AosXmlTagPtr &def, 
				const AosImportListenerPtr &listener,
				const AosTaskObjPtr &task,
				const AosRundataPtr &rdata);
	static bool	checkConfigStatic(
				const AosXmlTagPtr &def,
				const AosTaskObjPtr &task,
				const AosRundataPtr &rdata);

protected:
	bool	addThreadShellProcAsync(
				vector<OmnThrdShellProcPtr> &runners,
				const AosRundataPtr &rdata);
	bool	addThreadShellProcSync(
				vector<OmnThrdShellProcPtr> &runners,
				const AosRundataPtr &rdata);
	OmnThrdShellProcPtr createDataAsmFinishThrd(
				const AosDataAssemblerObjPtr &assm,
				const AosRundataPtr &rdata);

};
#endif

