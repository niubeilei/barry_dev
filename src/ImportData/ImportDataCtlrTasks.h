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
#ifndef Aos_ImportData_ImportDataCtlrTasks_h
#define Aos_ImportData_ImportDataCtlrTasks_h

#include "DataAssembler/Ptrs.h"
#include "ImportData/Ptrs.h"
#include "ImportData/ImportDataCtlr.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

#include <map>
using namespace std;


struct procGroup
{
	AosDataRecordObjPtr		mDataRecord;
	AosDataRecordObjPtr		mOutputRecord;
	AosDataAssemblerObjPtr	mDocAssembler;
	vector<AosDataProcObjPtr>	mDataProcs;
};


class AosImportDataCtlrTasks : public AosImportDataCtlr 
{
	u64							mTotalLen;
	bool						mNoUpdateTaskProcNum;
	int							mKeyIdx;
	int							mKeyLen;
	map<OmnString, procGroup>	mMap;

public:
	AosImportDataCtlrTasks(
			const AosXmlTagPtr &def, 
			const AosImportListenerPtr &listener,
			const AosTaskObjPtr &task,
			const AosRundataPtr &rdata);
	~AosImportDataCtlrTasks();
	
	int getKeyIdx() const {return mKeyIdx;}
	int getKeyLen() const {return mKeyLen;}
	map<OmnString, procGroup> & getProcGroup() {return mMap;}

	virtual bool start(const AosRundataPtr &rdata);
	virtual bool finish(
					const bool all_success,
					const OmnString &status_records,
					const AosRundataPtr &rdata);
	virtual bool thrdFinished(
					const AosImportDataThrdPtr &thrd,
					const AosRundataPtr &rdata);
	virtual bool setFileInfo(
					const AosTaskDataObjPtr &task_data,
					const AosRundataPtr &rdata);
	virtual bool updateTaskProgress(const AosRundataPtr &rdata);

	static	AosImportDataCtlrPtr createImportDataCtlr(
					const AosXmlTagPtr &def, 
					const AosImportListenerPtr &listener,
					const AosTaskObjPtr &task,
					const AosRundataPtr &rdata);
	static bool	checkConfig(
					const AosXmlTagPtr &def,
					const AosTaskObjPtr &task,
					const AosRundataPtr &rdata);

private:
	bool		config(
					const AosXmlTagPtr &task,
					const AosRundataPtr &rdata);
	bool		sendStart(const AosRundataPtr &rdata);
	bool		sendFinish(const AosRundataPtr &rdata);
	bool		splitDataCacher(const AosRundataPtr &rdata);
	bool		updateTaskProcNum(const AosRundataPtr &rdata);
	bool		getProcTotal(
					u64 &procLen,
					map<OmnString, u64> &procTotal,
					const AosRundataPtr &rdata);

};
#endif

