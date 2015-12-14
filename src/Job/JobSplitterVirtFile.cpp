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
// Modification History:
// 08/21/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobSplitterVirtFile.h"

#include "API/AosApi.h"
#include "Job/JobSplitterNames.h"
#include "Rundata/Rundata.h"
#include "TransUtil/RoundRobin.h"
#include "SEInterfaces/ServerInfo.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

/*
AosJobSplitterVirtFile::AosJobSplitterVirtFile(const bool flag)
:
AosJobSplitter(AOSJOBSPLITTER_VIRTUAL_FILE, AosJobSplitterId::eVirtualFile, flag),
mLock(OmnNew OmnMutex())
{
}


AosJobSplitterVirtFile::~AosJobSplitterVirtFile()
{
}


bool
AosJobSplitterVirtFile::config(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//<split zky_jobsplitterid="virfile">
	//	<virfile AOSTAG_TASK_SDOC_OBJID="xxxx" zky_vf_objid="xxxx" />
	//	<virfile AOSTAG_TASK_SDOC_OBJID="xxxx" zky_vf_objid="xxxx"/>
	//	...
	//</split>
	aos_assert_rr(sdoc, rdata, false);
	AosXmlTagPtr vf_tag = sdoc->getFirstChild();
	while(vf_tag)
	{
		FileInfo info;
		OmnString objid = vf_tag->getAttrStr("zky_vf_objid", "");
		if (objid == "")
		{
			AosSetErrorU(rdata, "virtual_file_objid_is_null") << enderr;
			return false;
		}
		info.mVirFileObjid = objid;
		OmnString sobjid = vf_tag->getAttrStr(AOSTAG_TASK_SDOC_OBJID, "");
		if (sobjid == "")
		{
			AosSetErrorU(rdata, "task_smartdoc_objid_is_null") << enderr;
			return false;
		}
		info.mSdocObjid = sobjid;
		OmnString taskid = vf_tag->getAttrStr("zky_taskid", "");
		if (taskid == "")
		{
			AosSetErrorU(rdata, "task_id_is_null") << enderr;
			return false;
		}
		info.mTaskId = taskid;
		mFileInfos.push_back(info);
		vf_tag = sdoc->getNextChild();
	}
	return true;
}


bool
AosJobSplitterVirtFile::splitTasks(
		map<OmnString, AosTaskDataObjPtr> &tasks, 
		const AosRundataPtr &rdata)
{
	tasks.clear();
	OmnString task_id;
	OmnString sdoc_objid;
	OmnString virfile_objid;
	AosTaskDataObjPtr data;
	for(u32 i=0; i<mFileInfos.size(); i++)
	{
		virfile_objid = mFileInfos[i].mVirFileObjid;
		sdoc_objid = mFileInfos[i].mSdocObjid;	
		task_id = mFileInfos[i].mTaskId;
		int physical_id = -1;
		AosVirtualFile vf(virfile_objid, rdata);
		vector<u64> segids = vf.getAllSegment(rdata);
		//base on virtual file to get the file distribute physicalids
		for (u32 j=0; j<segids.size(); j++)
		{
			//Jozhi 2013/03/22
			u64 dist_id = AosXmlDoc::getOwnDocid(segids[j]);
			u32 vir_num = AosGetNumCubes();
			aos_assert_r(vir_num != 0 && dist_id != 0, false);
			int cub_id = dist_id % vir_num;
			physical_id = AosGetMainSvrId(cub_id);
			//physical_id = AosGetPhysicalIdByDocid(segids[j]);
			if (physical_id < 0)
			{
				AosSetErrorU(rdata, "physical id error");
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}

			task_id << "_" << physical_id;
			if (tasks.find(task_id) == tasks.end())
			{
				data = AosTaskDataObj::createTaskDataVirtualFile(sdoc_objid, virfile_objid, physical_id);
				bool rslt = data->isDataExist(rdata);
				if (!rslt)
				{
					AosSetErrorU(rdata, "virtual file is not exist: ") << virfile_objid;
					OmnAlarm << rdata->getErrmsg() << enderr;
					return false;
				}

				tasks[task_id] = data;
			}
		}
	}
	return true;
}


AosJobSplitterObjPtr
AosJobSplitterVirtFile::create(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) const
{
	AosJobSplitterVirtFile * splitter = OmnNew AosJobSplitterVirtFile(false);
	bool rslt = splitter->config(sdoc, rdata);
	aos_assert_r(rslt, 0);
	return splitter;
}
*/

#endif
