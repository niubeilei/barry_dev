////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/04/27 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskUtil/DataColCtlr.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataAssembler/DataAssembler.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "TaskMgr/TaskMgr.h"


AosDataColCtlr::AosDataColCtlr(
		const OmnString &datacol_id,
		const AosLogicTaskObjPtr &logic_task)
:
mLock(OmnNew OmnMutex()),
mDataColId(datacol_id),
mLogicTask(logic_task),
mPhysicalId(-1)
{
}


AosDataColCtlr::~AosDataColCtlr()
{
}


bool
AosDataColCtlr::setDataColTag(
		const AosXmlTagPtr &tag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(tag, false);
	AosXmlTagPtr datacol_tag = tag->getFirstChild("datacollector");
	aos_assert_r(datacol_tag, false);
	OmnString type = datacol_tag->getAttrStr(AOSTAG_ZKY_TYPE);
	aos_assert_r(type != "", false);

	if(type == "iil")
	{
		// Ketty 2014/01/03 maybe type == "doc"
		bool rslt = setIILDataColTag(rdata, datacol_tag);
		aos_assert_r(rslt, false);
	}

	mDataProcConf = tag->clone(AosMemoryCheckerArgsBegin);

	mDataColTag = datacol_tag->clone(AosMemoryCheckerArgsBegin);
	//mDataColTag = tag->clone(AosMemoryCheckerArgsBegin);
	
	// Ketty 2014/08/22
	mCubeId = tag->getAttrInt(AOSTAG_CUBE_ID, -1);
	return true;
}

	
bool
AosDataColCtlr::setDataColTag(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &data_proc_conf,
		const AosXmlTagPtr &data_col_conf)
{
	// Ketty 2014/08/21
	mDataProcConf = data_proc_conf->clone(AosMemoryCheckerArgsBegin);;
	mDataColTag = data_col_conf->clone(AosMemoryCheckerArgsBegin);;

	OmnString type = mDataColTag->getAttrStr(AOSTAG_ZKY_TYPE);
	aos_assert_r(type != "", false);

	if(type == "iil")
	{
		// Ketty 2014/01/03 maybe type == "doc"
		bool rslt = setIILDataColTag(rdata, mDataColTag);
		aos_assert_r(rslt, false);
	}

	// Ketty 2014/08/22
	mCubeId = mDataColTag->getAttrInt(AOSTAG_CUBE_ID, -1);
	return true;
}


bool
AosDataColCtlr::setIILDataColTag(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &tag)
{
	// Ketty 2014/01/03
	bool rslt = AosDataAssembler::checkIILAsmConfigStatic(tag, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	AosXmlTagPtr iilAsmConfig = tag->getFirstChild("asm");
	aos_assert_r(iilAsmConfig, 0);

	//OmnString iilname = iilAsmConfig->getAttrStr(AOSTAG_IILNAME);
	OmnString iilname = tag->getAttrStr(AOSTAG_NAME);
	aos_assert_r(iilname != "", false);

	if (AosIsSuperIIL(iilname))
	{
		int64_t total = AosIILClientObj::getIILClient()->getTotalNumDocs(iilname, rdata);
		if (total > 0 && !tag->getAttrBool(AOSTAG_IGNORE_PARA, false))
		{
			iilname = AosIILName::composeParallelIILName(iilname);
			iilAsmConfig->setAttr(AOSTAG_IILNAME, iilname);
		}
	}
	
	mPhysicalId = tag->getAttrInt("zky_physicalid", -1);
	if(mPhysicalId == -1)
	{
		vector<u32> svr_ids = AosGetServerIds();		
		aos_assert_r(svr_ids.size() > 0, false);
		mPhysicalId = rand() % svr_ids.size();
	}
	aos_assert_r(mPhysicalId >= 0, false);
	
	return true;
}

bool
AosDataColCtlr::cleanOutPut(
		const int level,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
OmnScreen << "=======ready to clean OutPut ,level: " << level << " , task_docid: " << task_docid << endl;
	list<AosXmlTagPtr>::iterator itr;
	list<AosXmlTagPtr>::iterator tmp_itr;
	u64 docid = 0;
	if ((u32)level >= mFileList.size())
	{
		return true;
	}
	list<AosXmlTagPtr> info_list = mFileList[level];
	itr = info_list.begin();
	while(itr != info_list.end())
	{
		docid = (*itr)->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
		aos_assert_r(docid != 0, false);
		if (docid == task_docid)
		{
OmnScreen << "=======real to clean OutPut ,level: " << level << " , task_docid: " << task_docid << endl;
			tmp_itr = itr;
			itr++;
			info_list.erase(tmp_itr);
		}
		else
		{
			itr++;
		}
	}
	return true;
}

bool
AosDataColCtlr::cleanOutPutDocFixed(
		const int server_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
OmnScreen << "=======ready to clean OutPutDocFixed  server_id : " << server_id  << ", task_docid: " << task_docid << endl;
	if ((u32)server_id >= mDocFixedFileList.size())
	{
		return true;
	}
	list<AosXmlTagPtr>::iterator itr;
	list<AosXmlTagPtr>::iterator tmp_itr;
	list<AosXmlTagPtr> info_list = mDocFixedFileList[server_id];
	itr = info_list.begin();
	u64 docid = 0;
	while(itr != info_list.end())
	{
		docid = (*itr)->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
		aos_assert_r(docid != 0, false);
		if (docid == task_docid)
		{
			OmnScreen << "=======real to clean OutPutDocFixed ,server_id: " << server_id << " , task_docid: " << task_docid << endl;
			tmp_itr = itr;
			itr++;
			info_list.erase(tmp_itr);
		}
		else
		{
			itr++;
		}
	}
	return true;
}

bool
AosDataColCtlr::cleanOutPutDocCSV(
		const int server_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
OmnScreen << "=======ready to clean OutPutCSV  server_id : " << server_id  << ", task_docid: " << task_docid << endl;
	if ((u32)server_id >= mDocCSVFileList.size())
	{
		return true;
	}
	list<AosXmlTagPtr>::iterator itr;
	list<AosXmlTagPtr>::iterator tmp_itr;
	list<AosXmlTagPtr> info_list = mDocCSVFileList[server_id];
	itr = info_list.begin();
	u64 docid = 0;
	while(itr != info_list.end())
	{
		docid = (*itr)->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
		aos_assert_r(docid != 0, false);
		if (docid == task_docid)
		{
			OmnScreen << "=======real to clean OutPutDocCSV ,server_id: " << server_id << " , task_docid: " << task_docid << endl;
			tmp_itr = itr;
			itr++;
			info_list.erase(tmp_itr);
		}
		else
		{
			itr++;
		}
	}
	return true;
}

	
bool
AosDataColCtlr::addOutPutDocFixed(
		const AosXmlTagPtr &xml, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	u64 fileid = xml->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(fileid, false);
	
	OmnTagFuncInfo << endl;
	int phy_num = AosGetNumPhysicals();

	int server_id = xml->getAttrInt(AOSTAG_SERVERID, -1);
	aos_assert_r(server_id >=0 && server_id < phy_num, false);
	
	//Jozhi 2013-11-01
	AosXmlTagPtr info = xml->clone(AosMemoryCheckerArgsBegin);
	//AosXmlTagPtr info = xml;

	mLock->lock();
	if ((int)mDocFixedFileList.size() < server_id + 1)
	{
		mDocFixedFileList.resize(server_id + 1);	
	}
	mDocFixedFileList[server_id].push_back(info);

	if (mDocFixedFileList[server_id].size() >= AosLogicTask::eMaxFilePreReduceTask)
	{
		list<AosXmlTagPtr> info_list;
		for (int i=0; i<AosLogicTask::eMaxFilePreReduceTask; i++)
		{
			info_list.push_back(mDocFixedFileList[server_id].front());
			mDocFixedFileList[server_id].pop_front();
		}
	
		OmnString action_type = AOSACTTYPE_IMPORTDOC_FIXED;

		bool rslt = mLogicTask->createImportDocTaskPriv(mDataProcConf, 
				server_id, info_list, action_type, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	return true;
}

bool
AosDataColCtlr::addOutPutDocCSV(
		const AosXmlTagPtr &xml, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	u64 fileid = xml->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(fileid, false);
	
	OmnTagFuncInfo << endl;
	int phy_num = AosGetNumPhysicals();

	int server_id = xml->getAttrInt(AOSTAG_SERVERID, -1);
	aos_assert_r(server_id >=0 && server_id < phy_num, false);
	
	//Jozhi 2013-11-01
	AosXmlTagPtr info = xml->clone(AosMemoryCheckerArgsBegin);
	//AosXmlTagPtr info = xml;

	mLock->lock();
	if ((int)mDocCSVFileList.size() < server_id + 1)
	{
		mDocCSVFileList.resize(server_id + 1);	
	}
	mDocCSVFileList[server_id].push_back(info);

	if (mDocCSVFileList[server_id].size() >= AosLogicTask::eMaxFilePreReduceTask)
	{
		list<AosXmlTagPtr> info_list;
		for (int i=0; i<AosLogicTask::eMaxFilePreReduceTask; i++)
		{
			info_list.push_back(mDocCSVFileList[server_id].front());
			mDocCSVFileList[server_id].pop_front();
		}
	
		OmnString action_type = AOSACTTYPE_IMPORTDOC_CSV;

		bool rslt = mLogicTask->createImportDocTaskPriv(mDataProcConf,
				server_id, info_list, action_type, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	return true;
}

bool
AosDataColCtlr::addOutPut(
		const AosXmlTagPtr &xml, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	OmnTagFuncInfo << "One info in mFileList is: " << xml->toString() << endl;
	u64 fileid = xml->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(fileid, false);

	int physical_id = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(physical_id >= 0, false);

	int level = xml->getAttrInt(AOSTAG_LEVEL, 0);

	//Jozhi 2013-11-01
	AosXmlTagPtr info = xml->clone(AosMemoryCheckerArgsBegin);
	//AosXmlTagPtr info = xml;

	mLock->lock();
	if ((int)mFileList.size() < level + 1)
	{
		mFileList.resize(level + 1);	
	}
	mFileList[level].push_back(info);

	if (mFileList[level].size() >= AosLogicTask::eMaxFilePreReduceTask
			&& mActiveTaskDocids.size() == 0)
	{
		list<AosXmlTagPtr> info_list;
		for (int i=0; i<AosLogicTask::eMaxFilePreReduceTask; i++)
		{
			info_list.push_back(mFileList[level].front());
			mFileList[level].pop_front();
		}

		u64 task_docid = 0;
		physical_id = rand() % AosGetNumPhysicals();

		AosXmlTagPtr tag = mDataColTag->clone(AosMemoryCheckerArgsBegin);
		aos_assert_rl(tag, mLock, false);

		mLogicTask->createMergeFileTask(
			tag, physical_id, info_list, task_docid, rdata);
		mActiveTaskDocids.insert(task_docid);
	}
	mLock->unlock();
	return true;
}


bool 
AosDataColCtlr::mergeAllDocFiles(const AosRundataPtr &rdata)
{
	mLock->lock();
	int size;
	bool rslt;
	AosXmlTagPtr info;
	OmnString action_type = AOSACTTYPE_IMPORTDOC_FIXED;
	for(u32 i=0; i<mDocFixedFileList.size(); i++)
	{
		size = mDocFixedFileList[i].size();
		if (size == 0) continue;
		list<AosXmlTagPtr> info_list;
		int ss = 0;
		for(int k=0; k<size; k++)
		{
			info = mDocFixedFileList[i].front();
			info_list.push_back(info);
			mDocFixedFileList[i].pop_front();
			ss++;
			
			if (ss >= AosLogicTask::eMaxFilePreReduceTask || k+1 == size)
			{
				if (!isService(rdata))
				{
					rslt = mLogicTask->createImportDocTaskPriv(mDataProcConf, i, 
							info_list, action_type, rdata);
					aos_assert_rl(rslt, mLock, false);
				}
				else
				{
					OmnTagFuncInfo << endl;
					OmnNotImplementedYet;
					//mLogicTask->sendDataToReduce();
				}
					
				ss = 0;
				info_list.clear();
			}
		}
	}
	
	action_type = AOSACTTYPE_IMPORTDOC_CSV;
	for(u32 i=0; i<mDocCSVFileList.size(); i++)
	{
		size = mDocCSVFileList[i].size();
		if (size == 0) continue;
		list<AosXmlTagPtr> info_list;
		int ss = 0;
		for(int k=0; k<size; k++)
		{
			info = mDocCSVFileList[i].front();
			info_list.push_back(info);
			mDocCSVFileList[i].pop_front();
			ss++;
			
			if (ss >= AosLogicTask::eMaxFilePreReduceTask || k+1 == size)
			{
				if (!isService(rdata))
				{
					rslt = mLogicTask->createImportDocTaskPriv(mDataProcConf, i, 
							info_list, action_type, rdata);
					aos_assert_rl(rslt, mLock, false);
				}
				else
				{
					OmnTagFuncInfo << endl;
					OmnNotImplementedYet;
					//mLogicTask->sendDataToReduce();
				}
					
				ss = 0;
				info_list.clear();
			}
		}
	}
	mLock->unlock();
	return true;
}

bool
AosDataColCtlr::mergeAllFiles(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mActiveTaskDocids.size() > 0)
	{

		mLock->unlock();
		return true;
	}



	AosXmlTagPtr info;
	for (u32 level=0; level<mFileList.size(); level++)
	{
		int size = mFileList[level].size();
		if (size == 0) continue;
		if (size != 1)
		{
			list<AosXmlTagPtr> info_list;
			int ss = 0;
			for (int i=0; i<size; i++)
			{
				info = mFileList[level].front();
				info_list.push_back(info);
				mFileList[level].pop_front();
				ss++;

				if (ss >= AosLogicTask::eMaxFilePreReduceTask || i + 1 == size)
				{
					int physical_id = rand() % AosGetNumPhysicals();
					if (level + 1 == mFileList.size() && ss == size)
					{
						physical_id = mPhysicalId;
					}

					u64 task_docid = 0;

					AosXmlTagPtr tag = mDataColTag->clone(AosMemoryCheckerArgsBegin);
					aos_assert_rl(tag, mLock, false);

					if (!isService(rdata))
					{
						mLogicTask->createMergeFileTask(
							tag, physical_id, info_list, task_docid, rdata);
						mActiveTaskDocids.insert(task_docid);
					} 
					else
					{
						OmnTagFuncInfo << endl;
						OmnNotImplementedYet;
						//mLogicTask->sendDataToReduce();
					}

					ss = 0;
					info_list.clear();
				}
			}
			mLock->unlock();
			return true;
		}

		info = mFileList[level].front();
		mFileList[level].pop_front();

		if (level + 1 == mFileList.size())
		{
			mLock->unlock();
			return mergeLastFile(info, rdata);
		}

		info->setAttr(AOSTAG_LEVEL, level + 1);
		mFileList[level + 1].push_back(info);
	}
	mLock->unlock();

	//if (mFileList.empty())
	//{
	//	mergeLastFile(0, rdata);
	//}

	return true;
}


bool
AosDataColCtlr::mergeLastFile(
		const AosXmlTagPtr &info,
		const AosRundataPtr &rdata)
{
	if (!info)
	{
		mLogicTask->createMergeFinishTask(
			mDataProcConf, mPhysicalId, info, rdata);
		return true;
	}


	AosXmlTagPtr iiltag = mDataColTag->getFirstChild("asm");
	aos_assert_r(iiltag, false);
	int len = iiltag->getAttrInt(AOSTAG_LENGTH, -1);
	if(len == -1)
	{
		// Ketty 2014/01/10	 new version not has length. need get from output_record.
		//AosXmlTagPtr output_xml = iiltag->getFirstChild(AOSTAG_RECORD);
		AosXmlTagPtr output_xml = iiltag->getFirstChild("datarecord");
		aos_assert_r(output_xml, false);
	
		AosDataRecordObjPtr rcd = AosDataRecordObj::createDataRecordStatic(
				output_xml, 0, rdata.getPtr());
		len = rcd->getEstimateRecordLen();	
	}
	aos_assert_r(len > 0, false);
	
	//OmnString iilname = iiltag->getAttrStr(AOSTAG_IILNAME);
	OmnString iilname = mDataColTag->getAttrStr(AOSTAG_NAME);
	aos_assert_r(iilname != "", false);
	
	//mLogicTask->createSnapShotTask(
	//	iilname, mPhysicalId, AosModuleId::eIIL, rdata);

	u64 file_id = info->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	int physical_id = info->getAttrInt(AOSTAG_PHYSICALID, -1);

	AosDiskStat disk_stat;
	int64_t file_len;
	bool rslt = AosNetFileCltObj::getFileLengthStatic(
		file_id, physical_id, file_len, disk_stat, rdata.getPtr());
	aos_assert_r(rslt, rdata);
	aos_assert_r(file_len > 0, rdata);

	AosXmlTagPtr xml;
	int64_t start_pos = 0;
	//int64_t size;
	int64_t max_size = AosLogicTask::eMaxFileLenPreReduceTask;
	max_size -= (max_size % len);

	xml = info->clone(AosMemoryCheckerArgsBegin);
	xml->setAttr(AOSTAG_START_POS, start_pos);
	xml->setAttr(AOSTAG_LENGTH, file_len);
	xml->setAttr(AOSTAG_CUBE_ID, mCubeId);
	mLogicTask->createMergeFinishTask(
		mDataProcConf, mPhysicalId, xml, rdata);

	/*
	while(start_pos < file_len)
	{
		size = file_len - start_pos;
		if (size > max_size) size = max_size;
		
		//Jozhi 2013-11-01
		xml = info->clone(AosMemoryCheckerArgsBegin);
		//xml = info;
		xml->setAttr(AOSTAG_START_POS, start_pos);
		xml->setAttr(AOSTAG_LENGTH, size);
		start_pos += size;

		mLogicTask->createMergeFinishTask(
			mDataColTag, mPhysicalId, xml, rdata);
	}
	*/
	
	//mLogicTask->commitSnapShotTask(
	//	iilname, mPhysicalId, AosModuleId::eIIL, info, rdata);
	return true;	
}

	
int
AosDataColCtlr::getActiveTaskDocidNum() const
{
	mLock->lock();
	int size = mActiveTaskDocids.size();
	mLock->unlock();
	return size;
}


void
AosDataColCtlr::insertActiveTaskDocid(const u64 &task_docid)
{
	mLock->lock();
	mActiveTaskDocids.insert(task_docid);
	mLock->unlock();
}


void
AosDataColCtlr::eraseActiveTaskDocid(const u64 &task_docid)
{
	mLock->lock();
	if (mActiveTaskDocids.find(task_docid) != mActiveTaskDocids.end())
	{
		mActiveTaskDocids.erase(task_docid);
	}
	mLock->unlock();
}


// Ketty Need? 2014/09/28
/*
void
AosDataColCtlr::calGroupOutputNum()
{
	//Linda, 2014/07/08
	int svr_nums = AosGetNumPhysicals();
	OmnString group_type = mDataColTag->getAttrStr("group_type", "");
	if (group_type == "physical_group" && svr_nums > 1)
	{
		mLogicTask->calGroupDataColOutputNum(mDataColTag);
	}

	// Ketty 2014/08/14
	int cube_nums = AosGetNumCubes();
	if((group_type == "cube_shuffle" || group_type == "cube_assemble") 
		|| cube_num > 1)
	{
		mLogicTask->calGroupDataColOutputNum(mDataColTag);
	}
}
*/

bool
AosDataColCtlr::isService(AosRundataPtr rdata)
{
	u64 jobDocid = mLogicTask->getJobDocId();

	return AosTaskMgr::isService(rdata, jobDocid);
}

#endif
