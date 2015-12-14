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
#include "AosConf/DataProcJoin.h"
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
#include <boost/make_shared.hpp>
#include <string>

using boost::make_shared;
using AosConf::StatConf;
using AosConf::JobTask;

boost::shared_ptr<JobTask> 
StatConf::createDistinctConfig1() {
	OmnString pre_name = mStatDocName;
	pre_name << "_task001_distinct_";
	
	OmnString output_all_dsname = pre_name;
	output_all_dsname << "dataset_all";

	mDistinctOutputIILNameAll = "_zt4k_";
	mDistinctOutputIILNameAll << mStatDocName << "__all_level_distinct_" << mInternalId;

	OmnString output_all_rcdname = pre_name;
	output_all_rcdname << "rcd_all_iil";

	
	OmnString stat_definition_key = mStatDocName;
	stat_definition_key << "_distinct_internalid_" << mInternalId;
	
	boost::shared_ptr<DataSet> lhs_inputds, rhs_inputds;
	OmnString lhs_inputrcd_name, rhs_inputrcd_name;
		
	//create lhs_input_dataset
	OmnString lhs_inputds_name = mDistinctOdsAllDatasetName;
	lhs_inputrcd_name = mDistinctOutputRcdNameAll;
	lhs_inputds = boost::make_shared<DataSet>();
	lhs_inputds->setAttribute("zky_dataset_name", lhs_inputds_name);
/*	
	OmnString rhs_inputds_name = mDistinctInputOneDatasetName;
	rhs_inputrcd_name = mDistinctInputRcdNameOne;
	rhs_inputds = boost::make_shared<DataSet>();
	rhs_inputds->setAttribute("zky_dataset_name", rhs_inputds_name);
*/	
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

	rhs_inputrcd_name = "example1_task001_distinct_rcd_rhs_iil";
	//create rhs_inputds_datarecord
	boost::shared_ptr<DataRecord> input_rcd = boost::make_shared<DataRecord>();
	input_rcd->setAttribute("type", "iil");
	input_rcd->setAttribute("zky_name", rhs_inputrcd_name);
	input_rcd->setAttribute("zky_length", input_rcd_len);
	input_rcd->setField(input_df1);
	input_rcd->setField(input_df2);

	//create rhs_inputds_dataschema
	boost::shared_ptr<DataSchema> input_schema = boost::make_shared<DataSchema>();
	input_schema->setAttribute("zky_name", "example1_task001_distinct_schema_rhs");
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
	OmnString rhs_inputds_name = "rhs";
	rhs_inputds = boost::make_shared<DataSet>();
	rhs_inputds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	rhs_inputds->setAttribute("zky_name", rhs_inputds_name);
	rhs_inputds->setScanner(input_scanner);
	rhs_inputds->setSchema(input_schema);
	
/*
	AosXmlTagPtr xml_scanner = mInputDataset->getFirstChild("datascanner");
	OmnString dscanner_objid = xml_scanner->getAttrStr("jimo_objid");
	AosXmlTagPtr xml_schema = mInputDataset->getFirstChild("dataschema");
*/	
	//OmnString ds_name1 = mStatDocName;
	//ds_name1 << "_isdistinct_" << mInternalId << "_one";
	
	OmnString ds_name2  = mStatDocName;
	ds_name2 << "_isdistinct_" << mInternalId << "_all";

	//OmnString asm_recordname1 = prename;
	//asm_recordname1 <<  "_rcd_one"; 

	OmnString asm_recordname2 = pre_name;
	asm_recordname2 <<  "_rcd_all"; 

	OmnString collectorobjid1 = pre_name;
	collectorobjid1 << "_all_tmp_data";

	//OmnString collectorobjid2 = prename;
	//collectorobjid2 << "_one_tmp_data";

	//create maptask
	boost::shared_ptr<DataEngineJoinNew> de = boost::make_shared<DataEngineJoinNew>();
	de->setLhsDatasetName(lhs_inputds_name);
	de->setRhsDatasetName(rhs_inputds_name);

	de->setFirstRefName(lhs_inputrcd_name);
	de->setNextRefName(lhs_inputrcd_name);
	
	OmnString next_str = "next.str";
	OmnString first_docid = "first.docid";
	OmnString temp_docid = "temp.docid";
	
	OmnString output_all_field_name_docid = output_all_rcdname;
	output_all_field_name_docid << ".docid";

	OmnString output_all_field_name_str = output_all_rcdname;
	output_all_field_name_str << ".str";
	
	OmnString lhs_field_name_str = lhs_inputrcd_name;
	lhs_field_name_str << ".str";
	//de->setLhsFieldName(lhs_field_name_str);

	OmnString rhs_field_name_value = rhs_inputrcd_name;
	rhs_field_name_value << ".value";
	//de->setRhsFieldName(rhs_field_name_value);

	// cond_dataprocs
	boost::shared_ptr<DataProcJoin> cond_dpc1 = boost::make_shared<DataProcJoin>();
	cond_dpc1->setAttribute("zky_name", "joincmp");
	cond_dpc1->setLhsInput(lhs_field_name_str);
	cond_dpc1->setRhsInput(rhs_field_name_value);
	
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
	de->setDataProc("false_cond", false_dpc1);
	de->setDataProc("false_cond", false_dpc2);
	de->setDataProc("false_cond", false_dpc3);

	vector<OmnString> fieldnames;
	for(size_t i = 0; i < mStatFields.size(); i++)
	{
		OmnString temp = lhs_inputrcd_name;
		temp << "." << mStatFields[i];
		fieldnames.push_back(temp);
	}
	
	OmnString sep = lhs_inputrcd_name;
	sep << ".0x01";
	//boost::shared_ptr<DataProcCompose> dpc = boost::make_shared<DataProcCompose>();
	boost::shared_ptr<DataProcCompose> dpc1 = boost::make_shared<DataProcCompose>();
	for (size_t i=0; i<fieldnames.size(); i++)
	{
		//dpc->setInput(fieldnames[i]);
		dpc1->setInput(fieldnames[i]);
		if (i<fieldnames.size()-1)
		{
	//		dpc->setInput(sep);
			dpc1->setInput(sep);
		}
	}
	/*
	OmnString name = asm_recordname1;
	name << ".str";
	dpc->setOutput(name);
	de->setDataProc(dpc);
	*/
	OmnString name = asm_recordname2;
	name << ".str";
	dpc1->setOutput(name);
	de->setDataProc("false_cond", dpc1);

	boost::shared_ptr<DataProcCompose> dpc = boost::make_shared<DataProcCompose>();
	name = lhs_inputrcd_name;
	name << ".time";
	dpc->setInput(name);
	name = asm_recordname2;
	name << ".time";
	dpc->setOutput(name);
	de->setDataProc("false_cond", dpc);
	
	dpc = boost::make_shared<DataProcCompose>();
	name = lhs_inputrcd_name;
	name << ".value";
	dpc->setInput(name);
	name = asm_recordname2;
	name << ".value0";
	dpc->setOutput(name);
	de->setDataProc("false_cond", dpc);
	
	boost::shared_ptr<MapTask> mtask = boost::make_shared<MapTask>();
	mtask->setDataEngine(de);
	
//create reducetask
	
	//create reduce_task_dataproc1
	boost::shared_ptr<DataFieldStr> new_asm_df1 = boost::make_shared<DataFieldStr>();
	new_asm_df1->setAttribute("type", "str");
	new_asm_df1->setAttribute("zky_name", "str");
	new_asm_df1->setAttribute("zky_offset", "0");
	new_asm_df1->setAttribute("zky_length", mDistinctMaxLen);
	new_asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	int df_rcd_len1 = mDistinctMaxLen;

	boost::shared_ptr<DataFieldStr> new_asm_df2 = boost::make_shared<DataFieldStr>();
	new_asm_df2->setAttribute("type", "bin_u64");
	new_asm_df2->setAttribute("zky_name", "docid");
	new_asm_df2->setAttribute("zky_offset", df_rcd_len1);
	new_asm_df2->setAttribute("zky_length", "8");
	df_rcd_len1 += 8;

	boost::shared_ptr<DataRecord> rcdctnr_drall = boost::make_shared<DataRecord>();
	rcdctnr_drall->setAttribute("type", "fixbin");
	rcdctnr_drall->setAttribute("zky_name", output_all_rcdname);
	rcdctnr_drall->setAttribute("zky_length", df_rcd_len1);
	rcdctnr_drall->setField(new_asm_df1);
	rcdctnr_drall->setField(new_asm_df2);
	
	boost::shared_ptr<DataRecordCtnr> asm_dr_all = boost::make_shared<DataRecordCtnr>();
	asm_dr_all->setRecord(rcdctnr_drall);
	asm_dr_all->setAttribute("zky_name", output_all_rcdname);
	
	boost::shared_ptr<ReduceTaskDataProc> dpf_all= boost::make_shared<ReduceTaskDataProc>("iilbatchopr"); 
	dpf_all->setAttribute("zky_iilname", mDistinctOutputIILNameAll); 
	dpf_all->setAttribute("datasetname", output_all_dsname);
	dpf_all->setAttribute("zky_name", "example1_task002_distinct_proc_all");

	dpf_all->setCollectorType("iil");
	dpf_all->setCollectorAttr("zky_name", "example1_task002_distinct_datacollector_all"); 

	dpf_all->setAttribute(AOSTAG_OPERATOR, "strinc");
	dpf_all->setAssemblerType("strinc");
	dpf_all->setAssemblerAttr("zky_inctype", "set");

	dpf_all->setRecord(asm_dr_all);

	//create reduce_task_dataproc2
	boost::shared_ptr<DataFieldStr> asm_df1 = boost::make_shared<DataFieldStr>();
	asm_df1->setAttribute("type", "str");
	asm_df1->setAttribute("zky_name", "str");
	asm_df1->setAttribute("zky_offset", "0");
	asm_df1->setAttribute("zky_length", mMaxLen);
	asm_df1->setAttribute("zky_datatooshortplc", "cstr");
	/*
	int rcd_len1 = mMaxLen;

	boost::shared_ptr<DataFieldStr> asm_df2 = boost::make_shared<DataFieldStr>();
	asm_df2->setAttribute("type", "statid");
	asm_df2->setAttribute("zky_name", "docid");
	asm_df2->setAttribute("zky_offset", rcd_len1);
	asm_df2->setAttribute("zky_length", "8");
	asm_df2->setAttribute("stat_definition_key", stat_definition_key);
	asm_df2->setAttribute("zky_num_ids", "1000");
	rcd_len1 += 8;

	boost::shared_ptr<DataRecord> rcdctnr_dr1 = boost::make_shared<DataRecord>();
	rcdctnr_dr1->setAttribute("type", "fixbin");
	rcdctnr_dr1->setAttribute("zky_name", asm_recordname1);
	rcdctnr_dr1->setAttribute("zky_length", rcd_len1);
	rcdctnr_dr1->setField(asm_df1);
	rcdctnr_dr1->setField(asm_df2);
	*/
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

	OmnString vname, fun;
	vector<Aggregation> agrs;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		fun = mMeasures[i].agr_type;
		if (fun != "min" && fun != "max")
		{
			fun = "norm";
		}

		vname = "value";
		vname << i;
		boost::shared_ptr<DataFieldStr> asm_df4 = boost::make_shared<DataFieldStr>();
		asm_df4->setAttribute("type", "bin_int64");
		asm_df4->setAttribute("zky_name", vname);
		asm_df4->setAttribute("zky_offset", rcd_len2);
		asm_df4->setAttribute("zky_length", "8");
		
		Aggregation agr;
		agr.agr_type = "u64";
		agr.agr_pos = rcd_len2;
		agr.agr_fun = fun;
		agrs.push_back(agr);

		rcd_len2 += 8;
		rcdctnr_dr2->setField(asm_df4);
	}
	
	rcdctnr_dr2->setAttribute("type", "fixbin");
	rcdctnr_dr2->setAttribute("zky_name", asm_recordname2);
	rcdctnr_dr2->setAttribute("zky_length", rcd_len2);
	/*
	boost::shared_ptr<DataRecordCtnr> asm_dr = boost::make_shared<DataRecordCtnr>();
	asm_dr->setRecord(rcdctnr_dr1);
	asm_dr->setAttribute("zky_name", asm_recordname1);
	*/
	boost::shared_ptr<DataRecordCtnr> asm_dr2 = boost::make_shared<DataRecordCtnr>();
	asm_dr2->setRecord(rcdctnr_dr2);
	asm_dr2->setAttribute("zky_name", asm_recordname2);
/*	
	boost::shared_ptr<ReduceTaskDataProc> dpf= boost::make_shared<ReduceTaskDataProc>("createdatasetdoc");
	//dpf->setAttribute("zky_iilname", iilname2);
	dpf->setAttribute("datasetname", ds_name1);
	dpf->setAttribute("zky_name", "temp_example1_task001_proc_one");
	dpf->setCollectorType("iil");
	//dpf->setCollectorAttr("zky_objid", collectorobjid2);
	dpf->setCollectorAttr("zky_name", ds_name1);
	dpf->setAssemblerType("strinc");
	dpf->setAssemblerAttr("zky_inctype", "norm");
	dpf->setRecord(asm_dr);
	
	boost::shared_ptr<CompFun> cmp_fun = boost::make_shared<CompFun>("custom", rcd_len1);
	cmp_fun->setCmpField("str", 0);
	cmp_fun->setAggrField("u64", mMaxLen, "norm");
	
	dpf->setCompFun(cmp_fun);
*/	
	boost::shared_ptr<ReduceTaskDataProc> dpf2= boost::make_shared<ReduceTaskDataProc>("createdatasetdoc");
	//dpf2->setAttribute("zky_iilname", iilname1);
	dpf2->setAttribute("datasetname", ds_name2);
	dpf2->setAttribute("zky_name", "example1_task001_proc_all");
	dpf2->setCollectorType("iil");
	//dpf2->setCollectorAttr("zky_objid", collectorobjid1);
	dpf2->setCollectorAttr("zky_name", ds_name2);
	dpf2->setAttribute(AOSTAG_OPERATOR, "strinc");
	dpf2->setAssemblerType("strinc");
	dpf2->setAssemblerAttr("zky_inctype", "norm");
	dpf2->setRecord(asm_dr2);
	
	boost::shared_ptr<CompFun> fun1 = boost::make_shared<CompFun>("custom", rcd_len2);
	fun1->setCmpField("str", 0);
	fun1->setCmpField("u64", mMaxLen, 8);
	
	for(size_t i=0; i<agrs.size(); i++)
	{
		fun1->setAggrField(agrs[i].agr_type, agrs[i].agr_pos, agrs[i].agr_fun);
	}

	dpf2->setCompFun(fun1);

	boost::shared_ptr<ReduceTask> rtask = boost::make_shared<ReduceTask>();
	rtask->setDataProc(dpf_all);
	rtask->setDataProc(dpf2);

//create outputdatasets
	boost::shared_ptr<DataConnector> ds_connector = boost::make_shared<DataConnector>();
	ds_connector->setAttribute("jimo_objid", "dataconnector_idfiles_jimodoc_v0");
/*
	boost::shared_ptr<DataScanner> ds_scanner1 = boost::make_shared<DataScanner>();
	//ds_scanner1->setAttribute("jimo_objid", dscanner_objid);
	ds_scanner1->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	ds_scanner1->setAttribute("zky_name", "example1_task002_scanner");
	ds_scanner1->setAttribute("buff_cache_num", "9");
	ds_scanner1->setConnector(ds_connector);
*/
	boost::shared_ptr<DataScanner> ds_scanner2 = boost::make_shared<DataScanner>();
	//ds_scanner2->setAttribute("jimo_objid", dscanner_objid);
	ds_scanner2->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	ds_scanner2->setAttribute("zky_name", "example1_task003_scanner");
	ds_scanner2->setAttribute("buff_cache_num", "9");
	ds_scanner2->setConnector(ds_connector);
	/*
	boost::shared_ptr<DataFieldStr> outds_df = boost::make_shared<DataFieldStr>();
	outds_df->setAttribute("zky_name", "str");
	outds_df->setAttribute("type", "str");
	outds_df->setAttribute("zky_offset", "0");
	outds_df->setAttribute("zky_length", mMaxLen);
	outds_df->setAttribute("zky_datatooshortplc", "cstr");
	*/
	boost::shared_ptr<DataFieldStr> outds_df1 = boost::make_shared<DataFieldStr>();
	outds_df1->setAttribute("zky_name", "str");
	outds_df1->setAttribute("type", "str");
	outds_df1->setAttribute("zky_offset", "0");
	outds_df1->setAttribute("zky_length", mMaxLen);
	outds_df1->setAttribute("zky_datatooshortplc", "cstr");
	if (mStatConfLevel == "0-1")
	{
		for(size_t i = 0; i < mStatFields.size(); i++)
		{
			boost::shared_ptr<DataFieldStr> tmp_df = boost::make_shared<DataFieldStr>();
			tmp_df->setAttribute("type", "str");
			tmp_df->setAttribute("zky_name", mStatFields[i]);
			tmp_df->setAttribute("zky_isconst", "true");
			outds_df1->setDataField(tmp_df);
		}
	}
/*
	int output_rcd_len1 = mMaxLen;

	boost::shared_ptr<DataFieldStr> outds_df2 = boost::make_shared<DataFieldStr>();
	outds_df2->setAttribute("zky_name", "docid");
	outds_df2->setAttribute("type", "statid");
	outds_df2->setAttribute("zky_offset", output_rcd_len1);
	outds_df2->setAttribute("zky_length", "8");
	outds_df2->setAttribute("stat_definition_key", stat_definition_key);
	outds_df2->setAttribute("zky_num_ids", "1000");
	output_rcd_len1 += 8;
	
	boost::shared_ptr<DataFieldStr> outds_df3 = boost::make_shared<DataFieldStr>();
	outds_df3->setAttribute("zky_name", "statinternalid");
	outds_df3->setAttribute("type", "str");
	outds_df3->setAttribute("zky_isconst", "true");
	outds_df3->setAttribute("zky_ignore_serialize", "true");
	outds_df3->setConst(mInternalId);
*/	
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

	OmnString value_name;
	for(size_t i=0; i<mMeasures.size(); i++)
	{
		value_name = "value";
		value_name << i;
		boost::shared_ptr<DataFieldStr> outds_dfvalue = boost::make_shared<DataFieldStr>();
		outds_dfvalue->setAttribute("zky_name", value_name);
		outds_dfvalue->setAttribute("type", "bin_int64");
		outds_dfvalue->setAttribute("zky_offset", output_rcd_len2);
		outds_dfvalue->setAttribute("zky_length", "8");
		output_rcd_len2 += 8;
		ds_record2->setField(outds_dfvalue);
	}
	
	boost::shared_ptr<DataFieldStr> input_df3 = boost::make_shared<DataFieldStr>();
	input_df3->setAttribute("zky_name", "0x01");
	input_df3->setAttribute("type", "const");
	input_df3->setConst("0x01");
	ds_record2->setField(input_df3);
	
	boost::shared_ptr<DataFieldStr> outds_df2 = boost::make_shared<DataFieldStr>();
	outds_df2->setAttribute("zky_name", "docid");
	outds_df2->setAttribute("type", "statid");
	outds_df2->setAttribute("stat_definition_key", stat_definition_key);
	outds_df2->setAttribute("zky_num_ids", "1000");
	ds_record2->setField(outds_df2);
	
	boost::shared_ptr<DataFieldStr> outds_df3 = boost::make_shared<DataFieldStr>();
	outds_df3->setAttribute("zky_name", "statinternalid");
	outds_df3->setAttribute("type", "str");
	outds_df3->setAttribute("zky_isconst", "true");
	outds_df3->setAttribute("zky_ignore_serialize", "true");
	outds_df3->setConst(mInternalId);
	ds_record2->setField(outds_df3);

	//mOutputRcdNameOne = "example1_task002_rcd_lhs_one";
	/*
	mOutputRcdNameOne = mStatDocName;
	mOutputRcdNameOne << "_task002_rcd_lhs_one";
	boost::shared_ptr<DataRecord> ds_record = boost::make_shared<DataRecord>();
	ds_record->setAttribute("type", "fixbin");
	ds_record->setAttribute("zky_name", mOutputRcdNameOne);
	ds_record->setAttribute("zky_length", output_rcd_len1);
	ds_record->setField(outds_df);
	ds_record->setField(outds_df2);
	ds_record->setField(outds_df3);
	*/
	//mOutputRcdNameAll = "example1_task003_rcd_lhs_all";
	mOutputRcdNameAll = mStatDocName;
	mOutputRcdNameAll << "_task003_rcd_lhs_all";
	//boost::shared_ptr<DataRecord> ds_record2 = boost::make_shared<DataRecord>();
	//if (mInternalId == "0")
	ds_record2->setAttribute("type", "stat");
	ds_record2->setAttribute("zky_name", mOutputRcdNameAll);
	ds_record2->setAttribute("zky_length", output_rcd_len2);
	
//	boost::shared_ptr<DataSchema> ds_schema1 = boost::make_shared<DataSchema>();
//	ds_schema1->setAttribute("jimo_objid", "dataschema_unilength_jimodoc_v0");
//	ds_schema1->setAttribute("zky_name", "example1_task002_schema");
//	ds_schema1->setRecord(ds_record);

	boost::shared_ptr<DataSchema> ds_schema2 = boost::make_shared<DataSchema>();
	ds_schema2->setAttribute("jimo_objid", "dataschema_unilength_jimodoc_v0");
	ds_schema2->setAttribute("zky_name", "example1_task003_schema");
	ds_schema2->setRecord(ds_record2);
	
/*	
	boost::shared_ptr<DataSet> output_ds = boost::make_shared<DataSet>();
	output_ds->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	output_ds->setAttribute("zky_name", ds_name1);
	output_ds->setScanner(ds_scanner1);
	output_ds->setSchema(ds_schema1);
	mOdsOne = output_ds;
	mOdsOneDatasetName = ds_name1;
*/
	boost::shared_ptr<DataSet> output_ds = boost::make_shared<DataSet>();
	output_ds->setAttribute("zky_name", output_all_dsname);
	
	boost::shared_ptr<DataSet> output_ds2 = boost::make_shared<DataSet>();
	output_ds2->setAttribute("jimo_objid","dataset_bydatascanner_jimodoc_v0");
	output_ds2->setAttribute("zky_name", ds_name2);
	output_ds2->setScanner(ds_scanner2);
	output_ds2->setSchema(ds_schema2);
	mOdsAll = output_ds2;
	mOdsAllDatasetName = ds_name2;


	OmnString task_name = mStatDocName;
	//task_name << "_" << mAgrType << "_" << mInputValueFieldName << "_" << mInternalId << "_task001";
	task_name << "_distinct_" << mInternalId << "_task001";

	//create jobtask
	boost::shared_ptr<JobTask> jobtask = boost::make_shared<JobTask>();
	jobtask->setAttribute("zky_name", task_name);
	jobtask->setAttribute("zky_starttype", "auto");
	jobtask->setAttribute("zky_taskid", "task001");
	jobtask->setMapTask(mtask);
	jobtask->setReduceTask(rtask);
	jobtask->setInputDataset(lhs_inputds);
	jobtask->setInputDataset(rhs_inputds);
	jobtask->setOutputDataset(output_ds);
	jobtask->setOutputDataset(output_ds2);
	jobtask->setSplitType("join");
	//jobtask->setHasSplit(true);
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

