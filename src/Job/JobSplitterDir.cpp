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
// 2012/10/16 Created by Ken
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobSplitterDir.h"

#include "API/AosApi.h"
#include "Job/JobSplitterNames.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskDataType.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "TransUtil/RoundRobin.h"
#include "Debug/Debug.h"

AosJobSplitterDir::AosJobSplitterDir(const bool flag)
:
AosJobSplitter(AOSJOBSPLITTER_DIR, AosJobSplitterId::eDir, flag),
mSplitSize(eDftSplitSize),
mSplitFileNum(eDftSplitFileNum),
mRecursion(true),
mIgnoreHead(false),
mIsUnicomFile(false)
{
}


AosJobSplitterDir::~AosJobSplitterDir()
{
}


bool
AosJobSplitterDir::config(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);

	mSplitSize = sdoc->getAttrInt64(AOSTAG_SPLIT_SIZE, eDftSplitSize);
	if (mSplitSize < eMinSplitSize) mSplitSize = eMinSplitSize;
	if (mSplitSize > eMaxSplitSize) mSplitSize = eMaxSplitSize;

	mSplitFileNum = sdoc->getAttrInt64(AOSTAG_SPLIT_FILE_NUM, eDftSplitFileNum);
	if (mSplitFileNum < eMinSplitFileNum) mSplitFileNum = eMinSplitFileNum;
	if (mSplitFileNum > eMaxSplitFileNum) mSplitFileNum = eMaxSplitFileNum;

	mRecursion = sdoc->getAttrBool(AOSTAG_RECURSION, true);
	mIgnoreHead = sdoc->getAttrBool(AOSTAG_IGNORE_HEAD, false);

	mRowDelimiter = sdoc->getAttrStr(AOSTAG_ROW_DELIMITER);
	mCharacterType = sdoc->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	aos_assert_r(CodeConvertion::isValid(mCharacterType), false);

	mIsUnicomFile = sdoc->getAttrBool(AOSTAG_ISUNICOMFILE, false);
	mExtension = sdoc->getAttrStr(AOSTAG_EXTENSION);

	mSplitByDir = sdoc->getAttrBool("split_by_dir", false);

	return true;
}
	

bool
AosJobSplitterDir::splitTasks(
		const AosXmlTagPtr &def,
		const AosTaskDataObjPtr &task_data,
		vector<AosTaskDataObjPtr> &task_datas,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	aos_assert_r(task_data, false);

	//Jozhi 2013/09/14
	//task_datas.clear();
	AosTaskDataType::E type = task_data->getTaskDataType();
	if (type != AosTaskDataType::eDir && type != AosTaskDataType::eUnicomDir)
	{
		//task_datas.push_back(task_data);
		return true;
	}

	bool split_by_dir = def->getAttrBool("split_by_dir", mSplitByDir);

	int phy_num = AosGetNumPhysicals();
	int physical_id = def->getAttrInt(AOSTAG_PHYSICALID, -1);
	bool recursion = def->getAttrBool(AOSTAG_RECURSION, mRecursion);
	bool ignore_head = def->getAttrBool(AOSTAG_IGNORE_HEAD, mIgnoreHead);

	OmnString row_delimiter = def->getAttrStr(AOSTAG_ROW_DELIMITER, mRowDelimiter);
	OmnString character_type = def->getAttrStr(AOSTAG_CHARACTER, mCharacterType);
	aos_assert_r(CodeConvertion::isValid(character_type), false);

	vector<OmnString> dir_paths;
	OmnString dir_path = def->getNodeText();
	if (dir_path != "")
	{
		dir_paths.push_back(dir_path);
	}
	else
	{
		AosXmlTagPtr tag = def->getFirstChild(true);
		while (tag)
		{
			dir_path = tag->getNodeText();
			if (dir_path != "") dir_paths.push_back(dir_path);
			tag = def->getNextChild();
		}
	}
		
	if (dir_paths.size() == 0)
	{
		AosSetErrorU(rdata, "failed_retrieving_dirs");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (split_by_dir)
	{
		AosTaskDataObjPtr data;

		OmnTagFuncInfo << "start to split taskDataDir physical_id is: " 
			<< physical_id << endl;
		if (physical_id != -1)
		{
			data = AosTaskDataObj::createTaskDataDir(
				dir_paths, physical_id, recursion, ignore_head,
				character_type, row_delimiter);
			aos_assert_r(data, false);
			task_datas.push_back(data);
			return true;
		}
	
		for (int i = 0; i < phy_num; i++)
		{
			data = AosTaskDataObj::createTaskDataDir(
				dir_paths, i, recursion, ignore_head,
				character_type, row_delimiter);
			aos_assert_r(data, false);
			task_datas.push_back(data);
		}
		return true;
	}

	bool rslt = true;
	if (physical_id != -1)
	{
		rslt = split(dir_paths, physical_id, recursion,	ignore_head,
			character_type, row_delimiter, task_datas, rdata);
		if (!rslt)
		{
			//AosSetErrorU(rdata, "failed to split task_data: ");
			//OmnAlarm << rdata->getErrmsg() << enderr;
			AosSetEntityError(rdata, "JobSplitterDir_failed_to_split_task_data", "JobSplitter", "JobSplitterDir");
			return false;
		}
		return true;
	}
	
	for (int i = 0; i < phy_num; i++)
	{
		rslt = split(dir_paths, i, recursion, ignore_head,
			character_type, row_delimiter, task_datas, rdata);
		if (!rslt)
		{
			AosSetErrorU(rdata, "failed to split task_data: ");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	return true;
}


AosJobSplitterObjPtr
AosJobSplitterDir::create(
		const AosXmlTagPtr &sdoc,
		map<OmnString, OmnString> &job_env,
		const AosRundataPtr &rdata) const
{
	AosJobSplitterDir * splitter = OmnNew AosJobSplitterDir(false);
	splitter->setJobENV(job_env);
	bool rslt = splitter->config(sdoc, rdata);
	aos_assert_r(rslt, 0);

	OmnTagFuncInfo << endl;
	return splitter;
}


bool
AosJobSplitterDir::split(
		vector<OmnString> &dir_paths,
		const int physical_id,
		const bool recursion,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		vector<AosTaskDataObjPtr> &task_datas,
		const AosRundataPtr &rdata)
{
	bool rslt = true;
	int rec_level = recursion ? -1 : 0;
	vector<AosFileInfo> file_list;

	OmnTagFuncInfo << endl;
	for (u32 i=0; i<dir_paths.size(); i++)
	{
		if (mIsUnicomFile)
		{
			rslt = AosNetFileCltObj::getFileListByAssignExtStatic(
				mExtension, dir_paths[i], physical_id, rec_level, file_list, rdata.getPtr());
		}
		else
		{
			rslt = AosNetFileCltObj::getFileListStatic(
				dir_paths[i], physical_id, rec_level, file_list, rdata.getPtr());
		}
		aos_assert_r(rslt, false);
	}
	
	int64_t dev_id;
	map<int64_t, queue<AosFileInfo> > dev_map;
	for (u32 i=0; i<file_list.size(); i++)
	{
		//dev_id = file_list[i].mDevId;
		if (dev_map.find(dev_id) == dev_map.end())
		{
			queue<AosFileInfo> q;
			q.push(file_list[i]);
			dev_map[dev_id] = q;
		}
		else
		{
			dev_map[dev_id].push(file_list[i]);
		}
	}

	if (mIsUnicomFile)
	{
		rslt = splitUnicomFileByNum(
			task_datas, dev_map, physical_id, ignore_head,
			character_type, row_delimiter, rdata);
	}
	else
	{
		rslt = splitNormal(
			task_datas, dev_map, physical_id, ignore_head, 
			character_type, row_delimiter, rdata);
	}

	return rslt;
}

bool
AosJobSplitterDir::splitNormal(
		vector<AosTaskDataObjPtr> &task_datas,
		map<int64_t, queue<AosFileInfo> > &dev_map,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	int64_t file_len = 0;
	vector<AosFileInfo> fileinfos;
	AosTaskDataObjPtr data;

	map<int64_t, queue<AosFileInfo> >::iterator itr = dev_map.begin();
	map<int64_t, queue<AosFileInfo> >::iterator itr_2;
	OmnTagFuncInfo << endl;
	while (dev_map.size() != 0)
	{
		if (itr->second.size() > 0)
		{
			AosFileInfo info = itr->second.front();
			itr->second.pop();

			file_len += info.mFileLen;
			fileinfos.push_back(info);

			if (file_len >= mSplitSize 
				|| (int64_t)fileinfos.size() >= mSplitFileNum)
			{
				data = AosTaskDataObj::createTaskDataDir(
					fileinfos, physical_id, ignore_head,
					character_type, row_delimiter, false);
				aos_assert_r(data, false);

				task_datas.push_back(data);
	
				file_len = 0;
				fileinfos.clear();
			}
		}

		if (itr->second.size() == 0)
		{
			itr_2 = itr;
			itr++;
			dev_map.erase(itr_2);
		}
		else
		{
			itr++;
		}

		if (dev_map.size() == 0)
		{
			data = AosTaskDataObj::createTaskDataDir(
				fileinfos, physical_id, ignore_head,
				character_type, row_delimiter, false);
			aos_assert_r(data, false);
			task_datas.push_back(data);
			break;
		}

		if (itr == dev_map.end())
		{
			itr = dev_map.begin();
		}
	}
	return true;
}


bool 
AosJobSplitterDir::splitUnicomFileByNum(
		vector<AosTaskDataObjPtr> &task_datas,
		map<int64_t, queue<AosFileInfo> > &dev_map,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	vector<AosFileInfo> fileinfos;
	AosTaskDataObjPtr data;

	map<int64_t, queue<AosFileInfo> >::iterator itr = dev_map.begin();
	map<int64_t, queue<AosFileInfo> >::iterator itr_2;
	while (dev_map.size() != 0)
	{
		if (itr->second.size() > 0)
		{
			AosFileInfo info = itr->second.front();
			itr->second.pop();
			fileinfos.push_back(info);

			if ((int64_t)fileinfos.size() >= mSplitFileNum)
			{
				data = AosTaskDataObj::createTaskDataDir(
					fileinfos, physical_id, ignore_head,
					character_type, row_delimiter, mIsUnicomFile);
				aos_assert_r(data, false);

				task_datas.push_back(data);
	
				fileinfos.clear();
			}
		}

		if (itr->second.size() == 0)
		{
			itr_2 = itr;
			itr++;
			dev_map.erase(itr_2);
		}
		else
		{
			itr++;
		}

		if (dev_map.size() == 0)
		{
			data = AosTaskDataObj::createTaskDataDir(
				fileinfos, physical_id, ignore_head,
				character_type, row_delimiter, mIsUnicomFile);
			aos_assert_r(data, false);
			task_datas.push_back(data);
			break;
		}

		if (itr == dev_map.end())
		{
			itr = dev_map.begin();
		}
	}
	return true;
}

#endif
