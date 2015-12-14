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
#include "JimoDataProc/DataProcPip.h"
#include "DataProc/DataProc.h"
#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoDataProcPip_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoDataProcPip(version);
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


AosJimoDataProcPip::AosJimoDataProcPip(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcPip)
{
}


AosJimoDataProcPip::AosJimoDataProcPip(const AosJimoDataProcPip &proc)
:
AosJimoDataProc(proc)
{
	if (proc.mFirstProc)
		mFirstProc = proc.mFirstProc->cloneProc();

	AosDataProcObjPtr tmp_proc;
	vector<AosDataProcObjPtr> procs;
	for (size_t i = 0; i < proc.mProcs.size(); ++i)
	{
		procs.clear();
		for(size_t j = 0; j < proc.mProcs[i].size(); ++j)
		{
			tmp_proc = proc.mProcs[i][j]->cloneProc();
			procs.push_back(tmp_proc);
		}
		mProcs.push_back(procs);
	}

	for (size_t i = 0; i < proc.mInputRecords.size(); i++)
	{
		AosDataRecordObjPtr rcd = proc.mInputRecords[i]->clone(0 AosMemoryCheckerArgs);
		mInputRecords.push_back(rcd);
	}
}


AosJimoDataProcPip::~AosJimoDataProcPip()
{
}


void 
AosJimoDataProcPip::setInputDataRecords(
		vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}


bool
AosJimoDataProcPip::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//{
	// "type": "pip", 
	// "dataprocs":[
	// 			{"dataprocs":["dp2", "dp3", "dp4"]},
	// 			{"dataprocs":["dp2", "dp5"]},
	// 			{"dataprocs":["dp2", "dp6", "dp7"]},
	// 				]
	//}
	//]]></data_proc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);

	OmnString msg;
	OmnString dp_jsonstr = def->getNodeText();
	mName = def->getAttrStr("zky_name", "");
	aos_assert_r(mName != "", false);

	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_jsonstr, json);
	aos_assert_r(rslt, false);

	mProcs.clear();

	//get procs
	JSONValue procs = json["dataprocs"];
	rslt = getDataProcs(procs, rdata);
	aos_assert_r(rslt, false);

	//set dataAssemblerObjs
	for (size_t i = 0; i < mProcs.size(); ++i)
	{
		aos_assert_r(mProcs[i].size() > 0, false);

		vector<boost::shared_ptr<Output> > outputs = mProcs[i][mProcs[i].size()-1]->getOutputs();
		for (size_t i = 0; i < outputs.size(); ++i)
		{
			mOutputs.push_back(outputs[i]);
		}
	}

	return true;
}


bool
AosJimoDataProcPip::getDataProcs(
		const JSONValue &json,
		const AosRundataPtr &rdata)
{
	OmnString dpName = "", firstProcName = "";
	AosDataProcObjPtr proc;
	vector<boost::shared_ptr<Output> > outputs;
	JSONValue dataprocs;
	vector<AosDataProcObjPtr> procs;

	for (size_t i = 0; i < json.size(); ++i)
	{
		procs.clear();
		dataprocs = json[i]["dataprocs"];

		for(size_t j = 0; j < dataprocs.size(); ++j)
		{
			dpName = dataprocs[j].asString();

			if (i == 0 && j == 0) 
			{
				firstProcName = dpName;
				mFirstProc = createProc(firstProcName, rdata);
				aos_assert_r(mFirstProc, false);

				mFirstProc->setPipEnd(false);

				outputs = mFirstProc->getOutputs();
				mInputRecords[0] = outputs[0]->getRecord();
				continue;
			}
			else if (j == 0)
			{
				aos_assert_r(dpName == firstProcName, false);
				aos_assert_r(mFirstProc, false);

				outputs = mFirstProc->getOutputs();
				mInputRecords[0] = outputs[0]->getRecord();
				continue;
			}
		
			proc = createProc(dpName, rdata);
			aos_assert_r(proc, false);
		
			if (j != dataprocs.size() - 1)
			{
				proc->setPipEnd(false);

				outputs = proc->getOutputs();
				mInputRecords[0] = outputs[0]->getRecord();
			}
			else
			{
				proc->setPipEnd(true);
			}

			procs.push_back(proc);
		}

		mProcs.push_back(procs);
	}

	return true;
}


AosDataProcObjPtr
AosJimoDataProcPip::createProc(
		const OmnString &dpName,
		const AosRundataPtr &rdata)
{
	//get dataproc doc by dpname
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpName);          
	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	if (!doc)
	{
		OmnAlarm << __func__ << enderr;
		return NULL;
	}

	AosXmlTagPtr proc_conf = doc->getFirstChild("dataproc");
	aos_assert_r(proc_conf, NULL);

	proc_conf->setAttr("version", 1);

	//create dataproc
	AosDataProcObjPtr proc = AosDataProcObj::createDataProcStatic(  
			proc_conf, rdata);
	aos_assert_r(proc, NULL);

	proc->setTaskDocid(mTaskDocid);
	proc->setInputDataRecords(mInputRecords);          
	proc->config(proc_conf, rdata);

	return proc;
}


AosDataProcStatus::E
AosJimoDataProcPip::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	aos_assert_r(mFirstProc, AosDataProcStatus::eError);

	AosDataRecordObjPtr input_record0 = input_records[0];
	AosDataRecordObjPtr input_record1 = input_records[1];

	AosDataRecordObj *outputrecords[1] = {0};

	AosDataProcStatus::E status0 = mFirstProc->procData(rdata_raw, input_records, &outputrecords[0]);
	switch (status0)
	{
		case AosDataProcStatus::eLT:
		case AosDataProcStatus::eGT:
		case AosDataProcStatus::eEQ:
		case AosDataProcStatus::eContinue:
			break;

		default:
			return status0;
	}

	if (outputrecords[0] == NULL)
		return status0;

	AosDataProcStatus::E status;
	AosDataRecordObj* first_output_rcds = outputrecords[0];
	for (size_t i = 0; i < mProcs.size(); ++i)
	{
		for (size_t j = 0; j < mProcs[i].size(); ++j)
		{
			if (j == 0)
			{
				input_records[0] = first_output_rcds;
			}
			else
			{
				input_records[0] = outputrecords[0];
			}
			status = mProcs[i][j]->procData(rdata_raw, input_records, outputrecords);

			switch (status)
			{
				case AosDataProcStatus::eContinue:
					break;

				default:
					return status;
			}
		}
	}

	input_records[0] = input_record0.getPtr();
	input_records[1] = input_record1.getPtr();

	return status0;
}


bool
AosJimoDataProcPip::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	AosDataProcObjPtr firstProc = this->mFirstProc;
	vector<vector<AosDataProcObjPtr> > childProcs = this->mProcs;
	vector<vector<AosDataProcObjPtr> > tempChildProcs;

	vector<AosDataProcObjPtr> all_procs;
	for (size_t i = 0; i < procs.size(); i++)
	{
		AosJimoDataProcPip *pip_proc = static_cast<AosJimoDataProcPip*>(procs[i].getPtr());
		AosDataProcObjPtr proc = pip_proc->mFirstProc;
		all_procs.push_back(proc);
	}
	bool rslt = firstProc->finish(all_procs, rdata);
	aos_assert_r(rslt, rdata);

	for (size_t i = 0; i < childProcs.size(); i++)
	{
		for (size_t j = 0; j < childProcs[i].size(); j++)
		{
			all_procs.clear();
			AosDataProcObjPtr main_proc = childProcs[i][j];

			for (size_t k = 0; k < procs.size(); k++)
			{
				AosJimoDataProcPip *pip_proc = static_cast<AosJimoDataProcPip*>(procs[k].getPtr());
				tempChildProcs = pip_proc->mProcs;
				AosDataProcObjPtr proc = tempChildProcs[i][j];
				all_procs.push_back(proc);
			}
			rslt = main_proc->finish(all_procs, rdata);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}


bool
AosJimoDataProcPip::start(
		const AosRundataPtr &rdata)
{
	bool rslt = mFirstProc->start(rdata);
	aos_assert_r(rslt, false);

	for (size_t i = 0; i < mProcs.size(); i++)
	{
		for (size_t j = 0; j < mProcs[i].size(); j++)
		{
			rslt = mProcs[i][j]->start(rdata);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}


//bool
//AosJimoDataProcPip::finish(
//		const AosRundataPtr &rdata)
//{
//	vector<AosDataProcObjPtr> procs, v;
//	procs.push_back(mFirstProc);
//	mFirstProc->finish(procs, rdata);
//
//	for (size_t i = 0; i < mProcs.size(); i++)
//	{
//		v.clear();
//		v = mProcs[i];
//		for (size_t j = 0; j < v.size(); j++)
//		{
//			procs.clear();
//			procs.push_back(v[j]);
//			v[j]->finish(procs, rdata);
//		}
//	}
//
//	return true;
//}


AosJimoPtr 
AosJimoDataProcPip::cloneJimo() const
{
	return OmnNew AosJimoDataProcPip(*this);
}


AosDataProcObjPtr
AosJimoDataProcPip::cloneProc() 
{
	return OmnNew AosJimoDataProcPip(*this);
}


bool 
AosJimoDataProcPip::createByJql(
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
	dp_str << dpname << "\" jimo_objid=\"dataprocpipe_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//aos_assert_r(dp_xml, false);
	//
	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname));
	//return AosCreateDoc(dp_xml->toString(), true, rdata);

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


int 
AosJimoDataProcPip::getMaxThreads() const
{
	aos_assert_r(mFirstProc, -1);
	int numThreads = mFirstProc->getMaxThreads();
	if (numThreads == 1) return 1;
	for (size_t i = 0; i < mProcs.size(); i++)
	{
		for (size_t j = 0; j < mProcs[i].size(); j++)
		{
			aos_assert_r(mProcs[i][j], -1);
			numThreads = mProcs[i][j]->getMaxThreads();
			if (numThreads == 1) return 1;
		}
	}
	return numThreads;
}

