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
// This job splitter splits jobs based on IIL size. The input is an IIL.
// It creates one task for each N number of entries in the IIL.
//
// mFileName:	
// mRecordSize: If the file is fixed length records, this is the record length.
//
// Modification History:
// 05/14/2012 Created by Ketty 
// 06/02/2012 Job Working Directory
//     Each machine has a working directory:
//     		basedir/working/machine
//     Each job has a directory under its working directory:
//     		basedir/working/machine/job_<jobdocid>
//     Since job docids are unique, the directory name "job_<jobdocid>" is
//     guaranteed unique. This is called Job Working Directory.
//     When starting a job, the system will create a job doc for it. 
//     It then creates the working directory. 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobSplitterFile.h"

#include "API/AosApi.h"
#include "Job/JobSplitterNames.h"
#include "Rundata/Rundata.h"
#include "TransUtil/RoundRobin.h"
#include "SEInterfaces/ServerInfo.h"
#include "SEInterfaces/TaskDataType.h"
#include "SEInterfaces/TaskDataObj.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"

AosJobSplitterFile::AosJobSplitterFile(const bool flag)
:
AosJobSplitter(AOSJOBSPLITTER_FILE, AosJobSplitterId::eFile, flag)
{
}


AosJobSplitterFile::~AosJobSplitterFile()
{
}


bool
AosJobSplitterFile::config(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	return true;
}


bool
AosJobSplitterFile::splitTasks(
		const AosXmlTagPtr &def,
		const AosTaskDataObjPtr &task_data,
		vector<AosTaskDataObjPtr> &task_datas,
		const AosRundataPtr &rdata)
{
	//<splitter zky_jobsplitterid="file">
	//	<files>
	//		<task_data zky_physicalid="0">xxxx</task_data>
	//		...
	//	</files>
	//</splitter>

	aos_assert_r(def, false);	
	aos_assert_r(task_data, false);
	AosTaskDataType::E type = task_data->getTaskDataType();
	//Jozhi 2013/09/14
	if (type != AosTaskDataType::eFile)
	{
		//task_datas.clear();
		//task_datas.push_back(task_data);
		return true;
	}
	return true;
}


AosJobSplitterObjPtr
AosJobSplitterFile::create(
		const AosXmlTagPtr &sdoc,
		map<OmnString, OmnString> &job_env,
		const AosRundataPtr &rdata) const
{

	OmnTagFuncInfo << endl;
	AosJobSplitterFile * splitter = OmnNew AosJobSplitterFile(false);
	splitter->setJobENV(job_env);
	bool rslt = splitter->config(sdoc, rdata);
	aos_assert_r(rslt, 0);
	return splitter;
}

#endif
