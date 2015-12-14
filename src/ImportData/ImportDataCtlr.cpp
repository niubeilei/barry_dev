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
#include "ImportData/ImportDataCtlr.h"

#include "API/AosApi.h"
#include "DataAssembler/Ptrs.h"
#include "ImportData/ImportListener.h"
#include "ImportData/ImportDataCtlrNorm.h"
#include "ImportData/ImportDataCtlrTasks.h"
#include "ImportData/ImportDataThrd.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataCacherCreatorObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEUtil/ValueDefs.h"
#include "Thread/ThreadPool.h"
#include "Util/Ptrs.h"
#include "Util/ReadFile.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("importdata", __FILE__, __LINE__);


AosImportDataCtlr::AosImportDataCtlr(
		const AosXmlTagPtr &def, 
		const AosImportListenerPtr &listener,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
:
mStartTime(0),
mEndTime(0),
mProgress(0),
mPrevProgress(0),
mIsFinished(false),
mAsmFinishError(false),
mLock(OmnNew OmnMutex()),
mCondLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mListener(listener),
mTask(task)
{
}


AosImportDataCtlr::~AosImportDataCtlr()
{
	OmnScreen << "import dat ctlr is deleted" << endl;
}


AosImportDataCtlrPtr
AosImportDataCtlr::createImportDataCtlrStatic(
		const AosXmlTagPtr &def, 
		const AosImportListenerPtr &listener,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, 0);
	OmnString type = def->getAttrStr(AOSTAG_ZKY_TYPE);
	if (type == "norm")
	{
		return AosImportDataCtlrNorm::createImportDataCtlr(
					def, listener, task, rdata);
	}
	if (type == "tasks")
	{
		return AosImportDataCtlrTasks::createImportDataCtlr(
					def, listener, task, rdata);
	}
	OmnAlarm << "failed to create import data ctlr" << enderr;
	return 0;
}
	

bool
AosImportDataCtlr::checkConfigStatic(
		const AosXmlTagPtr &def, 
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	OmnString type = def->getAttrStr(AOSTAG_ZKY_TYPE);
	if (type == "norm")
	{
		return AosImportDataCtlrNorm::checkConfig(def, task, rdata);
	}
	if (type == "tasks")
	{
		return AosImportDataCtlrTasks::checkConfig(def, task, rdata);
	}
	OmnAlarm << "failed to check import data cfg" << enderr;
	return false;
}


AosXmlTagPtr
AosImportDataCtlr::resolveDataProcConfig(
		const AosXmlTagPtr &proc_def,
		const AosXmlTagPtr extension_config,
		const AosRundataPtr &rdata)
{
	aos_assert_r(proc_def, 0);

	OmnString config_id = proc_def->getAttrStr(AOSTAG_CONFIG_ID);
	if(config_id == "") return proc_def;
		
	if(!extension_config)
	{
		AosSetErrorU(rdata, "missing extension config") << proc_def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosXmlTagPtr extension_tag = extension_config->getFirstChild(true);
	while(extension_tag)
	{
		if(config_id == extension_tag->getAttrStr(AOSTAG_CONFIG_ID))
		{
			return extension_tag;
		}
		extension_tag = extension_config->getNextChild();
	}
		
	AosSetErrorU(rdata, "missing proc_tag") << proc_def->toString();
	OmnAlarm << rdata->getErrmsg() << enderr;
	return 0;
}


bool
AosImportDataCtlr::addThreadShellProcAsync(
		vector<OmnThrdShellProcPtr> &runners,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->procAsync(runners);
}


bool
AosImportDataCtlr::addThreadShellProcSync(
		vector<OmnThrdShellProcPtr> &runners,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->procSync(runners);
}
	

OmnThrdShellProcPtr
AosImportDataCtlr::createDataAsmFinishThrd(
		const AosDataAssemblerObjPtr &assm,
		const AosRundataPtr &rdata)
{
	OmnThrdShellProcPtr runner = OmnNew DataAsmFinishThrd(this, assm, rdata);
	return runner;
}


bool
AosImportDataCtlr::DataAsmFinishThrd::run()
{
	if (!mAsm)
	{
		mCtlr->mAsmFinishError = true;
		return true;
	}
		
	mRundata->setOk();
	bool rslt = mAsm->sendFinish(mRundata.getPtrNoLock());
	if (!rslt)
	{
		mCtlr->mAsmFinishError = true;
		return true;
	}
		
	mAsm->setTask(0);
	return true;
}


bool
AosImportDataCtlr::DataAsmFinishThrd::procFinished()
{
	return true;
}

bool
AosImportDataCtlr::setInputError()
{
OmnScreen << "=======================set input error: " << endl;
	aos_assert_r(mTask, false);
	mTask->setErrorType(AosTaskErrorType::eInPutError);
	return true;
}
