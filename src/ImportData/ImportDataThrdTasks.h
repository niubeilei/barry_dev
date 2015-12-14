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
#ifndef Aos_ImportData_ImportDataThrdTasks_h
#define Aos_ImportData_ImportDataThrdTasks_h

#include "alarm_c/alarm.h"
#include "ImportData/Ptrs.h"
#include "ImportData/ImportDataCtlr.h"
#include "ImportData/ImportDataCtlrTasks.h"
#include "ImportData/ImportDataThrd.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosImportDataThrdTasks : public AosImportDataThrd
{
	u64							mProcLen;

	int							mKeyIdx;
	int							mKeyLen;
	map<OmnString, procGroup>	mMap;
	map<OmnString, u64>			mProcTotal;

public:
	AosImportDataThrdTasks(
			const AosDataCacherObjPtr &cacher,
			const AosImportDataCtlrPtr &controller,
			const AosRundataPtr &rdata);

	~AosImportDataThrdTasks();
	
	// OmnThreadedShellProc Interface
	virtual	bool			run();
	virtual bool 			procFinished();

	u64						getProcLen() const {return mProcLen;}
	map<OmnString, u64> &	getProcTotal() {return mProcTotal;}

	static AosImportDataThrdPtr createImportDataThrd(
			const AosDataCacherObjPtr &cacher,
			const AosImportDataCtlrPtr &ctlr,
			const AosRundataPtr &rdata);

private:
	void					clear();
	bool					config(const AosRundataPtr &rdata);
	u64						getNextDocid(
								procGroup &group,
								const AosRundataPtr &rdata);
	AosDataProcStatus::E	processRecord(
								procGroup &group,
								u64 &docid,
								const AosRundataPtr &rdata);
};
#endif

