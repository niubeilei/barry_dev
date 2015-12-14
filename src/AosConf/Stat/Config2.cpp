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
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "AosConf/Stat/StatConf.h"

#include "AosConf/DataEngine.h"
#include "AosConf/DataEngineScanMul.h"
#include "AosConf/DataEngineJoinNew.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataProcJoin.h"
#include "AosConf/DataProcStatIndex.h"
#include "AosConf/DataProcStatDoc.h"
#include "AosConf/DataProcStatDocShuffle.h"
#include "AosConf/DataConnectorIIL.h"
#include "AosConf/ReduceTaskDataProc.h"
#include "AosConf/ReduceTask.h"
#include "AosConf/MapTask.h"
#include "AosConf/DataSet.h"
#include "AosConf/JobTask.h"
#include "AosConf/Job.h"
#include "SEUtil/IILName.h"
#include <boost/make_shared.hpp>
#include <string>

using boost::make_shared;
using AosConf::StatConf;
using AosConf::JobTask;

boost::shared_ptr<JobTask> 
StatConf::createConfig2() {
	OmnString pre_name = mStatDocName;
	pre_name << "_task002_";

	OmnString task_name = mStatDocName;
	if(mIsDistinct && mStatConfLevel == "0-1")
	{
		task_name << "_distinct_" << mInternalId << "_task002";
	}
	else
	{
		task_name << "_" << mInternalId << "_task002";
	}
	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	jobtask->setAttribute("zky_taskid", "task002");

	//ouputds_names
	vector<OmnString> outputds_names;
	vector<OmnString> outputproc_names;
	vector<OmnString> outputiil_names;
	vector<OmnString> outputcol_names;
	vector<OmnString> outputrcd_names;
	for(size_t i = 0; i < mStatFields.size(); i++)
	{
		OmnString temp = pre_name;
		temp << "dataset_" << mStatFields[i];
		outputds_names.push_back(temp);

		temp = pre_name;
		temp << "proc_" << mStatFields[i];
		outputproc_names.push_back(temp);

		//temp = "_zt44_";
		//temp << mStatDocName << "_key_" << mStatFields[i];
		temp = AosIILName::composeStatKeyIILName(mStatisticDocObjid, mStatIdentifyKey, mStatFields[i]);
		outputiil_names.push_back(temp);

		temp = pre_name;
		temp << "datacollector_" << mStatFields[i];
		outputcol_names.push_back(temp);

		temp = pre_name;
		temp << "rcd_" << mStatFields[i] << "_iil";
		outputrcd_names.push_back(temp);
	}

	mTask2FileDefName = pre_name;
	mTask2FileDefName << "_" << mInternalId << "_statdocdata_files";

	OmnString output_all_dsname = pre_name;
	output_all_dsname << "dataset_all";

	//mOutputIILNameAll = "_zt4k_";
	//mOutputIILNameAll << mStatDocName << "__all_level_" << mInternalId << "__$group";
	mOutputIILNameAll = "_zt4k_";
	mOutputIILNameAll << mStatisticDocObjid << "_" << mStatIdentifyKey  << "__$group";

	OmnString output_all_rcdname = pre_name;
	output_all_rcdname << "rcd_all_iil";

	OmnString output_statinternalid_dsname = pre_name;
	output_statinternalid_dsname << "dataset_statinternalid";

	OmnString output_statinternalid_rcdname = pre_name;
	output_statinternalid_rcdname << "rcd_statinternalid_iil";
	
	OmnString output_dsname = pre_name;
	if(mIsDistinct && mStatConfLevel == "0-1")
	{
		output_dsname << "_isdistinct_" << mInternalId << "_statdocdata";
	}
	else
	{
		output_dsname << "_" << mInternalId << "_statdocdata";
	}
	mStatDocDataDsName = output_dsname;

	OmnString output_rcd_name = mStatDocName;
	output_rcd_name << "_task004_rcd_buff_input";
	mStatDocDataRcdName = output_rcd_name;

	OmnString output_schema_name = mStatDocName;
	output_schema_name << "_task004_schema";

	OmnString output_scanner_name = mStatDocName;
	output_scanner_name << "_task004_scanner";
	
	OmnString df_rcd_name = pre_name;
	df_rcd_name << "rcd_statdocdata";

	OmnString df_connector_name = pre_name;
	df_connector_name << "datacollector_statdocdata";

	OmnString df_dataproc_name = pre_name;
	df_dataproc_name << "proc_statdocdata";

	//OmnString df_iilname = "_zt44_";
	//df_iilname << mStatDocName << "_task003_statdocdata";

	//create lhs_input_dataset
	//OmnString lhs_input_dsname = mOdsAllDatasetName;
	//OmnString lhs_inputrcd_name = mOutputRcdNameAll;
	//boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	//lhs_inputds->setAttribute("zky_dataset_name", lhs_input_dsname);
	
	//create lhs_input_dataset
	boost::shared_ptr<DataSplit> ds_split = boost::make_shared<DataSplit>();
	ds_split->setAttribute("jimo_objid", "dataspliter_filesbycube_jimodoc_v0");

	boost::shared_ptr<DataConnector> ds_connector = boost::make_shared<DataConnector>();
	ds_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");
	ds_connector->setAttribute("zky_file_defname", mFileDefName);
	ds_connector->setSplit(ds_split);

	boost::shared_ptr<DataScanner> ds_scanner2 = boost::make_shared<DataScanner>();
	ds_scanner2->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	ds_scanner2->setAttribute("zky_name", "example1_task003_scanner");
	ds_scanner2->setAttribute("buff_cache_num", "9");
	ds_scanner2->setConnector(ds_connector);
	
	boost::shared_ptr<DataFieldStr> outds_df1 = boost::make_shared<DataFieldStr>();
	outds_df1->setAttribute("zky_name", "str");
	outds_df1->setAttribute("type", "str");
	outds_df1->setAttribute("zky_offset", "0");
	outds_df1->setAttribute("zky_length", mMaxLen);
	outds_df1->setAttribute("zky_datatooshortplc", "cstr");
	
	boost::shared_ptr<DataRecord> ds_record2 = boost::make_shared<DataRecord>();
	ds_record2->setField(outds_df1);
	
	int output_rcd_len2 = mMaxLen;
	boost::shared_ptr<DataFieldStr> outds_dfday = boost::make_shared<DataFieldStr>();
	outds_dfday->setAttribute("zky_name", "time");
	outds_dfday->setAttribute("type", "bin_u64");
	outds_dfday->setAttribute("zky_offset", output_rcd_len2);
	outds_dfday->setAttribute("zky_length", "8");
	output_rcd_len2 += 8;
	ds_record2->setField(outds_dfday);

	OmnString value_name, type;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		type = "bin_int64";
		if (mMeasures[i].field_type == "double" || mMeasures[i].field_type == "number")
			type = "bin_double";

		value_name = "value";
		value_name << i;
		boost::shared_ptr<DataFieldStr> outds_dfvalue = boost::make_shared<DataFieldStr>();
		outds_dfvalue->setAttribute("zky_name", value_name);
		outds_dfvalue->setAttribute("type", type);
		outds_dfvalue->setAttribute("zky_offset", output_rcd_len2);
		outds_dfvalue->setAttribute("zky_length", "8");
		output_rcd_len2 += 8;
		ds_record2->setField(outds_dfvalue);
	}
	
	boost::shared_ptr<DataFieldStr> outds_cubeid = boost::make_shared<DataFieldStr>();
	outds_cubeid->setAttribute("zky_name", "cube_id");
	outds_cubeid->setAttribute("type", "bin_u64");
	outds_cubeid->setAttribute("zky_offset", output_rcd_len2);
	outds_cubeid->setAttribute("zky_length", "8");
	output_rcd_len2 += 8;
	ds_record2->setField(outds_cubeid);

	OmnString stat_definition_key = mStatDocName;
	stat_definition_key << "_" << mInternalId;
	boost::shared_ptr<DataFieldStr> outds_df2 = boost::make_shared<DataFieldStr>();
	outds_df2->setAttribute("zky_name", "docid");
	outds_df2->setAttribute("type", "statid");
	outds_df2->setAttribute("stat_definition_key", stat_definition_key);
	outds_df2->setAttribute("zky_num_ids", "1000");
	outds_df2->setAttribute("zky_value_from_field", "cube_id");
/*	
	boost::shared_ptr<DataFieldStr> outds_df3 = boost::make_shared<DataFieldStr>();
	outds_df3->setAttribute("zky_name", "statinternalid");
	outds_df3->setAttribute("type", "const");
	outds_df3->setConst(mInternalId);
*/	
	boost::shared_ptr<DataFieldStr> outds_df4 = boost::make_shared<DataFieldStr>();
	outds_df4->setAttribute("zky_name", "true");
	outds_df4->setAttribute("type", "const");
	outds_df4->setConst("1");
	
	boost::shared_ptr<DataFieldStr> outds_df5 = boost::make_shared<DataFieldStr>();
	outds_df5->setAttribute("zky_name", "false");
	outds_df5->setAttribute("type", "const");
	outds_df5->setConst("0");

	OmnString lhs_inputrcd_name = mStatDocName;
	lhs_inputrcd_name << "_task003_rcd_lhs_all";
	ds_record2->setField(outds_df2);
	//ds_record2->setField(outds_df3);
	ds_record2->setField(outds_df4);
	ds_record2->setField(outds_df5);
	ds_record2->setAttribute("type", "fixbin");
	ds_record2->setAttribute("zky_name", lhs_inputrcd_name);
	ds_record2->setAttribute("zky_length", output_rcd_len2);
	
	boost::shared_ptr<DataSchema> ds_schema2 = boost::make_shared<DataSchema>();
	ds_schema2->setAttribute("jimo_objid", "dataschema_unilength_jimodoc_v0");
	ds_schema2->setAttribute("zky_name", "example1_task003_schema");
	ds_schema2->setRecord(ds_record2);
	
	OmnString lhs_input_dsname = mStatDocName;
	lhs_input_dsname << "_" << mInternalId << "_all";
	boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	lhs_inputds->setAttribute("jimo_objid","dataset_bydatascanner_jimodoc_v0");
	lhs_inputds->setAttribute("zky_name", lhs_input_dsname);
	lhs_inputds->setScanner(ds_scanner2);
	lhs_inputds->setSchema(ds_schema2);

	//create rhs_inputds_datafields
	boost::shared_ptr<DataFieldStr> input_df1 = boost::make_shared<DataFieldStr>();
	input_df1->setAttribute("type", "str");
	input_df1->setAttribute("zky_name", "value");
	input_df1->setAttribute("zky_offset", "0");
	input_df1->setAttribute("zky_length", mMaxLen);
	input_df1->setAttribute("zky_datatooshortplc", "cstr");
	int input_rcd_len = mMaxLen;

	boost::shared_ptr<DataFieldStr> input_df2 = boost::make_shared<DataFieldStr>();
	input_df2->setAttribute("type", "bin_u64");
	input_df2->setAttribute("zky_name", "docid");
	input_df2->setAttribute("zky_offset", input_rcd_len);
	input_df2->setAttribute("zky_length", "8");
	input_rcd_len += 8;

	OmnString rhs_inputrcd_name = "example1_task002_rcd_rhs_iil";
	//create rhs_inputds_datarecord
	boost::shared_ptr<DataRecord> input_rcd = boost::make_shared<DataRecord>();
	input_rcd->setAttribute("type", "iil");
	input_rcd->setAttribute("zky_name", rhs_inputrcd_name);
	input_rcd->setAttribute("zky_length", input_rcd_len);
	input_rcd->setField(input_df1);
	input_rcd->setField(input_df2);

	//create rhs_inputds_dataschema
	boost::shared_ptr<DataSchema> input_schema = boost::make_shared<DataSchema>();
	input_schema->setAttribute("zky_name", "example1_task002_schema_rhs");
	input_schema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	input_schema->setRecord(input_rcd);

	//create rhs_inputds_dataconnector
	boost::shared_ptr<DataConnectorIIL> input_connector = boost::make_shared<DataConnectorIIL>();
	input_connector->setAttribute("jimo_objid", "dataconnector_iil_jimodoc_v0");
	input_connector->setAttribute("zky_iilname", mOutputIILNameAll); 
	input_connector->setAttribute("zky_blocksize", "2000000");
	input_connector->setQueryCond("an", "*"); 
	boost::shared_ptr<DataSplit> input_split = boost::make_shared<DataSplit>();
	input_split->setAttribute("jimo_objid", "datasplit_iilcubegroup_jimodoc_v0");
	input_connector->setSplit(input_split);

	//create rhs_inputds_datascanner
	boost::shared_ptr<DataScanner> input_scanner = boost::make_shared<DataScanner>();
	input_scanner->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	input_scanner->setAttribute("zky_name", "example1_task002_scanner_rhs");
	input_scanner->setAttribute("buff_cache_num", "1");
	input_scanner->setConnector(input_connector);

	//create rhs_input_dataset
	OmnString rhs_input_dsname = "rhs";
	boost::shared_ptr<DataSet> rhs_inputds = boost::make_shared<DataSet>();
	rhs_inputds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	rhs_inputds->setAttribute("zky_name", rhs_input_dsname);
	rhs_inputds->setScanner(input_scanner);
	rhs_inputds->setSchema(input_schema);
	
	//create output_dataset
	boost::shared_ptr<DataSet> output_ds_all = boost::make_shared<DataSet>();
	output_ds_all->setAttribute("zky_name", output_all_dsname);
	jobtask->setOutputDataset(output_ds_all);

	boost::shared_ptr<DataSet> output_ds_statinternalid = boost::make_shared<DataSet>();
	output_ds_statinternalid->setAttribute("zky_name", output_statinternalid_dsname);
	jobtask->setOutputDataset(output_ds_statinternalid);

	for(u32 i=0; i<outputds_names.size(); i++)
	{
		boost::shared_ptr<DataSet> output_ds = boost::make_shared<DataSet>();
		output_ds->setAttribute("zky_name",outputds_names[i]); 
		jobtask->setOutputDataset(output_ds);
	}
	
	boost::shared_ptr<DataSet> output_ds_statdocdata = boost::make_shared<DataSet>();
	output_ds_statdocdata->setAttribute("zky_name", output_dsname);
	jobtask->setOutputDataset(output_ds_statdocdata);
	
	//create reducetask
	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	
	//create reduce_task_dataproc1
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "str");
	asm_df1->setAttribute("zky_name", "str");
	asm_df1->setAttribute("zky_offset", "0");
	asm_df1->setAttribute("zky_length", mMaxLen);
	asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	int df_rcd_len1 = mMaxLen;

	boost::shared_ptr<DataFieldStr> asm_df2 = boost::make_shared<DataFieldStr>();
	asm_df2->setAttribute("type", "bin_u64");
	asm_df2->setAttribute("zky_name", "docid");
	asm_df2->setAttribute("zky_offset", df_rcd_len1);
	asm_df2->setAttribute("zky_length", "8");
	df_rcd_len1 += 8;

	boost::shared_ptr<DataRecord> rcdctnr_drall = boost::make_shared<DataRecord>();
	rcdctnr_drall->setAttribute("type", "fixbin");
	rcdctnr_drall->setAttribute("zky_name", output_all_rcdname);
	rcdctnr_drall->setAttribute("zky_length", df_rcd_len1);
	rcdctnr_drall->setField(asm_df1);
	rcdctnr_drall->setField(asm_df2);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr_all = boost::make_shared<DataRecordCtnr>();
	asm_dr_all->setRecord(rcdctnr_drall);
	asm_dr_all->setAttribute("zky_name", output_all_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf_all= boost::make_shared<ReduceTaskDataProc>("iilbatchopr"); 
	dpf_all->setAttribute("zky_iilname", mOutputIILNameAll); 
	dpf_all->setAttribute("datasetname", output_all_dsname);
	dpf_all->setAttribute("zky_name", output_all_dsname);
	dpf_all->setAttribute("run_mode", "run_child");
	//dpf_all->setAttribute("zky_name", "example1_task002_proc_all");

	dpf_all->setCollectorType("iil");
	dpf_all->setCollectorAttr("zky_name", output_all_dsname); 
	dpf_all->setCollectorAttr("group_type", "cube_assemble"); 
	//dpf_all->setCollectorAttr("zky_name", "example1_task002_datacollector_all"); 

	dpf_all->setAttribute(AOSTAG_OPERATOR, "strinc");
	dpf_all->setAssemblerType("strinc");
	dpf_all->setAssemblerAttr("zky_inctype", "set");

	boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", df_rcd_len1);
	cmp_fun->setCmpField("str", 0);
	cmp_fun->setAggrField("u64", df_rcd_len1-8, "set");
	dpf_all->setCompFun(cmp_fun);

	dpf_all->setRecord(asm_dr_all);
	rtask->setDataProc(dpf_all);
	
	//create reduce_task_dataproc2
	/*
	boost::shared_ptr<DataFieldStr> asm_df3 = boost::make_shared<DataFieldStr>();
	asm_df3->setAttribute("type", "str");
	asm_df3->setAttribute("zky_name", "str");
	asm_df3->setAttribute("zky_offset", "0");
	asm_df3->setAttribute("zky_length", "15");
	asm_df3->setAttribute("zky_datatooshortplc", "cstr");

	boost::shared_ptr<DataFieldStr> asm_df4 = boost::make_shared<DataFieldStr>();
	asm_df4->setAttribute("type", "bin_u64");
	asm_df4->setAttribute("zky_name", "docid");
	asm_df4->setAttribute("zky_offset", "16");
	asm_df4->setAttribute("zky_length", "8");

	boost::shared_ptr<DataRecord> rcdctnr_dr2 = boost::make_shared<DataRecord>();
	rcdctnr_dr2->setAttribute("type", "fixbin");
	rcdctnr_dr2->setAttribute("zky_name", output_statinternalid_rcdname);
	rcdctnr_dr2->setAttribute("zky_length", "24");
	rcdctnr_dr2->setField(asm_df3);
	rcdctnr_dr2->setField(asm_df4);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr2 = boost::make_shared<DataRecordCtnr>();
	asm_dr2->setRecord(rcdctnr_dr2);
	asm_dr2->setAttribute("zky_name", output_statinternalid_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf2= boost::make_shared<ReduceTaskDataProc>("iilbatchopr"); 
	dpf2->setAttribute("zky_iilname", output_statinternalid_iilname); 
	dpf2->setAttribute("datasetname", output_statinternalid_dsname); 
	dpf2->setAttribute("zky_name", output_statinternalid_dsname);
	//dpf2->setAttribute("zky_name", "example1_task002_proc_statinternalid");

	dpf2->setCollectorType("iil");
	dpf2->setCollectorAttr("zky_name", output_statinternalid_dsname);
	//dpf2->setCollectorAttr("zky_name", "example1_task002_datacollector_statinternalid");

	dpf2->setAssemblerType("stradd");
	dpf2->setRecord(asm_dr2);

	boost::shared_ptr<CompFun> fun = boost::make_shared<CompFun>("custom", 24);
	fun->setCmpField("str", 0);
	fun->setCmpField("u64", 16, 8);
	fun->setAggrField("u64", 16, "set");
	dpf2->setCompFun(fun);

	rtask->setDataProc(dpf2);
	*/
	//create reduce_task_dataproc3
	for(u32 i=0; i<outputds_names.size(); i++)
	{
		map<string, int>::iterator itr = mFieldLenMap.find(mStatFields[i]);
		aos_assert_r(itr != mFieldLenMap.end(),	boost::shared_ptr<JobTask> ());

		int rcd_len = itr->second;
		boost::shared_ptr<DataFieldStr> asm_df5 = boost::make_shared<DataFieldStr>();
		asm_df5->setAttribute("type", "str");
		asm_df5->setAttribute("zky_name", "str");
		asm_df5->setAttribute("zky_offset", "0");
		asm_df5->setAttribute("zky_length", rcd_len);
		asm_df5->setAttribute("zky_datatooshortplc", "cstr");

		boost::shared_ptr<DataFieldStr> asm_df6 = boost::make_shared<DataFieldStr>();
		asm_df6->setAttribute("type", "bin_u64");
		asm_df6->setAttribute("zky_name", "docid");
		asm_df6->setAttribute("zky_offset", rcd_len);
		asm_df6->setAttribute("zky_length", "8");
		rcd_len += 8;

		boost::shared_ptr<DataRecord> rcdctnr_dr3 = boost::make_shared<DataRecord>();
		rcdctnr_dr3->setAttribute("type", "fixbin");
		rcdctnr_dr3->setAttribute("zky_name", outputrcd_names[i]);
		rcdctnr_dr3->setAttribute("zky_length", rcd_len);
		rcdctnr_dr3->setField(asm_df5);
		rcdctnr_dr3->setField(asm_df6);
	
		boost::shared_ptr<DataRecordCtnr> asm_dr3 = boost::make_shared<DataRecordCtnr>();
		asm_dr3->setRecord(rcdctnr_dr3);
		asm_dr3->setAttribute("zky_name", outputrcd_names[i]);

		boost::shared_ptr<ReduceTaskDataProc> dpf3= boost::make_shared<ReduceTaskDataProc>("iilbatchopr"); 
		dpf3->setAttribute("zky_iilname", outputiil_names[i]); 
		dpf3->setAttribute("datasetname", outputds_names[i]); 
		dpf3->setAttribute("zky_name", outputproc_names[i]);

		dpf3->setCollectorType("iil");
		dpf3->setCollectorAttr("zky_name", outputcol_names[i]); 

		dpf3->setAttribute(AOSTAG_OPERATOR, "stradd");
		dpf3->setAssemblerType("stradd");
		dpf3->setAssemblerAttr("zky_inctype", "norm");

		boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", rcd_len);
		cmp_fun->setCmpField("str", 0);
		cmp_fun->setCmpField("u64", itr->second, 8);

		cmp_fun->setAggrField("u64", itr->second, "set");
		dpf3->setCompFun(cmp_fun);
		dpf3->setRecord(asm_dr3);
		rtask->setDataProc(dpf3);
	}	
	
	//change 2014/07/01
	//create reduce_task_dataproc4
	boost::shared_ptr<DataRecord> rcdctnr_dr = boost::make_shared<DataRecord>();
	
	mStatDocDataRcdLen = 0;
	mStatDocDataFields = "";
	boost::shared_ptr<DataFieldStr> new_asm_df1 = boost::make_shared<DataFieldStr>();
	new_asm_df1->setAttribute("type", "bin_u64");
	new_asm_df1->setAttribute("zky_name", "docid");
	new_asm_df1->setAttribute("zky_offset", "0");
	new_asm_df1->setAttribute("zky_length", "8");
	new_asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	mStatDocDataRcdLen += 8;
	rcdctnr_dr->setField(new_asm_df1);
	mStatDocDataFields << new_asm_df1->getConfig();

	boost::shared_ptr<DataFieldStr> new_asm_df2 = boost::make_shared<DataFieldStr>();
	new_asm_df2->setAttribute("type", "bin_u64");
	new_asm_df2->setAttribute("zky_name", "time");
	new_asm_df2->setAttribute("zky_offset", mStatDocDataRcdLen);
	new_asm_df2->setAttribute("zky_length", "8");
	mStatDocDataRcdLen += 8;
	rcdctnr_dr->setField(new_asm_df2);
	mStatDocDataFields << new_asm_df2->getConfig();

	OmnString vname;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		type = "bin_int64";
		if (mMeasures[i].field_type == "double" || mMeasures[i].field_type == "number")
			type = "bin_double";

		vname = "value";
		vname << i;
		boost::shared_ptr<DataFieldStr> new_asm_df3 = boost::make_shared<DataFieldStr>();
		new_asm_df3->setAttribute("type", type);
		new_asm_df3->setAttribute("zky_name", vname);
		new_asm_df3->setAttribute("zky_offset", mStatDocDataRcdLen);
		new_asm_df3->setAttribute("zky_length", "8");
		mStatDocDataRcdLen += 8;
		rcdctnr_dr->setField(new_asm_df3);
		mStatDocDataFields << new_asm_df3->getConfig();
	}

	boost::shared_ptr<DataFieldStr> new_asm_df4 = boost::make_shared<DataFieldStr>();
	new_asm_df4->setAttribute("type", "str");
	new_asm_df4->setAttribute("zky_name", "str");
	new_asm_df4->setAttribute("zky_offset", mStatDocDataRcdLen);
	new_asm_df4->setAttribute("zky_length", mMaxLen);
	new_asm_df4->setAttribute("zky_datatooshortplc", "cstr");
	mStatDocDataRcdLen += mMaxLen;
	rcdctnr_dr->setField(new_asm_df4);
	mStatDocDataFields << new_asm_df4->getConfig();
	
	boost::shared_ptr<DataFieldStr> new_asm_df5 = boost::make_shared<DataFieldStr>();
	new_asm_df5->setAttribute("type", "str");
	new_asm_df5->setAttribute("zky_name", "isnew");
	new_asm_df5->setAttribute("zky_offset", mStatDocDataRcdLen);
	new_asm_df5->setAttribute("zky_length", "1");
	mStatDocDataRcdLen += 1;
	rcdctnr_dr->setField(new_asm_df5);
	mStatDocDataFields << new_asm_df5->getConfig();

	rcdctnr_dr->setAttribute("type", "fixbin");
	rcdctnr_dr->setAttribute("zky_name", df_rcd_name);
	rcdctnr_dr->setAttribute("zky_length", mStatDocDataRcdLen);
	
	boost::shared_ptr<DataRecordCtnr> new_asm_dr = boost::make_shared<DataRecordCtnr>();
	new_asm_dr->setRecord(rcdctnr_dr);
	new_asm_dr->setAttribute("zky_name", df_rcd_name);
	
	boost::shared_ptr<ReduceTaskDataProc> new_dpf= boost::make_shared<ReduceTaskDataProc>("createtaskfile");
	//new_dpf->setAttribute("zky_iilname", df_iilname);
	new_dpf->setAttribute("datasetname", output_dsname); 
	new_dpf->setAttribute("zky_name", output_dsname);
	new_dpf->setAttribute("run_mode", "run_parent");
	new_dpf->setAttribute("zky_file_defname", mTask2FileDefName);
	//new_dpf->setAttribute("zky_name", df_dataproc_name);

	new_dpf->setCollectorType("iil");
	//new_dpf->setCollectorAttr("zky_name", df_connector_name); 
	new_dpf->setCollectorAttr("zky_name", output_dsname); 
	new_dpf->setCollectorAttr("group_type", "cube_assemble"); 

	new_dpf->setAttribute(AOSTAG_OPERATOR, "strinc");
	new_dpf->setAssemblerType("strinc");
	new_dpf->setAssemblerAttr("zky_inctype", "norm");

	new_dpf->setRecord(new_asm_dr);

	boost::shared_ptr<CompFun> fun1 = boost::make_shared<CompFun>("custom", mStatDocDataRcdLen);
	fun1->setCmpField("u64", 0, 8);
	fun1->setCmpField("u64", 8, 8);
	new_dpf->setCompFun(fun1);

	rtask->setDataProc(new_dpf);


	//create maptask    
	boost::shared_ptr<DataEngineJoinNew> de = boost::make_shared<DataEngineJoinNew>();
	de->setLhsDatasetName(lhs_input_dsname);
	de->setRhsDatasetName(rhs_input_dsname);

	de->setFirstRefName(lhs_inputrcd_name);
	de->setNextRefName(lhs_inputrcd_name);

	OmnString lhs_field_name_str = lhs_inputrcd_name;
	lhs_field_name_str << ".str";
	
	OmnString lhs_field_name_time = lhs_inputrcd_name;
	lhs_field_name_time << ".time";

	OmnString lhs_field_name_value = lhs_inputrcd_name;    
	lhs_field_name_value << ".value";
	
	OmnString lhs_field_name_true = lhs_inputrcd_name;    
	lhs_field_name_true << ".true";

	OmnString lhs_field_name_false = lhs_inputrcd_name;    
	lhs_field_name_false << ".false";

	OmnString rhs_field_name_docid = rhs_inputrcd_name;
	rhs_field_name_docid << ".docid";

	OmnString df_field_name_value = df_rcd_name;  
	df_field_name_value << ".value";
	
	OmnString df_field_name_docid = df_rcd_name;  
	df_field_name_docid << ".docid";
	
	OmnString df_field_name_time = df_rcd_name;  
	df_field_name_time << ".time";
	
	OmnString df_field_name_str = df_rcd_name;  
	df_field_name_str << ".str";

	OmnString df_field_name_isnew = df_rcd_name;  
	df_field_name_isnew << ".isnew";

	OmnString lhs_field_name_statinternalid = lhs_inputrcd_name;  
	lhs_field_name_statinternalid << ".statinternalid";

	OmnString next_str = "next.str";
	OmnString first_docid = "first.docid";
	OmnString temp_docid = "temp.docid";

	OmnString rhs_field_name = rhs_inputrcd_name;
	rhs_field_name << ".value";
	
	OmnString output_all_field_name_docid = output_all_rcdname;
	output_all_field_name_docid << ".docid";

	OmnString output_all_field_name_str = output_all_rcdname;
	output_all_field_name_str << ".str";
	
	OmnString output_statinternalid_field_name_str = output_statinternalid_rcdname;
	output_statinternalid_field_name_str << ".str";

	OmnString output_statinternalid_field_name_docid = output_statinternalid_rcdname;
	output_statinternalid_field_name_docid << ".docid";

	
	// cond_dataprocs
	boost::shared_ptr<DataProcJoin> cond_dpc1 = boost::make_shared<DataProcJoin>();
	cond_dpc1->setAttribute("zky_name", "joincmp");
	cond_dpc1->setLhsInput(lhs_field_name_str);
	cond_dpc1->setRhsInput(rhs_field_name);
	
	boost::shared_ptr<DataProcJoin> cond_dpc2 = boost::make_shared<DataProcJoin>();
	cond_dpc2->setAttribute("zky_name", "selfcmp");
	cond_dpc2->setLhsInput(lhs_field_name_str);
	cond_dpc2->setRhsInput(next_str);

	de->setDataProc("cond", cond_dpc1);
	de->setDataProc("cond", cond_dpc2);
	
	// false_cond_dataprocs
	boost::shared_ptr<DataProcCompose> false_dpc1 = boost::make_shared<DataProcCompose>();
	false_dpc1->setInput(first_docid);
	false_dpc1->setOutput(temp_docid);
	
	boost::shared_ptr<DataProcCompose> false_dpc2 = boost::make_shared<DataProcCompose>();
	false_dpc2->setInput(temp_docid);
	false_dpc2->setOutput(output_all_field_name_docid);

	boost::shared_ptr<DataProcCompose> false_dpc3 = boost::make_shared<DataProcCompose>();
	false_dpc3->setInput(lhs_field_name_str);
	false_dpc3->setOutput(output_all_field_name_str);

	boost::shared_ptr<DataProcStatIndex> false_dpc4 = boost::make_shared<DataProcStatIndex>();
	false_dpc4->setAttribute("zky_sep", "0x01");
	false_dpc4->setKeyInputFieldName(lhs_field_name_str);
	false_dpc4->setDocidInputFieldName(temp_docid);
	for(size_t i=0; i<outputrcd_names.size(); i++)
	{
		OmnString field_name = outputrcd_names[i];
		field_name << ".str";
		false_dpc4->setKeyOutputFieldName(field_name);

		field_name = outputrcd_names[i];
		field_name << ".docid";
		false_dpc4->setDocidOutputFieldName(field_name);
	}
/*
	boost::shared_ptr<DataProcCompose> false_dpc5 = boost::make_shared<DataProcCompose>();
	false_dpc5->setInput(lhs_field_name_statinternalid);
	false_dpc5->setOutput(output_statinternalid_field_name_str);
	
	boost::shared_ptr<DataProcCompose> false_dpc6 = boost::make_shared<DataProcCompose>();
	false_dpc6->setInput(temp_docid);
	false_dpc6->setOutput(output_statinternalid_field_name_docid);
*/	
	boost::shared_ptr<DataProcCompose> false_dpc7 = boost::make_shared<DataProcCompose>();
	false_dpc7->setInput(lhs_field_name_true);
	false_dpc7->setOutput(df_field_name_isnew);
	
	de->setDataProc("false_cond", false_dpc1);
	de->setDataProc("false_cond", false_dpc2);
	de->setDataProc("false_cond", false_dpc3);
	de->setDataProc("false_cond", false_dpc4);
	//de->setDataProc("false_cond", false_dpc5);
	//de->setDataProc("false_cond", false_dpc6);
	de->setDataProc("false_cond", false_dpc7);
	
	// true_cond_dataprocs
	boost::shared_ptr<DataProcCompose> true_dpc = boost::make_shared<DataProcCompose>();
	true_dpc->setInput(rhs_field_name_docid);
	true_dpc->setOutput(temp_docid);
	
	boost::shared_ptr<DataProcCompose> true_dpc1 = boost::make_shared<DataProcCompose>();
	true_dpc1->setInput(lhs_field_name_false);
	true_dpc1->setOutput(df_field_name_isnew);
	
	de->setDataProc("true_cond", true_dpc);
	de->setDataProc("true_cond", true_dpc1);

	// all_cond_dataprocs
	boost::shared_ptr<DataProcCompose> all_dpc_str = boost::make_shared<DataProcCompose>();
	all_dpc_str->setInput(lhs_field_name_str);
	all_dpc_str->setOutput(df_field_name_str);
		
	boost::shared_ptr<DataProcCompose> all_dpc_docid = boost::make_shared<DataProcCompose>();
	all_dpc_docid->setInput(temp_docid);
	all_dpc_docid->setOutput(df_field_name_docid);
		
	boost::shared_ptr<DataProcCompose> all_dpc_time = boost::make_shared<DataProcCompose>();
	all_dpc_time->setInput(lhs_field_name_time);
	all_dpc_time->setOutput(df_field_name_time);
		
	de->setDataProc("all_cond", all_dpc_str);
	de->setDataProc("all_cond", all_dpc_docid);
	de->setDataProc("all_cond", all_dpc_time);
	
	OmnString lhs_fname_value, df_fname_value;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		lhs_fname_value = lhs_field_name_value;
		lhs_fname_value << i;
		
		df_fname_value = df_field_name_value;
		df_fname_value << i;
		
		boost::shared_ptr<DataProcCompose> all_dpc_value = boost::make_shared<DataProcCompose>();
		all_dpc_value->setInput(lhs_fname_value);
		all_dpc_value->setOutput(df_fname_value);
		de->setDataProc("all_cond", all_dpc_value);
	}

	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	mtask->setDataEngine(de);
	
	jobtask->setMapTask(mtask);  
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(lhs_inputds);
	jobtask->setInputDataset(rhs_inputds);
	jobtask->setSplitType("cube_pair");
/*
	//create job
	Job job;
	job.setAttribute("zky_jobid", "rootjob");
	job.setAttribute("zky_ctnrobjid", "task_ctnr");
	job.setAttribute("log_ctnr", "log_ctnr");
	job.setAttribute("zky_ispublic", "true");
	job.setAttribute("zky_objid", "job_example1_stat_task002_20140513");
	job.setAttribute("zky_pctrs", "sdoc_unicom_doc");
	job.setAttribute("zky_public_doc", "true");
	job.setAttribute("zky_public_ctnr", "true");
	job.setAttribute("zky_job_version", "1");
	job.setJobTask(jobtask);

	string jobconf = job.getConfig();
cout <<  jobconf << std::endl;
*/
	return jobtask;
}


//============================================
// Following is Created by Ketty
boost::shared_ptr<JobTask> 
StatConf::createConfig2New()
{
	OmnString pre_name = mStatRunDocName;
	pre_name << "_task002_";

	OmnString task_name = mStatRunDocName;
	task_name << "_task002";

	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	jobtask->setAttribute("zky_taskid", "task002");

	//ouputds_names
	vector<OmnString> outputds_names;
	vector<OmnString> outputproc_names;
	vector<OmnString> outputiil_names;
	vector<OmnString> outputcol_names;
	vector<OmnString> outputrcd_names;
	for(size_t i = 0; i < mStatFields.size(); i++)
	{
		OmnString temp = pre_name;
		temp << "dataset_" << mStatFields[i];
		outputds_names.push_back(temp);

		temp = pre_name;
		temp << "proc_" << mStatFields[i];
		outputproc_names.push_back(temp);

		//temp = AosIILName::composeStatKeyIILName(mStatName, mStatIdentifyKey, mStatFields[i]);
		temp = AosIILName::composeStatKeyIILName(mStatName, mStatFields[i]);
		outputiil_names.push_back(temp);

		temp = pre_name;
		temp << "datacollector_" << mStatFields[i];
		outputcol_names.push_back(temp);

		temp = pre_name;
		temp << "rcd_" << mStatFields[i] << "_iil";
		outputrcd_names.push_back(temp);
	}

	mTask2FileDefName = pre_name;
	mTask2FileDefName << "_statdocdata_files";

	OmnString output_all_dsname = pre_name;
	output_all_dsname << "dataset_all";

	mOutputIILNameAll = "_zt4k_";
	//mOutputIILNameAll << mStatName << "_" << mStatIdentifyKey  << "__$group";
	mOutputIILNameAll << mStatName << "__$group";

	OmnString output_all_rcdname = pre_name;
	output_all_rcdname << "rcd_all_iil";

	OmnString output_statinternalid_rcdname = pre_name;
	output_statinternalid_rcdname << "rcd_statinternalid_iil";
	
	OmnString output_dsname = pre_name;
	output_dsname << "_statdocdata";
	mStatDocDataDsName = output_dsname;

	//OmnString output_rcd_name = mStatRunDocName;
	//output_rcd_name << "_task004_rcd_buff_input";
	//mStatDocDataRcdName = output_rcd_name;

	OmnString output_schema_name = mStatRunDocName;
	output_schema_name << "_task004_schema";

	OmnString output_scanner_name = mStatRunDocName;
	output_scanner_name << "_task004_scanner";
	
	OmnString df_rcd_name = pre_name;
	df_rcd_name << "rcd_statdocdata";

	OmnString df_connector_name = pre_name;
	df_connector_name << "datacollector_statdocdata";

	OmnString df_dataproc_name = pre_name;
	df_dataproc_name << "proc_statdocdata";

	OmnString lhs_input_dsname = mStatRunDocName;
	lhs_input_dsname << "_all";
	OmnString lhs_inputrcd_name = mStatRunDocName;
	lhs_inputrcd_name << "_task003_rcd_lhs_all";
	boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	createTask2LhsInputDataset(lhs_inputds,
			lhs_input_dsname, lhs_inputrcd_name);
	
	OmnString rhs_input_dsname = "rhs";
	OmnString rhs_inputrcd_name = "example1_task002_rcd_rhs_iil";
	boost::shared_ptr<DataSet> rhs_inputds = boost::make_shared<DataSet>();
	createTask2RhsInputDataset(rhs_inputds,
			rhs_input_dsname, rhs_inputrcd_name);

	createTask2OutputDataset(jobtask, output_all_dsname, 
			outputds_names, output_dsname);

	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	createTask2ReduceTask(rtask, output_all_rcdname, 
			output_all_dsname, outputds_names,
			outputrcd_names, outputiil_names, 
			outputproc_names, outputcol_names,
			df_rcd_name, output_dsname);
	
	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	createTask2MapTask(mtask, lhs_input_dsname, lhs_inputrcd_name,
			rhs_input_dsname, rhs_inputrcd_name, df_rcd_name,
			output_all_rcdname, outputrcd_names);
	
	jobtask->setMapTask(mtask);  
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(lhs_inputds);
	jobtask->setInputDataset(rhs_inputds);
	jobtask->setSplitType("cube_pair");
	return jobtask;
}


/*
bool
StatConf::createTask2LhsInputDataset(
		boost::shared_ptr<DataSet> &lhs_inputds,
		const OmnString &lhs_input_dsname,
		const OmnString &lhs_inputrcd_name)
{
	//create lhs_input_dataset
	boost::shared_ptr<DataSplit> ds_split = boost::make_shared<DataSplit>();
	ds_split->setAttribute("jimo_objid", "dataspliter_filesbycube_jimodoc_v0");

	boost::shared_ptr<DataConnector> ds_connector = boost::make_shared<DataConnector>();
	ds_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");
	ds_connector->setAttribute("zky_file_defname", mFileDefName);
	ds_connector->setSplit(ds_split);

	boost::shared_ptr<DataScanner> ds_scanner2 = boost::make_shared<DataScanner>();
	ds_scanner2->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	ds_scanner2->setAttribute("zky_name", "example1_task003_scanner");
	ds_scanner2->setAttribute("buff_cache_num", "9");
	ds_scanner2->setConnector(ds_connector);
	
	boost::shared_ptr<DataRecord> ds_record2 = boost::make_shared<DataRecord>();
	int output_rcd_len2 = 0;
	
	boost::shared_ptr<DataFieldStr> outds_df1 = boost::make_shared<DataFieldStr>();
	outds_df1->setAttribute("type", "str");
	outds_df1->setAttribute("zky_name", "str");
	outds_df1->setAttribute("zky_offset", "0");
	outds_df1->setAttribute("zky_length", mMaxLen);
	outds_df1->setAttribute("zky_datatooshortplc", "cstr");
	ds_record2->setField(outds_df1);
	output_rcd_len2 += mMaxLen;
	
	boost::shared_ptr<DataFieldStr> outds_cubeid = boost::make_shared<DataFieldStr>();
	outds_cubeid->setAttribute("type", "bin_u64");
	outds_cubeid->setAttribute("zky_name", "cube_id");
	outds_cubeid->setAttribute("zky_offset", output_rcd_len2);
	outds_cubeid->setAttribute("zky_length", "8");
	ds_record2->setField(outds_cubeid);
	output_rcd_len2 += 8;
	
	OmnString time_name;
	for(size_t i =0; i<mStatTimes.size(); i++)
	{
		time_name = "time";
		time_name << i;
	
		boost::shared_ptr<DataFieldStr> outds_dfday = boost::make_shared<DataFieldStr>();
		outds_dfday->setAttribute("type", "bin_u64");
		outds_dfday->setAttribute("zky_name", time_name);
		outds_dfday->setAttribute("zky_offset", output_rcd_len2);
		outds_dfday->setAttribute("zky_length", "8");
		output_rcd_len2 += 8;
		ds_record2->setField(outds_dfday);
	}

	OmnString value_name;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		value_name = "value";
		value_name << i;
		boost::shared_ptr<DataFieldStr> outds_dfvalue = boost::make_shared<DataFieldStr>();
		outds_dfvalue->setAttribute("zky_name", value_name);
		outds_dfvalue->setAttribute("type", "bin_u64");
		outds_dfvalue->setAttribute("zky_offset", output_rcd_len2);
		outds_dfvalue->setAttribute("zky_length", "8");
		output_rcd_len2 += 8;
		ds_record2->setField(outds_dfvalue);
	}
	
	OmnString stat_definition_key = mStatRunDocName;
	stat_definition_key << "_" << mInternalId;
	boost::shared_ptr<DataFieldStr> outds_df2 = boost::make_shared<DataFieldStr>();
	outds_df2->setAttribute("zky_name", "docid");
	outds_df2->setAttribute("type", "statid");
	outds_df2->setAttribute("stat_definition_key", stat_definition_key);
	outds_df2->setAttribute("zky_num_ids", "1000");
	outds_df2->setAttribute("zky_value_from_field", "cube_id");

	/	boost::shared_ptr<DataFieldStr> outds_df3 = boost::make_shared<DataFieldStr>();
	//outds_df3->setAttribute("zky_name", "statinternalid");
	//outds_df3->setAttribute("type", "const");
	//outds_df3->setConst(mInternalId);
	
	boost::shared_ptr<DataFieldStr> outds_df4 = boost::make_shared<DataFieldStr>();
	outds_df4->setAttribute("zky_name", "true");
	outds_df4->setAttribute("type", "const");
	outds_df4->setConst("1");
	
	boost::shared_ptr<DataFieldStr> outds_df5 = boost::make_shared<DataFieldStr>();
	outds_df5->setAttribute("zky_name", "false");
	outds_df5->setAttribute("type", "const");
	outds_df5->setConst("0");

	ds_record2->setField(outds_df2);
	//ds_record2->setField(outds_df3);
	ds_record2->setField(outds_df4);
	ds_record2->setField(outds_df5);
	ds_record2->setAttribute("type", "fixbin");
	ds_record2->setAttribute("zky_name", lhs_inputrcd_name);
	ds_record2->setAttribute("zky_length", output_rcd_len2);
	
	boost::shared_ptr<DataSchema> ds_schema2 = boost::make_shared<DataSchema>();
	ds_schema2->setAttribute("jimo_objid", "dataschema_unilength_jimodoc_v0");
	ds_schema2->setAttribute("zky_name", "example1_task003_schema");
	ds_schema2->setRecord(ds_record2);
	
	lhs_inputds->setAttribute("jimo_objid","dataset_bydatascanner_jimodoc_v0");
	lhs_inputds->setAttribute("zky_name", lhs_input_dsname);
	lhs_inputds->setScanner(ds_scanner2);
	lhs_inputds->setSchema(ds_schema2);

	return true;
}
*/


bool
StatConf::createTask2LhsInputDataset(
		boost::shared_ptr<DataSet> &lhs_inputds,
		const OmnString &lhs_input_dsname,
		const OmnString &lhs_inputrcd_name)
{
	//create lhs_input_dataset
	boost::shared_ptr<DataSplit> ds_split = boost::make_shared<DataSplit>();
	ds_split->setAttribute("jimo_objid", "dataspliter_filesbycube_jimodoc_v0");

	boost::shared_ptr<DataConnector> ds_connector = boost::make_shared<DataConnector>();
	ds_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");
	ds_connector->setAttribute("zky_file_defname", mFileDefName);
	ds_connector->setSplit(ds_split);

	boost::shared_ptr<DataScanner> ds_scanner2 = boost::make_shared<DataScanner>();
	ds_scanner2->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	ds_scanner2->setAttribute("zky_name", "example1_task003_scanner");
	ds_scanner2->setAttribute("buff_cache_num", "9");
	ds_scanner2->setConnector(ds_connector);

	// copy from task1's reduce data_record.
	boost::shared_ptr<DataRecord> ds_record2 = boost::make_shared<DataRecord>();

	ds_record2->setFields(mTask1ReduceDataRecord->getFields());

	OmnString stat_definition_key = mStatRunDocName;
	stat_definition_key << "_" << mInternalId;
	boost::shared_ptr<DataFieldStr> outds_df2 = boost::make_shared<DataFieldStr>();
	outds_df2->setAttribute("zky_name", "sdocid");
	outds_df2->setAttribute("type", "statid");
	outds_df2->setAttribute("stat_definition_key", stat_definition_key);
	outds_df2->setAttribute("zky_num_ids", "1000");
	outds_df2->setAttribute("zky_value_from_field", "cube_id");
	
	boost::shared_ptr<DataFieldStr> outds_df4 = boost::make_shared<DataFieldStr>();
	outds_df4->setAttribute("zky_name", "true");
	outds_df4->setAttribute("type", "const");
	outds_df4->setConst("1");
	
	boost::shared_ptr<DataFieldStr> outds_df5 = boost::make_shared<DataFieldStr>();
	outds_df5->setAttribute("zky_name", "false");
	outds_df5->setAttribute("type", "const");
	outds_df5->setConst("0");

	ds_record2->setField(outds_df2);
	ds_record2->setField(outds_df4);
	ds_record2->setField(outds_df5);
	
	map<string, string> attrs = mTask1ReduceDataRecord->getAttributes();
	map<string, string>::iterator itr = attrs.begin();
	for(; itr != attrs.end(); itr++)
	{
		ds_record2->setAttribute(itr->first, itr->second);
	}
	ds_record2->setAttribute("zky_name", lhs_inputrcd_name);
	
	boost::shared_ptr<DataSchema> ds_schema2 = boost::make_shared<DataSchema>();
	ds_schema2->setAttribute("jimo_objid", "dataschema_unilength_jimodoc_v0");
	ds_schema2->setAttribute("zky_name", "example1_task003_schema");
	ds_schema2->setRecord(ds_record2);
	
	lhs_inputds->setAttribute("jimo_objid","dataset_bydatascanner_jimodoc_v0");
	lhs_inputds->setAttribute("zky_name", lhs_input_dsname);
	lhs_inputds->setScanner(ds_scanner2);
	lhs_inputds->setSchema(ds_schema2);

	return true;
}

bool
StatConf::createTask2RhsInputDataset(
		boost::shared_ptr<DataSet> &rhs_inputds,
		const OmnString &rhs_input_dsname,
		const OmnString &rhs_inputrcd_name)
{
	//create rhs_inputds_datafields
	boost::shared_ptr<DataFieldStr> input_df1 = boost::make_shared<DataFieldStr>();
	input_df1->setAttribute("type", "str");
	input_df1->setAttribute("zky_name", "value");
	input_df1->setAttribute("zky_offset", "0");
	input_df1->setAttribute("zky_length", mMaxLen);
	input_df1->setAttribute("zky_datatooshortplc", "cstr");
	int input_rcd_len = mMaxLen;

	boost::shared_ptr<DataFieldStr> input_df2 = boost::make_shared<DataFieldStr>();
	input_df2->setAttribute("type", "bin_u64");
	input_df2->setAttribute("zky_name", "sdocid");
	input_df2->setAttribute("zky_offset", input_rcd_len);
	input_df2->setAttribute("zky_length", "8");
	input_rcd_len += 8;

	//create rhs_inputds_datarecord
	boost::shared_ptr<DataRecord> input_rcd = boost::make_shared<DataRecord>();
	input_rcd->setAttribute("type", "iil");
	input_rcd->setAttribute("zky_name", rhs_inputrcd_name);
	input_rcd->setAttribute("zky_length", input_rcd_len);
	input_rcd->setField(input_df1);
	input_rcd->setField(input_df2);

	//create rhs_inputds_dataschema
	boost::shared_ptr<DataSchema> input_schema = boost::make_shared<DataSchema>();
	input_schema->setAttribute("zky_name", "example1_task002_schema_rhs");
	input_schema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	input_schema->setRecord(input_rcd);

	//create rhs_inputds_dataconnector
	boost::shared_ptr<DataConnectorIIL> input_connector = boost::make_shared<DataConnectorIIL>();
	input_connector->setAttribute("jimo_objid", "dataconnector_iil_jimodoc_v0");
	input_connector->setAttribute("zky_iilname", mOutputIILNameAll); 
	input_connector->setAttribute("zky_blocksize", "2000000");
	input_connector->setQueryCond("an", "*"); 
	boost::shared_ptr<DataSplit> input_split = boost::make_shared<DataSplit>();
	input_split->setAttribute("jimo_objid", "datasplit_iilcubegroup_jimodoc_v0");
	input_connector->setSplit(input_split);

	//create rhs_inputds_datascanner
	boost::shared_ptr<DataScanner> input_scanner = boost::make_shared<DataScanner>();
	input_scanner->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	input_scanner->setAttribute("zky_name", "example1_task002_scanner_rhs");
	input_scanner->setAttribute("buff_cache_num", "1");
	input_scanner->setConnector(input_connector);

	//create rhs_input_dataset
	rhs_inputds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	rhs_inputds->setAttribute("zky_name", rhs_input_dsname);
	rhs_inputds->setScanner(input_scanner);
	rhs_inputds->setSchema(input_schema);
	
	return true;
}

bool
StatConf::createTask2OutputDataset(
		boost::shared_ptr<JobTask> &jobtask,
		const OmnString &output_all_dsname,
		vector<OmnString> &outputds_names,
		const OmnString &output_dsname)
{
	//create output_dataset
	boost::shared_ptr<DataSet> output_ds_all = boost::make_shared<DataSet>();
	output_ds_all->setAttribute("zky_name", output_all_dsname);
	jobtask->setOutputDataset(output_ds_all);

	for(u32 i=0; i<outputds_names.size(); i++)
	{
		boost::shared_ptr<DataSet> output_ds = boost::make_shared<DataSet>();
		output_ds->setAttribute("zky_name",outputds_names[i]); 
		jobtask->setOutputDataset(output_ds);
	}
	
	boost::shared_ptr<DataSet> output_ds_statdocdata = boost::make_shared<DataSet>();
	output_ds_statdocdata->setAttribute("zky_name", output_dsname);
	jobtask->setOutputDataset(output_ds_statdocdata);
	
	return true;
}


bool
StatConf::createTask2ReduceTask(
		boost::shared_ptr<ReduceTask> &rtask,
		const OmnString &output_all_rcdname,
		const OmnString &output_all_dsname,
		vector<OmnString> &outputds_names,
		vector<OmnString> &outputrcd_names,
		vector<OmnString> &outputiil_names,
		vector<OmnString> &outputproc_names,
		vector<OmnString> &outputcol_names,
		const OmnString &df_rcd_name,
		const OmnString &output_dsname)
{
	//create reduce_task_dataproc1
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "str");
	asm_df1->setAttribute("zky_name", "statkey");
	asm_df1->setAttribute("zky_offset", "0");
	asm_df1->setAttribute("zky_length", mMaxLen);
	asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	int df_rcd_len1 = mMaxLen;

	boost::shared_ptr<DataFieldStr> asm_df2 = boost::make_shared<DataFieldStr>();
	asm_df2->setAttribute("type", "bin_u64");
	asm_df2->setAttribute("zky_name", "sdocid");
	asm_df2->setAttribute("zky_offset", df_rcd_len1);
	asm_df2->setAttribute("zky_length", "8");
	df_rcd_len1 += 8;

	boost::shared_ptr<DataRecord> rcdctnr_drall = boost::make_shared<DataRecord>();
	rcdctnr_drall->setAttribute("type", "fixbin");
	rcdctnr_drall->setAttribute("zky_name", output_all_rcdname);
	rcdctnr_drall->setAttribute("zky_length", df_rcd_len1);
	rcdctnr_drall->setField(asm_df1);
	rcdctnr_drall->setField(asm_df2);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr_all = boost::make_shared<DataRecordCtnr>();
	asm_dr_all->setRecord(rcdctnr_drall);
	asm_dr_all->setAttribute("zky_name", output_all_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf_all= boost::make_shared<ReduceTaskDataProc>("iilbatchopr"); 
	dpf_all->setAttribute("zky_iilname", mOutputIILNameAll); 
	dpf_all->setAttribute("datasetname", output_all_dsname);
	dpf_all->setAttribute("zky_name", output_all_dsname);
	dpf_all->setAttribute("run_mode", "run_child");
	//dpf_all->setAttribute("zky_name", "example1_task002_proc_all");

	dpf_all->setCollectorType("iil");
	dpf_all->setCollectorAttr("zky_name", output_all_dsname); 
	dpf_all->setCollectorAttr("group_type", "cube_assemble"); 
	//dpf_all->setCollectorAttr("zky_name", "example1_task002_datacollector_all"); 

	dpf_all->setAttribute(AOSTAG_OPERATOR, "strinc");
	dpf_all->setAssemblerType("strinc");
	dpf_all->setAssemblerAttr("zky_inctype", "set");

	boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", df_rcd_len1);
	cmp_fun->setCmpField("str", 0);
	cmp_fun->setAggrField("u64", df_rcd_len1-8, "set");
	dpf_all->setCompFun(cmp_fun);

	dpf_all->setRecord(asm_dr_all);
	rtask->setDataProc(dpf_all);
	
	//create reduce_task_dataproc3
	for(u32 i=0; i<outputds_names.size(); i++)
	{
		map<string, int>::iterator itr = mFieldLenMap.find(mStatFields[i]);
		aos_assert_r(itr != mFieldLenMap.end(),	false);

		int rcd_len = itr->second;
		rcd_len++;
		boost::shared_ptr<DataFieldStr> asm_df5 = boost::make_shared<DataFieldStr>();
		asm_df5->setAttribute("type", "str");
		asm_df5->setAttribute("zky_name", "statkey");
		asm_df5->setAttribute("zky_offset", "0");
		asm_df5->setAttribute("zky_length", rcd_len);
		asm_df5->setAttribute("zky_datatooshortplc", "cstr");

		boost::shared_ptr<DataFieldStr> asm_df6 = boost::make_shared<DataFieldStr>();
		asm_df6->setAttribute("type", "bin_u64");
		asm_df6->setAttribute("zky_name", "sdocid");
		asm_df6->setAttribute("zky_offset", rcd_len);
		asm_df6->setAttribute("zky_length", "8");
		rcd_len += 8;

		boost::shared_ptr<DataRecord> rcdctnr_dr3 = boost::make_shared<DataRecord>();
		rcdctnr_dr3->setAttribute("type", "fixbin");
		rcdctnr_dr3->setAttribute("zky_name", outputrcd_names[i]);
		rcdctnr_dr3->setAttribute("zky_length", rcd_len);
		rcdctnr_dr3->setField(asm_df5);
		rcdctnr_dr3->setField(asm_df6);
	
		boost::shared_ptr<DataRecordCtnr> asm_dr3 = boost::make_shared<DataRecordCtnr>();
		asm_dr3->setRecord(rcdctnr_dr3);
		asm_dr3->setAttribute("zky_name", outputrcd_names[i]);

		boost::shared_ptr<ReduceTaskDataProc> dpf3= boost::make_shared<ReduceTaskDataProc>("iilbatchopr"); 
		dpf3->setAttribute("zky_iilname", outputiil_names[i]); 
		dpf3->setAttribute("datasetname", outputds_names[i]); 
		dpf3->setAttribute("zky_name", outputproc_names[i]);

		dpf3->setCollectorType("iil");
		dpf3->setCollectorAttr("zky_name", outputcol_names[i]); 

		dpf3->setAttribute(AOSTAG_OPERATOR, "stradd");
		dpf3->setAssemblerType("stradd");
		dpf3->setAssemblerAttr("zky_inctype", "norm");

		boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", rcd_len);
		cmp_fun->setCmpField("str", 0);
		cmp_fun->setCmpField("u64", itr->second+1, 8);

		cmp_fun->setAggrField("u64", itr->second+1, "set");
		dpf3->setCompFun(cmp_fun);
		
		dpf3->setRecord(asm_dr3);
		rtask->setDataProc(dpf3);
	}
	
	//change 2014/07/01
	//create reduce_task_dataproc4
	
	//boost::shared_ptr<DataRecord> rcdctnr_dr = boost::make_shared<DataRecord>();
	mTask2ReduceDataRecord = boost::make_shared<DataRecord>(); 
	
	mStatDocDataRcdLen = 0;
	//mStatDocDataFields = "";
	boost::shared_ptr<DataFieldStr> new_asm_df1 = boost::make_shared<DataFieldStr>();
	new_asm_df1->setAttribute("type", "bin_u64");
	new_asm_df1->setAttribute("zky_name", "sdocid");
	new_asm_df1->setAttribute("zky_offset", "0");
	new_asm_df1->setAttribute("zky_length", "8");
	new_asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	mStatDocDataRcdLen += 8;
	mTask2ReduceDataRecord->setField(new_asm_df1);
	//rcdctnr_dr->setField(new_asm_df1);
	//mStatDocDataFields << new_asm_df1->getConfig();

	OmnString time_name;
	for(size_t i =0; i<mStatTimes.size(); i++)
	{
		time_name = "time";
		time_name << i;
		boost::shared_ptr<DataFieldStr> new_asm_df2 = boost::make_shared<DataFieldStr>();
		new_asm_df2->setAttribute("type", "bin_u64");
		new_asm_df2->setAttribute("zky_name", time_name);
		new_asm_df2->setAttribute("zky_offset", mStatDocDataRcdLen);
		new_asm_df2->setAttribute("zky_length", "8");
		mStatDocDataRcdLen += 8;
		mTask2ReduceDataRecord->setField(new_asm_df2);
		//rcdctnr_dr->setField(new_asm_df2);
		//mStatDocDataFields << new_asm_df2->getConfig();
	}

	OmnString vname;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		OmnString type = "bin_int64";
		vname = "value";
		vname << i;
		boost::shared_ptr<DataFieldStr> new_asm_df3 = boost::make_shared<DataFieldStr>();

		if (mMeasures[i].field_type == "double" || mMeasures[i].field_type == "number")
			type = "bin_double";

		new_asm_df3->setAttribute("type", type);
		new_asm_df3->setAttribute("zky_name", vname);
		new_asm_df3->setAttribute("zky_offset", mStatDocDataRcdLen);
		new_asm_df3->setAttribute("zky_length", "8");
		mStatDocDataRcdLen += 8;
		mTask2ReduceDataRecord->setField(new_asm_df3);
		//rcdctnr_dr->setField(new_asm_df3);
		//mStatDocDataFields << new_asm_df3->getConfig();
	}

	boost::shared_ptr<DataFieldStr> new_asm_df4 = boost::make_shared<DataFieldStr>();
	new_asm_df4->setAttribute("type", "str");
	new_asm_df4->setAttribute("zky_name", "statkey");
	new_asm_df4->setAttribute("zky_offset", mStatDocDataRcdLen);
	new_asm_df4->setAttribute("zky_length", mMaxLen);
	new_asm_df4->setAttribute("zky_datatooshortplc", "cstr");
	mStatDocDataRcdLen += mMaxLen;
	mTask2ReduceDataRecord->setField(new_asm_df4);
	//rcdctnr_dr->setField(new_asm_df4);
	//mStatDocDataFields << new_asm_df4->getConfig();
	
	boost::shared_ptr<DataFieldStr> new_asm_df5 = boost::make_shared<DataFieldStr>();
	new_asm_df5->setAttribute("type", "str");
	new_asm_df5->setAttribute("zky_name", "isnew");
	new_asm_df5->setAttribute("zky_offset", mStatDocDataRcdLen);
	new_asm_df5->setAttribute("zky_length", "1");
	mStatDocDataRcdLen += 1;
	mTask2ReduceDataRecord->setField(new_asm_df5);
	//rcdctnr_dr->setField(new_asm_df5);
	//mStatDocDataFields << new_asm_df5->getConfig();

	mTask2ReduceDataRecord->setAttribute("type", "fixbin");
	mTask2ReduceDataRecord->setAttribute("zky_name", df_rcd_name);
	mTask2ReduceDataRecord->setAttribute("zky_length", mStatDocDataRcdLen);
	//rcdctnr_dr->setAttribute("type", "fixbin");
	//rcdctnr_dr->setAttribute("zky_name", df_rcd_name);
	//rcdctnr_dr->setAttribute("zky_length", mStatDocDataRcdLen);
	
	boost::shared_ptr<DataRecordCtnr> new_asm_dr = boost::make_shared<DataRecordCtnr>();
	//new_asm_dr->setRecord(rcdctnr_dr);
	new_asm_dr->setRecord(mTask2ReduceDataRecord);
	new_asm_dr->setAttribute("zky_name", df_rcd_name);
	
	boost::shared_ptr<ReduceTaskDataProc> new_dpf= boost::make_shared<ReduceTaskDataProc>("createtaskfile");
	//new_dpf->setAttribute("zky_iilname", df_iilname);
	new_dpf->setAttribute("datasetname", output_dsname); 
	new_dpf->setAttribute("zky_name", output_dsname);
	new_dpf->setAttribute("run_mode", "run_parent");
	new_dpf->setAttribute("zky_file_defname", mTask2FileDefName);
	//new_dpf->setAttribute("zky_name", df_dataproc_name);

	new_dpf->setCollectorType("iil");
	//new_dpf->setCollectorAttr("zky_name", df_connector_name); 
	new_dpf->setCollectorAttr("zky_name", output_dsname); 
	new_dpf->setCollectorAttr("group_type", "cube_assemble"); 

	new_dpf->setAttribute(AOSTAG_OPERATOR, "strinc");
	new_dpf->setAssemblerType("strinc");
	new_dpf->setAssemblerAttr("zky_inctype", "norm");

	new_dpf->setRecord(new_asm_dr);

	boost::shared_ptr<CompFun> fun1 = boost::make_shared<CompFun>("custom", mStatDocDataRcdLen);
	fun1->setCmpField("u64", 0, 8);
	fun1->setCmpField("u64", 8, 8);
	new_dpf->setCompFun(fun1);

	rtask->setDataProc(new_dpf);

	return true;
}

bool
StatConf::createTask2MapTask(
		boost::shared_ptr<MapTask> &mtask,
		const OmnString &lhs_input_dsname,
		const OmnString &lhs_inputrcd_name,
		const OmnString &rhs_input_dsname,
		const OmnString &rhs_inputrcd_name,
		const OmnString &df_rcd_name,
		const OmnString &output_all_rcdname,
		vector<OmnString> &outputrcd_names)
{
	//create maptask
	boost::shared_ptr<DataEngineJoinNew> de = boost::make_shared<DataEngineJoinNew>();
	de->setLhsDatasetName(lhs_input_dsname);
	de->setRhsDatasetName(rhs_input_dsname);

	de->setFirstRefName(lhs_inputrcd_name);
	de->setNextRefName(lhs_inputrcd_name);

	OmnString lhs_field_name_str = lhs_inputrcd_name;
	lhs_field_name_str << ".statkey";
	
	OmnString lhs_field_name_time = lhs_inputrcd_name;
	lhs_field_name_time << ".time";

	OmnString lhs_field_name_value = lhs_inputrcd_name;    
	lhs_field_name_value << ".value";
	
	OmnString lhs_field_name_true = lhs_inputrcd_name;    
	lhs_field_name_true << ".true";

	OmnString lhs_field_name_false = lhs_inputrcd_name;    
	lhs_field_name_false << ".false";

	OmnString rhs_field_name_docid = rhs_inputrcd_name;
	rhs_field_name_docid << ".sdocid";

	OmnString df_field_name_value = df_rcd_name;  
	df_field_name_value << ".value";
	
	OmnString df_field_name_docid = df_rcd_name;  
	df_field_name_docid << ".sdocid";
	
	OmnString df_field_name_time = df_rcd_name;  
	df_field_name_time << ".time";
	
	OmnString df_field_name_str = df_rcd_name;  
	df_field_name_str << ".statkey";

	OmnString df_field_name_isnew = df_rcd_name;  
	df_field_name_isnew << ".isnew";

	OmnString lhs_field_name_statinternalid = lhs_inputrcd_name;  
	lhs_field_name_statinternalid << ".statinternalid";

	OmnString next_str = "next.statkey";
	OmnString first_docid = "first.sdocid";
	OmnString temp_docid = "temp.docid";

	OmnString rhs_field_name = rhs_inputrcd_name;
	rhs_field_name << ".value";
	
	OmnString output_all_field_name_docid = output_all_rcdname;
	output_all_field_name_docid << ".sdocid";

	OmnString output_all_field_name_str = output_all_rcdname;
	output_all_field_name_str << ".statkey";
	
	// cond_dataprocs
	boost::shared_ptr<DataProcJoin> cond_dpc1 = boost::make_shared<DataProcJoin>();
	cond_dpc1->setAttribute("zky_name", "joincmp");
	cond_dpc1->setLhsInput(lhs_field_name_str);
	cond_dpc1->setRhsInput(rhs_field_name);
	
	boost::shared_ptr<DataProcJoin> cond_dpc2 = boost::make_shared<DataProcJoin>();
	cond_dpc2->setAttribute("zky_name", "selfcmp");
	cond_dpc2->setLhsInput(lhs_field_name_str);
	cond_dpc2->setRhsInput(next_str);

	de->setDataProc("cond", cond_dpc1);
	de->setDataProc("cond", cond_dpc2);
	
	// false_cond_dataprocs
	boost::shared_ptr<DataProcCompose> false_dpc1 = boost::make_shared<DataProcCompose>();
	false_dpc1->setInput(first_docid);
	false_dpc1->setOutput(temp_docid);
	
	boost::shared_ptr<DataProcCompose> false_dpc2 = boost::make_shared<DataProcCompose>();
	false_dpc2->setInput(temp_docid);
	false_dpc2->setOutput(output_all_field_name_docid);

	boost::shared_ptr<DataProcCompose> false_dpc3 = boost::make_shared<DataProcCompose>();
	false_dpc3->setInput(lhs_field_name_str);
	false_dpc3->setOutput(output_all_field_name_str);

	boost::shared_ptr<DataProcStatIndex> false_dpc4 = boost::make_shared<DataProcStatIndex>();
	false_dpc4->setAttribute("zky_sep", "0x01");
	false_dpc4->setKeyInputFieldName(lhs_field_name_str);
	false_dpc4->setDocidInputFieldName(temp_docid);
	for(size_t i=0; i<outputrcd_names.size(); i++)
	{
		OmnString field_name = outputrcd_names[i];
		field_name << ".statkey";
		false_dpc4->setKeyOutputFieldName(field_name);

		field_name = outputrcd_names[i];
		field_name << ".sdocid";
		false_dpc4->setDocidOutputFieldName(field_name);
	}
	boost::shared_ptr<DataProcCompose> false_dpc7 = boost::make_shared<DataProcCompose>();
	false_dpc7->setInput(lhs_field_name_true);
	false_dpc7->setOutput(df_field_name_isnew);
	
	de->setDataProc("false_cond", false_dpc1);
	de->setDataProc("false_cond", false_dpc2);
	de->setDataProc("false_cond", false_dpc3);
	de->setDataProc("false_cond", false_dpc4);
	//de->setDataProc("false_cond", false_dpc5);
	//de->setDataProc("false_cond", false_dpc6);
	de->setDataProc("false_cond", false_dpc7);
	
	// true_cond_dataprocs
	boost::shared_ptr<DataProcCompose> true_dpc = boost::make_shared<DataProcCompose>();
	true_dpc->setInput(rhs_field_name_docid);
	true_dpc->setOutput(temp_docid);
	
	boost::shared_ptr<DataProcCompose> true_dpc1 = boost::make_shared<DataProcCompose>();
	true_dpc1->setInput(lhs_field_name_false);
	true_dpc1->setOutput(df_field_name_isnew);
	
	de->setDataProc("true_cond", true_dpc);
	de->setDataProc("true_cond", true_dpc1);

	// all_cond_dataprocs
	boost::shared_ptr<DataProcCompose> all_dpc_str = boost::make_shared<DataProcCompose>();
	all_dpc_str->setInput(lhs_field_name_str);
	all_dpc_str->setOutput(df_field_name_str);
		
	boost::shared_ptr<DataProcCompose> all_dpc_docid = boost::make_shared<DataProcCompose>();
	all_dpc_docid->setInput(temp_docid);
	all_dpc_docid->setOutput(df_field_name_docid);
		
	//boost::shared_ptr<DataProcCompose> all_dpc_time = boost::make_shared<DataProcCompose>();
	//all_dpc_time->setInput(lhs_field_name_time);
	//all_dpc_time->setOutput(df_field_name_time);
		
	de->setDataProc("all_cond", all_dpc_str);
	de->setDataProc("all_cond", all_dpc_docid);
	//de->setDataProc("all_cond", all_dpc_time);
	
	OmnString lhs_fname_value, df_fname_value;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		lhs_fname_value = lhs_field_name_value;
		lhs_fname_value << i;
		
		df_fname_value = df_field_name_value;
		df_fname_value << i;
		
		boost::shared_ptr<DataProcCompose> all_dpc_value = boost::make_shared<DataProcCompose>();
		all_dpc_value->setInput(lhs_fname_value);
		all_dpc_value->setOutput(df_fname_value);
		de->setDataProc("all_cond", all_dpc_value);
	}

	OmnString lhs_fname_time, df_fname_time;
	for(size_t i =0; i<mStatTimes.size(); i++)
	{
		lhs_fname_time = lhs_field_name_time;
		lhs_fname_time << i;
		
		df_fname_time = df_field_name_time;
		df_fname_time << i;
		
		boost::shared_ptr<DataProcCompose> all_dpc_time = boost::make_shared<DataProcCompose>();
		all_dpc_time->setInput(lhs_fname_time);
		all_dpc_time->setOutput(df_fname_time);
		de->setDataProc("all_cond", all_dpc_time);
	}

	mtask->setDataEngine(de);
	
	return true;
}


