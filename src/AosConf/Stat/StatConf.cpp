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
#include "AosConf/DataFieldExpr.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataProcStatIndex.h"
#include "AosConf/DataProcStatDoc.h"
#include "AosConf/DataProcStatDocShuffle.h"
#include "AosConf/DataConnectorIIL.h"
#include "AosConf/ReduceTask.h"
#include "AosConf/MapTask.h"
#include "AosConf/DataSet.h"
#include "AosConf/JobTask.h"
#include "AosConf/Job.h"
#include "JQLStatement/JqlStatement.h"
#include "XmlUtil/SeXmlParser.h"
#include <boost/make_shared.hpp>
#include <string>

using boost::make_shared;

using AosConf::StatConf;
using AosConf::JobTask;
StatConf::StatConf(
		const AosXmlTagPtr &inputds,
		const AosXmlTagPtr &statdoc,
		map<OmnString, AosXmlTagPtr> &table_fields):
mInputDataset(inputds),
mStatDoc(statdoc),
mHasCount(false),
mHasDistinct(false),
mIsDistinct(false)
{
	mTableFields = table_fields;
	aos_assert(mStatDoc);
	mStatDocName = mStatDoc->getAttrStr(AOSTAG_OBJID, "");
	aos_assert(mStatDocName != "");

	mStatisticDocObjid = mStatDoc->getAttrStr("stat_doc_objid", "");
	aos_assert(mStatisticDocObjid != "");

	mTableName = mStatDoc->getAttrStr("zky_table_name", "");
	aos_assert(mTableName != "");

	mStatConfLevel = mStatDoc->getAttrStr("stat_conf_level", "0");
	mTimeFromField = mStatDoc->getAttrStr("zky_time_from_field", "");
	mTimeFromFormat = mStatDoc->getAttrStr("zky_origtmfmt", "");
	//felicia, 2014/10/09 for ketty
	//mTimeToFormat = mStatDoc->getAttrStr("zky_targettmfmt", "");
	mTimeToFormat = mTimeFromFormat;
	mInputTimeFieldName = mStatDocName;
	mInputTimeFieldName << "_time_field";

	mVector2d = mStatDoc->getFirstChild("vt2d_conn");
	aos_assert(mVector2d);

	mStartTimeSlot = mVector2d->getAttrStr("start_time_slot", "");
	aos_assert(mStartTimeSlot != "");
		
	int internal_id = mStatDoc->getAttrInt("internal_id", 0);
	bool has_distinct = mStatDoc->getAttrBool("has_distinct", false);
	if(has_distinct)
	{
		mDistinctInternalId = "";
		mDistinctInternalId << internal_id;
		mHasDistinct = has_distinct;
	}
	mStatisticsDoc.push_back(mStatDoc);
/*
	AosXmlTagPtr statics = mStatDoc->getFirstChild("internal_statistics");
	aos_assert(statics);

	AosXmlTagPtr xml = statics->getFirstChild("statistic");
	aos_assert(xml);

	while(xml)
	{
		int internal_id = xml->getAttrInt("internal_id", 0);
		bool has_distinct = xml->getAttrBool("has_distinct", false);
		if(has_distinct)
		{
			mDistinctInternalId = "";
			mDistinctInternalId << internal_id;
			mHasDistinct = has_distinct;
		}
		//mStatisticsDoc.insert(make_pair(internal_id, xml));
		mStatisticsDoc.push_back(xml);
		xml = statics->getNextChild();
	}
*/
}

StatConf::~StatConf() 
{
}

bool
StatConf::config(const AosXmlTagPtr &xml)
{
	mStatFields.clear();
	mFieldLenMap.clear();
	mMaxLen = 0;

	mInternalId = xml->getAttrStr("internal_id");
	aos_assert_r(mInternalId != "", false);

	mIsDistinct = xml->getAttrBool("zky_isdistinct", false);

	aos_assert_r(mInputDataset, false);

	AosXmlTagPtr schema_tag = mInputDataset->getFirstChild("dataschema");
	aos_assert_r(schema_tag, false);

	AosXmlTagPtr record_tag = schema_tag->getFirstChild("datarecord");
	aos_assert_r(record_tag, false);

	AosXmlTagPtr fields_tag = record_tag->getFirstChild("datafields");
	aos_assert_r(fields_tag, false);
/*
	if(mHasDistinct && mInternalId == "0" && mStatConfLevel == "0-1")
	{
		OmnString distinct_fields = xml->getAttrStr("distinct_fields", "");
		aos_assert_r(distinct_fields != "", false);
		AosStrSplit split(distinct_fields, ",");
		mDistinctFields = split.entriesV();

		AosXmlTagPtr field;
		mDistinctMaxLen = mDistinctFields.size()-1;
		for(size_t i=0; i<mDistinctFields.size(); i++)
		{
			field = fields_tag->getChildByAttr("zky_name", mDistinctFields[i]);
			if (!field)
			{
				map<OmnString, OmnString>::iterator itr = mStatTypes.find(mDistinctFields[i]);
				if (itr->second == "expr")
				{
					OmnString exprstr = mDistinctFields[i];
					AosJqlUtil::unescape(exprstr);

					OmnString field_str = "<datafield type=\"expr\" ";
					field_str << " zky_name=\"" << mDistinctFields[i] << "\" zky_length=\"20\">"
						<< "<expr><![CDATA[" << exprstr << "]]></expr>"
						<< "</datafield>";
					AosXmlParser xmlparser;
					AosXmlTagPtr node = xmlparser.parse(field_str, "" AosMemoryCheckerArgs);
					aos_assert_r(node, false);
					fields_tag->addNode(node);
					mDistinctMaxLen += 20;
					continue;
				}
				OmnAlarm << "never come here!" << enderr;
				return false;
			}
			aos_assert_r(field, false);

			int len = field->getAttrInt("zky_length", 50);
			aos_assert_r(len != -1, false);

			mDistinctMaxLen += len;
		}
	}
*/
	AosXmlTagPtr measures = xml->getFirstChild("measures");
	aos_assert_r(measures, false);

	AosXmlTagPtr measure_tag = measures->getFirstChild("measure");
	aos_assert_r(measure_tag, false);

	mHasCount = false;
	mMeasures.clear();
	OmnString agr_type, field_name, name, field_type, sdatatype;
	AosXmlTagPtr data_field_tag = NULL;
	while(measure_tag)
	{
		field_name = measure_tag->getAttrStr("field_name");
		aos_assert_r(field_name != "", false);

		data_field_tag = mTableFields[field_name];
		if (data_field_tag)
		{
			sdatatype = data_field_tag->getAttrStr("sdatatype");
			if (sdatatype != "")
			{
				field_type = sdatatype;
			}
			else
			{
				field_type = data_field_tag->getAttrStr("datatype");
			}
			aos_assert_r(field_type != "", false);
		}

		name = measure_tag->getAttrStr("zky_name");
		aos_assert_r(name != "", false);

		agr_type = measure_tag->getAttrStr("agr_type");
		aos_assert_r(agr_type != "", false);
		if(agr_type == "count")
		{
			mHasCount = true;
			if (mInternalId == "0")
			{
				field_name = mStatDocName;
				field_name << "0x2a";
			}
		}

		Measure ms;
		ms.agr_type = agr_type;
		ms.field_type = field_type;
		ms.field_name = field_name;
		ms.name = name;
		//ms.vector2d_control = vector2d_control;
		//ms.agr_type_node = agrtype_node;

		mMeasures.push_back(ms);
		measure_tag = measures->getNextChild();
	}	
	/*
	mVector2dControl = measure_tag->getFirstChild("vector2d_control");
	aos_assert_r(mVector2dControl, false);
	
	AosXmlTagPtr measure_field = measure_tag->getFirstChild("measure_field");
	aos_assert_r(measure_field, false);

	mInputValueFieldName = measure_field->getAttrStr("field_name");
	aos_assert_r(mInputValueFieldName != "", false);

	mAgrTypeNode = measure_tag->getFirstChild("agr_type");
	aos_assert_r(mAgrTypeNode, false);

	mAgrType = mAgrTypeNode->getAttrStr("type");
	aos_assert_r(mAgrType != "", false);
	*/

	mStatIdentifyKey = "";
	AosXmlTagPtr key_fields = xml->getFirstChild("key_fields");
	aos_assert_r(key_fields, false);
	AosXmlTagPtr field_tag = key_fields->getFirstChild();
	aos_assert_r(field_tag, false);
	while(field_tag)
	{
		OmnString field_name = field_tag->getAttrStr("field_name");
		aos_assert_r(field_name!= "", false);

		AosJqlUtil::escape(field_name);

		OmnString field_type = field_tag->getAttrStr("zky_type");
		if (field_type != "")
		{
			mStatTypes.insert(make_pair(field_name, field_type));
		}
		mStatFields.push_back(field_name);
		mStatIdentifyKey << field_name;
		field_tag = key_fields->getNextChild(); 
		if(field_tag)
		{
			mStatIdentifyKey << "_";
		}
	}
	//OmnString mInputRecordName = inputds->getAttrStr("zky_name", "");
	
	AosXmlTagPtr field;
	int len = 0;
	//mMaxLen = mStatFields.size()-1;
	for(size_t i=0; i<mStatFields.size(); i++)
	{
		field = fields_tag->getChildByAttr("zky_name", mStatFields[i]);
		if (!field)
		{
			map<OmnString, OmnString>::iterator itr = mStatTypes.find(mStatFields[i]);
			if (itr->second == "expr")
			{
				len = 20;
				OmnString exprstr = mStatFields[i];
				AosJqlUtil::unescape(exprstr);

				OmnString field_str = "<datafield type=\"expr\" ";
				field_str << " zky_name=\"" << mStatFields[i] << "\" zky_length=\"" << len << "\">"
					<< "<expr><![CDATA[" << exprstr << "]]></expr>"
					<< "</datafield>";
				AosXmlParser xmlparser;
				AosXmlTagPtr node = xmlparser.parse(field_str, "" AosMemoryCheckerArgs);
				aos_assert_r(node, false);
				fields_tag->addNode(node);
				mMaxLen += 50;
				mFieldLenMap.insert(make_pair(mStatFields[i], len));
				continue;
			}
			OmnAlarm << "never come here!" << enderr;
			return false;
		}
		aos_assert_r(field, false);
		
		len = field->getAttrInt("zky_length", 50);
		aos_assert_r(len != -1, false);

		mMaxLen += len;
		mFieldLenMap.insert(make_pair(mStatFields[i], len));
	}


	return true;
}

string
StatConf::createConfig(
		OmnString &objid, 
		OmnString &ctnr_objid,
		const u32 &max_thread) 
{
	Job job;
	job.setAttribute("zky_jobid", "rootjob");
	job.setAttribute("zky_ctnrobjid", "task_ctnr");
	job.setAttribute("log_ctnr", "log_ctnr");
	job.setAttribute("zky_ispublic", "true");
	job.setAttribute("zky_public_doc", "true");
	job.setAttribute("zky_public_ctnr", "true");
	job.setAttribute("zky_job_version", "1");
	
	for(size_t i=0; i < mStatisticsDoc.size(); i++)
	{
		bool rslt = config(mStatisticsDoc[i]);
		aos_assert_r(rslt, "");

		boost::shared_ptr<JobTask> jobtask1;
		if(mIsDistinct)
		{
			aos_assert_r(mHasDistinct && mStatConfLevel == "0-1", "");
			jobtask1 = createDistinctConfig1();
			jobtask1->setAttribute(AOSTAG_MAXNUMTHREAD, "1");
		}
		else
		{
			jobtask1 = createConfig1();
			jobtask1->setAttribute(AOSTAG_MAXNUMTHREAD, max_thread);
		}
		
		//create job
		boost::shared_ptr<JobTask> jobtask2 = createConfig2();
		boost::shared_ptr<JobTask> jobtask4 = createConfig4();
		aos_assert_r(jobtask1 != 0, "");
		aos_assert_r(jobtask2 != 0, "");
		aos_assert_r(jobtask4 != 0, "");
		job.setJobTask(jobtask1);
		job.setJobTask(jobtask2);
		job.setJobTask(jobtask4);
		
		if(mIsDistinct)
		{
			mIsDistinct = false;			
		}
	}
	
	string jobconf = job.getConfig();
	return jobconf;
}

bool
StatConf::createConfig() 
{
	Job job;
	job.setAttribute("zky_jobid", "rootjob");
	job.setAttribute("zky_ctnrobjid", "task_ctnr");
	job.setAttribute("log_ctnr", "log_ctnr");
	job.setAttribute("zky_ispublic", "true");
	job.setAttribute("zky_public_doc", "true");
	job.setAttribute("zky_public_ctnr", "true");
	job.setAttribute("zky_job_version", "1");
	
	for(size_t i=0; i < mStatisticsDoc.size(); i++)
	{
		bool rslt = config(mStatisticsDoc[i]);
		aos_assert_r(rslt, false);

		boost::shared_ptr<JobTask> jobtask1 = createConfig0();
		//jobtask1->setAttribute(AOSTAG_MAXNUMTHREAD, max_thread);
		
		//create job
		boost::shared_ptr<JobTask> jobtask2 = createConfig2();
		boost::shared_ptr<JobTask> jobtask4 = createConfig4();
		aos_assert_r(jobtask1 != 0, false);
		aos_assert_r(jobtask2 != 0, false);
		aos_assert_r(jobtask4 != 0, false);
		job.setJobTask(jobtask1);
		job.setJobTask(jobtask2);
		job.setJobTask(jobtask4);
		
		mTasks.push_back(jobtask2);
		mTasks.push_back(jobtask4);
	}
	
	//string jobconf = job.getConfig();
	//cout << " ===================job_doc============================= " << endl;
	//cout << jobconf << endl;
	//cout << " ======================================================== " << endl;
	return true;;
}



//============================================
// Following is Created by Ketty
StatConf::StatConf(map<OmnString, AosXmlTagPtr> &table_fields)
:
mHasCount(false),
mHasDistinct(false),
mIsDistinct(false)
{
	mTableFields = table_fields;
}

bool
StatConf::configNew(
		const AosXmlTagPtr &input_dataset_conf,
		const AosXmlTagPtr &run_stat_conf,
		map<OmnString, AosXmlTagPtr> &table_fields)
{
	// Ketty 2014/10/10
	// stat_name maybe: table_name + "_stat" + identify_key.
	// stat_name is the internal_stat's objid.
	//
	// run_stat_conf format:
	// <statistic zky_stat_name="xxx" >
	// 	<key_fields>
	// 		<field field_name="f11"></field>
	// 		<field field_name="f21"></field>
	// 		...	
	// 	</key_fields>
	// 	<time_field field_name="key_field8" time_format="xxx"></time_field>
	// 	<measures>
	// 		<measure field_name="key_field7" agr_type="sum" 
	// 			zky_name="sum0x28key_field70x29" zky_data_type="int64" 
	// 			grpby_time_unit="_year">
	// 		...
	// 	</measures>
	// </statistic>
	
	aos_assert_r(input_dataset_conf && run_stat_conf, false); 	
	bool rslt;
	mInputDataset = input_dataset_conf;
	mStatRunDoc = run_stat_conf;

	mStatName = mStatRunDoc->getAttrStr("zky_stat_name", "");
	aos_assert_r(mStatName != "", false);
	
	mStatRunDocName = mStatRunDoc->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(mStatRunDocName != "", false);

	rslt = configCondition(run_stat_conf);
	aos_assert_r(rslt, false);

	rslt = configTimeField(run_stat_conf);
	aos_assert_r(rslt, false);
	
	rslt = configMeasures(run_stat_conf);
	aos_assert_r(rslt, false);

	rslt = configStatKeyFields(run_stat_conf, table_fields);
	aos_assert_r(rslt, false);
	
	// Ketty TTTT 2014/11/20
	boost::shared_ptr<JobTask> jobtask1 = createConfig0New();
	aos_assert_r(jobtask1 != 0, false);

	if(mStatFields.size())
	{
		boost::shared_ptr<JobTask> jobtask2 = createConfig2New();
		aos_assert_r(jobtask2 != 0, false);
		mTasks.push_back(jobtask2);
	}

	boost::shared_ptr<JobTask> jobtask4 = createConfig4New();
	aos_assert_r(jobtask4 != 0, false);
	
	mTasks.push_back(jobtask4);
	
	return true;
}


bool
StatConf::configCondition(const AosXmlTagPtr &run_stat_conf)
{
	mCondText = "";
	AosXmlTagPtr cond = run_stat_conf->getFirstChild("cond");
	if (cond)
	{
		mCondText = cond->getNodeText();
	}
	return true;
}

bool
StatConf::configStatKeyFields(
		const AosXmlTagPtr &run_stat_conf,
		map<OmnString, AosXmlTagPtr> &table_fields)
{
	mStatFields.clear();
	//mStatIdentifyKey = "";
	mFieldLenMap.clear();
	mMaxLen = 0;
/*
	AosXmlTagPtr schema_tag = mInputDataset->getFirstChild("dataschema");
	aos_assert_r(schema_tag, false);

	AosXmlTagPtr record_tag = schema_tag->getFirstChild("datarecord");
	aos_assert_r(record_tag, false);

	AosXmlTagPtr datafields_tag = record_tag->getFirstChild("datafields");
	aos_assert_r(datafields_tag, false);
*/
	AosXmlTagPtr key_fields = run_stat_conf->getFirstChild("key_fields");
	// Ketty TTTT 2014/11/20
	//aos_assert_r(key_fields, false);
	if(!key_fields)	return true;

	AosXmlTagPtr key_field_tag = key_fields->getFirstChild();
	// Ketty TTTT 2014/11/20
	//aos_assert_r(key_field_tag, false);
	if(!key_field_tag)	return true;

	int field_len = -1;
	OmnString key_field_name, key_field_type;
	vector<OmnString> distStatFields; //used to adjust dist key field order
	distStatFields.clear();
	while(key_field_tag)
	{
		key_field_name = key_field_tag->getAttrStr("field_name");
		aos_assert_r(key_field_name!= "", false);
		AosJqlUtil::escape(key_field_name);

		if (isMeasureField(key_field_name))
			distStatFields.push_back(key_field_name);
		else
			mStatFields.push_back(key_field_name);

		//if(mStatIdentifyKey != "") mStatIdentifyKey << "_";
		//mStatIdentifyKey << key_field_name;
	
		key_field_type = key_field_tag->getAttrStr("zky_type");
		//field_len = calculateKeyFieldLen(datafields_tag, 
		//		key_field_name, key_field_type);
		field_len = calculateKeyFieldLen(table_fields, 
				key_field_name, key_field_type);
		aos_assert_r(field_len != -1, false);

		mMaxLen += field_len;
		mFieldLenMap.insert(make_pair(key_field_name, field_len));
		
		key_field_tag = key_fields->getNextChild(); 
	}

	//add dist key fields to the end
	for (u32 i = 0; i < distStatFields.size(); i++)
	{
		key_field_name = distStatFields[i];
		mStatFields.push_back(key_field_name);
	}
	
	// the max_len must contain the sep.
	mMaxLen += (mStatFields.size() - 1); 

	return true;
}

bool
StatConf::isMeasureField(OmnString name)
{
	OmnString measureName;

	for (u32 i = 0; i < mMeasures.size(); i++)
	{
		measureName = mMeasures[i].field_name;
		if (name == measureName)
			return true;
	}

	return false;
}

int
StatConf::calculateKeyFieldLen(
		//const AosXmlTagPtr &datafields_tag,
		map<OmnString, AosXmlTagPtr> &table_fields,
		const OmnString &key_field_name,
		const OmnString &key_field_type)
{
	int len;
	//AosXmlTagPtr data_field_tag = datafields_tag->getChildByAttr(
	//		"zky_name", key_field_name);
	AosXmlTagPtr data_field_tag = table_fields[key_field_name];
	if(data_field_tag)
	{
		//len = data_field_tag->getAttrInt("zky_length", -1);
		len = data_field_tag->getAttrInt("size", 50);
		aos_assert_r(len != -1, -1);
		
		return len;
	}

	if(key_field_type != "expr")
	{
		OmnAlarm << "never come here!" << enderr;
		return false;
	}
	
	len = 20;
/*
	OmnString field_str = "<datafield type=\"expr\" ";
	field_str << " zky_name=\"" << key_field_name << "\" "
		<< "zky_length=\"" << len << "\">"
		<< "<expr><![CDATA[" << exprstr << "]]></expr>"
		<< "</datafield>";
	AosXmlParser xmlparser;
	AosXmlTagPtr node = xmlparser.parse(field_str, "" AosMemoryCheckerArgs);
	aos_assert_r(node, false);
*/
	boost::shared_ptr<DataFieldExpr> data_field = boost::make_shared<DataFieldExpr>();  
	OmnString exprstr = key_field_name;
	OmnString field_name = key_field_name;
	AosJqlUtil::escape(field_name);
	data_field->setExpr(exprstr);
	data_field->setAttribute("zky_length", len);
	data_field->setAttribute("zky_name", field_name);
	data_field->setAttribute("type", "expr");
	mInputFields.push_back(data_field);	
	//datafields_tag->addNode(node);

	return len;
}


bool
StatConf::configTimeField(const AosXmlTagPtr &run_stat_conf)
{
	AosXmlTagPtr time_field_conf = run_stat_conf->getFirstChild("time_field");
	//aos_assert_r(time_field_conf, false);
	if(!time_field_conf)	return true;	// maybe has no time field.

	mTimeFromField = time_field_conf->getAttrStr("field_name", "");
	mTimeFromFormat = time_field_conf->getAttrStr("time_format", "");

	return true;	
}


bool
StatConf::configMeasures(const AosXmlTagPtr &run_stat_conf)
{
	OmnString name;
	AosXmlTagPtr data_field_tag = NULL;
	mHasCount = false;
	mMeasures.clear();
	mStatTimes.clear();
	
	AosXmlTagPtr measures_conf = run_stat_conf->getFirstChild("measures");
	aos_assert_r(measures_conf, false);

	AosXmlTagPtr measure_tag = measures_conf->getFirstChild("measure");
	aos_assert_r(measure_tag, false);

	set<OmnString> exist_grpby_timeunit;
	set<OmnString>::iterator itr;

	OmnString sdatatype;
	while(measure_tag)
	{
		name = "";
		Measure ms;
		ms.field_name = measure_tag->getAttrStr("field_name");
		aos_assert_r(ms.field_name != "", false);

		data_field_tag = mTableFields[ms.field_name];
		if (data_field_tag)
		{
			sdatatype = data_field_tag->getAttrStr("sdatatype");
			if (sdatatype != "")
			{
				ms.field_type = sdatatype;
			}
			else
			{
				ms.field_type = data_field_tag->getAttrStr("datatype");
			}
			aos_assert_r(ms.field_type != "", false);
		}

		ms.agr_type = measure_tag->getAttrStr("agr_type");
		aos_assert_r(ms.agr_type != "", false);
		(ms.agr_type).toLower();
		//if(ms.agr_type == "count" || ms.agr_type =="dist_count")
		if(ms.agr_type == "count")
		{
			mHasCount = true;
			ms.field_name = mStatDocName;
			ms.field_name << "0x2a";
		}

		name << measure_tag->getAttrStr("agr_type") << "(" << ms.field_name << ")";
		AosJqlUtil::escape(name);
		ms.name = name;

OmnScreen << " Andy    =========== measure name " << name << endl;

		ms.grpby_time_unit = measure_tag->getAttrStr("grpby_time_unit");
		if(ms.grpby_time_unit != "")
		{
			aos_assert_r(mTimeFromField != "", false);

			itr = exist_grpby_timeunit.find(ms.grpby_time_unit);
			if(itr == exist_grpby_timeunit.end())
			{
				exist_grpby_timeunit.insert(ms.grpby_time_unit);
				
				StatTime time;
				time.mName = mStatRunDocName;
				time.mName << "_time" << ms.grpby_time_unit;
				time.mToFormat = convertTimeUnitToTimeFormat(ms.grpby_time_unit);
				if(time.mToFormat == "" && ms.grpby_time_unit == "_alltime")
				{
					time.mToFormat = mTimeFromFormat;
				}
				aos_assert_r(time.mToFormat != "", false);
				
				ms.stat_time_idx = mStatTimes.size();
				mStatTimes.push_back(time);
			}
		}
		else
		{
			aos_assert_r(mTimeFromField == "", false);
			ms.stat_time_idx = -1;
		}

		mMeasures.push_back(ms);
		measure_tag = measures_conf->getNextChild();
	}
	return true;
}


OmnString
StatConf::convertTimeUnitToTimeFormat(const OmnString &time_unit_str)
{
	if(time_unit_str == "_epoch_time") 	return "epochtime";
	if(time_unit_str == "_hour") 	return "epochhour";
	if(time_unit_str == "_day") 	return "epochday";
	if(time_unit_str == "_week") 	return "epochweek";
	if(time_unit_str == "_month") 	return "epochmonth";
	if(time_unit_str == "_year") 	return "epochyear";
	if(time_unit_str == "_hour") 	return "epochhour";
	
	return "";
}


