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
// 2015/11/04 Created by Andy 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcTransTorturer.h"

#include "API/AosApiG.h"
#include "API/AosApiS.h"
#include "DataProc/DataProc.h"
#include "DocTrans/TestTrans.h"
#include "DocTrans/AsyncTestTrans.h"

#include "IILTrans/IILTestTrans.h"
#include "IILTrans/AsyncIILTestTrans.h"

#include "JobTrans/JobTestTrans.h"
#include "JobTrans/JobAsyncTestTrans.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataProcTransTorturer_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcTransTorturer(version);
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


AosDataProcTransTorturer::AosDataProcTransTorturer(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcTransTorturer),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mHours(0.0),
mCallbackCount(0)
{
}


AosDataProcTransTorturer::AosDataProcTransTorturer(const AosDataProcTransTorturer &proc)
:
AosJimoDataProc(proc),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mHours(proc.mHours),
mCallbackCount(0)
{
}


AosDataProcTransTorturer::~AosDataProcTransTorturer()
{
}


bool
AosDataProcTransTorturer::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//		"hours": "xxx"
	//	}
	//]]></dataproc>
	
	try
	{
		aos_assert_r(def, false);
		aos_assert_r(isVersion1(def), false);

		//create output record templates
		mName = def->getAttrStr(AOSTAG_NAME, "");
		string jsonstr = def->getNodeText();

		JSONReader reader;
		JSONValue jsonObj;
		bool rslt = reader.parse(jsonstr, jsonObj);
		aos_assert_r(rslt, false);

		OmnString hours = jsonObj["hours"].asString();
		mHours = atof(hours.data()); 
		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}

	return true;
}


bool
AosDataProcTransTorturer::createOutput(
		const OmnString &dpname,
		const JSONValue &json_conf,
		const AosRundataPtr &rdata)
{
	return true;
}
			

AosDataProcStatus::E
AosDataProcTransTorturer::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	bool rslt = false;
	int count = 0;
	u64 start_time = OmnGetTimestamp();
	u64 end_time = start_time + mHours*60*60*1000*1000;
	while(1)
	{
		if (count % ((u64)(mHours * 10000)) == 0)
		{
			u64 tmp = OmnGetTimestamp();
			if (end_time < tmp)
			{
				 return AosDataProcStatus::eExit;
			}
		}
		count ++;

		OmnSPtr<AosDataProcTransTorturer> thisptr(this, false);
		u64 docid = random() + 10000;
		docid << 32;
		i64	req_size = random()%(1000*1000*10);
		i64	resp_size = random()%(1000*1000*10);
		i64 sleep = random()%10000;

		OmnString iil_name  = "_zt44_index_" ;
		iil_name << docid;
		
		int svr_id = docid%AosGetNumPhysicals();

		if (sleep == 9) 
		{
			req_size = 1000;
			resp_size = 1000;
			sleep = 0;
		}
		else
		{
			req_size = 1000*1000*2;
			resp_size = 1000*1000*2;
			sleep = 0;
		}
		req_size = resp_size = 1000*1000 + count;

		AosBuffPtr resp;
		OmnSPtr<AosDocTestTrans> trans1 = OmnNew AosDocTestTrans(docid, req_size, resp_size, sleep, rdata_raw);
		rslt = AosSendTrans(rdata_raw, trans1); 
		aos_assert_r(rslt, AosDataProcStatus::eContinue);

		OmnSPtr<AosIILTestTrans> trans3 = OmnNew AosIILTestTrans(iil_name, req_size, resp_size, sleep, rdata_raw);
		rslt = AosSendTrans(rdata_raw, trans3); 
		aos_assert_r(rslt, AosDataProcStatus::eContinue);

		OmnSPtr<AosJobTestTrans> trans5 = OmnNew AosJobTestTrans(svr_id, req_size, resp_size, sleep, rdata_raw);
		rslt = AosSendTrans(rdata_raw, trans5); 
		aos_assert_r(rslt, AosDataProcStatus::eContinue);

		OmnSPtr<AosAsyncTestTrans> trans2 = OmnNew AosAsyncTestTrans(docid, req_size, resp_size, sleep, thisptr, rdata_raw);
		rslt = AosSendTransAsyncResp(rdata_raw, trans2); 
		aos_assert_r(rslt, AosDataProcStatus::eContinue);

		OmnSPtr<AosAsyncIILTestTrans> trans4 = OmnNew AosAsyncIILTestTrans(iil_name, req_size, resp_size, sleep, thisptr, rdata_raw);
		rslt = AosSendTransAsyncResp(rdata_raw, trans4); 
		aos_assert_r(rslt, AosDataProcStatus::eContinue);

		OmnSPtr<AosJobAsyncTestTrans> trans6 = OmnNew AosJobAsyncTestTrans(svr_id, req_size, resp_size, sleep, thisptr, rdata_raw);
		rslt = AosSendTransAsyncResp(rdata_raw, trans6); 
		aos_assert_r(rslt, AosDataProcStatus::eContinue);
		mLock->lock();
		mCallbackCount += 3;
		mLock->unlock();

OmnScreen << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	}
	return AosDataProcStatus::eContinue;
}

vector<AosDataRecordObjPtr> 
AosDataProcTransTorturer::getOutputRecords()
{
	vector<AosDataRecordObjPtr> v;
	return v;
}


AosJimoPtr 
AosDataProcTransTorturer::cloneJimo() const
{
	return OmnNew AosDataProcTransTorturer(*this);
}

AosDataProcObjPtr
AosDataProcTransTorturer::clone() 
{
	return OmnNew AosDataProcTransTorturer(*this);
}


bool 
AosDataProcTransTorturer::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr, 
		const AosJimoProgObjPtr &prog)
{
	AosXmlTagPtr dp_xml;
	
	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, json);
	aos_assert_r(rslt, false);
	//mJson = json;

	OmnString dp_str = "";
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataproc_trans_torturer_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";
	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


bool
AosDataProcTransTorturer::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		bool rslt = procs[i]->finish(rdata);
		aos_assert_r(rslt, false);
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
	}
	OmnScreen << "DataProcTransTorturer " << "(" << mName << ")" << " finished:" 
				<< " procDataCount:" << procDataCount
				<< ", OutputCount:" << procOutputCount << endl;
	return true;
}


bool
AosDataProcTransTorturer::finish(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mCallbackCount != 0) mCondVar->wait(mLock);
	OmnScreen << "DataProcTransTorturer is finished." << endl;
	mLock->unlock();
	return true;
}



void
AosDataProcTransTorturer::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	//mInputRecords = records;
}


void 
AosDataProcTransTorturer::callback(const AosBuffPtr &resp_buff, const bool svr_death)
{
	mLock->lock();
	mCallbackCount --;
	if (mCallbackCount == 0) mCondVar->signal();
	mLock->unlock();
}
