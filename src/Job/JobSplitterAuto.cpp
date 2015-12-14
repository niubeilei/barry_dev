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
// This splitter generates tasks purely based on the configurations, that is,
// tasks are not generated based on inputs (such as files). There are several
// generation methods:
// 1. Even Distribution over physical machines
//    Generate M tasks for each physical machine.
//
// 2. Even Distribution over virtual machines
//    Generate M tasks for each virtual machine.
//
// 3. Enumeration
//    The tasks to be generated will be listed in the configurations.
//
// Modification History:
// 08/29/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobSplitterAuto.h"

#include "API/AosApi.h"
#include "Job/JobSplitterNames.h"
#include "Rundata/Rundata.h"
#include "TransUtil/RoundRobin.h"
#include "SEInterfaces/ServerInfo.h"
#include "XmlUtil/XmlTag.h"

/*
AosJobSplitterAuto::AosJobSplitterAuto(const bool flag)
:
AosJobSplitter(AOSJOBSPLITTER_AUTO, AosJobSplitterId::eAuto, flag),
mLock(OmnNew OmnMutex())
{
}


AosJobSplitterAuto::~AosJobSplitterAuto()
{
}


bool
AosJobSplitterAuto::config(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, false);
	mSdoc = sdoc->clone(AosMemoryCheckerArgsBegin);
	OmnString str = sdoc->getAttrStr(AOSTAG_SPLIT_TYPE);
	mMethod = splitMethodStr2Enum(str);
	if (!isValidMethod(mMethod))
	{
		AosSetErrorU(rdata, "invalid_split_method") << ": " << sdoc->toString() << enderr;
		return false;
	}
	switch(mMethod)
	{
		case ePerPhysical:
		case ePerVirtual:
		case eEnumeration:
		default:
			configTasks(sdoc, rdata);
			break;
	}


	return true;
}


AosJobSplitterObjPtr
AosJobSplitterAuto::create(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata) const
{
	AosJobSplitterAuto * splitter = OmnNew AosJobSplitterAuto(false);
	bool rslt = splitter->config(sdoc, rdata);
	aos_assert_r(rslt, 0);
	return splitter;
}


bool
AosJobSplitterAuto::configTasks(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// The configuration should be:
	// 	<config> 
	// 		<tasks>
	// 			<task AOSTAG_TASK_SDOC_OBJID="xxx" AOSTAG_PHYSICALID="xxx" >filepath</task>
	// 			<task AOSTAG_TASK_SDOC_OBJID="xxx" AOSTAG_PHYSICALID="xxx" >filepath</task>
	// 			...
	// 		</tasks>
	//	</config>
	
	AosXmlTagPtr tasks = mSdoc->getFirstChild("tasks");
	aos_assert_rr(tasks, rdata, false);
	AosXmlTagPtr task = tasks->getFirstChild();
	while(task)
	{
		TaskInfo taskinfo;
		taskinfo.mFilename = task->getNodeText();
		if (taskinfo.mFilename == "")
		{
			AosSetErrorU(rdata, "missing_filename") << ": " << mSdoc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		taskinfo.mSdocObjid = task->getAttrStr(AOSTAG_TASK_SDOC_OBJID);
		taskinfo.mPhysicalId = task->getAttrInt(AOSTAG_PHYSICALID, -1);
		taskinfo.mTaskId = task->getAttrStr("zky_taskid", "");
		mTasksInfo.push_back(taskinfo);
		task = tasks->getNextChild();
	}

	if (mTasksInfo.size() < 0)
	{
		AosSetErrorU(rdata, "missing_files:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool
AosJobSplitterAuto::splitTasks(
		map<OmnString, AosTaskDataObjPtr> &tasks,
		const AosRundataPtr &rdata)
{
	tasks.clear();
	
	mLock->lock();
	
	int physical_id;
	OmnString task_id;
	OmnString sdoc_objid;
	OmnString filename;
	OmnString remote_fname;
	AosTaskDataObjPtr data;
	map<OmnString, AosTaskDataObjPtr>::iterator itr;
	for(u32 i = 0; i < mTasksInfo.size(); i++)
	{
		physical_id = mTasksInfo[i].mPhysicalId;
		sdoc_objid = mTasksInfo[i].mSdocObjid;
		task_id = mTasksInfo[i].mTaskId;
		filename = mTasksInfo[i].mFilename;

		itr = tasks.find(task_id);
		if (itr != tasks.end())
		{
			mLock->unlock();
			AosSetErrorU(rdata, "task_id is not unique: ") << task_id;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		
		if (filename == "")
		{
			mLock->unlock();
			AosSetErrorU(rdata, "failed_retrieving_file");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (sdoc_objid == "")
		{
			mLock->unlock();
			AosSetErrorU(rdata, "missing_task_sdoc_objid");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (physical_id == -1)
		{
			mLock->unlock();
			AosSetErrorU(rdata, "get physical id error");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		data = AosTaskDataObj::createTaskDataAuto(filename, sdoc_objid, physical_id);
		tasks[task_id] = data;
	}
	mLock->unlock();
	return true;
}

AosJobSplitterAuto::SplitMethod	
AosJobSplitterAuto::splitMethodStr2Enum(const OmnString &str)
{
	char c1 = str.data()[0];
	switch (c1)
	{
		case 'e':
			if(str == "enumeration") return eEnumeration;
		case 'p':
			if(str == "physical") return ePerVirtual;
		case 'v':
			if(str == "virtual") return eEnumeration;
		default:
			OmnAlarm <<  "missing split type!" << enderr;
			break;
	}
	return eInvalid; 
}
*/

#endif
