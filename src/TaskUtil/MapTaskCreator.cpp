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
#include "TaskUtil/MapTaskCreator.h"

#include "API/AosApi.h"
#include "Dataset/DatasetSplit.h"
#include "Rundata/Rundata.h"
#include "Porting/Sleep.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/ActionObj.h"
#include "Debug/Debug.h"

AosMapTaskCreator::AosMapTaskCreator()
{
}


AosMapTaskCreator::~AosMapTaskCreator()
{
}


bool
AosMapTaskCreator::checkConfigStatic(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		set<OmnString> &data_col_ids)
{
	aos_assert_r(logic_task_conf, false);
	bool rslt;

	rslt = checkInputDatasetConf(rdata, logic_task_conf);
	aos_assert_r(rslt, false);
	
	rslt = checkDataCollectorConf(rdata, logic_task_conf, data_col_ids);
	aos_assert_r(rslt, false);

	rslt = checkMapTaskActionsConf(rdata, logic_task_conf, data_col_ids);	
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosMapTaskCreator::checkInputDatasetConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	AosXmlTagPtr datasets_conf = getInputDatasetsTag(rdata, conf);
	aos_assert_r(datasets_conf, false);

	AosXmlTagPtr dataset_conf = datasets_conf->getFirstChild("dataset");
	OmnTagFuncInfo << endl;
	while(dataset_conf)
	{
		dataset_conf = AosDatasetObj::getDatasetConfig(rdata, dataset_conf);
		aos_assert_r(dataset_conf, false);
		dataset_conf = datasets_conf->getNextChild();

		OmnTagFuncInfo << endl;
	}
	
	OmnScreen << "!!!!!!!!!! Dataset need check config func." << endl;
	return true;
}

	
bool
AosMapTaskCreator::checkDataCollectorConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		set<OmnString> &collector_ids)
{
	// Ketty 2014/08/22
	OmnNotImplementedYet;
	return false;
	/*
	AosXmlTagPtr data_col_tags = getDataColConf(rdata, logic_task_conf, 0);
	aos_assert_r(data_col_tags, false);
	
	OmnString data_colid;
	AosXmlTagPtr data_col_conf = data_col_tags->getFirstChild(true);
	OmnTagFuncInfo << endl;
	while (data_col_conf)
	{
		data_colid = data_col_conf->getAttrStr(AOSTAG_NAME);
		OmnTagFuncInfo << "data_colid is: " << data_colid << endl;
		if(collector_ids.find(data_colid) != collector_ids.end())
		{
			AosSetError(rdata, "internal_error") << enderr;
			return false;
		}
		collector_ids.insert(data_colid);
		data_col_conf = data_col_tags->getNextChild();
	}

	return true;	
	*/
}


bool
AosMapTaskCreator::checkMapTaskActionsConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		set<OmnString> &data_col_ids)
{
	bool rslt;
	AosXmlTagPtr task_action_tags = getMapTaskActionsConf(rdata, logic_task_conf);
	if(!task_action_tags)
	{
		AosSetEntityError(rdata, "miss_task_actions",
			logic_task_conf->getTagname(), "")
			<< logic_task_conf << enderr;
		return false;
	}
	
	AosXmlTagPtr act_tag = task_action_tags->getFirstChild(true);
	if(!act_tag)
	{
		AosSetEntityError(rdata, "task_action_empty",
			logic_task_conf->getTagname(), "")
			<< logic_task_conf << enderr;
		return false;
	}

	AosActionObjPtr action;
	while (act_tag)
	{
		action = AosActionObj::getAction(act_tag, rdata);
		if (!action)
		{
			AosSetErrorU(rdata, "missing_actions:") << act_tag->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			AosSetEntityError(rdata, "task_action_type_error",
				act_tag->getTagname(), "")
				<< act_tag << enderr;
			return false;
		}
		
		OmnTagFuncInfo << "act tag name is: " << act_tag->getTagname() << endl;
		rslt = action->checkConfig(rdata, act_tag, data_col_ids);
		if (!rslt)
		{
			AosSetErrorUser(rdata, "maptask_action config error") << act_tag<< enderr;
			return false;
		}

		act_tag = task_action_tags->getNextChild();
	}

	return true;
}

bool
AosMapTaskCreator::create(
		const AosRundataPtr &rdata,
		const AosJobObjPtr &job,
		const AosLogicTaskObjPtr &logic_task,
		const AosXmlTagPtr &logic_task_conf)
{
	OmnTagFuncInfo << endl;

	// This func is the new verstion for
	aos_assert_r(logic_task_conf, false);

	//AosXmlTagPtr data_col_conf = getDataColConf(rdata, logic_task_conf);
	//aos_assert_r(data_col_conf, false);
	
	AosXmlTagPtr map_task_action_conf = getMapTaskActionsConf(rdata, logic_task_conf);
	aos_assert_r(map_task_action_conf, false);
	
	OmnString task_sdoc_objid = logic_task_conf->getAttrStr(AOSTAG_NAME); 
	aos_assert_r(task_sdoc_objid != "", false);
	
	int max_num_thrd = logic_task_conf->getAttrInt(AOSTAG_MAXNUMTHREAD, 1);	
	OmnString serviceName = job->getServiceName();

	AosXmlTagPtr datasets_conf = getInputDatasetsTag(rdata, logic_task_conf);
	aos_assert_r(datasets_conf, false);

	vector<AosXmlTagPtr> datasets_confs;

	bool rslt;
	if (serviceName == "")
	{
		rslt = AosDatasetSplit::staticSplit(
			rdata, datasets_conf, datasets_confs, job->getJobDocid());
		aos_assert_r(rslt, false);
		aos_assert_r(!datasets_confs.empty(), false);
	} else {
		//no data split for streaming mode in task creation stage
		datasets_confs.push_back(datasets_conf);
	}

	OmnString map_task_id;
	OmnString ds_conf_unit;
	AosXmlTagPtr map_task_doc;
	int physical_id = 0;

	AosXmlTagPtr data_col_conf;
	for (u32 i=0; i<datasets_confs.size(); i++)
	{
		physical_id = getPhysicalId(rdata, datasets_confs[i]); 
		if (physical_id < 0)
		{
			//omnstreaming mode, the dataset may not be ready
			physical_id = rand() % AosGetNumPhysicals();
		}

		aos_assert_r(physical_id >= 0, false);

		ds_conf_unit = datasets_confs[i]->toString();
		aos_assert_r(ds_conf_unit != "", false);

		map_task_id = logic_task->getLogicId();
		map_task_id << "_" << AOSTASKNAME_MAPTASK << "_" << i;
	
		//data_col_str = getDataColConfStr(data_col_conf, datasets_conf[i]);
		data_col_conf = getDataColConf(rdata, 
				logic_task->getDataColTags(), datasets_confs[i]);
		aos_assert_r(data_col_conf, false);
	
		map_task_doc = createTaskDoc(
			rdata, job, logic_task->getLogicId(), map_task_id,
			task_sdoc_objid, max_num_thrd, physical_id,
			ds_conf_unit, data_col_conf->toString(), map_task_action_conf);

		rslt = logic_task->addTask(rdata, map_task_doc);
		aos_assert_r(rslt, false);
		OmnTagFuncInfo << "create a map task for dataset: " << ds_conf_unit << endl;
	}

	u64 docid = map_task_doc->getAttrU64(AOSTAG_DOCID, 0);
	if (serviceName != "")
	{
		//use datasets_conf instead of datasets_confs since it include
		//only one input_datasets before split
		AosXmlTagPtr dataset_conf = datasets_conf->getFirstChild("dataset");
		OmnString name;
		while (dataset_conf)
		{
			//name = dataset_conf->getAttrStr("zky_name", "");
			//if (name == "")
				name = dataset_conf->getAttrStr("zky_dataset_name", "");

			if (name != "")
			{
				//add the task to downstream map
				job->addDownStream(name, docid);
			}
			dataset_conf = datasets_conf->getNextChild("dataset");
		}
	}

	return true;
}


AosXmlTagPtr
AosMapTaskCreator::getInputDatasetsTag(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	AosXmlTagPtr datasets_conf = conf->getFirstChild("input_datasets");
	if(!datasets_conf)
	{
		AosSetEntityError(rdata, "miss_dataset_conf", conf->getTagname(), "input_datasets")
			<< conf << enderr;
		return 0;
	}
	return datasets_conf;
}


AosXmlTagPtr
AosMapTaskCreator::getDataColConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &orig_datacol_confs,
		const AosXmlTagPtr &dataset_conf)
{
	aos_assert_r(orig_datacol_confs && dataset_conf, 0);	
	
	OmnString str = "<";
	str << AOSTAG_DATA_COLLECTORS << "/>";
	AosXmlParser parser;
	AosXmlTagPtr new_datacol_confs = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(new_datacol_confs, 0);

	AosXmlTagPtr each_conf = orig_datacol_confs->getFirstChild("datacollector");
	aos_assert_r(each_conf, 0);

	OmnString grp_type;
	while(each_conf)
	{
		grp_type = each_conf->getAttrStr("group_type", "");
		if(grp_type != "cube_assemble")
		{
			new_datacol_confs->addNode(each_conf);
			each_conf = orig_datacol_confs->getNextChild();
			continue;	
		}
		
		int expect_cube_id = getCubeIdFromDataset(rdata, dataset_conf);
		aos_assert_r(expect_cube_id != -1, 0);
		
		int crt_cube_id = -1;
		if(!each_conf->getAttrBool("is_parent"))
		{
			crt_cube_id = each_conf->getAttrInt(AOSTAG_CUBE_ID, -1);
			aos_assert_r(crt_cube_id != -1, 0);
		}

		if(expect_cube_id == crt_cube_id)
		{
			new_datacol_confs->addNode(each_conf);
		}
		
		each_conf = orig_datacol_confs->getNextChild();
	}
	
	return new_datacol_confs;
}


/*
AosXmlTagPtr
AosMapTaskCreator::getDataColConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &logic_task_conf,
		const AosXmlTagPtr &dataset_conf)
{
	OmnString datacol_str = "<";
	datacol_str << AOSTAG_DATA_COLLECTORS << "/>";
	AosXmlParser parser;
	AosXmlTagPtr data_col_tags = parser.parse(datacol_str, "" AosMemoryCheckerArgs);
	aos_assert_r(data_col_tags, 0);

	AosXmlTagPtr reduce_task_conf = logic_task_conf->getFirstChild("reduce_task");
	aos_assert_r(reduce_task_conf, 0);

	AosXmlTagPtr actions_conf = reduce_task_conf->getFirstChild("actions");
	aos_assert_r(actions_conf, 0);

	AosXmlTagPtr dataproc_conf = actions_conf->getFirstChild("dataproc");

	AosXmlTagPtr datacol_conf;
	while(dataproc_conf)
	{
		datacol_conf = dataproc_conf->getFirstChild("datacollector");
		aos_assert_r(datacol_conf, 0);

		datacol_conf = datacol_conf->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(datacol_conf, 0);

		OmnString type = datacol_conf->getAttrStr("group_type", "");
		// Ketty 2014/08/14
		//if (type == "physical_group")
		if (type == "physical_group" || type == "cube_shuffle")
		{
			bool rslt = getGroupDataCol(type, data_col_tags, datacol_conf);
			aos_assert_r(rslt, 0);
		}
		else
		{
			if(type == "cube_assemble" && dataset_conf)
			{
				// Ketty 2014/08/21
				int cube_id = dataset_conf->getAttrInt(AOSTAG_CUBE_ID, -1);
				aos_assert_r(cube_id != -1, false);
		
				OmnString datacol_id = datacol_conf->getAttrStr(AOSTAG_NAME);
				aos_assert_r(datacol_id != "", false);	
				datacol_id << "_" << cube_id;
				datacol_conf->setAttrStr(AOSTAG_NAME, datacol_id);
			}
			data_col_tags->addNode(datacol_conf);
		}

		OmnTagFuncInfo << "add one datacol_conf to Map Task. Conf is: "
			<< datacol_conf << endl;
		dataproc_conf = actions_conf->getNextChild();
	}

	return data_col_tags;
}

bool
AosMapTaskCreator::getGroupDataCol(
		const OmnString &grp_type,  // Ketty 2014/08/14
		const AosXmlTagPtr &data_col_tags,
		const AosXmlTagPtr &datacol_conf)
{
	OmnString tmp_colid, datacol_id, rcd_name;
	AosXmlTagPtr iilasm, record;

	// Ketty 2014/08/19
	//vector<u32> svr_ids = AosGetServerIds();		
	//aos_assert_r(svr_ids.size() > 0, false);
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

	//for(size_t i = 0; i< svr_ids.size(); i++)
	for(u32 i = 0; i< grp_num; i++)
	{
		AosXmlTagPtr tag = datacol_conf->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(tag, false);

		datacol_id = tag->getAttrStr(AOSTAG_NAME);
		aos_assert_r(datacol_id != "", false);

		tag->setAttr("zky_old_datacolid", datacol_id);

		datacol_id << "_" << i;
		tag->setAttr(AOSTAG_NAME, datacol_id);

		iilasm = tag->getFirstChild("asm");
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
			rcd_name = datacol_id;
		}
		record->setAttr("zky_name", rcd_name);
		data_col_tags->addNode(tag);
	}
	return true;
}
*/


AosXmlTagPtr
AosMapTaskCreator::getMapTaskActionsConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, 0);
	AosXmlTagPtr map_task_conf = conf->getFirstChild("map_task");
	if(!map_task_conf)
	{
		AosSetEntityError(rdata, "miss_map_task_conf", conf->getTagname(), AOSTAG_DATA_COLLECTORS)
			<< conf << enderr;
		return 0;
	}

	AosXmlTagPtr act_conf = map_task_conf->getFirstChild("actions");
	if(!map_task_conf)
	{
		AosSetEntityError(rdata, "miss_map_task_conf", map_task_conf->getTagname(), AOSTAG_DATA_COLLECTORS)
			<< conf << enderr;
		return 0;
	}

	return act_conf;
}


AosXmlTagPtr
AosMapTaskCreator::createTaskDoc(
		const AosRundataPtr &rdata,
		const AosJobObjPtr &job,
		const OmnString &logic_task_id,
		const OmnString &new_task_id,
		const OmnString &task_sdoc_objid,
		const int max_num_thrd,
		const int physical_id,
		const OmnString &dataset_conf,
		const OmnString &data_col_conf,
		const AosXmlTagPtr &task_action_conf)
{
	aos_assert_r(job, 0);
	OmnString docstr;
	docstr << "<task "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_TASK << "\" "
		<< AOSTAG_STYPE << "=\"" << AOSSTYPE_TASK << "\" "
		<< AOSTAG_TASK_TYPE << "=\"" << AOSTASKNAME_MAPTASK << "\" "
		<< AOSTAG_TASKID << "=\"" << new_task_id << "\" "
		<< AOSTAG_LOGICID << "=\"" << logic_task_id << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << physical_id << "\" "
		<< AOSTAG_JOB_DOCID << "=\"" << job->getJobDocid() << "\" "
		<< AOSTAG_SERVICE_NAME << "=\"" << job->getServiceName() << "\" "
		<< AOSTAG_JOBSERVERID << "=\"" << job->getJobPhysicalId() << "\" "
		<< AOSTAG_ISPUBLIC << "=\"" << job->isJobPublic() << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"" << job->isJobPublic() << "\" "
		<< AOSTAG_PUBLIC_DOC << "=\"" << job->isJobPublic() << "\" "
		<< AOSTAG_TASK_SDOC_OBJID << "=\"" << task_sdoc_objid << "\" " 
		<< AOSTAG_PARENTC << "=\"" << job->getTaskContainer() << "\" "
		<< AOSTAG_LOG_CONTAINER << "=\"" << job->getLogContainer() << "\" "
		<< AOSTAG_TASK_VERSION << "=\"" << job->getVersion() << "\" "
		<< AOSTAG_MAXNUMTHREAD << "=\"" << max_num_thrd << "\" "
		<< AOSTAG_STATUS << "=\"" << AOSTASKSTATUS_STOP << "\" "
		<< AOSTAG_START_TIME << "=\"0\" "
		<< AOSTAG_START_STAMP << "=\"0\" "
		<< AOSTAG_END_TIME << "=\"0\" "
		<< AOSTAG_PROGRESS << "=\"0\" ";

	// Linda 2014/05/09
	bool need_snapshot = false;
	if (task_action_conf) 
	{
		AosXmlTagPtr action_conf = task_action_conf->getFirstChild();	
		aos_assert_r(action_conf, 0);

		need_snapshot = action_conf->getAttrBool("need_snapshot", false);
	}
	docstr << "need_snapshot" << "=\"" << (need_snapshot?"true":"false");
	docstr << "\" >";

	docstr << "<" << AOSTAG_ENV << ">";
	map<OmnString, OmnString> & task_env = job->getJobENV();
	map<OmnString, OmnString>::iterator itr = task_env.begin();
	while(itr != task_env.end())
	{
		docstr << "<entry zky_key=\"" << itr->first << "\">"
			   << "<![CDATA[" << itr->second << "]]>"
			   << "</entry>";
		itr++;
	}
	docstr << "</" << AOSTAG_ENV << ">";

	if(dataset_conf != "")	docstr << dataset_conf;	
	if(data_col_conf != "") docstr << data_col_conf;
	if(task_action_conf) docstr << task_action_conf->toString();
	docstr << "</task>";

	AosXmlTagPtr task_doc = AosCreateDoc(docstr, job->isJobPublic(), rdata);
	aos_assert_r(task_doc, 0);
	task_doc = task_doc->clone(AosMemoryCheckerArgsBegin);
	OmnTagFuncInfo << "Create one map task doc: " << task_doc->toString() << endl;
	return task_doc;
}


/*
OmnString
AosMapTaskCreator::getDataColConfStr(const AosXmlTagPtr data_col_conf)
{
	// replace data_col_conf's tagname with AOSTAG_DATA_COLLECTORS.
	OmnString tag_name = data_col_conf->getTagname();
	if(data_col_conf->isRootTag())
	{
		AosXmlTagPtr first_child = data_col_conf->getFirstChild();
		aos_assert_r(first_child, "");
		tag_name = first_child->getTagname();
	}
	OmnString data_col_conf_str = data_col_conf->toString();
	if(tag_name != AOSTAG_DATA_COLLECTORS)
	{
		// replace the tag name.
		int beg_loc = data_col_conf_str.findSubString(tag_name, 0);
		aos_assert_r(beg_loc >=0, "");
		data_col_conf_str.replace(beg_loc, tag_name.length(), AOSTAG_DATA_COLLECTORS);	
		
		int end_loc = data_col_conf_str.findSubString(tag_name, data_col_conf_str.length()-tag_name.length() - 10);
		//aos_assert_r(end_loc > 0, "");
		if (end_loc > 0)
		{
			data_col_conf_str.replace(end_loc, tag_name.length(), AOSTAG_DATA_COLLECTORS);	
		}
	}
	
	return data_col_conf_str;
}
*/

int
AosMapTaskCreator::getPhysicalId(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &datasets_conf)
{
	//<dataset>
	//	<datascanner>
	//		<dataconnector AOSTAG_PHYSICALID="1">
	//			<versions>
	//				<ver_0><![CDATA[]]></ver_0>
	//			</versions>
	//		</dataconnector>
	// </datascanner>
	//	<dataschema>
	//		<datarecord>
	//			<datafields>
	//				<datafield/>
	//			</datafields>
	//		</datarecord>
	//	</dataschema>
	//</dataset>
	aos_assert_r(datasets_conf, -1);

	AosXmlTagPtr dataset_conf = datasets_conf->getFirstChild("dataset");
	aos_assert_r(dataset_conf, -1);

	dataset_conf = AosDatasetObj::getDatasetConfig(rdata, dataset_conf);
	aos_assert_r(dataset_conf, -1);

	AosXmlTagPtr data_scanner_conf = dataset_conf->getFirstChild("datascanner");
	if (!data_scanner_conf)
	{
		int physical_id = rand() % AosGetNumPhysicals();
		return physical_id;
	}

	AosXmlTagPtr data_conn_conf = data_scanner_conf->getFirstChild("dataconnector");
	aos_assert_r(data_conn_conf, -1);

	int physical_id = data_conn_conf->getAttrInt(AOSTAG_PHYSICALID, -1); 
	if (physical_id < 0)
	{
		physical_id = rand() % AosGetNumPhysicals();
	}
	return physical_id;
}

int
AosMapTaskCreator::getCubeIdFromDataset(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &datasets_conf)
{
	aos_assert_r(datasets_conf, -1);

	AosXmlTagPtr dataset_conf = datasets_conf->getFirstChild("dataset");
	aos_assert_r(dataset_conf, -1);

	dataset_conf = AosDatasetObj::getDatasetConfig(rdata, dataset_conf);
	aos_assert_r(dataset_conf, -1);

	AosXmlTagPtr data_scanner_conf = dataset_conf->getFirstChild("datascanner");
	aos_assert_r(data_scanner_conf, -1);

	AosXmlTagPtr data_conn_conf = data_scanner_conf->getFirstChild("dataconnector");
	aos_assert_r(data_conn_conf, -1);

	int cube_id = data_conn_conf->getAttrInt(AOSTAG_CUBE_ID, -1); 
	return cube_id ;
}

#endif
