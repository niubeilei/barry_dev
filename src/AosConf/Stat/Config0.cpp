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
#include "AosConf/DataEngineScanSingle.h"
#include "AosConf/DataEngineJoin.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataProcIf.h"
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
#include "AosConf/DataFieldExpr.h"
#include <boost/make_shared.hpp>
#include <string>

using boost::make_shared;

using AosConf::StatConf;
using AosConf::JobTask;

boost::shared_ptr<JobTask> 
StatConf::createConfig0() {
	OmnString inputValueFieldName = "";
	if (mHasCount && mStatConfLevel != "1")
	{
		inputValueFieldName = mStatDocName;
		inputValueFieldName << "0x2a";		//0x2a is actual *;
	}

	OmnString stat_definition_key = mStatDocName;
	stat_definition_key << "_" << mInternalId;
	
	boost::shared_ptr<DataSet> inputds;
	OmnString inputrecordname;
	
	OmnString df3_name = mStatDocName;
	df3_name << "_0x01";
	boost::shared_ptr<DataFieldStr> input_df3 = boost::make_shared<DataFieldStr>();
	input_df3->setAttribute("zky_name", df3_name);
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

	OmnString math_field_name = mStatDocName;
	math_field_name << "_statkey";

	boost::shared_ptr<DataFieldMath> input_df7 = boost::make_shared<DataFieldMath>();
	input_df7->setAttribute("zky_name", math_field_name);
	input_df7->setAttribute("zky_opr", "concat");
	input_df7->setAttribute("zky_sep", "0x01");
	//input_df7->setAttribute(AOSTAG_VALUE_DFT_VALUE, "NULL");		// Ketty 2014/11/01
	for(size_t i=0; i<mStatFields.size(); i++)
	{
		input_df7->setFieldName(mStatFields[i]);
	}

	if (mInternalId == "0" || mStatConfLevel == "1")
	{
		//create inputdataset
		inputds = boost::make_shared<DataSet>(mInputDataset);
		boost::shared_ptr<DataSchema> inds_schema = inputds->getSchema();
		boost::shared_ptr<DataRecord> inputrcd = inds_schema->getRecord();
		inputrcd->setField(input_df3);
		mInputFields.push_back(input_df3);
		if (mHasCount && mStatConfLevel != "1")
		{
			inputrcd->setField(input_df4);
			mInputFields.push_back(input_df4);
		}
		if(mTimeFromField == "")
		{
			inputrcd->setField(input_df5);
			mInputFields.push_back(input_df5);
		}
		else
		{
			inputrcd->setField(input_df6);
			mInputFields.push_back(input_df6);
		}
		inputrcd->setField(input_df7);
		mInputFields.push_back(input_df7);
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

	OmnString statname = mStatDocName;
	
	OmnString asm_recordname = statname;
	asm_recordname <<  "_rcd_all"; 

	OmnString collectorobjid1 = statname;
	collectorobjid1 << "_all_tmp_data";

	//create maptask
	boost::shared_ptr<DataEngineScanSingle> de = boost::make_shared<DataEngineScanSingle>();
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
	lhs_fname_value << "." << math_field_name;
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

	de->setDataProc(dpc);
	mMapDataProcs.push_back(dpc);
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
	mReduceDataProcs.push_back(dpf2);

//create outputdatasets
	boost::shared_ptr<DataSet> output_ds_all = boost::make_shared<DataSet>();
	output_ds_all->setAttribute("zky_name", ds_name2);
	mOutputds.push_back(output_ds_all);

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
	
	return jobtask;
}


//============================================
// Following is Created by Ketty
	boost::shared_ptr<JobTask> 
StatConf::createConfig0New()
{
	//create inputdataset
	boost::shared_ptr<DataSet> inputds;
	inputds = boost::make_shared<DataSet>(mInputDataset);
	boost::shared_ptr<DataSchema> inds_schema = inputds->getSchema();
	
	OmnString asm_recordname = mStatRunDocName;
	asm_recordname << "_rcd_all"; 

	boost::shared_ptr<DataRecord> inputrcd = inds_schema->getRecord();
	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	createTask1MapTask(mtask, inputrcd, asm_recordname);
	
	OmnString ds_name2  = mStatRunDocName;
	ds_name2 << "_all";

	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	createTask1ReduceTask(rtask, ds_name2, asm_recordname);

	//create outputdatasets
	boost::shared_ptr<DataSet> output_ds_all = boost::make_shared<DataSet>();
	output_ds_all->setAttribute("zky_name", ds_name2);
	mOutputds.push_back(output_ds_all);

	OmnString task_name = mStatRunDocName;
	task_name << "_task001";

	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	jobtask->setAttribute("zky_starttype", "auto");
	jobtask->setAttribute("zky_taskid", "task001");
	jobtask->setMapTask(mtask);
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(inputds);
	jobtask->setOutputDataset(output_ds_all);
	
	return jobtask;
}


/*
bool
StatConf::addTask1TimeDataField(shared_ptr<DataRecord> &inputrcd)
{
	OmnString time_field_name;
	if(mTimeFromField == "")
	{
		time_field_name = mStatRunDocName;
		time_field_name << "_time_field";

		shared_ptr<DataFieldStr> input_df = make_shared<DataFieldStr>();
		input_df->setAttribute("zky_name", time_field_name);
		input_df->setAttribute("type", "const");
		input_df->setConst(0);
		
		mInputTimeFieldNames.push_back(time_field_name);

		inputrcd->setField(input_df);
		mInputFields.push_back(input_df);
		return true;
	}

	set<OmnString> exist_grpby_timeunit;
	set<OmnString>::iterator itr;
	OmnString time_to_format;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		time_field_name = mStatRunDocName;
		time_field_name << "_time_field" << mMeasures[i].grpby_time_unit;

		itr = exist_grpby_timeunit.find(mMeasures[i].grpby_time_unit);
		if(itr != exist_grpby_timeunit.end())	continue;
		
		shared_ptr<DataFieldStr> input_df = make_shared<DataFieldStr>();
		input_df->setAttribute("zky_name", time_field_name);
		input_df->setAttribute("type", "time");
		input_df->setAttribute(AOSTAG_VALUEFROMFIELD, mTimeFromField);
		input_df->setAttribute(AOSTAG_ORIG_TIME_FORMAT, mTimeFromFormat);

		time_to_format = convertTimeUnitToTimeFormat(mMeasures[i].grpby_time_unit);
		if(time_to_format == "" && mMeasures[i].grpby_time_unit == "_alltime")
		{
			time_to_format = mTimeFromFormat;
		}
		aos_assert_r(time_to_format != "", false);
		input_df->setAttribute(AOSTAG_TARGET_TIME_FORMAT, time_to_format);

		exist_grpby_timeunit.insert(mMeasures[i].grpby_time_unit);	
		mInputTimeFieldNames.push_back(time_field_name);
		
		inputrcd->setField(input_df);
		mInputFields.push_back(input_df);
	}
	
	return true;
}
*/

	
bool
StatConf::addTask1TimeDataField(boost::shared_ptr<DataRecord> &inputrcd)
{
	for(size_t i=0; i<mStatTimes.size(); i++)
	{
		boost::shared_ptr<DataFieldStr> input_df = boost::make_shared<DataFieldStr>();
		input_df->setAttribute("zky_name", mStatTimes[i].mName);
		input_df->setAttribute("type", "time");
		input_df->setAttribute(AOSTAG_VALUEFROMFIELD, mTimeFromField);
		input_df->setAttribute(AOSTAG_ORIG_TIME_FORMAT, mTimeFromFormat);
		input_df->setAttribute(AOSTAG_TARGET_TIME_FORMAT, mStatTimes[i].mToFormat);

		inputrcd->setField(input_df);
		mInputFields.push_back(input_df);
	}
	
	return true;
}


bool
StatConf::createTask1InputDataRecord(boost::shared_ptr<DataRecord> &inputrcd)
{
	addTask1TimeDataField(inputrcd);
	
	//if (mHasCount && mStatConfLevel != "1")
	if (mHasCount)
	{
		//OmnString inputValueFieldName = mStatRunDocName;
		OmnString inputValueFieldName = "0x2a";		//0x2a is actual *;
	
		boost::shared_ptr<DataFieldStr> input_df4 = boost::make_shared<DataFieldStr>();
		input_df4->setAttribute("zky_name", inputValueFieldName);
		input_df4->setAttribute("type", "const");
		input_df4->setConst("1");
		
		inputrcd->setField(input_df4);
		mInputFields.push_back(input_df4);
	}
	
	if(mStatFields.size() == 0)		return addTask1InputDataRecordNoKey(inputrcd);
		
	boost::shared_ptr<DataFieldMath> input_df1 = boost::make_shared<DataFieldMath>();
	
	OmnString math_field_name = mStatRunDocName;
	math_field_name << "_statkey";

	//input_df1->setAttribute("zky_name", "statkey");
	input_df1->setAttribute("zky_name", math_field_name);
	
	input_df1->setAttribute("zky_opr", "concat");
	input_df1->setAttribute("zky_sep", "0x01");
	//input_df1->setAttribute(AOSTAG_VALUE_DFT_VALUE, "NULL");		// Ketty 2014/11/01
	for(size_t i=0; i<mStatFields.size(); i++)
	{
		input_df1->setFieldName(mStatFields[i]);
	}

	inputrcd->setField(input_df1);
	mInputFields.push_back(input_df1);

	OmnString lhs_fname_value, df_fname_value;
	if (mOp == JQLTypes::eDrop)
	{
		for(size_t i=0; i<mMeasures.size(); i++)
		{
			boost::shared_ptr<DataFieldExpr> input_expr = boost::make_shared<DataFieldExpr>();
			string expr = "-1*";
			expr += mMeasures[i].field_name;
			string zname = "";
			zname = mMeasures[i].field_name<<"_";
			input_expr->setAttribute("zky_name", zname);
			input_expr->setExpr(expr);
			inputrcd->setField(input_expr);
			mInputFields.push_back(input_expr);
		}
	}
		
	return true;
}

bool
StatConf::addTask1InputDataRecordNoKey(boost::shared_ptr<DataRecord> &inputrcd)
{
	boost::shared_ptr<DataFieldStr> input_df1 = boost::make_shared<DataFieldStr>();
	input_df1->setAttribute("zky_name", "statkey");
	input_df1->setAttribute("type", "const");
	input_df1->setConst("_const_");
	
	boost::shared_ptr<DataFieldStr> input_df2 = boost::make_shared<DataFieldStr>();
	input_df2->setAttribute("zky_name", "sdocid");
	input_df2->setAttribute("type", "const");
	input_df2->setConst("1");

	boost::shared_ptr<DataFieldStr> input_df3 = boost::make_shared<DataFieldStr>();
	input_df3->setAttribute("zky_name", "isnew");
	input_df3->setAttribute("type", "const");
	input_df3->setConst("1");

	inputrcd->setField(input_df1);
	inputrcd->setField(input_df2);
	inputrcd->setField(input_df3);	
	mInputFields.push_back(input_df1);
	mInputFields.push_back(input_df2);
	mInputFields.push_back(input_df3);
	
	return true;
}


bool
StatConf::createTask1MapTask(
		boost::shared_ptr<MapTask> &mtask,
		boost::shared_ptr<DataRecord> &inputrcd,
		const OmnString &asm_recordname)
{
	createTask1InputDataRecord(inputrcd);
	OmnString inputrecordname = inputrcd->getAttribute("zky_name");

	if(mStatFields.size() == 0)	return createTask1MapTaskNoKey(mtask, inputrecordname, asm_recordname);

	//create maptask
	boost::shared_ptr<DataEngineScanSingle> de = boost::make_shared<DataEngineScanSingle>();
	de->setAttribute("zky_type", "dataengine_scan_multircd");	
	
	boost::shared_ptr<DataProcStatDocShuffle> dpc = boost::make_shared<DataProcStatDocShuffle>();
	dpc->setAttribute("shuffle_type", "cube");

	OmnString lhs_fname_value, df_fname_value;
	
	OmnString math_field_name = mStatRunDocName;
	math_field_name << "_statkey";
	
	lhs_fname_value = inputrecordname;
	lhs_fname_value << "." << math_field_name;
	//lhs_fname_value << ".statkey";
	df_fname_value = asm_recordname;
	df_fname_value << ".statkey";
	dpc->set("stat_key", lhs_fname_value, df_fname_value);
	
	df_fname_value = asm_recordname;
	df_fname_value << ".cube_id";
	dpc->set("stat_cubeid", "", df_fname_value);

	for(size_t i=0; i<mStatTimes.size(); i++)
	{
		lhs_fname_value = inputrecordname;
		lhs_fname_value << "." << mStatTimes[i].mName;

		df_fname_value = asm_recordname;
		df_fname_value << ".time" << i;
	
		dpc->set("stat_time", lhs_fname_value, df_fname_value);
	}

	for(size_t i=0; i<mMeasures.size(); i++)
	{
		lhs_fname_value = inputrecordname;
		lhs_fname_value << "." << mMeasures[i].field_name;
		
		df_fname_value = asm_recordname;
		df_fname_value << ".value" << i;
		
		dpc->set("stat_value", lhs_fname_value, df_fname_value);
	}

	if (mCondText != "")
	{
		boost::shared_ptr<DataProcIf> cond_dpc = boost::make_shared<DataProcIf>();		
		cond_dpc->setCondsText(mCondText);
		cond_dpc->setDataProc(dpc);
		de->setDataProc(cond_dpc);
		mMapDataProcs.push_back(cond_dpc);
	}
	else
	{
		de->setDataProc(dpc);
		mMapDataProcs.push_back(dpc);
	}

	mtask->setDataEngine(de);
	
	return true;
}


bool
StatConf::createTask1ReduceTask(
		boost::shared_ptr<ReduceTask> &rtask,
		const OmnString &ds_name2,
		const OmnString &asm_recordname)
{
	if(mStatFields.size() == 0)	return createTask1ReduceTaskNoKey(rtask, ds_name2, asm_recordname);
	
	u32 rcd_len2 = 0;
	//boost::shared_ptr<DataRecord> rcdctnr_dr2 = make_shared<DataRecord>();
	mTask1ReduceDataRecord = boost::make_shared<DataRecord>(); 

	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	u32 key_field_off = 0; 
	asm_df1->setAttribute("type", "str");
	asm_df1->setAttribute("zky_name", "statkey");
	asm_df1->setAttribute("zky_offset", rcd_len2);
	asm_df1->setAttribute("zky_length", mMaxLen);
	asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	rcd_len2 += mMaxLen;
	//rcdctnr_dr2->setField(asm_df1);
	mTask1ReduceDataRecord->setField(asm_df1);
	
	boost::shared_ptr<DataFieldStr> asm_df5 = boost::make_shared<DataFieldStr>();
	asm_df5->setAttribute("type", "bin_u64");
	asm_df5->setAttribute("zky_name", "cube_id");
	asm_df5->setAttribute("zky_offset", rcd_len2);
	asm_df5->setAttribute("zky_length", "8");
	rcd_len2 += 8;
	//rcdctnr_dr2->setField(asm_df5);
	mTask1ReduceDataRecord->setField(asm_df5);

	OmnString time_fname;
	vector<u32> time_field_off;
	for(size_t i =0; i<mStatTimes.size(); i++)
	{
		time_fname = "time";
		time_fname << i;
		time_field_off.push_back(rcd_len2);

		boost::shared_ptr<DataFieldStr> asm_df3 = boost::make_shared<DataFieldStr>();
		asm_df3->setAttribute("type", "bin_u64");
		asm_df3->setAttribute("zky_name", time_fname);
		asm_df3->setAttribute("zky_offset", rcd_len2);
		asm_df3->setAttribute("zky_length", "8");
		rcd_len2 += 8;
		//rcdctnr_dr2->setField(asm_df3);
		mTask1ReduceDataRecord->setField(asm_df3);
	}

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
		//rcdctnr_dr2->setField(asm_df4);
		mTask1ReduceDataRecord->setField(asm_df4);
	}

	mTask1ReduceDataRecord->setAttribute("type", "fixbin");
	mTask1ReduceDataRecord->setAttribute("zky_name", asm_recordname);
	mTask1ReduceDataRecord->setAttribute("zky_length", rcd_len2);
	//rcdctnr_dr2->setAttribute("type", "fixbin");
	//rcdctnr_dr2->setAttribute("zky_name", asm_recordname);
	//rcdctnr_dr2->setAttribute("zky_length", rcd_len2);

	boost::shared_ptr<DataRecordCtnr> asm_dr2 = boost::make_shared<DataRecordCtnr>();
	//asm_dr2->setRecord(rcdctnr_dr2);
	asm_dr2->setRecord(mTask1ReduceDataRecord);
	asm_dr2->setAttribute("zky_name", asm_recordname);

	mFileDefName = mStatRunDocName;
	mFileDefName << "_all_files";

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
	cmp_fun->setCmpField("str", key_field_off);
	for(u32 i=0; i<time_field_off.size(); i++)
	{
		cmp_fun->setCmpField("u64", time_field_off[i], 8);
	}

	for(size_t i=0; i<aggrs.size(); i++)
	{
		cmp_fun->setAggrField(aggrs[i].agr_type, aggrs[i].agr_pos, aggrs[i].agr_fun);
	}
	dpf2->setCompFun(cmp_fun);

	rtask->setDataProc(dpf2);
	mReduceDataProcs.push_back(dpf2);
	
	return true;
}


bool
StatConf::createTask1MapTaskNoKey(
		boost::shared_ptr<MapTask> &mtask,
		const OmnString &inputrecordname,
		const OmnString &asm_recordname)
{
	//create maptask
	boost::shared_ptr<DataEngineScanSingle> de = boost::make_shared<DataEngineScanSingle>();
	de->setAttribute("zky_type", "dataengine_scan_multircd");	
	
	OmnString lhs_fname_value, df_fname_value;

	boost::shared_ptr<DataProcCompose> key_dpc = boost::make_shared<DataProcCompose>();
	lhs_fname_value = inputrecordname;
	lhs_fname_value << ".statkey";
	key_dpc->setInput(lhs_fname_value);
	df_fname_value = asm_recordname;
	df_fname_value << ".statkey";
	key_dpc->setOutput(df_fname_value);
	de->setDataProc(key_dpc);
	mMapDataProcs.push_back(key_dpc);

	boost::shared_ptr<DataProcCompose> sdocid_dpc = boost::make_shared<DataProcCompose>();
	lhs_fname_value = inputrecordname;
	lhs_fname_value << ".sdocid";
	sdocid_dpc->setInput(lhs_fname_value);
	df_fname_value = asm_recordname;
	df_fname_value << ".sdocid";
	sdocid_dpc->setOutput(df_fname_value);
	de->setDataProc(sdocid_dpc);
	mMapDataProcs.push_back(sdocid_dpc);

	boost::shared_ptr<DataProcCompose> isnew_dpc = boost::make_shared<DataProcCompose>();
	lhs_fname_value = inputrecordname;
	lhs_fname_value << ".isnew";
	isnew_dpc->setInput(lhs_fname_value);
	df_fname_value = asm_recordname;
	df_fname_value << ".isnew";
	isnew_dpc->setOutput(df_fname_value);
	de->setDataProc(isnew_dpc);
	mMapDataProcs.push_back(isnew_dpc);
	
	for(size_t i=0; i<mStatTimes.size(); i++)
	{
		boost::shared_ptr<DataProcCompose> time_dpc = boost::make_shared<DataProcCompose>();
		
		lhs_fname_value = inputrecordname;
		lhs_fname_value << "." << mStatTimes[i].mName;
		time_dpc->setInput(lhs_fname_value);

		df_fname_value = asm_recordname;
		df_fname_value << ".time" << i;
		time_dpc->setOutput(df_fname_value);
	
		de->setDataProc(time_dpc);
		mMapDataProcs.push_back(time_dpc);
	}
	
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		boost::shared_ptr<DataProcCompose> measure_dpc = boost::make_shared<DataProcCompose>();
	
		lhs_fname_value = inputrecordname;
		lhs_fname_value << "." << mMeasures[i].field_name;
		measure_dpc->setInput(lhs_fname_value);
		df_fname_value = asm_recordname;
		df_fname_value << ".value" << i;
		measure_dpc->setOutput(df_fname_value);
	
		de->setDataProc(measure_dpc);
		mMapDataProcs.push_back(measure_dpc);
	}

	return true;
}


bool
StatConf::createTask1ReduceTaskNoKey(
		boost::shared_ptr<ReduceTask> &rtask,
		const OmnString &ds_name2,
		const OmnString &asm_recordname)
{
	u32 rcd_len2 = 0;
	//boost::shared_ptr<DataRecord> rcdctnr_dr2 = make_shared<DataRecord>();
	mTask1ReduceDataRecord = boost::make_shared<DataRecord>(); 

	u32 key_field_off = 0; 
	u32 key_len = strlen("_const_") + 1;
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "str");
	asm_df1->setAttribute("zky_name", "statkey");
	asm_df1->setAttribute("zky_offset", rcd_len2);
	asm_df1->setAttribute("zky_length", key_len);
	asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	rcd_len2 += key_len;
	//rcdctnr_dr2->setField(asm_df1);
	mTask1ReduceDataRecord->setField(asm_df1);

	boost::shared_ptr<DataFieldStr> asm_df5 = boost::make_shared<DataFieldStr>();
	asm_df5->setAttribute("type", "bin_u64");
	asm_df5->setAttribute("zky_name", "sdocid");
	asm_df5->setAttribute("zky_offset", rcd_len2);
	asm_df5->setAttribute("zky_length", "8");
	rcd_len2 += 8;
	//rcdctnr_dr2->setField(asm_df5);
	mTask1ReduceDataRecord->setField(asm_df5);
	
	boost::shared_ptr<DataFieldStr> asm_df6 = boost::make_shared<DataFieldStr>();
	asm_df6->setAttribute("type", "str");
	asm_df6->setAttribute("zky_name", "isnew");
	asm_df6->setAttribute("zky_offset", rcd_len2);
	asm_df6->setAttribute("zky_length", "1");
	rcd_len2 += 1;
	//rcdctnr_dr2->setField(asm_df6);
	mTask1ReduceDataRecord->setField(asm_df6);

	OmnString time_fname;
	vector<u32> time_field_off;
	for(size_t i =0; i<mStatTimes.size(); i++)
	{
		time_fname = "time";
		time_fname << i;
		time_field_off.push_back(rcd_len2);

		boost::shared_ptr<DataFieldStr> asm_df3 = boost::make_shared<DataFieldStr>();
		asm_df3->setAttribute("type", "bin_u64");
		asm_df3->setAttribute("zky_name", time_fname);
		asm_df3->setAttribute("zky_offset", rcd_len2);
		asm_df3->setAttribute("zky_length", "8");
		rcd_len2 += 8;
		//rcdctnr_dr2->setField(asm_df3);
		mTask1ReduceDataRecord->setField(asm_df3);
	}

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
		//rcdctnr_dr2->setField(asm_df4);
		mTask1ReduceDataRecord->setField(asm_df4);
	}

	mTask1ReduceDataRecord->setAttribute("type", "fixbin");
	mTask1ReduceDataRecord->setAttribute("zky_name", asm_recordname);
	mTask1ReduceDataRecord->setAttribute("zky_length", rcd_len2);
	//rcdctnr_dr2->setAttribute("type", "fixbin");
	//rcdctnr_dr2->setAttribute("zky_name", asm_recordname);
	//rcdctnr_dr2->setAttribute("zky_length", rcd_len2);

	boost::shared_ptr<DataRecordCtnr> asm_dr2 = boost::make_shared<DataRecordCtnr>();
	//asm_dr2->setRecord(rcdctnr_dr2);
	asm_dr2->setRecord(mTask1ReduceDataRecord);
	asm_dr2->setAttribute("zky_name", asm_recordname);

	mFileDefName = mStatRunDocName;
	mFileDefName << "_all_files";

	boost::shared_ptr<ReduceTaskDataProc> dpf2= boost::make_shared<ReduceTaskDataProc>("createtaskfile");
	dpf2->setAttribute("datasetname", ds_name2);
	//dpf2->setAttribute("run_mode", "run_parent");
	dpf2->setAttribute("zky_file_defname", mFileDefName);
	//dpf2->setAttribute("zky_name", "example1_task001_proc_all");
	dpf2->setAttribute("zky_name", ds_name2);
	dpf2->setCollectorType("iil");
	dpf2->setCollectorAttr("zky_name", ds_name2);
	//dpf2->setCollectorAttr("group_type", "cube_shuffle");
	dpf2->setAttribute(AOSTAG_OPERATOR, "strinc");
	dpf2->setAssemblerType("strinc");
	dpf2->setAssemblerAttr("zky_inctype", "norm");
	dpf2->setRecord(asm_dr2);

	boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", rcd_len2);
	cmp_fun->setCmpField("str", key_field_off);
	for(u32 i=0; i<time_field_off.size(); i++)
	{
		cmp_fun->setCmpField("u64", time_field_off[i], 8);
	}

	for(size_t i=0; i<aggrs.size(); i++)
	{
		cmp_fun->setAggrField(aggrs[i].agr_type, aggrs[i].agr_pos, aggrs[i].agr_fun);
	}
	dpf2->setCompFun(cmp_fun);

	rtask->setDataProc(dpf2);
	mReduceDataProcs.push_back(dpf2);
	
	return true;
}
