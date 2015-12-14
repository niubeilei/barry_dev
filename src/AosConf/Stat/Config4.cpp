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
StatConf::createConfig4() {
	OmnString pre_name = mStatDocName;
	pre_name << "_task004";
	
	OmnString lhs_inputds_name = mStatDocDataDsName;
	OmnString lhs_inputrcd_name = mStatDocDataRcdName;
	
	OmnString input_schema_name = mStatDocName;
	input_schema_name << "_task004_schema";

	OmnString input_scanner_name = mStatDocName;
	input_scanner_name << "_task004_scanner";

	OmnString output_oldvalue_dsname = pre_name;
	output_oldvalue_dsname << "_dataset_oldvalue";
	
	OmnString output_newvalue_dsname = pre_name;
	//output_newvalue_dsname << "_" << mAgrType << "_" << mInputValueFieldName << "_" << mInternalId << "_newvalue";
	if(mIsDistinct && mStatConfLevel == "0-1")
	{
		output_newvalue_dsname << "_isdistinct_" << mInternalId << "_newvalue";
	}
	else
	{
		output_newvalue_dsname << "_" << mInternalId << "_newvalue";
	}
	mNewValueDsName = output_newvalue_dsname;

	OmnString output_newvalue_rcdname = mStatDocName;
	output_newvalue_rcdname << "_task005_rcd_newvalue_input";
	mNewValueRcdName = output_newvalue_rcdname;

	OmnString output_newvalue_schemaname = mStatDocName;
	output_newvalue_schemaname << "_task005_schema";

	OmnString output_newvalue_scannername = mStatDocName;
	output_newvalue_scannername << "_task005_scanner";

	OmnString df_oldvalue_procname = pre_name;
	df_oldvalue_procname << "_proc_oldvalue";

	OmnString df_oldvalue_collectorname = pre_name;
	df_oldvalue_collectorname << "_datacollector_oldvalue";

	OmnString df_oldvalue_rcdname = pre_name;
	df_oldvalue_rcdname << "_rcd_oldvalue";

	OmnString df_oldvalue_iilname = "_zt4k_";
	df_oldvalue_iilname << mStatDocName << "_oldvalue_temp";

	OmnString df_newvalue_rcdname = pre_name;
	df_newvalue_rcdname << "_rcd_newvalue";

	OmnString df_newvalue_procname = pre_name;
	df_newvalue_procname << "_proc_newvalue";

	OmnString df_newvalue_collectorname = pre_name;
	df_newvalue_collectorname << "_datacollector_newvalue";

	//create lhs_input_dataset
	//boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	//lhs_inputds->setAttribute("zky_dataset_name", lhs_inputds_name);
	
	//create inputds_datafields
	boost::shared_ptr<DataFieldStr> input_df = boost::make_shared<DataFieldStr>();
	input_df->setAttribute("type", "bin_varbuff");
	input_df->setAttribute("zky_name", "all");
	input_df->setAttribute("zky_offset", "0");
	input_df->setAttribute("zky_bufflength", "50000000");
	input_df->setAttribute("zky_minlen", mStatDocDataRcdLen);

	//create inputds_datarecord
	boost::shared_ptr<DataRecord> input_rcd = boost::make_shared<DataRecord>();
	input_rcd->setAttribute("type", "buff");
	input_rcd->setAttribute("zky_name", lhs_inputrcd_name);
	input_rcd->setField(input_df);

	//create inputds_dataschema
	boost::shared_ptr<DataSchema> input_schema = boost::make_shared<DataSchema>();
	input_schema->setAttribute("zky_name", input_schema_name);
	input_schema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	input_schema->setRecord(input_rcd);

	//create inputds_datasplit
	boost::shared_ptr<DataSplit> input_split = boost::make_shared<DataSplit>();
	input_split->setAttribute("jimo_objid", "dataspliter_filesbycube_jimodoc_v0");
	//input_split->setAttribute("jimo_objid", "dataspliter_files_jimodoc_v0");

	//create inputds_dataconnector
	boost::shared_ptr<DataConnector> input_connector = boost::make_shared<DataConnector>();
	//input_connector->setAttribute("jimo_objid", "dataconnector_dir_jimodoc_v0");
	input_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");
	input_connector->setAttribute("zky_file_defname", mTask2FileDefName);
	input_connector->setSplit(input_split);

	//create inputds_datascanner
	boost::shared_ptr<DataScanner> input_scanner = boost::make_shared<DataScanner>();
	input_scanner->setAttribute("jimo_objid", "datascanner_parallel_jimodoc_v0");
	input_scanner->setAttribute("zky_name", input_scanner_name);
	input_scanner->setAttribute("buff_cache_num", "9");
	input_scanner->setConnector(input_connector);

	//create input_dataset
	boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	lhs_inputds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	lhs_inputds->setAttribute("zky_name", lhs_inputds_name);
	lhs_inputds->setScanner(input_scanner);
	lhs_inputds->setSchema(input_schema);
	mOdsStatDocData = lhs_inputds;
	
	//create outputds1
	boost::shared_ptr<DataSet> output_ds1 = boost::make_shared<DataSet>();
	output_ds1->setAttribute("zky_name", output_oldvalue_dsname);

	//create outputds_datafields
	boost::shared_ptr<DataFieldStr> output_df1 = boost::make_shared<DataFieldStr>();
	output_df1->setAttribute("type", "bin_u64");
	output_df1->setAttribute("zky_name", "value");
	output_df1->setAttribute("zky_offset", "0");
	output_df1->setAttribute("zky_length", "8");

	boost::shared_ptr<DataFieldStr> output_df2 = boost::make_shared<DataFieldStr>();
	output_df2->setAttribute("type", "bin_u64");
	output_df2->setAttribute("zky_name", "docid");
	output_df2->setAttribute("zky_offset", "8");
	output_df2->setAttribute("zky_length", "8");

	//create outputds_datarecord
	boost::shared_ptr<DataRecord> output_rcd = boost::make_shared<DataRecord>();
	output_rcd->setAttribute("type", "fixbin");
	output_rcd->setAttribute("zky_length", "16");
	output_rcd->setAttribute("zky_name", output_newvalue_rcdname);
	output_rcd->setField(output_df1);
	output_rcd->setField(output_df2);

	//create outputds_dataschema
	boost::shared_ptr<DataSchema> output_schema = boost::make_shared<DataSchema>();
	output_schema->setAttribute("zky_name", output_newvalue_schemaname);
	output_schema->setAttribute("jimo_objid", "dataschema_unilength_jimodoc_v0");
	output_schema->setRecord(output_rcd);

	//create outputds_dataconnector
	boost::shared_ptr<DataConnector> output_connector = boost::make_shared<DataConnector>();
	output_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");

	//create outputds_datascanner
	boost::shared_ptr<DataScanner> output_scanner = boost::make_shared<DataScanner>();
	output_scanner->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	output_scanner->setAttribute("zky_name", output_newvalue_scannername);
	output_scanner->setAttribute("buff_cache_num", "9");
	output_scanner->setConnector(output_connector);

	//create output_dataset
	boost::shared_ptr<DataSet> output_ds2 = boost::make_shared<DataSet>();
	output_ds2->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	output_ds2->setAttribute("zky_name", output_newvalue_dsname);
	output_ds2->setScanner(output_scanner);
	output_ds2->setSchema(output_schema);
	mOdsNewValue = output_ds2;

	//create reducetask1
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "bin_u64");
	asm_df1->setAttribute("zky_name", "value");
	asm_df1->setAttribute("zky_offset", "0");
	asm_df1->setAttribute("zky_length", "8");

	boost::shared_ptr<DataFieldStr> asm_df2 = boost::make_shared<DataFieldStr>();
	asm_df2->setAttribute("type", "bin_u64");
	asm_df2->setAttribute("zky_name", "docid");
	asm_df2->setAttribute("zky_offset", "8");
	asm_df2->setAttribute("zky_length", "8");

	boost::shared_ptr<DataRecord> rcdctnr_dr1 = boost::make_shared<DataRecord>();
	rcdctnr_dr1->setAttribute("type", "fixbin");
	rcdctnr_dr1->setAttribute("zky_name", df_oldvalue_rcdname);
	rcdctnr_dr1->setAttribute("zky_length", "16");
	rcdctnr_dr1->setField(asm_df1);
	rcdctnr_dr1->setField(asm_df2);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr = boost::make_shared<DataRecordCtnr>();
	asm_dr->setRecord(rcdctnr_dr1);
	asm_dr->setAttribute("zky_name", df_oldvalue_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf= boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
	dpf->setAttribute("zky_iilname", df_oldvalue_iilname);
	dpf->setAttribute("datasetname", output_oldvalue_dsname); 
	dpf->setAttribute("zky_name", df_oldvalue_procname);

	dpf->setCollectorType("iil");
	dpf->setCollectorAttr("zky_name", df_oldvalue_collectorname); 

	dpf->setAttribute(AOSTAG_OPERATOR, "u64add");
	dpf->setAssemblerType("u64add");
	dpf->setAssemblerAttr("zky_inctype", "norm");

	dpf->setRecord(asm_dr);

	boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", 16);
	cmp_fun->setCmpField("u64", 0);
	cmp_fun->setAggrField("u64", 8, "norm");
	dpf->setCompFun(cmp_fun);

	//create reducetask2
	boost::shared_ptr<DataFieldStr> asm_df3 = boost::make_shared<DataFieldStr>();
	asm_df3->setAttribute("type", "bin_u64");
	asm_df3->setAttribute("zky_name", "value");
	asm_df3->setAttribute("zky_offset", "0");
	asm_df3->setAttribute("zky_length", "8");

	boost::shared_ptr<DataFieldStr> asm_df4 = boost::make_shared<DataFieldStr>();
	asm_df4->setAttribute("type", "bin_u64");
	asm_df4->setAttribute("zky_name", "docid");
	asm_df4->setAttribute("zky_offset", "8");
	asm_df4->setAttribute("zky_length", "8");

	boost::shared_ptr<DataRecord> rcdctnr_dr2 = boost::make_shared<DataRecord>();
	rcdctnr_dr2->setAttribute("type", "fixbin");
	rcdctnr_dr2->setAttribute("zky_name", df_newvalue_rcdname);
	rcdctnr_dr2->setAttribute("zky_length", "16");
	rcdctnr_dr2->setField(asm_df3);
	rcdctnr_dr2->setField(asm_df4);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr2 = boost::make_shared<DataRecordCtnr>();
	asm_dr2->setRecord(rcdctnr_dr2);
	asm_dr2->setAttribute("zky_name", df_newvalue_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf2= boost::make_shared<ReduceTaskDataProc>("createdatasetdoc");
	dpf2->setAttribute("datasetname", output_newvalue_dsname);
	dpf2->setAttribute("zky_name", df_newvalue_procname);

	dpf2->setCollectorType("iil");
	dpf2->setCollectorAttr("zky_name", df_newvalue_collectorname); 

	dpf2->setAttribute(AOSTAG_OPERATOR, "u64add");
	dpf2->setAssemblerType("u64add");
	dpf2->setAssemblerAttr("zky_inctype", "norm");

	boost::shared_ptr<CompFun> cmp_fun2 = boost::make_shared<CompFun>("custom", 8);
	cmp_fun2->setCmpField("u64", 0);
	cmp_fun2->setAggrField("u64", 8, "norm");
	dpf2->setCompFun(cmp_fun2);


	dpf2->setRecord(asm_dr2);

	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	rtask->setDataProc(dpf);
	rtask->setDataProc(dpf2);

	//create maptask    
	boost::shared_ptr<DataEngineScanMul> de = boost::make_shared<DataEngineScanMul>();
	de->setAttribute("zky_type", "dataengine_scan_multircd");	

	boost::shared_ptr<DataProcStatDoc> dpc = boost::make_shared<DataProcStatDoc>();
	//dpc->setAttribute("zky_stat_def_doc_objid", mStatDocName);
	dpc->setAttribute("zky_stat_objid", mStatisticDocObjid);
	dpc->setAttribute("zky_stat_identify_key", mStatIdentifyKey);
	dpc->setAttribute("stat_internal_id", mInternalId);
	dpc->setStatInfo("stat_key", "str");
	dpc->setStatInfo("stat_docid", "docid");
	dpc->setStatInfo("stat_time", "time");
	dpc->setStatInfo("stat_isnew_flag", "isnew");

	OmnString input_fieldname_all = lhs_inputrcd_name;
	input_fieldname_all << ".all";

	OmnString oldvalue_fieldname_value = df_oldvalue_rcdname;
	oldvalue_fieldname_value << ".value";

	OmnString oldvalue_fieldname_docid = df_oldvalue_rcdname;
	oldvalue_fieldname_docid << ".docid";

	OmnString newvalue_fieldname_value = df_newvalue_rcdname;
	newvalue_fieldname_value << ".value";

	OmnString newvalue_fieldname_docid = df_newvalue_rcdname;
	newvalue_fieldname_docid << ".docid";
	
	dpc->setDataFieldsStr(mStatDocDataFields);
	dpc->setInputFieldName(input_fieldname_all);
	//dpc->setOldValue(oldvalue_fieldname_value, oldvalue_fieldname_docid);
	//dpc->setNewValue(newvalue_fieldname_value, newvalue_fieldname_docid);
	dpc->setLength(mStatDocDataRcdLen);
	OmnString fname;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		fname = "value";
		fname << i;
	
		OmnNotImplementedYet;
		//dpc->setStatValue(fname, mMeasures[i].name);
		//dpc->setStatVt2D(fname, mMeasures[i].vector2d_control, mVector2d, mMeasures[i].agr_type_node);                       
	}

	de->setDataProc(lhs_inputrcd_name, dpc);

	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	mtask->setDataEngine(de);

	OmnString task_name = mStatDocName;
	//task_name << "_" << mAgrType << "_" << mInputValueFieldName << "_" << mInternalId << "_task004";
	if(mIsDistinct && mStatConfLevel == "0-1")
	{
		task_name << "_distinct_" << mInternalId << "_task004";
	}
	else
	{
		task_name << "_" << mInternalId << "_task004";
	}
	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
//	jobtask->setAttribute("zky_starttype", "auto");
	jobtask->setAttribute("zky_taskid", "task004");
	jobtask->setMapTask(mtask);  
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(lhs_inputds);
	jobtask->setOutputDataset(output_ds1);
	jobtask->setOutputDataset(output_ds2);
/*
	//create job
	Job job;
	job.setAttribute("zky_jobid", "rootjob");
	job.setAttribute("zky_ctnrobjid", "task_ctnr");
	job.setAttribute("log_ctnr", "log_ctnr");
	job.setAttribute("zky_ispublic", "true");
	job.setAttribute("zky_objid", "job_example1_stat_task004_20140513");
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


boost::shared_ptr<JobTask> 
StatConf::createConfig4New() {
	OmnString pre_name = mStatRunDocName;
	pre_name << "_task003";
	
	OmnString output_rcd_name = mStatRunDocName;
	output_rcd_name << "_task004_rcd_buff_input";
	mStatDocDataRcdName = output_rcd_name;
	
	OmnString lhs_inputrcd_name = mStatDocDataRcdName;
	
	OmnString output_oldvalue_dsname = pre_name;
	output_oldvalue_dsname << "_dataset_oldvalue";
	
	OmnString output_newvalue_dsname = pre_name;
	output_newvalue_dsname << "_newvalue";
	mNewValueDsName = output_newvalue_dsname;

	OmnString output_newvalue_rcdname = mStatRunDocName;
	output_newvalue_rcdname << "_task005_rcd_newvalue_input";
	mNewValueRcdName = output_newvalue_rcdname;

	OmnString output_newvalue_schemaname = mStatRunDocName;
	output_newvalue_schemaname << "_task005_schema";

	OmnString output_newvalue_scannername = mStatRunDocName;
	output_newvalue_scannername << "_task005_scanner";

	OmnString df_oldvalue_procname = pre_name;
	df_oldvalue_procname << "_proc_oldvalue";

	OmnString df_oldvalue_collectorname = pre_name;
	df_oldvalue_collectorname << "_datacollector_oldvalue";

	OmnString df_oldvalue_rcdname = pre_name;
	df_oldvalue_rcdname << "_rcd_oldvalue";

	OmnString df_oldvalue_iilname = "_zt4k_";
	df_oldvalue_iilname << mStatRunDocName << "_oldvalue_temp";

	OmnString df_newvalue_rcdname = pre_name;
	df_newvalue_rcdname << "_rcd_newvalue";

	OmnString df_newvalue_procname = pre_name;
	df_newvalue_procname << "_proc_newvalue";

	OmnString df_newvalue_collectorname = pre_name;
	df_newvalue_collectorname << "_datacollector_newvalue";

	boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	createTask3InputDataset(lhs_inputds, lhs_inputrcd_name);
	/*
	//create inputds_datafields
	boost::shared_ptr<DataFieldStr> input_df = boost::make_shared<DataFieldStr>();
	input_df->setAttribute("type", "bin_varbuff");
	input_df->setAttribute("zky_name", "all");
	input_df->setAttribute("zky_offset", "0");
	input_df->setAttribute("zky_bufflength", "50000000");
	input_df->setAttribute("zky_minlen", mStatDocDataRcdLen);

	//create inputds_datarecord
	boost::shared_ptr<DataRecord> input_rcd = boost::make_shared<DataRecord>();
	input_rcd->setAttribute("type", "buff");
	input_rcd->setAttribute("zky_name", lhs_inputrcd_name);
	input_rcd->setField(input_df);

	//create inputds_dataschema
	boost::shared_ptr<DataSchema> input_schema = boost::make_shared<DataSchema>();
	input_schema->setAttribute("zky_name", input_schema_name);
	input_schema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	input_schema->setRecord(input_rcd);

	//create inputds_datasplit
	boost::shared_ptr<DataSplit> input_split = boost::make_shared<DataSplit>();
	input_split->setAttribute("jimo_objid", "dataspliter_filesbycube_jimodoc_v0");
	//input_split->setAttribute("jimo_objid", "dataspliter_files_jimodoc_v0");

	//create inputds_dataconnector
	boost::shared_ptr<DataConnector> input_connector = boost::make_shared<DataConnector>();
	//input_connector->setAttribute("jimo_objid", "dataconnector_dir_jimodoc_v0");
	input_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");

	// Ketty TTTT 2014/11/20
	//input_connector->setAttribute("zky_file_defname", mTask2FileDefName);
	if(mStatFields.size())
	{
		input_connector->setAttribute("zky_file_defname", mFileDefName);
	}
	else
	{
		input_connector->setAttribute("zky_file_defname", mTask2FileDefName);
	}
	input_connector->setSplit(input_split);

	//create inputds_datascanner
	boost::shared_ptr<DataScanner> input_scanner = boost::make_shared<DataScanner>();
	input_scanner->setAttribute("jimo_objid", "datascanner_parallel_jimodoc_v0");
	input_scanner->setAttribute("zky_name", input_scanner_name);
	input_scanner->setAttribute("buff_cache_num", "9");
	input_scanner->setConnector(input_connector);

	//create input_dataset
	boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	lhs_inputds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	lhs_inputds->setAttribute("zky_name", lhs_inputds_name);
	lhs_inputds->setScanner(input_scanner);
	lhs_inputds->setSchema(input_schema);
	mOdsStatDocData = lhs_inputds;
	*/

	//create outputds1
	boost::shared_ptr<DataSet> output_ds1 = boost::make_shared<DataSet>();
	output_ds1->setAttribute("zky_name", output_oldvalue_dsname);

	//create outputds_datafields
	boost::shared_ptr<DataFieldStr> output_df1 = boost::make_shared<DataFieldStr>();
	output_df1->setAttribute("type", "bin_u64");
	output_df1->setAttribute("zky_name", "value");
	output_df1->setAttribute("zky_offset", "0");
	output_df1->setAttribute("zky_length", "8");

	boost::shared_ptr<DataFieldStr> output_df2 = boost::make_shared<DataFieldStr>();
	output_df2->setAttribute("type", "bin_u64");
	output_df2->setAttribute("zky_name", "docid");
	output_df2->setAttribute("zky_offset", "8");
	output_df2->setAttribute("zky_length", "8");

	//create outputds_datarecord
	boost::shared_ptr<DataRecord> output_rcd = boost::make_shared<DataRecord>();
	output_rcd->setAttribute("type", "fixbin");
	output_rcd->setAttribute("zky_length", "16");
	output_rcd->setAttribute("zky_name", output_newvalue_rcdname);
	output_rcd->setField(output_df1);
	output_rcd->setField(output_df2);

	//create outputds_dataschema
	boost::shared_ptr<DataSchema> output_schema = boost::make_shared<DataSchema>();
	output_schema->setAttribute("zky_name", output_newvalue_schemaname);
	output_schema->setAttribute("jimo_objid", "dataschema_unilength_jimodoc_v0");
	output_schema->setRecord(output_rcd);

	//create outputds_dataconnector
	boost::shared_ptr<DataConnector> output_connector = boost::make_shared<DataConnector>();
	output_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");

	//create outputds_datascanner
	boost::shared_ptr<DataScanner> output_scanner = boost::make_shared<DataScanner>();
	output_scanner->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	output_scanner->setAttribute("zky_name", output_newvalue_scannername);
	output_scanner->setAttribute("buff_cache_num", "9");
	output_scanner->setConnector(output_connector);

	//create output_dataset
	boost::shared_ptr<DataSet> output_ds2 = boost::make_shared<DataSet>();
	output_ds2->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	output_ds2->setAttribute("zky_name", output_newvalue_dsname);
	output_ds2->setScanner(output_scanner);
	output_ds2->setSchema(output_schema);
	mOdsNewValue = output_ds2;

	//create reducetask1
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "bin_u64");
	asm_df1->setAttribute("zky_name", "value");
	asm_df1->setAttribute("zky_offset", "0");
	asm_df1->setAttribute("zky_length", "8");

	boost::shared_ptr<DataFieldStr> asm_df2 = boost::make_shared<DataFieldStr>();
	asm_df2->setAttribute("type", "bin_u64");
	asm_df2->setAttribute("zky_name", "docid");
	asm_df2->setAttribute("zky_offset", "8");
	asm_df2->setAttribute("zky_length", "8");

	boost::shared_ptr<DataRecord> rcdctnr_dr1 = boost::make_shared<DataRecord>();
	rcdctnr_dr1->setAttribute("type", "fixbin");
	rcdctnr_dr1->setAttribute("zky_name", df_oldvalue_rcdname);
	rcdctnr_dr1->setAttribute("zky_length", "16");
	rcdctnr_dr1->setField(asm_df1);
	rcdctnr_dr1->setField(asm_df2);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr = boost::make_shared<DataRecordCtnr>();
	asm_dr->setRecord(rcdctnr_dr1);
	asm_dr->setAttribute("zky_name", df_oldvalue_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf= boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
	dpf->setAttribute("zky_iilname", df_oldvalue_iilname);
	dpf->setAttribute("datasetname", output_oldvalue_dsname); 
	dpf->setAttribute("zky_name", df_oldvalue_procname);

	dpf->setCollectorType("iil");
	dpf->setCollectorAttr("zky_name", df_oldvalue_collectorname); 

	dpf->setAttribute(AOSTAG_OPERATOR, "u64add");
	dpf->setAssemblerType("u64add");
	dpf->setAssemblerAttr("zky_inctype", "norm");

	boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", 16);
	cmp_fun->setCmpField("u64", 0);
	cmp_fun->setAggrField("u64", 8, "norm");
	dpf->setCompFun(cmp_fun);

	dpf->setRecord(asm_dr);

	//create reducetask2
	boost::shared_ptr<DataFieldStr> asm_df3 = boost::make_shared<DataFieldStr>();
	asm_df3->setAttribute("type", "bin_u64");
	asm_df3->setAttribute("zky_name", "value");
	asm_df3->setAttribute("zky_offset", "0");
	asm_df3->setAttribute("zky_length", "8");

	boost::shared_ptr<DataFieldStr> asm_df4 = boost::make_shared<DataFieldStr>();
	asm_df4->setAttribute("type", "bin_u64");
	asm_df4->setAttribute("zky_name", "docid");
	asm_df4->setAttribute("zky_offset", "8");
	asm_df4->setAttribute("zky_length", "8");

	boost::shared_ptr<DataRecord> rcdctnr_dr2 = boost::make_shared<DataRecord>();
	rcdctnr_dr2->setAttribute("type", "fixbin");
	rcdctnr_dr2->setAttribute("zky_name", df_newvalue_rcdname);
	rcdctnr_dr2->setAttribute("zky_length", "16");
	rcdctnr_dr2->setField(asm_df3);
	rcdctnr_dr2->setField(asm_df4);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr2 = boost::make_shared<DataRecordCtnr>();
	asm_dr2->setRecord(rcdctnr_dr2);
	asm_dr2->setAttribute("zky_name", df_newvalue_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf2= boost::make_shared<ReduceTaskDataProc>("createdatasetdoc");
	dpf2->setAttribute("datasetname", output_newvalue_dsname);
	dpf2->setAttribute("zky_name", df_newvalue_procname);

	dpf2->setCollectorType("iil");
	dpf2->setCollectorAttr("zky_name", df_newvalue_collectorname); 

	dpf2->setAttribute(AOSTAG_OPERATOR, "u64add");
	dpf2->setAssemblerType("u64add");
	dpf2->setAssemblerAttr("zky_inctype", "norm");

	dpf2->setRecord(asm_dr2);

	boost::shared_ptr<CompFun> cmp_fun2 = boost::make_shared<CompFun>("custom", 8);
	cmp_fun2->setCmpField("u64", 0);
	cmp_fun2->setAggrField("u64", 8, "norm");
	dpf2->setCompFun(cmp_fun2);


	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	rtask->setDataProc(dpf);
	rtask->setDataProc(dpf2);

	//create maptask    
	boost::shared_ptr<DataEngineScanMul> de = boost::make_shared<DataEngineScanMul>();
	de->setAttribute("zky_type", "dataengine_scan_multircd");	
	
	//OmnString input_fieldname_all = lhs_inputrcd_name;
	//input_fieldname_all << ".all";

	boost::shared_ptr<DataProcStatDoc> dpc = boost::make_shared<DataProcStatDoc>();
	createTask3DataProcStatDoc(dpc, lhs_inputrcd_name);
	de->setDataProc(lhs_inputrcd_name, dpc);

	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	mtask->setDataEngine(de);

	OmnString task_name = mStatRunDocName;
	task_name << "_task003";
	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	jobtask->setAttribute("zky_taskid", "task003");
	jobtask->setMapTask(mtask);  
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(lhs_inputds);
	jobtask->setOutputDataset(output_ds1);
	jobtask->setOutputDataset(output_ds2);
	return jobtask;
}


bool
StatConf::createTask3InputDataset(
		boost::shared_ptr<DataSet> &lhs_inputds,
		const OmnString &lhs_inputrcd_name)
{
	boost::shared_ptr<DataRecord> input_rcd = boost::make_shared<DataRecord>();
	//createTask3InputDataRecord(input_rcd, lhs_inputrcd_name);
	
	input_rcd->setAttribute("type", "buff");
	input_rcd->setAttribute("zky_name", lhs_inputrcd_name);
	
	//create inputds_datafields
	boost::shared_ptr<DataFieldStr> input_df = boost::make_shared<DataFieldStr>();
	input_df->setAttribute("type", "bin_varbuff");
	input_df->setAttribute("zky_name", "all");
	input_df->setAttribute("zky_offset", "0");
	input_df->setAttribute("zky_bufflength", "50000000");
	//input_df->setAttribute("zky_minlen", mStatDocDataRcdLen);

	OmnString rcd_len_str;
	if(mStatFields.size())
	{
		rcd_len_str = mTask2ReduceDataRecord->getAttribute("zky_length");
	}
	else
	{
		rcd_len_str = mTask1ReduceDataRecord->getAttribute("zky_length");
	}
	input_df->setAttribute("zky_minlen", rcd_len_str);

	input_rcd->setField(input_df);

	//create inputds_dataschema
	OmnString input_schema_name = mStatRunDocName;
	input_schema_name << "_task003_schema";
	boost::shared_ptr<DataSchema> input_schema = boost::make_shared<DataSchema>();
	input_schema->setAttribute("zky_name", input_schema_name);
	input_schema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	input_schema->setRecord(input_rcd);
	
	//create inputds_dataconnector
	boost::shared_ptr<DataConnector> input_connector = boost::make_shared<DataConnector>();
	input_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");

	// Ketty TTTT 2014/11/20
	//input_connector->setAttribute("zky_file_defname", mTask2FileDefName);
	if(mStatFields.size())
	{
		input_connector->setAttribute("zky_file_defname", mTask2FileDefName);
	
		//create inputds_datasplit
		boost::shared_ptr<DataSplit> input_split = boost::make_shared<DataSplit>();
		input_split->setAttribute("jimo_objid", "dataspliter_filesbycube_jimodoc_v0");
	
		input_connector->setSplit(input_split);
	}
	else
	{
		input_connector->setAttribute("zky_file_defname", mFileDefName);
	}

	//create inputds_datascanner
	OmnString input_scanner_name = mStatRunDocName;
	input_scanner_name << "_task003_scanner";

	boost::shared_ptr<DataScanner> input_scanner = boost::make_shared<DataScanner>();
	input_scanner->setAttribute("jimo_objid", "datascanner_parallel_jimodoc_v0");
	input_scanner->setAttribute("zky_name", input_scanner_name);
	input_scanner->setAttribute("buff_cache_num", "9");
	input_scanner->setConnector(input_connector);

	//create input_dataset
	OmnString lhs_inputds_name = mStatDocDataDsName;
	
	lhs_inputds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	lhs_inputds->setAttribute("zky_name", lhs_inputds_name);
	lhs_inputds->setScanner(input_scanner);
	lhs_inputds->setSchema(input_schema);
	mOdsStatDocData = lhs_inputds;
	
	return true;
}


bool
StatConf::createTask3InputDataRecord(
		boost::shared_ptr<DataRecord> &input_rcd,
		const OmnString &lhs_inputrcd_name)
{
	map<string, string> attrs;
	if(mStatFields.size() == 0)
	{
		// use task1's reduce data_record.
		input_rcd->setFields(mTask1ReduceDataRecord->getFields());
		attrs = mTask1ReduceDataRecord->getAttributes();
	}
	else
	{
		// use task2's reduce data_record.
		input_rcd->setFields(mTask2ReduceDataRecord->getFields());
		attrs = mTask2ReduceDataRecord->getAttributes();
	}
	
	map<string, string>::iterator itr = attrs.begin();
	for(; itr != attrs.end(); itr++)
	{
		input_rcd->setAttribute(itr->first, itr->second);
	}

	// Temp.
	//input_rcd->setAttribute("zky_name", lhs_inputrcd_name);
	if(lhs_inputrcd_name != "")	input_rcd->setAttribute("zky_name", lhs_inputrcd_name);
	
	return true;
}


bool
StatConf::createTask3DataProcStatDoc(
		boost::shared_ptr<DataProcStatDoc> &dpc,
		const OmnString &lhs_inputrcd_name)
{
	//boost::shared_ptr<DataProcStatDoc> dpc = boost::make_shared<DataProcStatDoc>();
	dpc->setAttribute("zky_stat_name", mStatName);
	dpc->setStatInfo("stat_key", "statkey");
	dpc->setStatInfo("stat_docid", "sdocid");
	dpc->setStatInfo("stat_isnew_flag", "isnew");

	OmnString input_fieldname_all = lhs_inputrcd_name;
	input_fieldname_all << ".all";

	//OmnString oldvalue_fieldname_value = df_oldvalue_rcdname;
	//oldvalue_fieldname_value << ".value";

	//OmnString oldvalue_fieldname_docid = df_oldvalue_rcdname;
	//oldvalue_fieldname_docid << ".docid";

	//OmnString newvalue_fieldname_value = df_newvalue_rcdname;
	//newvalue_fieldname_value << ".value";

	//OmnString newvalue_fieldname_docid = df_newvalue_rcdname;
	//newvalue_fieldname_docid << ".docid";

	// Ketty TTTT
	//dpc->setDataFieldsStr(mStatDocDataFields);
	//dpc->setInputFieldName(input_fieldname_all);
	//dpc->setLength(mStatDocDataRcdLen);

	boost::shared_ptr<DataRecord> input_rcd = boost::make_shared<DataRecord>();
	createTask3InputDataRecord(input_rcd, "");
	input_rcd->setAttribute("zky_input_field_name", input_fieldname_all);
	input_rcd->setTagName("input");
	dpc->setInputRecord(input_rcd);
	
	OmnString time_input_name, value_input_name;
	OmnString grpby_time_unit;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		value_input_name = "value";
		value_input_name << i;
	
		if(mMeasures[i].stat_time_idx == -1)
		{
			time_input_name = "";
			grpby_time_unit = "";
		}
		else
		{
			time_input_name = "time";
			time_input_name << mMeasures[i].stat_time_idx;
			grpby_time_unit = mMeasures[i].grpby_time_unit;		
		}
		
		dpc->setStatMeasure(mMeasures[i].name, grpby_time_unit,
				time_input_name, value_input_name);
	}
	
	return true;
}

