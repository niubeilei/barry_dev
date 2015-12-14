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
StatConf::createConfig3() {
	OmnString stat_name = mStatDocName;
	stat_name << "_task003_";

	OmnString rhs_inputrcd_name = stat_name;
	rhs_inputrcd_name << "rcd_rhs_iil";

	OmnString input_schema_name = stat_name;
	input_schema_name << "schema_rhs";

	OmnString input_scanner_name = stat_name;
	input_scanner_name << "scanner_rhs";
	
	OmnString input_iilname = mOutputIILNameAll;

	OmnString output_dsname = stat_name;
	//output_dsname << "_" << mAgrType << "_" << mInputValueFieldName << "_" << mInternalId << "_statdocdata";
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

	OmnString df_rcd_name = stat_name;
	df_rcd_name << "rcd_statdocdata";

	OmnString df_connector_name = stat_name;
	df_connector_name << "datacollector_statdocdata";

	OmnString df_dataproc_name = stat_name;
	df_dataproc_name << "proc_statdocdata";

	OmnString df_iilname = "_zt44_";
	df_iilname << mStatDocName << "_task003_statdocdata";

	OmnString rhs_inputds_name = "rhs";

	//create lhs_input_dataset
	OmnString lhs_inputds_name = mOdsAllDatasetName;
	OmnString lhs_inputrcd_name = mOutputRcdNameAll;
	boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	lhs_inputds->setAttribute("zky_dataset_name", lhs_inputds_name);

	//create rhs_inputds_datafields
	int input_rcd_len = mMaxLen;
	boost::shared_ptr<DataFieldStr> input_df1 = boost::make_shared<DataFieldStr>();
	input_df1->setAttribute("type", "str");
	input_df1->setAttribute("zky_name", "value");
	input_df1->setAttribute("zky_offset", "0");
	input_df1->setAttribute("zky_length", input_rcd_len);
	input_df1->setAttribute("zky_datatooshortplc", "cstr");

	boost::shared_ptr<DataFieldStr> input_df2 = boost::make_shared<DataFieldStr>();
	input_df2->setAttribute("type", "bin_u64");
	input_df2->setAttribute("zky_name", "docid");
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
	input_schema->setAttribute("zky_name", input_schema_name);
	input_schema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	input_schema->setRecord(input_rcd);

	//create rhs_inputds_dataconnector
	boost::shared_ptr<DataConnectorIIL> input_connector = boost::make_shared<DataConnectorIIL>();
	input_connector->setAttribute("jimo_objid", "dataconnector_iil_jimodoc_v0");
	input_connector->setAttribute("zky_iilname", input_iilname); 
	input_connector->setAttribute("zky_blocksize", "2000000");
	input_connector->setQueryCond("an", "*"); 

	//create rhs_inputds_datascanner
	boost::shared_ptr<DataScanner> input_scanner = boost::make_shared<DataScanner>();
	input_scanner->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	input_scanner->setAttribute("zky_name", input_scanner_name);
	input_scanner->setAttribute("buff_cache_num", "1");
	input_scanner->setConnector(input_connector);

	//create rhs_input_dataset
	boost::shared_ptr<DataSet> rhs_inputds = boost::make_shared<DataSet>();
	rhs_inputds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	rhs_inputds->setAttribute("zky_name", rhs_inputds_name);
	rhs_inputds->setScanner(input_scanner);
	rhs_inputds->setSchema(input_schema);
	
	//create reducetask
	boost::shared_ptr<DataRecord> rcdctnr_dr = boost::make_shared<DataRecord>();
	
	mStatDocDataRcdLen = 0;
	mStatDocDataFields = "";
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "bin_u64");
	asm_df1->setAttribute("zky_name", "docid");
	asm_df1->setAttribute("zky_offset", "0");
	asm_df1->setAttribute("zky_length", "8");
	asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	mStatDocDataRcdLen += 8;
	rcdctnr_dr->setField(asm_df1);
	mStatDocDataFields << asm_df1->getConfig();

	boost::shared_ptr<DataFieldStr> asm_df2 = boost::make_shared<DataFieldStr>();
	asm_df2->setAttribute("type", "bin_u64");
	asm_df2->setAttribute("zky_name", "time");
	asm_df2->setAttribute("zky_offset", mStatDocDataRcdLen);
	asm_df2->setAttribute("zky_length", "8");
	mStatDocDataRcdLen += 8;
	rcdctnr_dr->setField(asm_df2);
	mStatDocDataFields << asm_df2->getConfig();

	OmnString vname, type;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		type = "bin_int64";
		if (mMeasures[i].field_type == "double" || mMeasures[i].field_type == "number")
			type = "bin_double";

		vname = "value";
		vname << i;
		boost::shared_ptr<DataFieldStr> asm_df3 = boost::make_shared<DataFieldStr>();

		asm_df3->setAttribute("type", type);
		asm_df3->setAttribute("zky_name", vname);
		asm_df3->setAttribute("zky_offset", mStatDocDataRcdLen);
		asm_df3->setAttribute("zky_length", "8");
		mStatDocDataRcdLen += 8;
		rcdctnr_dr->setField(asm_df3);
		mStatDocDataFields << asm_df3->getConfig();
	}
	/*
	boost::shared_ptr<DataFieldStr> asm_df3 = boost::make_shared<DataFieldStr>();
	asm_df3->setAttribute("type", "bin_u64");
	asm_df3->setAttribute("zky_name", "value");
	asm_df3->setAttribute("zky_offset", mStatDocDataRcdLen);
	asm_df3->setAttribute("zky_length", "8");
	mStatDocDataRcdLen += 8;
	*/

	boost::shared_ptr<DataFieldStr> asm_df4 = boost::make_shared<DataFieldStr>();
	asm_df4->setAttribute("type", "str");
	asm_df4->setAttribute("zky_name", "str");
	asm_df4->setAttribute("zky_offset", mStatDocDataRcdLen);
	asm_df4->setAttribute("zky_length", mMaxLen);
	asm_df4->setAttribute("zky_datatooshortplc", "cstr");
	mStatDocDataRcdLen += mMaxLen;
	rcdctnr_dr->setField(asm_df4);
	mStatDocDataFields << asm_df4->getConfig();

	//boost::shared_ptr<DataRecord> rcdctnr_dr = boost::make_shared<DataRecord>();
	rcdctnr_dr->setAttribute("type", "fixbin");
	rcdctnr_dr->setAttribute("zky_name", df_rcd_name);
	rcdctnr_dr->setAttribute("zky_length", mStatDocDataRcdLen);
	//rcdctnr_dr->setField(asm_df1);
	//rcdctnr_dr->setField(asm_df2);
	//rcdctnr_dr->setField(asm_df3);
	//rcdctnr_dr->setField(asm_df4);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr = boost::make_shared<DataRecordCtnr>();
	asm_dr->setRecord(rcdctnr_dr);
	asm_dr->setAttribute("zky_name", df_rcd_name);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf= boost::make_shared<ReduceTaskDataProc>("createdatasetdoc");
	dpf->setAttribute("zky_iilname", df_iilname);
	dpf->setAttribute("datasetname", output_dsname); 
	dpf->setAttribute("zky_name", df_dataproc_name);

	dpf->setCollectorType("iil");
	dpf->setCollectorAttr("zky_name", df_connector_name); 
	dpf->setCollectorAttr("group_type", "physical_group"); 

	dpf->setAttribute(AOSTAG_OPERATOR, "strinc");
	dpf->setAssemblerType("strinc");
	dpf->setAssemblerAttr("zky_inctype", "norm");

	dpf->setRecord(asm_dr);

	boost::shared_ptr<CompFun> fun = boost::make_shared<CompFun>("custom", mStatDocDataRcdLen);
	fun->setCmpField("u64", 0, 8);
	fun->setCmpField("u64", 8, 8);
	dpf->setCompFun(fun);
	
	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	rtask->setDataProc(dpf);

	//create outputds_datafields
	boost::shared_ptr<DataFieldStr> output_df = boost::make_shared<DataFieldStr>();
	output_df->setAttribute("type", "bin_varbuff");
	output_df->setAttribute("zky_name", "all");
	output_df->setAttribute("zky_offset", "0");
	output_df->setAttribute("zky_bufflength", "50000000");
	output_df->setAttribute("zky_minlen", mStatDocDataRcdLen);

	//create outputds_datarecord
	boost::shared_ptr<DataRecord> output_rcd = boost::make_shared<DataRecord>();
	output_rcd->setAttribute("type", "buff");
	output_rcd->setAttribute("zky_name", output_rcd_name);
	output_rcd->setField(output_df);

	//create outputds_dataschema
	boost::shared_ptr<DataSchema> output_schema = boost::make_shared<DataSchema>();
	output_schema->setAttribute("zky_name", output_schema_name);
	output_schema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	output_schema->setRecord(output_rcd);

	//create outputds_datasplit
	boost::shared_ptr<DataSplit> output_split = boost::make_shared<DataSplit>();
	output_split->setAttribute("jimo_objid", "dataspliter_files_jimodoc_v0");

	//create outputds_dataconnector
	boost::shared_ptr<DataConnector> output_connector = boost::make_shared<DataConnector>();
	output_connector->setAttribute("jimo_objid", "dataconnector_dir_jimodoc_v0");
	output_connector->setSplit(output_split);

	//create outputds_datascanner
	boost::shared_ptr<DataScanner> output_scanner = boost::make_shared<DataScanner>();
	output_scanner->setAttribute("jimo_objid", "datascanner_parallel_jimodoc_v0");
	output_scanner->setAttribute("zky_name", output_scanner_name);
	output_scanner->setAttribute("buff_cache_num", "9");
	output_scanner->setConnector(output_connector);

	//create output_dataset
	boost::shared_ptr<DataSet> output_ds = boost::make_shared<DataSet>();
	output_ds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	output_ds->setAttribute("zky_name", output_dsname);
	output_ds->setScanner(output_scanner);
	output_ds->setSchema(output_schema);
	mOdsStatDocData = output_ds;

	//create maptask   
	boost::shared_ptr<DataEngineJoin> de = boost::make_shared<DataEngineJoin>();
	de->setConditionAttr("join_type", "left");
	de->setConditionAttr("unique", "false");
	de->setLhsDatasetName(lhs_inputds_name);
	de->setRhsDatasetName(rhs_inputds_name);
	de->setCondType("true_cond");
	de->setAttribute("need_snapshot", "true");

	OmnString lhs_field_name_str = lhs_inputrcd_name;
	lhs_field_name_str << ".str";

	OmnString lhs_field_name_epochday = lhs_inputrcd_name;
	lhs_field_name_epochday << ".time";

	OmnString lhs_field_name_value = lhs_inputrcd_name;
	lhs_field_name_value << ".value";

	OmnString rhs_field_name_value = rhs_inputrcd_name;
	rhs_field_name_value << ".value";

	OmnString rhs_field_name_docid = rhs_inputrcd_name;
	rhs_field_name_docid << ".docid";

	OmnString df_field_name_docid = df_rcd_name;
	df_field_name_docid << ".docid";

	OmnString df_field_name_epochday = df_rcd_name;
	df_field_name_epochday << ".time";

	OmnString df_field_name_value = df_rcd_name;
	df_field_name_value << ".value";

	OmnString df_field_name_str = df_rcd_name;
	df_field_name_str << ".str";

	de->setLhsFieldName(lhs_field_name_str);
	de->setRhsFieldName(rhs_field_name_value);
	
	boost::shared_ptr<DataProcStatDocShuffle> dpc = boost::make_shared<DataProcStatDocShuffle>();
	//dpc->setStatVt2DControl(mVector2dControl);
	OmnString lhs_fname_value, df_fname_value;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
//		dpc->setStatVt2DControl(mMeasures[i].vector2d_control);
		lhs_fname_value = lhs_field_name_value;
		lhs_fname_value << i;
		df_fname_value = df_field_name_value;
		df_fname_value << i;
		dpc->set("statvalue", lhs_fname_value, df_fname_value);
	}

	dpc->set("statid", rhs_field_name_docid, df_field_name_docid);
	dpc->set("stattime", lhs_field_name_epochday, df_field_name_epochday);
//	dpc->set("statvalue", lhs_field_name_value, df_field_name_value);
	dpc->set("statmeasure", lhs_field_name_str, df_field_name_str);

	de->setDataProc(dpc);
	
	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	mtask->setDataEngine(de);


	OmnString task_name = mStatDocName;
	//task_name << "_" << mAgrType << "_" << mInputValueFieldName << "_" << mInternalId << "_task003";
	if(mIsDistinct && mStatConfLevel == "0-1")
	{
		task_name << "_distinct_" << mInternalId << "_task003";
	}
	else
	{
		task_name << "_" << mInternalId << "_task003";
	}

	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	//jobtask->setAttribute("zky_starttype", "auto");
	jobtask->setAttribute("zky_taskid", "task003");
	jobtask->setMapTask(mtask);  
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(lhs_inputds);
	jobtask->setInputDataset(rhs_inputds);
	jobtask->setOutputDataset(output_ds);
//	jobtask->setHasSplit(true);
/*
	//create job
	Job job;
	job.setAttribute("zky_jobid", "rootjob");
	job.setAttribute("zky_ctnrobjid", "task_ctnr");
	job.setAttribute("log_ctnr", "log_ctnr");
	job.setAttribute("zky_ispublic", "true");
	job.setAttribute("zky_objid", "job_example1_stat_task003_20140513");
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
