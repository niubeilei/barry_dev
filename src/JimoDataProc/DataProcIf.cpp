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
// 2013/12/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcIf.h"
#include "DataProc/DataProc.h"
#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoDataProcIf_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoDataProcIf(version);
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


AosJimoDataProcIf::AosJimoDataProcIf(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcIf)
{
}


AosJimoDataProcIf::AosJimoDataProcIf(const AosJimoDataProcIf &proc)
:
AosJimoDataProc(proc)
{
	if (proc.mCondition)
	{
		mCondition = proc.mCondition->cloneExpr();
	}
	for (size_t i = 0; i < proc.mTrueProcs.size(); i++)
	{
		AosDataProcObjPtr dp = proc.mTrueProcs[i]->cloneProc();
		mTrueProcs.push_back(dp);
	}
	for (size_t i = 0; i < proc.mFalseProcs.size(); i++)
	{
		AosDataProcObjPtr dp = proc.mFalseProcs[i]->cloneProc();
		mFalseProcs.push_back(dp);
	}

	mInputRecords = proc.mInputRecords;
}


AosJimoDataProcIf::~AosJimoDataProcIf()
{
}


void
AosJimoDataProcIf::setInputDataRecords(
		vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool
AosJimoDataProcIf::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//	{
	// 		"type": "if", 
	// 		"condition": "f1>5",
	// 		"true_procs":["dp1", "dp2"],
	// 		"false_procs":["dp3", "dp4"]
	// }
	//]]></data_proc>
	aos_assert_r(def, false);
	OmnString dp_jsonstr = def->getNodeText();

	OmnString msg;
	mName = def->getAttrStr("zky_name", "");
	aos_assert_r(mName != "", false);

	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_jsonstr, json);
	aos_assert_r(rslt, false);

	//condition
	OmnString cond_str = json["condition"].asString("");
	aos_assert_r(cond_str != "", false);

	cond_str << ";";
	mCondition = AosParseExpr(cond_str, msg, rdata.getPtr());
	if (!mCondition)                                                       
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << "" << enderr;
		OmnAlarm << cond_str << enderr;
		return false;
	}

	mTrueProcs.clear();
	mFalseProcs.clear();

	//get true procs
	JSONValue true_procs = json["true_procs"];
	rslt = getTrueProcs(true_procs, rdata);
	aos_assert_r(rslt, false);

	//get false procs
	JSONValue false_procs = json["false_procs"];
	rslt = getFalseProcs(false_procs, rdata);
	aos_assert_r(rslt, false);

	//set dataAssemblerObjs
	for (size_t i = 0; i < mTrueProcs.size(); i++)
	{
		vector<boost::shared_ptr<Output> > outputs = mTrueProcs[i]->getOutputs();
		for (size_t j=0 ; j < outputs.size(); j++)
		{
			mOutputs.push_back(outputs[j]);
		}
	}
	for (size_t i = 0; i < mFalseProcs.size(); i++)
	{
		vector<boost::shared_ptr<Output> > outputs = mFalseProcs[i]->getOutputs();
		for (size_t j = 0; j < outputs.size(); j++)
		{
			mOutputs.push_back(outputs[j]);
		}
	}

	return true;
}


bool
AosJimoDataProcIf::getTrueProcs(
		const JSONValue &json,
		const AosRundataPtr &rdata)
{
	OmnString dpName, objid;
	AosXmlTagPtr doc;
	AosDataProcObjPtr proc;
	for (size_t i = 0; i < json.size(); i++)
	{
		//get dataproc doc by dpname
		dpName = json[i].asString();
		objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpName);          
		doc = AosGetDocByObjid(objid, rdata);
		if (!doc)
		{
			OmnAlarm << __func__ << enderr;
			return false;
		}

		doc = doc->getFirstChild("dataproc");
		aos_assert_r(doc, false);

		//create dataproc
		proc = AosDataProcObj::createDataProcStatic(  
				        doc, rdata);
		aos_assert_r(proc, false);

		proc->setTaskDocid(mTaskDocid);
		proc->setInputDataRecords(mInputRecords);          
		proc->config(doc, rdata);

		mTrueProcs.push_back(proc);
	}

	return true;
}


bool
AosJimoDataProcIf::getFalseProcs(
		const JSONValue &json,
		const AosRundataPtr &rdata)
{
	OmnString dpName, objid;
	AosXmlTagPtr doc;
	AosDataProcObjPtr proc;
	for (size_t i = 0; i < json.size(); i++)
	{
		//get dataproc doc by dpname
		dpName = json[i].asString();
		objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpName);          
		doc = AosGetDocByObjid(objid, rdata);
		if (!doc)
		{
			OmnAlarm << __func__ << enderr;
			return false;
		}

		doc = doc->getFirstChild("dataproc");
		aos_assert_r(doc, false);

		//create dataproc
		proc = AosDataProcObj::createDataProcStatic(
						doc, rdata);
		aos_assert_r(proc, false);

		proc->setTaskDocid(mTaskDocid);
		proc->setInputDataRecords(mInputRecords);          
		proc->config(doc, rdata);

		mFalseProcs.push_back(proc);
	}

	return true;
}


AosDataProcStatus::E
AosJimoDataProcIf::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	aos_assert_r(mCondition, AosDataProcStatus::eError);

	AosDataRecordObj * input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);

	AosDataRecordObjPtr record = input_record;
	vector<AosDataProcObjPtr> * procs = &mTrueProcs;

	AosValueRslt value_rslt;
	mCondition->getValue(rdata_raw, record.getPtr(), value_rslt);
	bool rslt = value_rslt.getBool();
	if (!rslt)
	{
		procs = &mFalseProcs;
	}
//OmnScreen << mName << "rslt:" << rslt << endl;

	AosDataProcStatus::E status;
	for (u32 i = 0; i < (*procs).size(); i++)
	{
		status = (*procs)[i]->procData(rdata_raw, input_records, output_records);
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


bool
AosJimoDataProcIf::start(const AosRundataPtr &rdata)
{
	bool rslt = false;
	for (size_t i = 0; i < mTrueProcs.size(); i++)
	{
		rslt = mTrueProcs[i]->start(rdata);
		aos_assert_r(rslt, false);
	}
	for (size_t i = 0; i < mFalseProcs.size(); i++)
	{
		rslt = mFalseProcs[i]->start(rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosJimoDataProcIf::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
//	for (size_t i = 0; i < procs.size(); i++)
//	{
//		bool rslt = procs[i]->finish(rdata);
//		aos_assert_r(rslt, false);
//	}
	vector<AosDataProcObjPtr> trueProcs = this->mTrueProcs;
	vector<AosDataProcObjPtr> falseProcs = this->mFalseProcs;
	vector<vector<AosDataProcObjPtr> >  tempTrueProcs, tempFalseProcs;

	for (size_t i = 0; i < procs.size(); i++)
	{
		AosJimoDataProcIf *if_proc = static_cast<AosJimoDataProcIf*>(procs[i].getPtr());
		vector<AosDataProcObjPtr> v1 = if_proc->mTrueProcs;
		vector<AosDataProcObjPtr> v2 = if_proc->mFalseProcs;
		tempTrueProcs.push_back(v1);
		tempFalseProcs.push_back(v2);
	}

	for (size_t i = 0; i < trueProcs.size(); i++)
	{
		vector<AosDataProcObjPtr> all_procs;
		AosDataProcObjPtr main_proc = trueProcs[i];
		for (size_t j = 0; j < tempTrueProcs.size(); j++)
		{
			AosDataProcObjPtr proc = tempTrueProcs[j][i];
			all_procs.push_back(proc);
		}
		bool rslt = main_proc->finish(all_procs, rdata);
		aos_assert_r(rslt, false);
	}
	for (size_t i = 0; i < falseProcs.size(); i++)
	{
		vector<AosDataProcObjPtr> all_procs;
		AosDataProcObjPtr main_proc = falseProcs[i];
		for (size_t j = 0; j < tempFalseProcs.size(); j++)
		{
			AosDataProcObjPtr proc = tempFalseProcs[j][i];
			all_procs.push_back(proc);
		}
		bool rslt = main_proc->finish(all_procs, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


//bool
//AosJimoDataProcIf::finish(
//		const AosRundataPtr &rdata)
//{
//	vector<AosDataProcObjPtr> procs;
//	for (size_t i = 0; i < mTrueProcs.size(); i++)
//	{
//		procs.clear();
//		procs.push_back(mTrueProcs[i]);
//		mTrueProcs[i]->finish(procs, rdata);
//	}
//	for (size_t i = 0; i < mFalseProcs.size(); i++)
//	{
//		procs.clear();
//		procs.push_back(mFalseProcs[i]);
//		mFalseProcs[i]->finish(procs, rdata);
//	}
//
//	return true;
//}


AosJimoPtr 
AosJimoDataProcIf::cloneJimo() const
{
	return OmnNew AosJimoDataProcIf(*this);
}


AosDataProcObjPtr
AosJimoDataProcIf::cloneProc() 
{
	return OmnNew AosJimoDataProcIf(*this);
}


bool 
AosJimoDataProcIf::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataprocif_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//aos_assert_r(dp_xml, false);
	
	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname));
	//return AosCreateDoc(dp_xml->toString(), true, rdata);
	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


int 
AosJimoDataProcIf::getMaxThreads() const
{
	int numThreads;
	for (size_t i = 0; i < mTrueProcs.size(); i++)
	{
		numThreads = mTrueProcs[i]->getMaxThreads();
		if (numThreads == 1) return 1;
	}
	for (size_t i = 0; i < mFalseProcs.size(); i++)
	{
		numThreads = mFalseProcs[i]->getMaxThreads();
		if (numThreads == 1) return 1;
	}
	return numThreads;
}

