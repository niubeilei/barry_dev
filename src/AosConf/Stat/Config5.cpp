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
StatConf::createConfig5() {
	OmnString pre_name = mStatDocName;
	pre_name << "_task005";
	
	OmnString lhs_inputds_name = mNewValueDsName;
	OmnString lhs_inputrcd_name = mNewValueRcdName;
	
	OmnString output_dsname = pre_name;
	output_dsname << "_dataset_newvalue";

	OmnString df_rcdname = pre_name;
	df_rcdname << "_rcd_newvalue_output";

	OmnString df_collectorname = pre_name;
	df_collectorname << "_datacollector_newvalue";

	OmnString df_procname = pre_name;
	df_procname << "_proc_newvalue";

	OmnString df_iilname = "_zt4k_";
	df_iilname << mStatDocName << "_newvalue";

	//create lhs_input_dataset
	boost::shared_ptr<DataSet> lhs_inputds = boost::make_shared<DataSet>();
	lhs_inputds->setAttribute("zky_dataset_name", lhs_inputds_name);
	
	//create outputds1
	boost::shared_ptr<DataSet> output_ds = boost::make_shared<DataSet>();
	output_ds->setAttribute("zky_name", output_dsname);

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

	boost::shared_ptr<DataRecord> rcdctnr_dr = boost::make_shared<DataRecord>();
	rcdctnr_dr->setAttribute("type", "fixbin");
	rcdctnr_dr->setAttribute("zky_name", df_rcdname);
	rcdctnr_dr->setAttribute("zky_length", "16");
	rcdctnr_dr->setField(asm_df1);
	rcdctnr_dr->setField(asm_df2);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr = boost::make_shared<DataRecordCtnr>();
	asm_dr->setRecord(rcdctnr_dr);
	asm_dr->setAttribute("zky_name", df_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf= boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
	dpf->setAttribute("zky_iilname", df_iilname); 
	dpf->setAttribute("datasetname", output_dsname); 
	dpf->setAttribute("zky_name", df_procname);

	dpf->setCollectorType("iil");
	dpf->setCollectorAttr("zky_name", df_collectorname); 

	dpf->setAssemblerType("u64add");
	dpf->setAssemblerAttr("zky_inctype", "norm");

	dpf->setRecord(asm_dr);

	boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", 16);
	cmp_fun->setCmpField("u64", 0);
	cmp_fun->setAggrField("u64", 8, "norm");
	dpf->setCompFun(cmp_fun);


	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	rtask->setDataProc(dpf);

	//create maptask    //?????
	boost::shared_ptr<DataEngineScanMul> de = boost::make_shared<DataEngineScanMul>();
	de->setAttribute("zky_type", "dataengine_scan_multircd");	
	
	OmnString lhs_field_name_value = lhs_inputrcd_name;
	lhs_field_name_value << ".value";

	OmnString lhs_field_name_docid = lhs_inputrcd_name;
	lhs_field_name_docid << ".docid";

	OmnString output_field_name_value = df_rcdname;
	output_field_name_value << ".value";

	OmnString output_field_name_docid = df_rcdname;
	output_field_name_docid << ".docid";

	boost::shared_ptr<DataProcCompose> dpc1 = boost::make_shared<DataProcCompose>();
	dpc1->setInput(lhs_field_name_value);
	dpc1->setOutput(output_field_name_value);

	boost::shared_ptr<DataProcCompose> dpc2 = boost::make_shared<DataProcCompose>();
	dpc2->setInput(lhs_field_name_docid);
	dpc2->setOutput(output_field_name_docid);

	boost::shared_ptr<DataProcStatDoc> dpc = boost::make_shared<DataProcStatDoc>();
	de->setDataProc(lhs_inputrcd_name, dpc1);
	de->setDataProc(lhs_inputrcd_name, dpc2);
	
	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	mtask->setDataEngine(de);
	
	OmnString task_name = mStatDocName;
	//task_name << "_" << mAgrType << "_" << mInputValueFieldName << "_" << mInternalId << "_task005";
	task_name << "_" << mInternalId << "_task005";
	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	//jobtask->setAttribute("zky_starttype", "auto");
	jobtask->setAttribute("zky_taskid", "task005");
	jobtask->setMapTask(mtask);  
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(lhs_inputds);
	jobtask->setOutputDataset(output_ds);
/*
	//create job
	Job job;
	job.setAttribute("zky_jobid", "rootjob");
	job.setAttribute("zky_ctnrobjid", "task_ctnr");
	job.setAttribute("log_ctnr", "log_ctnr");
	job.setAttribute("zky_ispublic", "true");
	job.setAttribute("zky_objid", "job_example1_stat_task005_20140513");
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



