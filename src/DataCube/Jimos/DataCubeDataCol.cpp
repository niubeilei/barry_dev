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
// This data cube has a number of files. The files are listed through
// the config. All files belong to the same physical/virtual.
//
// Modification History:
// 2015/01/05 created by Andy zhang
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeDataCol.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "TaskMgr/Task.h"
#include "JobTrans/NotifyTaskMgrGetDataColOutput.h"
#include "JobTrans/GetFileListByDataCol.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeDataCol_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeDataCol(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosDataCubeDataCol::AosDataCubeDataCol(const u32 version)
:
AosDataCubeCtnr(AOS_DATACUBETYPE_DATACOLLECTOR, version)
{
}

AosDataCubeDataCol::~AosDataCubeDataCol()
{
}


bool
AosDataCubeDataCol::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata
	// 	<data_cube zky_datacolid="xxx" zky_jobid="xxxxx" >
	// 	</data_cube>
	
	aos_assert_rr(worker_doc, rdata, false);
	mMetadata = worker_doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(mMetadata, false);

	mReadBlockSize = mMetadata->getAttrInt64("read_block_size", eDftReadBlockSize);

	mColId = mMetadata->getAttrStr(AOSTAG_DATACOLLECTOR_ID, "");
	aos_assert_r(mColId != "", false);

	return true;
}


bool	
AosDataCubeDataCol::getFileInfo(const AosRundataPtr &rdata)
{
	aos_assert_r(mTaskDocid> 0, false);
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	u64 job_docid = task->getJobDocid();
	aos_assert_r(job_docid > 0, false);


	AosTransPtr trans;
	AosBuffPtr resp;                                        
	bool timeout = false;                                   
	trans = OmnNew AosNotifyTaskMgrGetDataColOutput(mColId, job_docid, mTaskDocid, AosGetSelfServerId());
	//trans = OmnNew AosGetFileListByDataCol(mColId, job_docid, AosGetSelfServerId(), false, false);
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);  

	aos_assert_r(rslt, false);                              
	aos_assert_r(!timeout, false);                          
	aos_assert_r(resp && resp->dataLen() >= 0, false);      

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);

	OmnString str = resp->getOmnStr("");
	aos_assert_r(str != "", false);

	AosXmlTagPtr files_conf = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs);
	aos_assert_r(files_conf, false);

	AosXmlTagPtr file_conf = files_conf->getNextChild();
	while (file_conf)
	{                                     
		AosFileInfo info;
		rslt = info.serializeFrom(file_conf);
		aos_assert_r(rslt, false);

		aos_assert_r(info.mFileId != 0, false);
		aos_assert_r(AosIsValidPhysicalIdNorm(info.mPhysicalId), false);

		AosDataCubeFilePtr cube_file = OmnNew AosDataCubeFile(info, mReadBlockSize, mMetadata);
		cube_file->setTaskDocid(mTaskDocid);
		addDataConnector(cube_file);
		file_conf = files_conf->getNextChild();
	}
	return true;
}


AosDataConnectorObjPtr 
AosDataCubeDataCol::cloneDataConnector()
{
	return OmnNew AosDataCubeDataCol(*this);
}


AosJimoPtr 
AosDataCubeDataCol::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeDataCol(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


void 
AosDataCubeDataCol::setTaskDocid(const u64 task_docid)
{
	mTaskDocid = task_docid;
	AosRundataPtr rdata = OmnApp::getRundata();
	bool rslt = getFileInfo(rdata);
	aos_assert(rslt);
}


