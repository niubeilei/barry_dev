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
#include "AosConf/DataEngineJoin.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataProcCompose.h"
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
#include <boost/make_shared.hpp>
#include <string>

using boost::make_shared;
using AosConf::StatConf;
using AosConf::JobTask;

boost::shared_ptr<JobTask> 
StatConf::createDistinctConfig2() {
	OmnString pre_name = mStatDocName;
	pre_name << "_task002_distinct_";

	OmnString task_name = mStatDocName;
	//task_name << "_" << mAgrType << "_" << mInputValueFieldName << "_" << mInternalId << "_task002";
	task_name << "_distinct_level_" << mInternalId << "_task002";
	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	//jobtask->setAttribute("zky_starttype", "auto");
	jobtask->setAttribute("zky_taskid", "task002");
/*
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

		temp = "_zt44_";
		temp << mStatDocName << "_key_" << mStatFields[i];
		outputiil_names.push_back(temp);

		temp = pre_name;
		temp << "datacollector_" << mStatFields[i];
		outputcol_names.push_back(temp);

		temp = pre_name;
		temp << "rcd_" << mStatFields[i] << "_iil";
		outputrcd_names.push_back(temp);
	}
*/	
	OmnString output_all_dsname = pre_name;
	output_all_dsname << "dataset_all";

	mDistinctOutputIILNameAll = "_zt4k_";
	mDistinctOutputIILNameAll << mStatDocName << "__all_level_distinct_" << mInternalId;

	OmnString output_all_rcdname = pre_name;
	output_all_rcdname << "rcd_all_iil";

	//create lhs_input_dataset
	//boost::shared_ptr<DataSet> lhs_inputds = mOdsOne;
	boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	OmnString lhs_input_dsname = mDistinctOdsOneDatasetName;
	OmnString lhs_inputrcd_name = mDistinctOutputRcdNameOne;
	lhs_inputds->setAttribute("zky_dataset_name", lhs_input_dsname);
	

	//create rhs_inputds_datafields
	boost::shared_ptr<DataFieldStr> input_df1 = boost::make_shared<DataFieldStr>();
	input_df1->setAttribute("type", "str");
	input_df1->setAttribute("zky_name", "value");
	input_df1->setAttribute("zky_offset", "0");
	input_df1->setAttribute("zky_length", mDistinctMaxLen);
	input_df1->setAttribute("zky_datatooshortplc", "cstr");
	int input_rcd_len = mDistinctMaxLen;

	boost::shared_ptr<DataFieldStr> input_df2 = boost::make_shared<DataFieldStr>();
	input_df2->setAttribute("type", "bin_u64");
	input_df2->setAttribute("zky_name", "docid");
	input_df2->setAttribute("zky_offset", input_rcd_len);
	input_df2->setAttribute("zky_length", "8");
	input_rcd_len += 8;

	OmnString rhs_inputrcd_name = "example1_task002_distinct_rcd_rhs_iil";
	//create rhs_inputds_datarecord
	boost::shared_ptr<DataRecord> input_rcd = boost::make_shared<DataRecord>();
	input_rcd->setAttribute("type", "iil");
	input_rcd->setAttribute("zky_name", rhs_inputrcd_name);
	input_rcd->setAttribute("zky_length", input_rcd_len);
	input_rcd->setField(input_df1);
	input_rcd->setField(input_df2);

	//create rhs_inputds_dataschema
	boost::shared_ptr<DataSchema> input_schema = boost::make_shared<DataSchema>();
	input_schema->setAttribute("zky_name", "example1_task002_distinct_schema_rhs");
	input_schema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	input_schema->setRecord(input_rcd);

	//create rhs_inputds_dataconnector
	boost::shared_ptr<DataConnectorIIL> input_connector = boost::make_shared<DataConnectorIIL>();
	input_connector->setAttribute("jimo_objid", "dataconnector_iil_jimodoc_v0");
	input_connector->setAttribute("zky_iilname", mDistinctOutputIILNameAll); 
	input_connector->setAttribute("zky_blocksize", "2000000");
	input_connector->setQueryCond("an", "*"); 

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
	// output1
	boost::shared_ptr<DataSet> output_ds_all = boost::make_shared<DataSet>();
	output_ds_all->setAttribute("zky_name", output_all_dsname);
	jobtask->setOutputDataset(output_ds_all);

	
	//output2
	boost::shared_ptr<DataConnector> ds_connector = boost::make_shared<DataConnector>();
	ds_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");
	
	boost::shared_ptr<DataScanner> distinct_ds_scanner2 = boost::make_shared<DataScanner>();
	//ds_scanner2->setAttribute("jimo_objid", dscanner_objid);
	distinct_ds_scanner2->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	distinct_ds_scanner2->setAttribute("zky_name", "example1_task003_distinct_input_scanner");
	distinct_ds_scanner2->setAttribute("buff_cache_num", "9");
	distinct_ds_scanner2->setConnector(ds_connector);

	boost::shared_ptr<DataFieldStr> distinct_outds_df1 = boost::make_shared<DataFieldStr>();
	distinct_outds_df1->setAttribute("zky_name", "str");
	distinct_outds_df1->setAttribute("type", "str");
	distinct_outds_df1->setAttribute("zky_offset", "0");
	distinct_outds_df1->setAttribute("zky_length", mDistinctMaxLen);
	distinct_outds_df1->setAttribute("zky_datatooshortplc", "cstr");
	for(size_t i = 0; i < mDistinctFields.size(); i++)
	{
		boost::shared_ptr<DataFieldStr> tmp_df = boost::make_shared<DataFieldStr>();
		tmp_df->setAttribute("type", "str");
		tmp_df->setAttribute("zky_name", mDistinctFields[i]);
		tmp_df->setAttribute("zky_isconst", "true");
		distinct_outds_df1->setDataField(tmp_df);
	}

	mDistinctInputRcdNameOne = mStatDocName;
	mDistinctInputRcdNameOne << "_distinct_task002_rcd_rhs_input_one";

	mDistinctInputOneDatasetName = mStatDocName;
	mDistinctInputOneDatasetName << "_distinct_task002_dataset_rhs_input_one";

	boost::shared_ptr<DataRecord> distinct_ds_record2 = boost::make_shared<DataRecord>();
	distinct_ds_record2->setField(distinct_outds_df1);
	distinct_ds_record2->setAttribute("type", "stat");
	distinct_ds_record2->setAttribute("zky_name", mDistinctInputRcdNameOne);
	distinct_ds_record2->setAttribute("zky_length", mDistinctMaxLen);

	boost::shared_ptr<DataSchema> distinct_ds_schema2 = boost::make_shared<DataSchema>();
	distinct_ds_schema2->setAttribute("jimo_objid", "dataschema_unilength_jimodoc_v0");
	distinct_ds_schema2->setAttribute("zky_name", "example1_task003_distinct_input_schema");
	distinct_ds_schema2->setRecord(distinct_ds_record2);

	boost::shared_ptr<DataSet> distinct_output_ds2 = boost::make_shared<DataSet>();
	distinct_output_ds2->setAttribute("jimo_objid","dataset_bydatascanner_jimodoc_v0");
	distinct_output_ds2->setAttribute("zky_name", mDistinctInputOneDatasetName);
	distinct_output_ds2->setScanner(distinct_ds_scanner2);
	distinct_output_ds2->setSchema(distinct_ds_schema2);
	mDistinctOdsInputOne = distinct_output_ds2;
	jobtask->setOutputDataset(distinct_output_ds2);

	//create reducetask
	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	
	//create reduce_task_dataproc1
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "str");
	asm_df1->setAttribute("zky_name", "str");
	asm_df1->setAttribute("zky_offset", "0");
	asm_df1->setAttribute("zky_length", mDistinctMaxLen);
	asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	int df_rcd_len1 = mDistinctMaxLen;

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
	dpf_all->setAttribute("zky_iilname", mDistinctOutputIILNameAll); 
	dpf_all->setAttribute("datasetname", output_all_dsname);
	dpf_all->setAttribute("zky_name", "example1_task002_distinct_proc_all");

	dpf_all->setCollectorType("iil");
	dpf_all->setCollectorAttr("zky_name", "example1_task002_distinct_datacollector_all"); 

	dpf_all->setAssemblerType("strinc");
	dpf_all->setAssemblerAttr("zky_inctype", "set");

	dpf_all->setRecord(asm_dr_all);
	rtask->setDataProc(dpf_all);
	
	//create reduce_task_dataproc2
	OmnString distinct_asm_recordname1 = pre_name;
	distinct_asm_recordname1 << "input_rcd_one"; 
	
	boost::shared_ptr<DataRecord> distinct_rcdctnr_dr1 = boost::make_shared<DataRecord>();
	distinct_rcdctnr_dr1->setAttribute("type", "fixbin");
	distinct_rcdctnr_dr1->setAttribute("zky_name", distinct_asm_recordname1);
	distinct_rcdctnr_dr1->setAttribute("zky_length", mDistinctMaxLen);
	distinct_rcdctnr_dr1->setField(asm_df1);

	boost::shared_ptr<DataRecordCtnr> distinct_asm_dr = boost::make_shared<DataRecordCtnr>();
	distinct_asm_dr->setRecord(distinct_rcdctnr_dr1);
	distinct_asm_dr->setAttribute("zky_name", distinct_asm_recordname1);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf= boost::make_shared<ReduceTaskDataProc>("createdatasetdoc");
	dpf->setAttribute("datasetname", mDistinctInputOneDatasetName);
	dpf->setAttribute("zky_name", "temp_example1_task002_distinct_input_proc_one");
	dpf->setCollectorType("iil");
	dpf->setCollectorAttr("zky_name", mDistinctInputOneDatasetName);
	dpf->setAssemblerType("strinc");
	dpf->setAssemblerAttr("zky_inctype", "norm");
	dpf->setRecord(distinct_asm_dr);
	
	boost::shared_ptr<CompFun> fun = boost::make_shared<CompFun>("custom", mDistinctMaxLen);
	fun->setCmpField("str", 0);
	fun->setCmpField("u64", mMaxLen, 8);
	fun->setAggrField("u64", mDistinctMaxLen, "norm");
	
	dpf->setCompFun(fun);

	rtask->setDataProc(dpf);

	//create maptask    
	boost::shared_ptr<DataEngineJoin> de = boost::make_shared<DataEngineJoin>();
	de->setConditionAttr("join_type", "left");
	de->setLhsDatasetName(lhs_input_dsname);
	de->setRhsDatasetName(rhs_input_dsname);
	de->setCondType("false_cond");

	OmnString lhs_field_name_str = lhs_inputrcd_name;
	lhs_field_name_str << ".str";
	de->setLhsFieldName(lhs_field_name_str);

	OmnString rhs_field_name = rhs_inputrcd_name;
	rhs_field_name << ".value";
	de->setRhsFieldName(rhs_field_name);
	
	OmnString lhs_field_name_docid = lhs_inputrcd_name;
	lhs_field_name_docid << ".docid";
	
	OmnString output_all_field_name_docid = output_all_rcdname;
	output_all_field_name_docid << ".docid";

	OmnString output_all_field_name_str = output_all_rcdname;
	output_all_field_name_str << ".str";
	
	OmnString output_one_field_name_str = distinct_asm_recordname1;
	output_one_field_name_str << ".str";
	
	boost::shared_ptr<DataProcCompose> dpc1 = boost::make_shared<DataProcCompose>();
	dpc1->setInput(lhs_field_name_docid);
	dpc1->setOutput(output_all_field_name_docid);

	boost::shared_ptr<DataProcCompose> dpc2 = boost::make_shared<DataProcCompose>();
	dpc2->setInput(lhs_field_name_str);
	dpc2->setOutput(output_all_field_name_str);

	boost::shared_ptr<DataProcCompose> dpc3 = boost::make_shared<DataProcCompose>();
	dpc3->setInput(lhs_field_name_str);
	dpc3->setOutput(output_one_field_name_str);
	
	de->setDataProc(dpc1);
	de->setDataProc(dpc2);
	de->setDataProc(dpc3);

	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	mtask->setDataEngine(de);
	
	jobtask->setMapTask(mtask);  
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(lhs_inputds);
	jobtask->setInputDataset(rhs_inputds);
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

