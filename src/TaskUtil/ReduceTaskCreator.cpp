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
#include "TaskUtil/ReduceTaskCreator.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/ActionType.h"
#include "TaskUtil/ReduceCreateHitIIL.h"
#include "TaskUtil/ReduceIILBatchOpr.h"
#include "TaskUtil/ReduceImportDocFixed.h"
#include "TaskUtil/ReduceImportDocNormal.h"
#include "TaskUtil/ReduceImportDocCSV.h"
#include "TaskUtil/ReduceMergeFile.h"
#include "TaskUtil/ReduceCreateDatasetDoc.h"
#include "TaskUtil/ReduceCreateTaskFile.h"
#include "TaskUtil/ReduceDeleteGroupDoc.h"
#include "Debug/Debug.h"
#include "TaskUtil/ReduceIILPatternOpr.h"


AosReduceTaskCreator::AosReduceTaskCreator()
:
mType("")
{
}

AosReduceTaskCreator::AosReduceTaskCreator(const OmnString &tt)
:
mType(tt)
{
}

AosReduceTaskCreator::~AosReduceTaskCreator()
{
}


bool
AosReduceTaskCreator::checkConfigStatic(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		set<OmnString> &data_col_ids)
{
	OmnTagFuncInfo << endl;

	AosXmlTagPtr reduce_conf = logic_task_conf->getFirstChild("reduce_tasks");
	if (!reduce_conf) return true;

	AosXmlTagPtr task_conf = reduce_conf->getFirstChild("reduce_task");
	if(!task_conf)
	{
		AosSetEntityError(rdata, "miss_task_conf", logic_task_conf->getTagname(), AOSTAG_DATA_COLLECTORS)
			<< logic_task_conf << enderr;
		return 0;
	}

	while(task_conf)
	{
		bool rslt = checkColIdConf(rdata, task_conf, data_col_ids);
		aos_assert_r(rslt, false);
		task_conf = reduce_conf->getNextChild();
	}
	return true;
}


bool
AosReduceTaskCreator::checkColIdConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &reduce_tags,
		set<OmnString> &data_col_ids)
{
	AosXmlTagPtr col_id_tag = reduce_tags->getFirstChild(true);
	if(!col_id_tag)
	{
		AosSetEntityError(rdata, "task_action_empty",
			reduce_tags->getTagname(), "")
			<< reduce_tags << enderr;
		return false;
	}

	while (col_id_tag)
	{
		OmnString data_col_id = col_id_tag->getNodeText();
		if(data_col_ids.find(data_col_id) == data_col_ids.end()) 
		{
			AosSetErrorUser(rdata, "reducetask_config error") << col_id_tag << enderr;
			return false;
		}

		col_id_tag = reduce_tags->getNextChild();
	}
	return true;
}


bool
AosReduceTaskCreator::parse(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		map<OmnString, ReduceConf> &reduce_map)
{
	AosXmlTagPtr reduce_conf = logic_task_conf->getFirstChild("reduce_task");
	if(!reduce_conf) return true;

	AosXmlTagPtr actions_conf = reduce_conf->getFirstChild("actions");
	if (!actions_conf) return true;

	AosXmlTagPtr action_conf = actions_conf->getFirstChild();
	OmnString type, data_col_id;
	AosXmlTagPtr datacol_conf;
	while(action_conf)
	{
		OmnString datasetname = action_conf->getAttrStr("datasetname");
		aos_assert_r(datasetname != "", false);

		type = action_conf->getAttrStr("type");
		aos_assert_r(type != "", false);

		datacol_conf = action_conf->getFirstChild("datacollector");
		aos_assert_r(datacol_conf, false);

		data_col_id = datacol_conf->getAttrStr(AOSTAG_NAME);
		aos_assert_r(data_col_id != "", false);

		ReduceConf info;
		info.mType = type;
		// Linda, 2014/04/18 tmp
		if (type == AOSACTTYPE_CREATEDATASETDOC)
		{
			AosXmlTagPtr dataset_tag = getOutputDatasetsTag(rdata, logic_task_conf, datasetname);
			if(dataset_tag)
			{
				dataset_tag = dataset_tag->clone(AosMemoryCheckerArgsBegin);
				info.mOutputDataset = dataset_tag;
				OmnTagFuncInfo << "found an output dataset for an action: " << dataset_tag->toString() << endl; 
			}
		}

		reduce_map.insert(make_pair(data_col_id, info));
		OmnTagFuncInfo << "Insert entry to reduce_map for the task with collector id: "
			<< data_col_id << endl;
		action_conf = actions_conf->getNextChild();
	}
	return true;
}


AosXmlTagPtr
AosReduceTaskCreator::getOutputDatasetsTag(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf,
		const OmnString &name)
{
	AosXmlTagPtr datasets_conf = conf->getFirstChild("output_datasets");
	if(!datasets_conf)
	{
		AosSetEntityError(rdata, "miss_dataset_conf", conf->getTagname(), "input_datasets")
			<< conf << enderr;
		return 0;
	}

	AosXmlTagPtr dataset_conf = datasets_conf->getFirstChild();
	while (dataset_conf)
	{
		if (dataset_conf->getAttrStr(AOSTAG_NAME, "") == name)
		{
			if (dataset_conf->getFirstChild())
			{
				return dataset_conf;
			}
			else
			{
				return 0;
			}
		}
		dataset_conf = datasets_conf->getNextChild();
	}
	return 0;
}



/*
bool
AosReduceTaskCreator::parse(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		map<OmnString, ReduceConf> &reduce_map)
{
	AosXmlTagPtr reduce_conf = logic_task_conf->getFirstChild("reduce_tasks");
	if(!reduce_conf) return true;

	AosXmlTagPtr task_conf = reduce_conf->getFirstChild("reduce_task");
	while(task_conf)
	{
		OmnString type = task_conf->getAttrStr(AOSTAG_ZKY_TYPE, "");
		aos_assert_r(type != "", false);

		bool rslt = dataColConf(rdata, task_conf, type, reduce_map);
		aos_assert_r(rslt, false);
		task_conf = reduce_conf->getNextChild();
	}
	return true;
}

bool
AosReduceTaskCreator::dataColConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &reduce_tags,
		const OmnString &type,
		map<OmnString, ReduceConf> &reduce_map)
{
	AosXmlTagPtr col_id_tag = reduce_tags->getFirstChild(true);
	aos_assert_r(col_id_tag, false);

	OmnString group_type = reduce_tags->getAttrStr("group_type", "");
	vector<u32> svr_ids = AosGetServerIds();		
	aos_assert_r(svr_ids.size() > 0, false);

	OmnString tmp_colid;
	while (col_id_tag)
	{
		OmnString data_col_id = col_id_tag->getNodeText();
		aos_assert_r(data_col_id != "", false);
		OmnString objid = col_id_tag->getAttrStr("zky_objid", "");

		if (reduce_map.count(data_col_id) != 0)
		{
			OmnAlarm << "data_col_id" << enderr;
		}
		ReduceConf info;
		info.mType = type;
		info.mObjid = objid;

		if (group_type == "physical_group")
		{
			for(size_t i=0; i<svr_ids.size(); i++)
			{
				tmp_colid = data_col_id;
				tmp_colid << "_" << i;
				reduce_map.insert(make_pair(tmp_colid, info));
			}
		}
		else
		{
			reduce_map.insert(make_pair(data_col_id, info));
		}

		col_id_tag = reduce_tags->getNextChild();
	}
	return true;
}
*/

AosXmlTagPtr
AosReduceTaskCreator::create(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &dataproc_tag)
{
	AosXmlTagPtr sdoc;
	OmnTagFuncInfo << "create a reduce task" << endl;

	//use a uniform creator for common nodes instead of 
	//switching different reduce task types. At the 
	//beginning, there is no data collector file.
	//
	//we need to generate the following xml:
	// <sdoc zky_physicalid="0" zky_taskid="t2_load_task_reduce_task_0" ...........  >
	//  <dataproc>
	//     ...................
	//	 <files>
	//	 </files>
	//  </dataproc>
	// </sdoc>
	OmnString docStr = "<sdoc></sdoc>";
	OmnString fileStr = "<files></files>";
	OmnString procStr = dataproc_tag->toString();
	AosXmlTagPtr pdoc = AosStr2Xml(rdata.getPtr(), procStr AosMemoryCheckerArgs);
	AosXmlTagPtr fdoc = AosStr2Xml(rdata.getPtr(), fileStr AosMemoryCheckerArgs);

	sdoc = AosStr2Xml(rdata.getPtr(), docStr AosMemoryCheckerArgs);
	pdoc->addNode(fdoc);
	sdoc->addNode(pdoc);

	return sdoc;
}

AosXmlTagPtr
AosReduceTaskCreator::create(
		const AosRundataPtr &rdata,
		map<OmnString, ReduceConf> &reduce_map,
		const int physical_id,
		const AosXmlTagPtr &dataproc_tag,
		list<AosXmlTagPtr> &info_list)

{
	OmnTagFuncInfo << "create a reduce task for a job" << endl;
	// Ketty 2014/08/22
	/*
	AosXmlTagPtr datacol_tag = dataproc_tag->getFirstChild("datacollector");
	if (!datacol_tag)
	{
		datacol_tag = dataproc_tag;
	}
	aos_assert_r(datacol_tag, 0);

	OmnString datacol_id = datacol_tag->getAttrStr(AOSTAG_DATACOLLECTOR_ID, "");
	if (datacol_id == "")
	{
		datacol_id = datacol_tag->getAttrStr(AOSTAG_NAME, "");
	}
	aos_assert_r(datacol_id != "", 0);

	map<OmnString, ReduceConf>::iterator itr = reduce_map.find(datacol_id); 
	aos_assert_r(itr != reduce_map.end(), 0);
	
	OmnString type = itr->second.mType;
	*/	

	OmnString type = dataproc_tag->getAttrStr("type");
	aos_assert_r(type != "", 0);
	AosActionType::E tt = AosActionType::toEnum(type);
	
	AosXmlTagPtr sdoc;
	AosReduceTaskCreator * reduce_creator;

	switch(tt)
	{
	case AosActionType::eCreateHitIIL:
		 reduce_creator = OmnNew AosReduceCreateHitIIL(type, physical_id, dataproc_tag, info_list);
		 reduce_creator->serializeTo(sdoc, rdata);
		 break;

	case AosActionType::eIILBatchOpr:
	case AosActionType::eJimoTableBatchOpr:
		 reduce_creator = OmnNew AosReduceIILBatchOpr(type, physical_id, dataproc_tag, info_list); 
		 reduce_creator->serializeTo(sdoc, rdata);
		 break;
	
	case AosActionType::eIILPatternOpr:
	case AosActionType::eIILPatternOpr2:
		 reduce_creator = OmnNew AosReduceIILPatternOpr(type, physical_id, dataproc_tag, info_list); 
		 reduce_creator->serializeTo(sdoc, rdata);
		 break;

	case AosActionType::eImportDocFixed:
		 reduce_creator = OmnNew AosReduceImportDocFixed(type, physical_id, info_list); 
		 reduce_creator->serializeTo(sdoc, rdata);
		 break;

	// Ketty 2014/08/22 will delete later.
	//case AosActionType::eCreateDatasetDoc:
	//	 reduce_creator = OmnNew AosCreateDatasetDoc(
	//			 type, itr->second.mOutputDataset, 
	//			 physical_id, info_list); 
	//	 reduce_creator->serializeTo(sdoc, rdata);
	//	 break;

	case AosActionType::eImportDocNormal:
		 reduce_creator = OmnNew AosReduceImportDocNormal(type, physical_id, info_list); 
		 reduce_creator->serializeTo(sdoc, rdata);
		 break;

	case AosActionType::eImportDocCSV:
		 reduce_creator = OmnNew AosReduceImportDocCSV(type, physical_id, info_list); 
		 reduce_creator->serializeTo(sdoc, rdata);
		 break;
	
	case AosActionType::eCreateTaskFile:
		 reduce_creator = OmnNew AosReduceCreateTaskFile(type, physical_id, dataproc_tag, info_list); 
		 reduce_creator->serializeTo(sdoc, rdata);
		 break;
	case AosActionType::eDeleteGroupDoc:
		 reduce_creator = OmnNew AosReduceDeleteGroupDoc(type, physical_id, info_list); 
		 reduce_creator->serializeTo(sdoc, rdata);
		 break;

	default:
		 OmnAlarm << "internal_error" << enderr;
	}
	return sdoc;
}


AosXmlTagPtr
AosReduceTaskCreator::create(
		const AosRundataPtr &rdata,
		const int physical_id,
		const AosXmlTagPtr &datacol_tag,
		list<AosXmlTagPtr> &info_list)
{
	AosXmlTagPtr sdoc; 
	OmnString type = AOSACTTYPE_MERGEFILE;
	AosReduceTaskCreator * reduce_creator;
	reduce_creator = OmnNew AosReduceMergeFile(type, physical_id, datacol_tag, info_list); 
	reduce_creator->serializeTo(sdoc, rdata);
	return sdoc;
}

bool	
AosReduceTaskCreator::create(
		const AosRundataPtr &rdata,
		const AosJobObjPtr &job,
		const AosLogicTaskObjPtr &logic_task,
		const AosXmlTagPtr &logic_task_conf)
{
	OmnTagFuncInfo << endl;
	aos_assert_r(logic_task_conf, false);

	AosXmlTagPtr reduce_task_doc;
	int physical_id = 0; //default value and up to change
	bool rslt;
	map<OmnString, ReduceConf> reduce_map;

	//parse to get reduce_map
	rslt = parse(rdata, logic_task_conf, reduce_map);
	aos_assert_r(rslt, false);

	//set infoList to be empty at the beginning
	list<AosXmlTagPtr> infoList;
	infoList.clear();

	//get reduce task actions
	AosXmlTagPtr reduce_task_conf = logic_task_conf->getFirstChild("reduce_task");
	if(!reduce_task_conf)
	{
		AosSetEntityError(rdata, "miss_reduce_task_conf", logic_task_conf->getTagname(), 
				AOSTAG_DATA_COLLECTORS) << logic_task_conf << enderr;
		return 0;
	}

	AosXmlTagPtr act_conf = reduce_task_conf->getFirstChild("actions");
	if(!reduce_task_conf)
	{
		AosSetEntityError(rdata, "miss_reduce_task_conf", logic_task_conf->getTagname(), 
				AOSTAG_DATA_COLLECTORS) << logic_task_conf << enderr;
		return 0;
	}

	//get max number thread
	int max_num_thrd = logic_task_conf->getAttrInt(AOSTAG_MAXNUMTHREAD, 1);

	OmnString reduce_task_id;
	bool needSnapshot; 
	OmnString snapshotStr; 
	int i = 0;
	AosXmlTagPtr dataproc_conf = act_conf->getFirstChild("dataproc");
	while (dataproc_conf)
	{
		OmnTagFuncInfo << endl;
		reduce_task_id = logic_task->getLogicId();
		reduce_task_id << "_" << AOSTASKNAME_REDUCETASK << "_" << i;

		needSnapshot = dataproc_conf->getAttrBool("need_snapshot", false);
		snapshotStr = needSnapshot?"true":"false";

		OmnTagFuncInfo << "create job or service reduce task: "
			<< job->getServiceName() << endl;
		if (!job->isService())
			reduce_task_doc = create(rdata, reduce_map, physical_id, dataproc_conf, infoList);
		else
			reduce_task_doc = create(rdata, dataproc_conf);

		aos_assert_r(reduce_task_doc.getPtr(), false);
		modifyReduceTaskDoc(rdata, reduce_task_doc, 
				job, logic_task->getLogicId(), 
				reduce_task_id, physical_id,
				max_num_thrd, snapshotStr);

		//add the doc into task list
		OmnTagFuncInfo << "The reduce task doc is: " << reduce_task_doc->toString() << endl;

		//add the task to downstream map
		u64 docid = reduce_task_doc->getAttrU64(AOSTAG_DOCID, 0);
		AosXmlTagPtr datacol_conf = dataproc_conf->getFirstChild("datacollector");
		OmnString data_col_id = datacol_conf->getAttrStr(AOSTAG_NAME);

		job->addDownStream(data_col_id, docid);

		//add the task to logic task maps 
		rslt = logic_task->addTask(rdata, reduce_task_doc);
		aos_assert_r(rslt, false);

		//go to the next node
		dataproc_conf = act_conf->getNextChild();
		i++;
	}

	return true;
}

bool
AosReduceTaskCreator::modifyReduceTaskDoc(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &reduce_task_doc,
		const AosJobObjPtr &job,
		const OmnString &logic_task_id,
		const OmnString &new_task_id,
		const int physical_id,
		const int max_num_thrd,
		const OmnString snapshotStr)
{
	aos_assert_r(job, 0);
	OmnString str;

	reduce_task_doc->setAttr(AOSTAG_TASKID, new_task_id);
	reduce_task_doc->setAttr(AOSTAG_LOGICID, logic_task_id);
	reduce_task_doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_TASK);
	reduce_task_doc->setAttr(AOSTAG_STYPE, AOSSTYPE_TASK);
	reduce_task_doc->setAttr(AOSTAG_TASK_TYPE, AOSTASKNAME_REDUCETASK);
	reduce_task_doc->setAttr(AOSTAG_JOB_DOCID, job->getJobDocid());
	reduce_task_doc->setAttr(AOSTAG_JOBSERVERID, job->getJobPhysicalId());
	reduce_task_doc->setAttr(AOSTAG_ISPUBLIC, job->isJobPublic());
	reduce_task_doc->setAttr(AOSTAG_CTNR_PUBLIC , job->isJobPublic());
	reduce_task_doc->setAttr(AOSTAG_PUBLIC_DOC , job->isJobPublic());
    //reduce_task_doc->setAttr(AOSTAG_TASK_SDOC_OBJID, task_sdoc_objid);
	reduce_task_doc->setAttr(AOSTAG_SERVICE_NAME, job->getServiceName());
	reduce_task_doc->setAttr(AOSTAG_PHYSICALID, physical_id);
	reduce_task_doc->setAttr(AOSTAG_PARENTC, job->getTaskContainer());
	reduce_task_doc->setAttr(AOSTAG_LOG_CONTAINER, job->getLogContainer());
	reduce_task_doc->setAttr(AOSTAG_TASK_VERSION, job->getVersion());
    reduce_task_doc->setAttr(AOSTAG_MAXNUMTHREAD, max_num_thrd);
    reduce_task_doc->setAttr(AOSTAG_STATUS, AOSTASKSTATUS_STOP);
    reduce_task_doc->setAttr(AOSTAG_START_TIME, "0");
    reduce_task_doc->setAttr(AOSTAG_START_STAMP, "0");
    reduce_task_doc->setAttr(AOSTAG_END_TIME, "0");
    reduce_task_doc->setAttr(AOSTAG_PROGRESS, "0");
    reduce_task_doc->setAttr("need_snapshot", snapshotStr);

	str = "";
	str << "<" << AOSTAG_ENV << ">";
	map<OmnString, OmnString> & task_env = job->getJobENV();
	map<OmnString, OmnString>::iterator itr = task_env.begin();
	while(itr != task_env.end())
	{
		str << "<entry zky_key=\"" << itr->first << "\">"
			   << "<![CDATA[" << itr->second << "]]>"
			   << "</entry>";
		itr++;
	}

	str << "</" << AOSTAG_ENV << ">";
	AosXmlTagPtr envNode = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs); 
	aos_assert_r(envNode, false);

	//insert env node into the task doc
	reduce_task_doc->addNode(envNode);
	reduce_task_doc = AosCreateDoc(reduce_task_doc->toString(), job->isJobPublic(), rdata); 
	OmnTagFuncInfo << "modify the reduce task doc to be complete" << endl;

	return true;
}

void 
AosReduceTaskCreator::setInfoList(list<AosXmlTagPtr> infoList)
{
	mInfoList = infoList;
}

list<AosXmlTagPtr> 
AosReduceTaskCreator::getInfoList()
{
	return mInfoList;
}

void
AosReduceTaskCreator::initInfoList()
{
	mInfoList.clear();
}

#endif
