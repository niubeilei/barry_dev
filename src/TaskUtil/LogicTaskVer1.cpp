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
// 07/27/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskUtil/LogicTaskVer1.h"

#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "TaskUtil/Recycle.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskUtil.h"
#include "TaskMgr/TaskLogKeys.h"
#include "Job/JobLogKeys.h"
#include "Job/JobMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskDataType.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/TaskMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/SeXmlParser.h"

#include "TaskUtil/MapTaskCreator.h"
#include "Debug/Debug.h"

static bool mShowLog = true;

AosLogicTaskVer1::AosLogicTaskVer1(
		const u64 &job_docid)
:
AosLogicTask(AosLogicTaskType::eVer1, job_docid),
mFinishedMapTaskNum(0),
mCalcuationSize(0)
{
}

AosLogicTaskVer1::~AosLogicTaskVer1()
{
	if (mShowLog) OmnScreen << "erase logic id: " << mLogicId << endl;
	mDataColCtlr.clear();
}


bool
AosLogicTaskVer1::config(
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	aos_assert_r(conf, false);
	mLogicId = conf->getAttrStr(AOSTAG_NAME);
	aos_assert_r(mLogicId != "", false);
	OmnString name = conf->getAttrStr("zky_task_name");
	if (name != "")
	{
		mLogicTaskSdoc = AosGetDocByKey(rdata, "", "zky_task_name", name);
	}
	else
	{
		mLogicTaskSdoc = conf->clone(AosMemoryCheckerArgsBegin);
	}
	bool rslt = configDataCols(rdata);
	aos_assert_r(rslt, false);
	rslt = setReduceMap(rdata);	
	aos_assert_r(rslt, false);
	return true;
}


bool
AosLogicTaskVer1::configDataCols(const AosRundataPtr &rdata)
{
	aos_assert_r(mLogicTaskSdoc, false);
	bool rslt;
	
	AosXmlTagPtr task_doc = mLogicTaskSdoc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(task_doc, false);

	AosXmlTagPtr reduce_task_conf = task_doc->getFirstChild("reduce_task");
	aos_assert_r(reduce_task_conf, false);

	AosXmlTagPtr actions_conf = reduce_task_conf->getFirstChild("actions");
	aos_assert_r(actions_conf, false);

	AosXmlTagPtr datacol_conf;
	OmnString datacol_id, grp_type;

	AosXmlTagPtr dataproc_conf = actions_conf->getFirstChild("dataproc");
	while(dataproc_conf)
	{
		datacol_conf = dataproc_conf->getFirstChild("datacollector");
		aos_assert_r(datacol_conf, false);
		
		datacol_id = datacol_conf->getAttrStr(AOSTAG_NAME);
		aos_assert_r(datacol_id != "", false);

		grp_type = datacol_conf->getAttrStr("group_type", "");
		if (grp_type == "")
		{
			rslt = initDataColCtlr(rdata, datacol_id, dataproc_conf, datacol_conf);
			aos_assert_r(rslt, false);
		}
		else if(grp_type == "physical_group")
		{
			rslt = getDataColsBySvrShuffle(rdata, dataproc_conf, datacol_id);
			aos_assert_r(rslt, false);
		
			// Ketty Need ? yes Need!
			ColInfo col_info;
			col_info.mOutputNum = AosGetNumPhysicals(); 
			mInfoMap.insert(make_pair(datacol_id, col_info));
		}
		else if(grp_type == "cube_shuffle")
		{
			rslt = getDataColsByCubeShuffle(rdata, dataproc_conf, datacol_id);
			aos_assert_r(rslt, false);
		
			// Ketty Need ? yes Need!
			ColInfo col_info;
			col_info.mOutputNum = AosGetNumCubes(); 
			mInfoMap.insert(make_pair(datacol_id, col_info));
		}
		else if(grp_type == "cube_assemble")
		{
			rslt = getDataColsByCubeAsm(rdata, dataproc_conf, datacol_id);
			aos_assert_r(rslt, false);
			
			// Ketty Need ? yes Need!
			ColInfo col_info;
			col_info.mOutputNum = AosGetNumCubes(); 
			mInfoMap.insert(make_pair(datacol_id, col_info));
		}
		else
		{
			OmnShouldNeverComeHere;
			return false;
		}

		dataproc_conf = actions_conf->getNextChild();
	}
	
	return true;
}

bool
AosLogicTaskVer1::initDataColCtlr(
		const AosRundataPtr &rdata,
		const OmnString data_colid,
		const AosXmlTagPtr &data_proc_conf,
		AosXmlTagPtr &data_col_conf)
{
	// Ketty 2014/08/21
	AosLogicTaskObjPtr thisptr(this, false);
	
	AosDataColCtlrPtr ctlr = OmnNew AosDataColCtlr(data_colid, thisptr);
	bool rslt = ctlr->setDataColTag(rdata, data_proc_conf, data_col_conf);
	aos_assert_r(rslt, false);
	
	pair<map<OmnString, AosDataColCtlrPtr>::iterator, bool> pr;
	pr = mDataColCtlr.insert(make_pair(data_colid, ctlr));
	aos_assert_r(pr.second, false);	
	
	if(!mDataColTags)
	{
		OmnString datacol_str = "<";
		datacol_str << AOSTAG_DATA_COLLECTORS << "/>";
		AosXmlParser parser;
		mDataColTags = parser.parse(datacol_str, "" AosMemoryCheckerArgs);
		aos_assert_r(mDataColTags, false);
	}
	mDataColTags->addNode(data_col_conf);

	return true;
}

bool
AosLogicTaskVer1::getDataColsBySvrShuffle(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &orig_dataproc_conf,
		const OmnString &orig_datacol_id)
{
	// Ketty 2014/08/21
	aos_assert_r(orig_dataproc_conf, false);
	bool rslt;
	
	AosXmlTagPtr orig_datacol_conf;
	orig_datacol_conf = orig_dataproc_conf->getFirstChild("datacollector");
	aos_assert_r(orig_datacol_conf, false);
	
	// the parent_dataCol.
	orig_datacol_conf->setAttr("is_parent", true);
	rslt = initDataColCtlr(rdata, orig_datacol_id, orig_dataproc_conf, orig_datacol_conf);
	aos_assert_r(rslt, false);

	AosXmlTagPtr new_dataproc_conf;
	OmnString new_datacolid, rcd_name;
	AosXmlTagPtr new_datacol_conf;
	AosXmlTagPtr iilasm, record;
	
	u32 grp_num = AosGetNumPhysicals();
	for(u32 i= 0; i< grp_num; i++)
	{
		new_datacolid = orig_datacol_id;
		new_datacolid << "_" << i;
		
		new_datacol_conf = orig_datacol_conf->clone(AosMemoryCheckerArgsBegin); 
		aos_assert_r(new_datacol_conf, false);

		new_datacol_conf->setAttr(AOSTAG_NAME, new_datacolid);
		new_datacol_conf->setAttr("zky_old_datacolid", orig_datacol_id);
		new_datacol_conf->setAttr("is_parent", false);
		
		iilasm = new_datacol_conf->getFirstChild("asm");
		aos_assert_r(iilasm, false);

		record = iilasm->getFirstChild("datarecord");
		aos_assert_r(iilasm, false);

		rcd_name = record->getAttrStr("zky_name");
		if (rcd_name != "")
		{
			rcd_name << "_" << i;
		}
		else
		{
			rcd_name = new_datacolid;
		}
		record->setAttr("zky_name", rcd_name);

		// Ice 2014/08/20
		//tmp_col->setAttr("zky_physicalid", i);
		
		new_dataproc_conf = orig_dataproc_conf->clone(AosMemoryCheckerArgsBegin); 
		aos_assert_r(new_datacol_conf, false);
		new_dataproc_conf->removeNode("datacollector", false, false);
		new_dataproc_conf->addNode(new_datacol_conf);

		rslt = initDataColCtlr(rdata, new_datacolid,
				new_dataproc_conf, new_datacol_conf);
		aos_assert_r(rslt, false);
	}

	return true;
}

bool
AosLogicTaskVer1::getDataColsByCubeShuffle(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &orig_dataproc_conf,
		const OmnString &orig_datacol_id)
{
	// Ketty 2014/08/21
	aos_assert_r(orig_dataproc_conf, false);
	bool rslt;

	AosXmlTagPtr orig_datacol_conf;
	orig_datacol_conf = orig_dataproc_conf->getFirstChild("datacollector");
	aos_assert_r(orig_datacol_conf, false);

	// the parent_dataCol.
	orig_datacol_conf->setAttr("is_parent", true);
	rslt = initDataColCtlr(rdata, orig_datacol_id, orig_dataproc_conf, orig_datacol_conf);
	aos_assert_r(rslt, false);
	
	AosXmlTagPtr new_dataproc_conf;
	OmnString new_datacolid, rcd_name;
	AosXmlTagPtr new_datacol_conf;
	AosXmlTagPtr iilasm, record;
	
	u32 grp_num = AosGetNumCubes();
	for(u32 i= 0; i< grp_num; i++)
	{
		new_datacolid = orig_datacol_id;
		new_datacolid << "_" << i;
		
		new_datacol_conf = orig_datacol_conf->clone(AosMemoryCheckerArgsBegin); 
		aos_assert_r(new_datacol_conf, false);

		new_datacol_conf->setAttr(AOSTAG_NAME, new_datacolid);
		new_datacol_conf->setAttr("zky_old_datacolid", orig_datacol_id);
		new_datacol_conf->setAttr("is_parent", false);
		
		iilasm = new_datacol_conf->getFirstChild("asm");
		aos_assert_r(iilasm, false);

		record = iilasm->getFirstChild("datarecord");
		aos_assert_r(iilasm, false);

		rcd_name = record->getAttrStr("zky_name");
		if (rcd_name != "")
		{
			rcd_name << "_" << i;
		}
		else
		{
			rcd_name = new_datacolid;
		}
		record->setAttr("zky_name", rcd_name);
			
		// Ice 2014/08/20
		//tmp_col->setAttr("zky_physicalid", i);
		new_datacol_conf->setAttr(AOSTAG_CUBE_ID, i);
		
		new_dataproc_conf = orig_dataproc_conf->clone(AosMemoryCheckerArgsBegin); 
		aos_assert_r(new_datacol_conf, false);
		new_dataproc_conf->removeNode("datacollector", false, false);
		new_dataproc_conf->addNode(new_datacol_conf);

		rslt = initDataColCtlr(rdata, new_datacolid,
				new_dataproc_conf, new_datacol_conf);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosLogicTaskVer1::getDataColsByCubeAsm(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &orig_dataproc_conf,
		const OmnString &orig_datacol_id)
{
	// Ketty 2014/08/21
	aos_assert_r(orig_dataproc_conf, false);
	bool rslt;

	AosXmlTagPtr orig_datacol_conf;
	orig_datacol_conf = orig_dataproc_conf->getFirstChild("datacollector");
	aos_assert_r(orig_datacol_conf, false);
	
	// the parent_dataCol.
	orig_datacol_conf->setAttr("is_parent", true);
	rslt = initDataColCtlr(rdata, orig_datacol_id, orig_dataproc_conf, orig_datacol_conf);
	aos_assert_r(rslt, false);
	
	AosXmlTagPtr new_dataproc_conf;
	OmnString new_datacolid, rcd_name;
	AosXmlTagPtr new_datacol_conf;
	
	u32 grp_num = AosGetNumCubes();
	for(u32 i= 0; i< grp_num; i++)
	{
		new_datacolid = orig_datacol_id;
		new_datacolid << "_" << i;
		
		new_datacol_conf = orig_datacol_conf->clone(AosMemoryCheckerArgsBegin); 
		aos_assert_r(new_datacol_conf, false);

		new_datacol_conf->setAttr(AOSTAG_NAME, new_datacolid);
		new_datacol_conf->setAttr("zky_old_datacolid", orig_datacol_id);
		new_datacol_conf->setAttr("is_parent", false);
		
		// Ice 2014/08/20
		//tmp_col->setAttr("zky_physicalid", i);
		new_datacol_conf->setAttr(AOSTAG_CUBE_ID, i);
		
		new_dataproc_conf = orig_dataproc_conf->clone(AosMemoryCheckerArgsBegin); 
		aos_assert_r(new_datacol_conf, false);
		new_dataproc_conf->removeNode("datacollector", false, false);
		new_dataproc_conf->addNode(new_datacol_conf);

		rslt = initDataColCtlr(rdata, new_datacolid,
				new_dataproc_conf, new_datacol_conf);
		aos_assert_r(rslt, false);
	}

	return true;
	
}


bool
AosLogicTaskVer1::setDataColTags(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	// Ketty 2014/08/21 this is version0 will delete later....
	if (!xml) return true;
	bool rslt = true;
	OmnString datacol_id, type;
	AosDataColCtlrPtr ctlr;
	map<OmnString, AosDataColCtlrPtr> data_cols;

	AosLogicTaskObjPtr thisptr(this, false);
	AosXmlTagPtr col = xml->getFirstChild(true);

	vector<AosXmlTagPtr> groupCols;
	while(col)
	{
		datacol_id = col->getAttrStr(AOSTAG_NAME);
		aos_assert_r(datacol_id != "", false);

		if (data_cols.find(datacol_id) != data_cols.end())
		{
			AosSetErrorU(rdata, "datacol_id exist"); 
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		
		//felicia, 2014/01/23
		type = col->getAttrStr("group_type", "");
		// Ketty 2014/08/14
		if (type == "physical_group" || type == "cube_shuffle" || type == "cube_assemble")
		{
			groupCols.push_back(col);
			col = xml->getNextChild();
			continue;
		}

		ctlr = OmnNew AosDataColCtlr(datacol_id, thisptr);
		rslt = ctlr->setDataColTag(col, rdata);
		aos_assert_r(rslt, false);

		data_cols[datacol_id] = ctlr;

		col = xml->getNextChild();
	}

	rslt = createGroupDataCol(data_cols, type, groupCols, xml, rdata);

	mLock->lock();
	mDataColTags = xml->clone(AosMemoryCheckerArgsBegin);
	mDataColCtlr = data_cols;
	mLock->unlock();
	return true;
}


bool
AosLogicTaskVer1::createGroupDataCol(
		map<OmnString, AosDataColCtlrPtr> &data_cols,
		const OmnString &grp_type,	// Ketty 2014/08/14
		const vector<AosXmlTagPtr> &groupCols,
		const AosXmlTagPtr &pxml,
		const AosRundataPtr &rdata)
{
	// Ketty 2014/08/21 this is version0 will delete later....
	if (groupCols.size() == 0) return true;
	OmnTagFuncInfo << endl;
	aos_assert_r(pxml, false);
	AosXmlTagPtr xml = pxml->getFirstChild("actions");
	aos_assert_r(xml, false);
		
	// Ketty 2014/08/14
	u32 grp_num = 0;
	if(grp_type == "physical_group")
	{
		grp_num = AosGetNumPhysicals();
	}
	else
	{
		aos_assert_r(grp_type == "cube_shuffle", false);
		grp_num = AosGetNumCubes();
	}
	aos_assert_r(grp_num > 0, false);

	bool rslt;
	AosDataColCtlrPtr ctlr;
	OmnString tmp_colid, datacol_id, rcd_name;
	AosXmlTagPtr tmp_col, iilasm, record, datacol_tag, tmp_datacol;
	
	AosLogicTaskObjPtr thisptr(this, false);
	for(size_t k = 0; k < groupCols.size(); k++)
	{
		datacol_tag = groupCols[k]->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(datacol_tag, false);

		AosXmlTagPtr tag = groupCols[k]->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(tag, false);

		tag = tag->getFirstChild("datacollector");
		aos_assert_r(tag, false);

		xml->removeNode(groupCols[k]);

		datacol_id = tag->getAttrStr(AOSTAG_NAME);
		aos_assert_r(datacol_id != "", false);

		//for(size_t i = 0; i< svr_ids.size(); i++)
		for(size_t i = 0; i< grp_num; i++)
		{
			tmp_col = tag->clone(AosMemoryCheckerArgsBegin);
			aos_assert_r(tmp_col, false);

			tmp_datacol = datacol_tag->clone(AosMemoryCheckerArgsBegin);
			aos_assert_r(tmp_datacol, false);

			tmp_colid = datacol_id;
			tmp_colid << "_" << i;

			tmp_col->setAttr(AOSTAG_NAME, tmp_colid);
			tmp_col->setAttr("zky_old_datacolid", datacol_id);

			iilasm = tmp_col->getFirstChild("asm");
			aos_assert_r(iilasm, false);

			record = iilasm->getFirstChild("datarecord");
			aos_assert_r(iilasm, false);

			//rcd_name = tmp_col->getAttrStr("zky_name");
			rcd_name = record->getAttrStr("zky_name");
			if (rcd_name != "")
			{
				rcd_name << "_" << i;
			}
			else
			{
				rcd_name = tmp_colid;
			}
			record->setAttr("zky_name", rcd_name);
		
			// Ice 2014/08/20
			//tmp_col->setAttr("zky_physicalid", i);
			tmp_datacol->removeNode("datacollector", false, false);
			tmp_datacol->addNode(tmp_col);

			ctlr = OmnNew AosDataColCtlr(tmp_colid, thisptr);
			rslt = ctlr->setDataColTag(tmp_datacol, rdata);
			aos_assert_r(rslt, false);
			data_cols[tmp_colid] = ctlr;
			xml->addNode(tmp_datacol);
		}
	}
	return true;
}


void
AosLogicTaskVer1::clear()
{
	// job
	mJobDocid = 0;
	mDataColTags = 0;
	mTaskInfo.clear();
	mDataColCtlr.clear();
}


bool
AosLogicTaskVer1::updateTaskOutPutPriv(
		const u64 &task_docid,
		const AosXmlTagPtr &outputs,
		const AosRundataPtr &rdata)
{
	// task
	aos_assert_r(task_docid != 0, false);
	aos_assert_r(outputs, false);
	OmnTagFuncInfo << "task_docid is: " << task_docid << endl;
	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	AosXmlTagPtr tmp_tag = outputs;
	task_doc->addNode(tmp_tag);
	rdata->setOk();
	AosModifyDoc(task_doc, rdata);
	AosXmlTagPtr xml = task_doc->getFirstChild("output_data__n");
	aos_assert_rl(xml, mLock, false);
	if (mCalcuationSize == 0)
	{
		int tag_num = xml->getNumSubtags();
		int each_num = tag_num / mDataColCtlr.size();
		int each_reduce_num = 4;
		while(each_num >= eMaxFilePreReduceTask)
		{
			each_reduce_num += (each_num / eMaxFilePreReduceTask);
			each_num = each_num / eMaxFilePreReduceTask;
		}
		int recude_num = each_reduce_num * mDataColCtlr.size();
		mCalcuationSize = mMapTaskNum + recude_num;
	
		AosXmlTagPtr entry = xml->getFirstChild(true);
		while(entry)
		{
			AosTaskDataType::E type = AosTaskDataType::toEnum(entry->getAttrStr(AOSTAG_TYPE));
			if (type == AosTaskDataType::eOutPutDoc)
			{
				mCalcuationSize += mMapTaskNum * AosGetNumPhysicals();
				break;
			}
			entry = xml->getNextChild();
		}
	}

	//save storage file size
	AosXmlTagPtr entry = xml->getFirstChild(true);
	while(entry)
	{
		AosTaskDataType::E type = AosTaskDataType::toEnum(entry->getAttrStr(AOSTAG_TYPE));
		if (type == AosTaskDataType::eOutPutDoc || type == AosTaskDataType::eOutPutIIL)
		{
			u64 file_id = entry->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
			aos_assert_rl(file_id != 0, mLock, false);
			int physical_id = entry->getAttrInt(AOSTAG_PHYSICALID, -1);
			aos_assert_rl(physical_id != -1, mLock, false);
			int64_t file_len = 0;
			AosDiskStat disk_stat;
			bool rslt = AosNetFileCltObj::getFileLengthStatic(
				file_id, physical_id, file_len, disk_stat, rdata.getPtr());
			if (!rslt)
			{
				mLock->unlock();
				return false;
			}
			entry->setAttr(AOSTAG_LENGTH, file_len);
		}
		entry = xml->getNextChild();
	}
	mLock->unlock();

	bool rslt = addOutPut(xml, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosLogicTaskVer1::addOutPut(
		const AosXmlTagPtr &xml, 
		const AosRundataPtr &rdata)
{
	// task
	aos_assert_r(xml, false);
	bool rslt;
	OmnString datacol_id;
	AosTaskDataType::E type;
	AosDataColCtlrPtr ctlr;
	map<OmnString, AosDataColCtlrPtr>::iterator itr;
	AosXmlTagPtr entry = xml->getFirstChild(true);

	OmnTagFuncInfo << endl;
	while(entry)
	{
		type = AosTaskDataType::toEnum(entry->getAttrStr(AOSTAG_TYPE));
		if (type == AosTaskDataType::eOutPutIIL)
		{
			datacol_id = entry->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			itr = mDataColCtlr.find(datacol_id);
			aos_assert_r(itr != mDataColCtlr.end(), false);
			ctlr = itr->second;
			rslt = ctlr->addOutPut(entry, rdata);
		}
		else if (type == AosTaskDataType::eOutPutDoc && entry->getAttrBool(AOSTAG_IS_FIXED, true))
		{
			datacol_id = entry->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			itr = mDataColCtlr.find(datacol_id);
			aos_assert_r(itr != mDataColCtlr.end(), false);
			ctlr = itr->second;
			rslt = ctlr->addOutPutDocFixed(entry, rdata);
			aos_assert_r(rslt, false);
		}
		else if (type == AosTaskDataType::eOutPutDoc && !entry->getAttrBool(AOSTAG_IS_FIXED, true))
		{
			datacol_id = entry->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			itr = mDataColCtlr.find(datacol_id);
			aos_assert_r(itr != mDataColCtlr.end(), false);
			ctlr = itr->second;
			rslt = ctlr->addOutPutDocCSV(entry, rdata);
			aos_assert_r(rslt, false);
		}

		entry = xml->getNextChild();
	}
	return true;
}


bool
AosLogicTaskVer1::createMergeFileTask(
		const AosXmlTagPtr &datacol_tag,
		const int physical_id,
		list<AosXmlTagPtr> &info_list,
		u64 &task_docid,
		const AosRundataPtr &rdata)
{
	// task
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
	aos_assert_r(job, false);

	AosXmlTagPtr sdoc;
	if (job->getVersion() == 1)
	{
		sdoc = AosReduceTaskCreator::create(rdata, 
				physical_id, datacol_tag, info_list);
	}
	else
	{
		aos_assert_r(datacol_tag, false);
		aos_assert_r(info_list.size() > 0, false);

		AosXmlTagPtr xml = *(info_list.begin());
		aos_assert_r(xml, false);
		int level = xml->getAttrInt(AOSTAG_LEVEL, -1);

		OmnString str;
		str << "<sdoc "
				<< AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
				<< AOSTAG_PHYSICALID << "=\"" << physical_id << "\">"
			<< "<action "
				<< AOSTAG_TYPE << "=\"" << AOSACTTYPE_MERGEFILE << "\" "
				<< AOSTAG_LEVEL << "=\"" << level << "\">"
			<< datacol_tag->toString()
			<< "<files>";
		list<AosXmlTagPtr>::iterator itr = info_list.begin();
		AosXmlTagPtr output_tag;
		while(itr != info_list.end())
		{
			output_tag = *itr;
			str << output_tag->toString();
			itr++;
		}
		str << "</files>";
		str << "</action></sdoc>";

		sdoc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	}
	aos_assert_r(sdoc, false);

	AosTaskObjPtr task = AosLogicTask::createTaskPriv(job, sdoc, rdata);
	aos_assert_r(task, false);
		
	bool rslt = addMemberTaskPriv(job, task, rdata);
	aos_assert_r(rslt, false);

	task_docid = task->getTaskDocid();
	aos_assert_r(task_docid != 0, false);
	rslt = AosLogicTask::findInputFrom(task_docid, info_list, rdata);
	aos_assert_r(rslt, false);

	AosTaskInfoPtr task_info = getTaskInfo(task_docid, rdata);
	aos_assert_r(task_info, false);

	task_info->setStatus(AosTaskStatus::eWait);
	rslt = job->addTaskInfo(task_info, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosLogicTaskVer1::findInputFrom(
		const u64 &to_task_docid,
		const AosXmlTagPtr &input,
		const AosRundataPtr &rdata)
{
	u64 from_task_docid = input->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
	aos_assert_r(from_task_docid != 0, false);
	AosXmlTagPtr task_doc = AosGetDocByDocid(from_task_docid, rdata);
	aos_assert_r(task_doc, false);
	AosXmlTagPtr outputs = task_doc->getFirstChild("output_data__n");
	aos_assert_r(outputs, false);
	u64 input_file_id = input->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(input_file_id != 0, false);
	u64 output_file_id = 0;
	AosXmlTagPtr output = outputs->getFirstChild();
	while(output)
	{
		output_file_id = output->getAttrU64(AOSTAG_STORAGE_FILEID, 0);	
		if (input_file_id == output_file_id)
		{
			output->setAttr(AOSTAG_TO_TASK_DOCID, to_task_docid);
			break;
		}
		output = outputs->getNextChild();
	}
	AosModifyDoc(task_doc, rdata);
	return true;
}


bool
AosLogicTaskVer1::readyToCreate(
		const AosXmlTagPtr &data_proc_tag,
		const AosXmlTagPtr &info,
		list<AosXmlTagPtr> &info_list)
{
	AosXmlTagPtr datacol_tag = data_proc_tag->getFirstChild("datacollector");
	aos_assert_r(datacol_tag, false);

	OmnString group_type = datacol_tag->getAttrStr("group_type", "");
	if(group_type == "")
	{
		if (info) info_list.push_back(info);
		return true;
	}

	OmnString run_mode = data_proc_tag->getAttrStr("run_mode", "");
	aos_assert_r(run_mode != "", false);	

	if(run_mode == "run_parent")
	{
		return procParentDataCol(datacol_tag, group_type, info, info_list);
	}

	if(run_mode == "run_child")
	{
		return procChildDataCol(datacol_tag, group_type, info, info_list);
	}
	
	OmnShouldNeverComeHere;
	return false;
}


bool
AosLogicTaskVer1::procParentDataCol(
		const AosXmlTagPtr &datacol_tag,
		const OmnString &group_type,
		const AosXmlTagPtr &info,
		list<AosXmlTagPtr> &info_list)
{
	if(datacol_tag->getAttrBool("is_parent"))
	{
		// should never come here. parent data col never run.
		// triggerd by child data_collect.
		return false;
		//return true;
	}
	
	int svr_nums = -1;
	if(group_type == "physical_group")
	{
		svr_nums = AosGetNumPhysicals();
	}
	else
	{
		aos_assert_r(group_type == "cube_shuffle" ||
				group_type == "cube_assemble", false);
		svr_nums = AosGetNumCubes();
	}
	
	OmnString parent_colid = datacol_tag->getAttrStr("zky_old_datacolid", "");
	map<OmnString, ColInfo>::iterator itr = mInfoMap.find(parent_colid); 
	aos_assert_r(itr != mInfoMap.end(), false);
	itr->second.mFinishNum ++;
	if(info) itr->second.mInfoList.push_back(info);

	// try to trigger run parent data_col.
	aos_assert_r(itr->second.mFinishNum <= svr_nums, false);
	if (itr->second.mFinishNum == itr->second.mOutputNum)
	{
		info_list = itr->second.mInfoList;
		if (itr->second.mFinishNum < svr_nums)
		{
			OmnScreen << "finishednum(" << itr->second.mFinishNum << ") < svr_nums(" << svr_nums << ")"<< endl;
		}
		return true;
	}
	return false;
}


bool
AosLogicTaskVer1::procChildDataCol(
		const AosXmlTagPtr &datacol_tag,
		const OmnString &group_type,
		const AosXmlTagPtr &info,
		list<AosXmlTagPtr> &info_list)
{
	if(datacol_tag->getAttrBool("is_parent"))
	{
		// should never come here.
		return false;
	}

	if (info) info_list.push_back(info);
	return true;
}


bool
AosLogicTaskVer1::createMergeFinishTask(
		const AosXmlTagPtr &dataproc_tag,
		const int physical_id,
		const AosXmlTagPtr &info,
		const AosRundataPtr &rdata)
{
	// task
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
	aos_assert_r(job, false);

	aos_assert_r(dataproc_tag, false);

	AosXmlTagPtr datacol_tag = dataproc_tag->getFirstChild("datacollector");
	aos_assert_r(datacol_tag, false);

	AosXmlTagPtr sdoc;
	if (job->getVersion() == 1)
	{
		list<AosXmlTagPtr> info_list;
		if (!readyToCreate(dataproc_tag, info, info_list)) return true;
		if (!info) return true;

		sdoc = AosReduceTaskCreator::create(rdata, mReduceTaskTypeMap,
				physical_id, dataproc_tag, info_list);
	}
	else
	{
		aos_assert_r(info, false);
		AosXmlTagPtr iilasm_tag = datacol_tag->getFirstChild("asm");
		aos_assert_r(iilasm_tag, false);

		OmnString iiltype = iilasm_tag->getAttrStr("zky_asmtype");

		OmnString str;
		str << "<sdoc "
				<< AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
				<< AOSTAG_PHYSICALID << "=\"" << physical_id << "\">"
			<< "<action ";
		
		if (iiltype == "hitadd" || iiltype == "hitremove")
		{
			str << AOSTAG_TYPE << "=\"createhitiil\">";
		}
		else if (iiltype == "jimotable_add")
		{
			str << AOSTAG_TYPE << "=\"" << AOSACTTYPE_JIMOTABLE_BATCHOPR << "\">";
		}
		else
		{
			str << AOSTAG_TYPE << "=\"" << AOSACTTYPE_IILBATCHOPR << "\">";
		}

		str	<< datacol_tag->toString()
			<< "<files>"
				<< info->toString()
			<< "</files>"
			<< "</action></sdoc>";
		sdoc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	}
	aos_assert_r(sdoc, false);

	AosTaskObjPtr task = AosLogicTask::createTaskPriv(job, sdoc, rdata);
	aos_assert_r(task, false);	

	bool rslt = addMemberTaskPriv(job, task, rdata);
	aos_assert_r(rslt, false);

	u64 task_docid = task->getTaskDocid();
	aos_assert_r(task_docid != 0, false);

	rslt = findInputFrom(task_docid, info, rdata);
	aos_assert_r(rslt, false);

	AosTaskInfoPtr task_info = getTaskInfo(task_docid, rdata);
	aos_assert_r(task_info, false);

	task_info->setStatus(AosTaskStatus::eWait);
	rslt = job->addTaskInfo(task_info, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosLogicTaskVer1::createImportDocTaskPriv(
		const AosXmlTagPtr &dataproc_tag,
		const int physical_id,
		list<AosXmlTagPtr> &doc_list,
		const OmnString &action_type,
		const AosRundataPtr &rdata)
{
	//task
	if(doc_list.size() == 0) return true;

	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
	aos_assert_r(job, false);
	AosXmlTagPtr sdoc;
	list<AosXmlTagPtr>::iterator itr;
	if (job->getVersion() == 1)
	{
		OmnTagFuncInfo << endl;
		aos_assert_r(doc_list.size() > 0, false);
		itr = doc_list.begin();
		aos_assert_r(itr != doc_list.end(), false);
		sdoc = AosReduceTaskCreator::create(rdata, mReduceTaskTypeMap,
				physical_id, dataproc_tag, doc_list);
	}
	else
	{
		OmnTagFuncInfo << endl;
		OmnString str;
		str << "<sdoc "
				<< AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
				<< AOSTAG_PHYSICALID << "=\"" << physical_id << "\">"
			<< "<action "
				<< AOSTAG_TYPE << "=\"" << action_type << "\">"
			<< "<files>";
		list<AosXmlTagPtr>::iterator itr = doc_list.begin();
		while (itr != doc_list.end())
		{
			str << (*itr)->toString();
			itr++;
		}
		str << "</files>";
		str << "</action></sdoc>";

		sdoc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	}
	aos_assert_r(sdoc, false);


	AosTaskObjPtr task = AosLogicTask::createTaskPriv(job, sdoc, rdata);
	aos_assert_r(task, false);
	
	bool rslt = addMemberTaskPriv(job, task, rdata);
	aos_assert_r(rslt, false);

	u64 task_docid = task->getTaskDocid();
	aos_assert_r(task_docid != 0, false);

	rslt = AosLogicTask::findInputFrom(task_docid, doc_list, rdata);
	aos_assert_r(rslt, false);

	AosTaskInfoPtr task_info = getTaskInfo(task_docid, rdata);
	aos_assert_r(task_info, false);

	task_info->setStatus(AosTaskStatus::eWait);
	rslt = job->addTaskInfo(task_info, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosLogicTaskVer1::taskFinished(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(task_docid, false);
	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	task_doc->setAttr(AOSTAG_STATUS, AosTaskStatus::toStr(AosTaskStatus::eFinish));
	task_doc->setAttr(AOSTAG_END_TIME, OmnGetTime(AosLocale::eChina));
	task_doc->setAttr(AOSTAG_PROGRESS, "100");                        
	bool rslt = AosModifyDoc(task_doc, rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return taskInfoFinished(task_docid, rdata);
}

bool
AosLogicTaskVer1::taskInfoFinished(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	//Job and Task
	mLock->lock();
	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_rl(info, mLock, false);
	info->setStatus(AosTaskStatus::eFinish);
	info->setProgress(100);
	mFinishedNum++;
	OmnTagFuncInfo << "mFinishedNum is: " << mFinishedNum 
		<< " finished task doc is: " << info->getTaskDocid() <<  endl;
	if (info->getTaskType() == AosTaskType::eMapTask)
	{
		mFinishedMapTaskNum++;
	}
	aos_assert_r(mFinishedMapTaskNum <= mMapTaskNum, false);
	bool all_map_finished = (mFinishedMapTaskNum == mMapTaskNum);
	mStartTasks.erase(task_docid);
	mLock->unlock();
	AosDataColCtlrPtr ctlr;
	map<OmnString, AosDataColCtlrPtr>::iterator itr = mDataColCtlr.begin();
	OmnTagFuncInfo << "mDataColCtlr's size is: " << mDataColCtlr.size() << endl;
	while (itr != mDataColCtlr.end())
	{
		ctlr = itr->second;
		ctlr->eraseActiveTaskDocid(info->getTaskDocid());
		OmnTagFuncInfo << "service judgement: jobdocid is: " << mJobDocid << endl;
		if (all_map_finished && !AosTaskMgr::isService(rdata, mJobDocid))
		{
OmnScreen << "*************************************all_map_finished: " << endl;
			ctlr->mergeAllFiles(rdata);
			ctlr->mergeAllDocFiles(rdata);
		}
		itr++;
	}

	if (!isFinished())
	{
		return updateLogicTaskProgress(rdata);
	}

	mProgress = 100;
	mStatus = AosTaskStatus::eFinish;
	return true;
}

bool
AosLogicTaskVer1::taskFinished(
		const AosJobObjPtr &job,
		const AosTaskRunnerInfoPtr &run_info,
		const AosRundataPtr &rdata) 
{
	// job and task
	// 'task' has finished. It checks whether it can kick off the next
	// task, if any.
	u64 task_docid = run_info->getTaskDocid();
	if (!job->isValid(task_docid, run_info->getTaskStartSTime(), rdata)) return true; 
	OmnString output_str = run_info->getTaskOutPuts();
	AosXmlTagPtr outputs;
	if (output_str != "")
	{
		outputs = AosXmlParser::parse(output_str AosMemoryCheckerArgs);
		if (outputs)
		{
			bool rslt = updateTaskOutPutPriv(task_docid, outputs, rdata);
			if (!rslt)
			{
				//may output error, to fail task
				AosTaskInfoPtr task_info = getTaskInfo(task_docid, rdata);
				aos_assert_r(task_info, false);
				AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
				aos_assert_r(job, false);
				int server_id = task_info->getTaskServerId();
				aos_assert_r(server_id != -1, false);
				OmnNotImplementedYet;
				//mLock->lock();
				//rslt = outputErrorLocked(job, task_info, server_id, rdata);
				//mLock->unlock();
				aos_assert_r(rslt, false);
				return true;
			}
		}
	}

	OmnString snapshots_str= run_info->getTaskSnapShots();
	AosXmlTagPtr snapshots_tag;
	if (snapshots_str != "")
	{
		snapshots_tag = AosXmlParser::parse(snapshots_str AosMemoryCheckerArgs);
		if (snapshots_tag)
		{
			bool rslt = updateJobSnapShotPriv(task_docid, snapshots_tag, rdata);
			aos_assert_r(rslt, false);
		}
	}
	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	task_doc->setAttr(AOSTAG_STATUS, AosTaskStatus::toStr(AosTaskStatus::eFinish));
	task_doc->setAttr(AOSTAG_END_TIME, OmnGetTime(AosLocale::eChina));
	task_doc->setAttr(AOSTAG_PROGRESS, "100");                        
	bool rslt = AosModifyDoc(task_doc, rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	rslt = taskInfoFinished(task_docid, rdata);
	aos_assert_r(rslt, false);

	AosTaskInfoPtr info = getTaskInfo(task_docid, rdata);
	aos_assert_r(info, false);

	OmnString task_id = info->getTaskId(); 
	OmnScreen << "Task finished:" << task_id << ":" << task_docid << endl;

	OmnString msg = AOSJOBLOG_TASK_FINISHED;
	msg << ":" << task_id << ":" << task_docid;
	job->logJob(msg, rdata);

	rslt = job->updateJobProgress(rdata);
	aos_assert_r(rslt, false);

	rslt = job->scheduleNextTask(run_info, rdata);
	aos_assert_r(rslt, false);
	
	job->checkJobFinishing(rdata);
	return true;
}


bool
AosLogicTaskVer1::logTask(
		const OmnString &entry_key,
		const AosTaskInfoPtr &info,
		const AosJobObjPtr &job,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<tasklog "
		   << "zky_task_docid" << "=\"" << info->getTaskDocid() << "\" "
		   << AOSTAG_PARENTC << "=\"" << job->getLogContainer() << "\" "
		   << AOSTAG_LOG_TYPE << "=\"" << AOSOTYPE_TASKLOG << "\" "
		   << AOSTAG_LOG_ENTRY_KEY << "=\"" << entry_key << "\" "
		   << AOSTAG_JOB_DOCID << "=\"" << mJobDocid << "\" />";

	return AosAddContainerLog(job->getLogContainer(), docstr, rdata);
}


bool
AosLogicTaskVer1::checkConfigStatic(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf)
{
	set<OmnString> data_col_ids;
	bool rslt = AosMapTaskCreator::checkConfigStatic(rdata, logic_task_conf, data_col_ids);
	aos_assert_r(rslt, false);

	rslt = AosReduceTaskCreator::checkConfigStatic(rdata, logic_task_conf, data_col_ids);
	aos_assert_r(rslt, false);
	return true;
}

//
//this method create a doc for all the output dataset config xml
//this method is used for streaming mode
//
bool
AosLogicTaskVer1::createOutputDatasetDocs(
						const AosRundataPtr &rdata,
						const AosJobObjPtr &job)
{
	// Ketty 2014/09/17
	AosXmlTagPtr logic_task_conf = mLogicTaskSdoc;
	aos_assert_r(logic_task_conf, false);
	AosXmlTagPtr output_dataset_confs = logic_task_conf->getFirstChild("output_datasets"); 
	if (output_dataset_confs)
	{
		AosXmlTagPtr schema_conf;
		AosXmlTagPtr scanner_conf;
		AosXmlTagPtr cube_conf;
		AosXmlTagPtr output_dataset_conf;
		AosXmlTagPtr output_dataset_doc;
		output_dataset_conf = output_dataset_confs->getFirstChild("dataset");
		OmnString name;
		while (output_dataset_conf)
		{
			schema_conf = output_dataset_conf->getFirstChild("dataschema");
			scanner_conf = output_dataset_conf->getFirstChild("datascanner");
			if (scanner_conf)
				cube_conf = scanner_conf->getFirstChild("dataconnector");

			if (schema_conf && cube_conf)
			{
				//this output dataset will be used by other task(s)
				//save it into a doc
				output_dataset_conf->setAttr(AOSTAG_OTYPE, "dataset");
				name = output_dataset_conf->getAttrStr(AOSTAG_NAME);
				output_dataset_conf->setAttr("zky_dataset_name", name);
			
				//need to change the data connector to buff cube
				cube_conf->setAttr("jimo_objid", "dataconnector_buff_jimodoc_v0");
				cube_conf->setAttr("zky_dataset_name", name);
				scanner_conf->setAttr("jimo_objid", "datascanner_cube_jimodoc_v0");

				//create the doc
				output_dataset_doc = AosCreateDoc(output_dataset_conf->toString(), 
						job->isJobPublic(), rdata);

				aos_assert_r(output_dataset_doc, false);
				OmnTagFuncInfo << output_dataset_doc->toString() << endl;
			}
			
			output_dataset_conf = output_dataset_confs->getNextChild();
		}
	}

	return true;
}

bool
AosLogicTaskVer1::createMapTasks(
			const AosRundataPtr &rdata,
			const AosJobObjPtr &job)
{
	AosLogicTaskObjPtr thisptr(this, false);
	aos_assert_r(mLogicTaskSdoc, false);
	bool rslt = AosMapTaskCreator::create(rdata, job, thisptr, mLogicTaskSdoc);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosLogicTaskVer1::createReduceTasks(
			const AosRundataPtr &rdata,
			const AosJobObjPtr &job)
{
	AosXmlTagPtr logic_task_conf = mLogicTaskSdoc;
	aos_assert_r(logic_task_conf, false);
	AosLogicTaskObjPtr thisptr(this, false);

	OmnTagFuncInfo << "create a reduce task for job: " << job->getJobDocid() << endl;
	bool rslt = AosReduceTaskCreator::create(rdata, job, thisptr, logic_task_conf);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosLogicTaskVer1::setReduceMap(const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;
	map<OmnString, AosReduceTaskCreator::ReduceConf> reduce_map;
	bool rslt = AosReduceTaskCreator::parse(rdata, mLogicTaskSdoc, reduce_map);
	aos_assert_r(rslt, false);
	mReduceTaskTypeMap = reduce_map;
	return true;
}


bool 
AosLogicTaskVer1::reset(const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;

	mStartedNum = 0;
	mFinishedNum = 0;
	mMapTaskNum = 0;
	mFinishedMapTaskNum = 0;
	mProgress = 0;
	mCalcuationSize = 0;
	mStatus = AosTaskStatus::eStop;
	mTaskInfo.clear();
	//assume each reset means one round completed
	mRunNum++;
	return true;
}

#endif
