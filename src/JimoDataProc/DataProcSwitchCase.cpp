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
// 2015/05/15 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcSwitchCase.h"
#include "DataProc/DataProc.h"
#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoDataProcSwitchCase_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoDataProcSwitchCase(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosJimoDataProcSwitchCase::AosJimoDataProcSwitchCase(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcSwitchCase),
mRawSwitchValue(0)
{
}


AosJimoDataProcSwitchCase::AosJimoDataProcSwitchCase(const AosJimoDataProcSwitchCase &proc)
:
AosJimoDataProc(proc),
mRawSwitchValue(0)
{
	if (proc.mSwitchValue)
	{
		mSwitchValue = proc.mSwitchValue->cloneExpr();
		mRawSwitchValue = mSwitchValue.getPtr();
	}

	//mCaseProcs clone
	map<OmnString, vector<AosDataProcObjPtr> >::const_iterator itr;
	itr = (proc.mCaseProcs).begin();
	vector<AosDataProcObjPtr> v;
	vector<AosDataProcObj*> v_raw;
	AosDataProcObjPtr dp;
	OmnString dp_name;
	map<OmnString, AosDataProcObjPtr> data_procs;
	for(itr = proc.mCaseProcs.begin(); itr != proc.mCaseProcs.end(); ++itr)
	{
		v.clear();
		v_raw.clear();
		for (size_t i = 0; i < itr->second.size(); ++i)
		{
			dp_name = itr->second[i]->getDataProcName();
			if (data_procs.count(dp_name) == 0)
			{
				dp = (itr->second)[i]->cloneProc();
				aos_assert(dp);
				data_procs.insert(make_pair(dp_name, dp));
			}
			else
			{
				dp = data_procs[dp_name];
			}

			//dp = (itr->second)[i];
			v.push_back(dp);
			v_raw.push_back(dp.getPtr());
		}
		OmnString key = itr->first;
		mCaseProcs.insert(make_pair(key, v));
		mRawCaseProcs.insert(make_pair(key, v_raw));
	}

	for (u32 i=0; i<proc.mInputRecords.size(); i++)
	{
		mInputRecords.push_back((proc.mInputRecords[i])->clone(0 AosMemoryCheckerArgs));
	}

	mName = proc.mName;
}


bool
AosJimoDataProcSwitchCase::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	map<OmnString, vector<AosDataProcObjPtr> >	caseProcs = this->mCaseProcs;
	map<OmnString, vector<AosDataProcObjPtr> >::iterator itr = caseProcs.begin();
	map<OmnString, vector<AosDataProcObjPtr> >	tempCaseProcs;
	map<OmnString, vector<AosDataProcObjPtr> >::iterator itr_temp;

	AosDataProcObjPtr proc;
	OmnString dp_name;
	set<OmnString> data_procs;
	for (; itr != caseProcs.end(); ++itr)
	{
		OmnString name = itr->first;
		vector<AosDataProcObjPtr> dps = itr->second;
		for (size_t i = 0; i < dps.size(); i++)
		{
			AosDataProcObjPtr main_proc = dps[i];
			dp_name = main_proc->getDataProcName();
			vector<AosDataProcObjPtr> new_procs;
			for (size_t j = 0; j < procs.size(); j++)
			{
				AosJimoDataProcSwitchCase *switch_proc = static_cast<AosJimoDataProcSwitchCase*>(procs[j].getPtr());
				tempCaseProcs = switch_proc->mCaseProcs;
				itr_temp = tempCaseProcs.find(name);
				if (itr_temp != tempCaseProcs.end())
				{
					proc = itr_temp->second[i];
					new_procs.push_back(proc);
				}
			}
			if (data_procs.count(dp_name) == 0)
			{
				data_procs.insert(dp_name);
				bool rslt = main_proc->finish(new_procs, rdata);
				aos_assert_r(rslt, false);
			}
		}
	}
	return true;
}


bool 
AosJimoDataProcSwitchCase::start(
		const AosRundataPtr &rdata)       
{
	map<OmnString, vector<AosDataProcObjPtr> >::iterator itr = mCaseProcs.begin();
	vector<AosDataProcObjPtr> v;
	for (; itr != mCaseProcs.end(); ++itr)
	{
		v = itr->second;
		for (size_t i = 0; i < v.size(); ++i)
		{
			v[i]->setTargetReporter(this);
			v[i]->start(rdata);
		}
	}
	return true;
}


AosJimoDataProcSwitchCase::~AosJimoDataProcSwitchCase()
{
}


void
AosJimoDataProcSwitchCase::setInputDataRecords(
		vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool
AosJimoDataProcSwitchCase::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//{
	//	"type": "switch_case", 
	//	"switch_value": "key_field1*2",
	//	"switch_cases": 
	//			[
	//					{
	//						"case":["0", "1"],
	//						"dataprocs":["dp1"]
	//					},
	//					{
	//						"case":["2"],
	//						"dataprocs":["dp2"]
	//					},
	//					{
	//						"case":["3"],
	//						"dataprocs":["dp3"]
	//					},
	//					{
	//						"case":["default"],
	//						"dataprocs":["dp4"]
	//					}
	//					]
	//}
	//]]></data_proc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);

	OmnString dp_jsonstr = def->getNodeText();
	mName = def->getAttrStr("zky_name", "");
	aos_assert_r(mName != "", false);

	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_jsonstr, json);
	aos_assert_r(rslt, false);

	//switch value
	OmnString value_str = json["switch_value"].asString();
	aos_assert_r(value_str != "", false);

	OmnString msg;
	value_str << ";";
	mSwitchValue = AosParseExpr(value_str, msg, rdata.getPtr());
	if (!mSwitchValue)                                                       
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << "" << enderr;
		OmnAlarm << value_str << enderr;
		return false;
	}

	mRawSwitchValue = mSwitchValue.getPtr();

	//cases
	JSONValue switch_cases = json["switch_cases"];
	aos_assert_r(switch_cases, false);

	mCaseProcs.clear();
	//get dataprocs
	rslt = getDataProcs(switch_cases, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosJimoDataProcSwitchCase::getDataProcs(
		const JSONValue &json,
		const AosRundataPtr &rdata)
{
	//json:
	// {
	//   "case":["0", "1"],
	//   "dataprocs":["dp1"]
	// },
	// {
	//   "case":["2"],
	//   "dataprocs":["dp2"]
	// }
	// ,
	// {
	//   "case":["3"],
	//   "dataprocs":["dp3"]
	// },
	// {
	//   "case":["default"],
	//   "dataprocs":["dp4"]
	// }
	OmnString case_str = "";
	JSONValue dps, cases;
	vector<AosDataProcObjPtr> v;
	vector<AosDataProcObj*> v_raw;
	AosDataProcObjPtr proc;
	OmnString dp_name;

	map<OmnString, AosDataProcObjPtr> data_procs;
	vector<u32> idexs;

	for (size_t i = 0; i < json.size(); ++i)
	{
		//map<OmnString, AosDataProcObjPtr> procMap;
		v.clear();
		v_raw.clear();
		idexs.clear();
		cases = json[i]["case"];
		dps = json[i]["dataprocs"];
		for (size_t j = 0; j < dps.size(); ++j)
		{
			dp_name = dps[j]["zky_name"].asString();
			if (data_procs.count(dp_name) == 0)
			{
				proc = createProc(dps[j], rdata);
				aos_assert_r(proc, false);
				data_procs.insert(make_pair(dp_name, proc));
				idexs.push_back(v.size());
			}
			else
			{
				proc = data_procs[dp_name];
			}

			//mProcNameMap.insert(make_pair(dp_name, 1));
			//procMap.insert(make_pair(dp_name, proc));

			v.push_back(proc);
			v_raw.push_back(proc.getPtr());
		}
		//mDpNameProcs.push_back(procMap);

		for (size_t j = 0; j < cases.size(); ++j)
		{
			case_str = cases[j].asString();
			aos_assert_r(case_str != "", false);

			mCaseProcs.insert(make_pair(case_str, v));
			mRawCaseProcs.insert(make_pair(case_str, v_raw));
		}

		//set dataAssemblerObjs
		vector<boost::shared_ptr<Output> > outputs;
		for (size_t i = 0; i < idexs.size(); ++i)
		{
			outputs = v[idexs[i]]->getOutputs();
			for (size_t j=0 ; j < outputs.size(); j++)
			{
				mOutputs.push_back(outputs[j]);
			}
		}
	}

	return true;
}


AosDataProcObjPtr
AosJimoDataProcSwitchCase::createProc(
		const JSONValue &dpJSON,
		const AosRundataPtr rdata)
{
	////get dataproc doc by dpname
	//OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dp_name);          
	//AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	//AosXmlTagPtr doc = AosXmlParser::parse(dpstr AosMemoryCheckerArgs);

	string dp_name = dpJSON["zky_name"].asString();
	string jimo_objid = dpJSON["jimo_objid"].asString();
	string dp_str = "";
	dp_str += "<dataproc zky_name=\"";                                      
	dp_str += dp_name + "\" jimo_objid=\"" + jimo_objid + "\">";  
	dp_str += "<![CDATA[" + string(dpJSON.toStyledString()) + "]]></dataproc>";              
	AosXmlTagPtr proc_conf = AosXmlParser::parse(dp_str AosMemoryCheckerArgs);
	if (!proc_conf)
	{
		OmnAlarm << __func__ << enderr;
		return NULL;
	}

	//AosXmlTagPtr proc_conf = doc->getFirstChild("dataproc");
	//aos_assert_r(proc_conf, NULL);

	proc_conf->setAttr("version", 1);

	//create dataproc
	AosDataProcObjPtr proc = AosDataProcObj::createDataProcStatic(  
			proc_conf, rdata);
	aos_assert_r(proc, NULL);

	proc->setTaskDocid(mTaskDocid);
	proc->setInputDataRecords(mInputRecords);          
	proc->config(proc_conf, rdata);
	OmnString name = proc->getDataProcName();
	aos_assert_r(name != "", proc);
	return proc;
}


AosDataProcStatus::E
AosJimoDataProcSwitchCase::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	aos_assert_r(mRawSwitchValue, AosDataProcStatus::eError);
	AosDataRecordObj * input_record = input_records[0];
	bool rslt = mRawSwitchValue->getValue(rdata_raw, input_record, mValue);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	OmnString value = mValue.getStr();

	map<OmnString, vector<AosDataProcObj*> >::iterator itr;
	itr = mRawCaseProcs.find(value);
	if (itr == mRawCaseProcs.end())		
	{
		itr = mRawCaseProcs.find("default");
		if (itr == mRawCaseProcs.end())
		{
			OmnScreen << "may be Alarm" << endl;
			return AosDataProcStatus::eContinue;
		}
	}
	AosDataProcStatus::E status;
	size_t size = itr->second.size();
	for (size_t i = 0; i < size; i++)
	{
		status = itr->second[i]->procData(rdata_raw, input_records, output_records);
		switch (status)
		{
		case AosDataProcStatus::eContinue:
			 break;
		
		default:
			 return status;
		}
	}
	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosJimoDataProcSwitchCase::cloneJimo() const
{
	return OmnNew AosJimoDataProcSwitchCase(*this);
}


AosDataProcObjPtr
AosJimoDataProcSwitchCase::cloneProc() 
{
	return OmnNew AosJimoDataProcSwitchCase(*this);
}


bool 
AosJimoDataProcSwitchCase::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	/**************************************************************
	{
	    "switch_cases": [
	        {
	            "case": [
	                "unicom_duanxin"
	            ],
	            "dataprocs": [
	                "job2_dp_index_unicom_duanxin_system_type",
	                "job2_dp_index_unicom_duanxin_deal_time"
	            ]
	        },
	        {
	            "case": [
	                "unicom_liuliang"
	            ],
	            "dataprocs": [
	                "job2_dp_index_unicom_liuliang_system_type",
	                "job2_dp_index_unicom_liuliang_msisdn"			
	            ]
	        },
	        {
	            "case": [
	                "unicom_yuyin"
	            ],
	            "dataprocs": [
	                "job2_dp_index_unicom_yuyin_system_type",
	                "job2_dp_stat_groupby_db_unicom_unicom_callback_stat"
	            ]
	        },
	        {
	            "case": [
	                "unicom_zengzhi"
	            ],
	            "dataprocs": [
	                "job2_dp_index_unicom_zengzhi_system_type",
	                "job2_dp_stat_groupby_db_unicom_unicom_callback_stat"
	            ]
	        }
	    ],
	    "switch_value": "tablename",
	    "type": "switch_case"
	}
	**************************************************************/

	//1. repair JSON string, we will modify "dataprocs" node 
	//	modify dataproc's name to dataproc json string 
	JSONValue switchCaseJOSN;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, switchCaseJOSN);
	aos_assert_r(rslt, false);

	for (UInt i = 0; i < switchCaseJOSN["switch_cases"].size(); i++)
	{
		JSONValue newDataprocsJOSN;
		JSONValue oneCaseJSON = switchCaseJOSN["switch_cases"][i];
		for (UInt j=0; j<oneCaseJSON["dataprocs"].size(); j++)
		{
			string dataprocname = oneCaseJSON["dataprocs"][j].asString();
			aos_assert_r(dataprocname != "", false);
			string objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dataprocname);
			AosXmlTagPtr jimodoc = prog->getLogicDoc(rdata, objid);
			aos_assert_r(jimodoc, false);
			AosXmlTagPtr dpnode = jimodoc->getFirstChild("dataproc");
			aos_assert_r(dpnode, false);
			string dpjsonstr = dpnode->getNodeText();

			JSONValue dpJSON;
			rslt = reader.parse(dpjsonstr, dpJSON);
			aos_assert_r(rslt, false);
			dpJSON["zky_name"] = string(dpnode->getAttrStr("zky_name"));
			dpJSON["jimo_objid"] = string(dpnode->getAttrStr("jimo_objid"));
			newDataprocsJOSN.append(dpJSON);
			//newDataprocsJOSN.append(jimodoc->toString().data());
		}
		switchCaseJOSN["switch_cases"][i]["dataprocs"] = newDataprocsJOSN;
	}

	// 2. compose dataproc switchcase config 
	string new_json_str = switchCaseJOSN.toStyledString();
	string objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataprocswitchcase_jimodoc_v0\">";
	dp_str << "<![CDATA[" << new_json_str << "]]></dataproc>";
	dp_str << " </jimodataproc>";

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


int 
AosJimoDataProcSwitchCase::getMaxThreads() const
{
	int numThreads;
	map<OmnString, vector<AosDataProcObjPtr> >::const_iterator itr = mCaseProcs.begin();
	for (; itr != mCaseProcs.end(); ++itr)
	{
		vector<AosDataProcObjPtr> procs = itr->second;
		for (size_t i = 0; i < procs.size(); i++)
		{
			numThreads = procs[i]->getMaxThreads();
			if (numThreads == 1) return 1;
		}
	}
	return numThreads;
}

