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
#ifndef Aos_ImportData_ImportDataThrdNorm_h
#define Aos_ImportData_ImportDataThrdNorm_h

#include "alarm_c/alarm.h"
#include "ImportData/Ptrs.h"
#include "ImportData/ImportDataCtlr.h"
#include "ImportData/ImportDataThrd.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosImportDataThrdNorm : public AosImportDataThrd
{
	u64							mProcLen;
	u64							mProcTotal;
	
	AosDataRecordObjPtr			mDataRecord;
	AosDataRecordObjPtr			mOutputRecord;
	AosDataAssemblerObjPtr		mDocAssembler;
	vector<AosDataProcObjPtr>	mDataProcs;

public:
	AosImportDataThrdNorm(
			const AosDataCacherObjPtr &cacher,
			const AosImportDataCtlrPtr &controller,
			const AosRundataPtr &rdata);

	~AosImportDataThrdNorm();
	
	// OmnThreadedShellProc Interface
	virtual	bool			run();
	virtual bool 			procFinished();

	u64						getProcLen() const {return mProcLen;}
	u64						getProcTotal() {return mProcTotal;}

	static AosImportDataThrdPtr createImportDataThrd(
			const AosDataCacherObjPtr &cacher,
			const AosImportDataCtlrPtr &ctlr,
			const AosRundataPtr &rdata);

private:
	void					clear();
	bool					config(const AosRundataPtr &rdata);
	u64						getNextDocid(const AosRundataPtr &rdata);
	AosDataProcStatus::E	processRecord(
								u64 &docid,
								const AosRundataPtr &rdata);
};
#endif

