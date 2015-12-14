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
#include "AosConf/DataProcCreateDatasetDoc.h"
#include "AosConf/ReduceTask.h"
#include "AosConf/MapTask.h"
#include "AosConf/DataSet.h"
#include "AosConf/JobTask.h"
#include "AosConf/Job.h"
#include "AosConf/DataFieldMath.h"
#include <boost/make_shared.hpp>
#include <string>

using boost::make_shared;
using AosConf::StatConf;
using AosConf::JobTask;

boost::shared_ptr<JobTask> 
StatConf::createConfig1() {
	OmnString inputValueFieldName = "";
	if (mHasCount && mStatConfLevel != "1")
	{
		// Ketty 2014/06/05
		//mInputValueFieldName = "field_count";
		inputValueFieldName = "0x2a";		//0x2a is actual *;
	}

	OmnString stat_definition_key = mStatDocName;
	stat_definition_key << "_" << mInternalId;
	
	OmnString distinct_stat_definition_key = mStatDocName;
	distinct_stat_definition_key << "_distinct_" << mDistinctInternalId;

	boost::shared_ptr<DataSet> inputds;
	OmnString inputrecordname;
		
	boost::shared_ptr<DataFieldStr> input_df3 = boost::make_shared<DataFieldStr>();
	input_df3->setAttribute("zky_name", "0x01");
	input_df3->setAttribute("type", "const");
	input_df3->setConst("0x01");

	boost::shared_ptr<DataFieldStr> input_df4 = boost::make_shared<DataFieldStr>();
	input_df4->setAttribute("zky_name", inputValueFieldName);
	input_df4->setAttribute("type", "const");
	input_df4->setConst("1");
	
	boost::shared_ptr<DataFieldStr> input_df5 = boost::make_shared<DataFieldStr>();
	input_df5->setAttribute("zky_name", mInputTimeFieldName);
	input_df5->setAttribute("type", "const");
	input_df5->setConst(mStartTimeSlot);
	
	boost::shared_ptr<DataFieldStr> input_df6 = boost::make_shared<DataFieldStr>();
	input_df6->setAttribute("zky_name", mInputTimeFieldName);
	input_df6->setAttribute("type", "time");
	input_df6->setAttribute(AOSTAG_VALUEFROMFIELD, mTimeFromField);
	input_df6->setAttribute(AOSTAG_ORIG_TIME_FORMAT, mTimeFromFormat);
	input_df6->setAttribute(AOSTAG_TARGET_TIME_FORMAT, mTimeToFormat);

	boost::shared_ptr<DataFieldMath> input_df7 = boost::make_shared<DataFieldMath>();
	input_df7->setAttribute("zky_name", "statkey");
	input_df7->setAttribute("zky_opr", "concat");
	input_df7->setAttribute("zky_sep", "0x01");
	for(size_t i=0; i<mStatFields.size(); i++)
	{
		input_df7->setFieldName(mStatFields[i]);
	}

	boost::shared_ptr<DataFieldStr> input_df_distinct_count_value = boost::make_shared<DataFieldStr>();
	input_df_distinct_count_value->setAttribute("zky_name", "distinct_count_value");
	input_df_distinct_count_value->setAttribute("type", "const");
	input_df_distinct_count_value->setConst("1");

	boost::shared_ptr<DataFieldStr> input_df_distinct_count_time = boost::make_shared<DataFieldStr>();
	input_df_distinct_count_time->setAttribute("zky_name", "distinct_count_time");
	input_df_distinct_count_time->setAttribute("type", "const");
	input_df_distinct_count_time->setConst(mStartTimeSlot);

	if (mInternalId == "0" || mStatConfLevel == "1")
	{
		//create inputdataset
		inputds = boost::make_shared<DataSet>(mInputDataset);
		boost::shared_ptr<DataSchema> inds_schema = inputds->getSchema();
		boost::shared_ptr<DataRecord> inputrcd = inds_schema->getRecord();
		inputrcd->setField(input_df3);
		if (mHasCount && mStatConfLevel != "1")
		{
			inputrcd->setField(input_df4);
		}
		if(mHasDistinct && mStatConfLevel == "0-1")
		{
			inputrcd->setField(input_df_distinct_count_value);
			inputrcd->setField(input_df_distinct_count_time);
		}
		if(mTimeFromField == "")
		{
			inputrcd->setField(input_df5);
		}
		else
		{
			inputrcd->setField(input_df6);
		}
		inputrcd->setField(input_df7);
		inputrecordname = inputrcd->getAttribute("zky_name");
	}
	else if(mStatConfLevel == "0-1")
	{
		OmnString lhs_inputds_name = mOdsInputAllDatasetName;
		inputrecordname = mInputRcdNameAll;
		inputds = boost::make_shared<DataSet>();
		inputds->setAttribute("zky_dataset_name", lhs_inputds_name);
	}

	AosXmlTagPtr xml_scanner = mInputDataset->getFirstChild("datascanner");
	OmnString dscanner_objid = xml_scanner->getAttrStr("jimo_objid");
	AosXmlTagPtr xml_schema = mInputDataset->getFirstChild("dataschema");
	OmnString ds_name1 = mStatDocName;
	ds_name1 << "_" << mInternalId << "_one";
	
	OmnString ds_name2  = mStatDocName;
	ds_name2 << "_" << mInternalId << "_all";

	OmnString distinct_ds_name2 = mStatDocName;
	distinct_ds_name2 << "_distinct_" << mDistinctInternalId << "_all";

	OmnString statname = mStatDocName;
	
	//OmnString asm_recordname1 = statname;
	//asm_recordname1 <<  "_rcd_one"; 

	OmnString asm_recordname = statname;
	asm_recordname <<  "_rcd_all"; 

	OmnString distinct_asm_recordname2 = statname;
	distinct_asm_recordname2 << "_distinct_rcd_all"; 

	OmnString collectorobjid1 = statname;
	collectorobjid1 << "_all_tmp_data";

	OmnString distinct_collectorobjid1 = statname;
	distinct_collectorobjid1 << "_distinct_all_tmp_data";

	//create maptask
	boost::shared_ptr<DataEngineScanMul> de = boost::make_shared<DataEngineScanMul>();
	de->setAttribute("zky_type", "dataengine_scan_multircd");	
	
	boost::shared_ptr<DataProcStatDocShuffle> dpc = boost::make_shared<DataProcStatDocShuffle>();
	dpc->setAttribute("shuffle_type", "cube");

	OmnString lhs_fname_value, df_fname_value;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		lhs_fname_value = inputrecordname;
		lhs_fname_value << "." << mMeasures[i].field_name;
		
		df_fname_value = asm_recordname;
		df_fname_value << ".value" << i;
		
		dpc->set("stat_value", lhs_fname_value, df_fname_value);
	}

	lhs_fname_value = inputrecordname;
	lhs_fname_value << ".statkey";
	df_fname_value = asm_recordname;
	df_fname_value << ".str";
	dpc->set("stat_key", lhs_fname_value, df_fname_value);

	lhs_fname_value = inputrecordname;
	lhs_fname_value << "." << mInputTimeFieldName;
	df_fname_value = asm_recordname;
	df_fname_value << ".time";
	dpc->set("stat_time", lhs_fname_value, df_fname_value);

	df_fname_value = asm_recordname;
	df_fname_value << ".cube_id";
	dpc->set("stat_cubeid", "", df_fname_value);

	de->setDataProc(inputrecordname, dpc);
	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	mtask->setDataEngine(de);

//create reducetask
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "str");
	asm_df1->setAttribute("zky_name", "str");
	asm_df1->setAttribute("zky_offset", "0");
	asm_df1->setAttribute("zky_length", mMaxLen);
	asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	
	boost::shared_ptr<DataRecord> rcdctnr_dr2 = boost::make_shared<DataRecord>();
	rcdctnr_dr2->setField(asm_df1);
	
	int rcd_len2 = mMaxLen;
	boost::shared_ptr<DataFieldStr> asm_df3 = boost::make_shared<DataFieldStr>();
	asm_df3->setAttribute("type", "bin_u64");
	asm_df3->setAttribute("zky_name", "time");
	asm_df3->setAttribute("zky_offset", rcd_len2);
	asm_df3->setAttribute("zky_length", "8");
	rcd_len2 += 8;
	rcdctnr_dr2->setField(asm_df3);

	OmnString vname, fun, type;
	vector<Aggregation>   aggrs;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		type = "bin_int64";
		fun = mMeasures[i].agr_type;
		if (fun != "min" && fun != "max")
		{
			fun = "norm";
		}

		vname = "value";
		vname << i;
		boost::shared_ptr<DataFieldStr> asm_df4 = boost::make_shared<DataFieldStr>();
		if (mMeasures[i].field_type == "double" || mMeasures[i].field_type == "number")
			type = "bin_double";

		asm_df4->setAttribute("type", type);
		asm_df4->setAttribute("zky_name", vname);
		asm_df4->setAttribute("zky_offset", rcd_len2);
		asm_df4->setAttribute("zky_length", "8");
		
		Aggregation aggr;

		if (mMeasures[i].field_type == "double" 
				|| mMeasures[i].field_type == "number")
			aggr.agr_type = "double";
		else
			aggr.agr_type = "int64";

		aggr.agr_fun = fun;
		aggr.agr_pos = rcd_len2;
		aggrs.push_back(aggr);
		
		rcd_len2 += 8;
		rcdctnr_dr2->setField(asm_df4);
	}
	
	boost::shared_ptr<DataFieldStr> asm_df5 = boost::make_shared<DataFieldStr>();
	asm_df5->setAttribute("type", "bin_u64");
	asm_df5->setAttribute("zky_name", "cube_id");
	asm_df5->setAttribute("zky_offset", rcd_len2);
	asm_df5->setAttribute("zky_length", "8");
	rcd_len2 += 8;
	rcdctnr_dr2->setField(asm_df5);
	
	rcdctnr_dr2->setAttribute("type", "fixbin");
	rcdctnr_dr2->setAttribute("zky_name", asm_recordname);
	rcdctnr_dr2->setAttribute("zky_length", rcd_len2);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr2 = boost::make_shared<DataRecordCtnr>();
	asm_dr2->setRecord(rcdctnr_dr2);
	asm_dr2->setAttribute("zky_name", asm_recordname);

	mFileDefName = mStatDocName;
	mFileDefName << "_" << mInternalId << "_all_files";

	boost::shared_ptr<ReduceTaskDataProc> dpf2= boost::make_shared<ReduceTaskDataProc>("createtaskfile");
	dpf2->setAttribute("datasetname", ds_name2);
	dpf2->setAttribute("run_mode", "run_parent");
	dpf2->setAttribute("zky_file_defname", mFileDefName);
	//dpf2->setAttribute("zky_name", "example1_task001_proc_all");
	dpf2->setAttribute("zky_name", ds_name2);
	dpf2->setCollectorType("iil");
	dpf2->setCollectorAttr("zky_name", ds_name2);
	dpf2->setCollectorAttr("group_type", "cube_shuffle");
	dpf2->setAttribute(AOSTAG_OPERATOR, "strinc");
	dpf2->setAssemblerType("strinc");
	dpf2->setAssemblerAttr("zky_inctype", "norm");
	dpf2->setRecord(asm_dr2);

	boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", rcd_len2);
	cmp_fun->setCmpField("str", 0);
	cmp_fun->setCmpField("u64", mMaxLen, 8);

	for(size_t i=0; i<aggrs.size(); i++)
	{
		cmp_fun->setAggrField(aggrs[i].agr_type, aggrs[i].agr_pos, aggrs[i].agr_fun);
	}
	dpf2->setCompFun(cmp_fun);

	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	rtask->setDataProc(dpf2);
/*
	if (mHasDistinct && mInternalId == "0" && mStatConfLevel == "0-1")
	{
		
		shared_ptr<DataFieldStr> distinct_asm_df1 = make_shared<DataFieldStr>();
		distinct_asm_df1->setAttribute("type", "str");
		distinct_asm_df1->setAttribute("zky_name", "str");
		distinct_asm_df1->setAttribute("zky_offset", "0");
		distinct_asm_df1->setAttribute("zky_length", mDistinctMaxLen);
		distinct_asm_df1->setAttribute("zky_datatooshortplc", "cstr");
		
		shared_ptr<DataRecord> distinct_rcdctnr_dr2 = make_shared<DataRecord>();
		distinct_rcdctnr_dr2->setField(distinct_asm_df1);

		int distinct_rcd_len2 = mDistinctMaxLen;
		shared_ptr<DataFieldStr> distinct_asm_df3 = make_shared<DataFieldStr>();
		distinct_asm_df3->setAttribute("type", "bin_u64");
		distinct_asm_df3->setAttribute("zky_name", "time");
		distinct_asm_df3->setAttribute("zky_offset", distinct_rcd_len2);
		distinct_asm_df3->setAttribute("zky_length", "8");
		distinct_rcd_len2 += 8;
		distinct_rcdctnr_dr2->setField(distinct_asm_df3);

		OmnString distinct_aggregations = "<";
		distinct_aggregations << AOSTAG_AGGREGATIONS << ">";

		shared_ptr<DataFieldStr> distinct_asm_df4 = make_shared<DataFieldStr>();
		distinct_asm_df4->setAttribute("type", "bin_u64");
		distinct_asm_df4->setAttribute("zky_name", "value");
		distinct_asm_df4->setAttribute("zky_offset", distinct_rcd_len2);
		distinct_asm_df4->setAttribute("zky_length", "8");
		distinct_aggregations << "<" << AOSTAG_AGGREGATION << " agr_pos=\"" << distinct_rcd_len2 << "\" "
			<< "agr_type=\"u64\" "
			<< "agr_fun=\"set\" />";

		distinct_rcd_len2 += 8;
		distinct_rcdctnr_dr2->setField(distinct_asm_df4);
	
		distinct_aggregations << "</" << AOSTAG_AGGREGATIONS << ">";

		distinct_rcdctnr_dr2->setAttribute("type", "fixbin");
		distinct_rcdctnr_dr2->setAttribute("zky_name", distinct_asm_recordname2);
		distinct_rcdctnr_dr2->setAttribute("zky_length", distinct_rcd_len2);
		
		shared_ptr<DataRecordCtnr> distinct_asm_dr2 = make_shared<DataRecordCtnr>();
		distinct_asm_dr2->setRecord(distinct_rcdctnr_dr2);
		distinct_asm_dr2->setAttribute("zky_name", distinct_asm_recordname2);

		shared_ptr<ReduceTaskDataProc> dpf2= make_shared<ReduceTaskDataProc>("createdatasetdoc");
		dpf2->setAttribute("datasetname", distinct_ds_name2);
		dpf2->setAttribute("zky_name", "example1_task001_distinct_proc_all");
		dpf2->setCollectorType("iil");
		dpf2->setCollectorAttr("zky_name", distinct_ds_name2);
		dpf2->setAssemblerType("strinc");
		dpf2->setAssemblerAttr("zky_inctype", "set");
		dpf2->setRecord(distinct_asm_dr2);
	
		boost::shared_ptr<CompFun> fun1 = boost::make_shared<CompFun>("custom", distinct_rcd_len2);
		fun1->setCmpField("str", 0);
		fun1->setCmpField("u64", mDistinctMaxLen, 8);
		
		dpf2->setCompFun(fun1);
		rtask->setDataProc(dpf2);
	}
*/
//create outputdatasets
	boost::shared_ptr<DataSet> output_ds_all = boost::make_shared<DataSet>();
	output_ds_all->setAttribute("zky_name", ds_name2);

	OmnString task_name = mStatDocName;
	task_name << "_" << mInternalId << "_task001";

	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	jobtask->setAttribute("zky_starttype", "auto");
	jobtask->setAttribute("zky_taskid", "task001");
	jobtask->setMapTask(mtask);
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(inputds);
	jobtask->setOutputDataset(output_ds_all);
/*
	//create job
	Job job;
	job.setAttribute("zky_jobid", "rootjob");
	job.setAttribute("zky_ctnrobjid", "task_ctnr");
	job.setAttribute("log_ctnr", "log_ctnr");
	job.setAttribute("zky_ispublic", "true");
	job.setAttribute("zky_objid", "job_example1_stat_task001_20140513");
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

