////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/StreamDataProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataRecord/StreamRecordset.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include "Porting/Sleep.h"
#include "StreamEngine/StreamSnapshot.h"
#include "JobTrans/UpdateRDDStatusTrans.h"
#include <string>

AosStreamDataProc::AosStreamDataProc(
		const int version,
		const AosJimoType::E type)
:
AosJimoDataProc(version, AosJimoType::eDataProcStream)
{
	mName = "";
	mLock = OmnNew OmnMutex();
	mCondVar = OmnNew OmnCondVar();
	mTask = 0;
	mService = 0;
	mCount = 0;
	mInputRecordWithDocid = 0;
	mInputListenerMap.clear();
}

AosStreamDataProc::AosStreamDataProc(const AosStreamDataProc &proc)
:
AosJimoDataProc(proc)
{
	mCount = 0;
	mInputRecordWithDocid = 0;
	mCondVar = OmnNew OmnCondVar();
	mLock = OmnNew OmnMutex();
}


AosStreamDataProc::~AosStreamDataProc()
{
}


bool
AosStreamDataProc::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &dpConf,
		const AosXmlTagPtr &dsConf)
{
	try
	{
		mName = dpConf->getAttrStr(AOSTAG_NAME, "");
		aos_assert_r(mName != "", false);

		OmnString jsonStr = dpConf->getNodeText();
		JSONReader reader;
		JSONValue jsonConfig;
		bool rslt = reader.parse(jsonStr, jsonConfig);
		aos_assert_r(rslt, false);

		registerListeners(jsonConfig);
		registerSpeakers(jsonConfig);	

		if (dsConf)
		{
			OmnString name;
			AosXmlTagPtr datasetConf;
			AosXmlTagPtr datacolConf;
			AosXmlTagPtr asmConf;
			AosXmlTagPtr compDoc;
			
			datasetConf = dsConf->getFirstChild("dataset");
			while (datasetConf)
			{
				name = "output";
				datacolConf = datasetConf->getFirstChild("datacollector");
				aos_assert_r(datacolConf, false);

				asmConf = datacolConf->getFirstChild("asm");
				if(asmConf)
				{
					compDoc = asmConf->getFirstChild("CompareFun");	
					if (compDoc) addComp(name, compDoc);
				}
				else
				{
					compDoc = datacolConf->getFirstChild("CompareFun");	
					if (compDoc) addComp(name, compDoc);
				}

				datasetConf = dsConf->getNextChild("dataset");
			}
		}	
	}
	catch (...)
	{   
		OmnScreen << "JSONException..." << endl;
		return false;
	}

	bool rslt = config(dpConf, rdata);
	aos_assert_r(rslt, false);


	return true;
}

bool 
AosStreamDataProc::signal(const int threadLogicId)
{
	return true;
}

bool 
AosStreamDataProc::run(const AosRundataPtr &rdata)
{
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	OmnThreadedObjPtr thisPtr(this, false);                                                                  
	mThread = OmnNew OmnThread(thisPtr, mName, 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}

bool 
AosStreamDataProc::finish(const AosRundataPtr &rdata)
{
	return true;
}

bool
AosStreamDataProc::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	bool rslt = false;
	AosRundata* rdata = mRundata.getPtr();
	while (state == OmnThrdStatus::eActive)
	{
		AosRDDPtr rdd;
		mLock->lock();
		if (!mWaitRDDs.empty())
		{
			rdd = mWaitRDDs.front();
			mWaitRDDs.pop_front();
		}
		else
		{
			bool timeout = false;
			mCondVar->timedWait(mLock, timeout, 1);
			mLock->unlock();
			continue;
		}
		mCount++;
		mLock->unlock();
		aos_assert_r(rdd, false);

		rslt = startRDD(rdd, rdata);
		aos_assert_r(rslt, false);

		rslt = procData(rdata, rdd);
		aos_assert_r(rslt, false);
		
		int remain = 0;
		rslt = outputHandler(remain, rdd, rdata);
		aos_assert_r(rslt, false);

		rslt = finishRDD(rdd, remain, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

//
//add the current dataproc object to its input dataset's
//listener list
//
bool
AosStreamDataProc::registerListeners(const JSONValue &jsonConfig)
{
	OmnString inputName;
	map<OmnString, OmnString>::iterator itr;
	JSONValue jsonInputs;
	
	//
	// "inputListeners" : {
	// 		"ds1" :	 "in1",
	// 		"dp1.out1" : "in2"
	// 	}
	//
	// TODO by Lina
	//generate mInputListenerMap from mJsonInput
	try
	{
		jsonInputs = jsonConfig["inputListeners"];
		Value::Members members = jsonInputs.getMemberNames();
		//std::string name;
		OmnString name;
		for (u32 i = 0; i < members.size(); i++)
		{   
			name = members[i];
			OmnScreen << "name=" << name << " , value=" << jsonInputs[name].asString() << endl;
			mInputListenerMap[name] = jsonInputs[name].asString();
			mService->registerListeners(name, this);
		}
	}
	catch (...)
	{   
		OmnScreen << "JSONException..." << endl;
		return false;
	}

	return true;
}

//
//register output speakers
//
bool
AosStreamDataProc::registerSpeakers(const JSONValue &jsonConfig)
{
	try
	{
		OmnString speakerName;
		JSONValue jsonOutputs = jsonConfig["outputSpeakers"];

		for (u32 i = 0; i < jsonOutputs.size(); i++){
			speakerName = jsonOutputs[i].asString();
			mOutputSpeakerList.push_back(speakerName);
		}
	}
	catch (...)
	{   
		OmnScreen << "JSONException..." << endl;
		return false;
	}
	return true;
}


bool
AosStreamDataProc::inputHandler(const AosRDDPtr &rdd)
{
	mLock->lock();
	mWaitRDDs.push_back(rdd);
	mCondVar->signal();
	mLock->unlock();
	return true;
}

int
AosStreamDataProc::numOutputs(int &remain)
{
	for (u32 i=0; i < mOutputSpeakerList.size(); i++)
	{
		OmnString downstream_name;
		downstream_name << mName << "." << mOutputSpeakerList[i];
		mService->findLastRemain(downstream_name, remain);
	}
	return true;
}

bool
AosStreamDataProc::outputHandler(int &remain, const AosRDDPtr &input_rdd, AosRundata* rdata)
{
	AosRDDPtr output_rdd;
	OmnString outputName;
	map<OmnString, AosRDDPtr>::iterator itr;
	map<OmnString, AosCompareFunPtr>::iterator cmp_itr; 
	for(u32 i = 0; i < mOutputSpeakerList.size(); i++)
	{
		outputName = mOutputSpeakerList[i];
		OmnString downstream_name;
		downstream_name << mName << "." << outputName;

		if (itr == mCurOutputMap.end())
		{
OmnScreen << "********jozhi debug, no output: " << downstream_name << endl;
			mService->findLastRemain(downstream_name, remain);
			continue;
		}

		output_rdd = mCurOutputMap[outputName];
		if (!output_rdd)
		{
OmnScreen << "********jozhi debug, no output: " << downstream_name << endl;
			mService->findLastRemain(downstream_name, remain);
			continue;
		}

		aos_assert_r(output_rdd, false);
		if (output_rdd->isEmpty())
		{
OmnScreen << "********jozhi debug, no output: " << downstream_name << endl;
			mService->findLastRemain(downstream_name, remain);
			continue;
		}

		cmp_itr = mCompMap.find(outputName);
		if (cmp_itr != mCompMap.end())
		{
			output_rdd->sort(rdata, cmp_itr->second);
		}

		output_rdd->setDataId(input_rdd->getDataId());
		output_rdd->setRDDId(input_rdd->getRDDId());
		bool rslt = AosService::dataArrived(rdata, downstream_name, output_rdd);
		aos_assert_r(rslt, false);
		mCurOutputMap.erase(outputName);
	}
	mCurOutputMap.clear();
	return true;
}


bool
AosStreamDataProc::addOutput(
		AosRundata *rdata,
		const OmnString &outputName, 
		AosDataRecordObjPtr &rcd)
{
	AosRDDPtr rdd;
	map<OmnString, AosRDDPtr>::iterator itr;
	itr = mCurOutputMap.find(outputName);
	if (itr == mCurOutputMap.end())
	{
		AosRecordsetObjPtr rs = AosRecordsetObj::createStreamRecordsetStatic(rdata, rcd);
		rdd = OmnNew AosRDD("", mName, "", mService->getId(), 0, rs, rdata);
		mCurOutputMap[outputName] = rdd;
	}
	else
	{
		rdd = itr->second;
	}
	aos_assert_r(rdd, false);
	bool rslt = rdd->appendRecord(rdata, rcd);
	aos_assert_r(rslt, false);
	return true;
}


/*
bool
AosStreamDataProc::addDataField(
								 AosConf::DataRecordFixbin &dr,
								 const OmnString &name,
								 const OmnString &type,
								 const OmnString &shortplc,
								 const u32 offset,
								 const u32 len
								 )
{
	boost::shared_ptr<AosConf::DataField> df = boost::make_shared<AosConf::DataField>();

	df = boost::make_shared<AosConf::DataField>();
	df->setAttribute("zky_name", name);
	df->setAttribute("type", type);

	if (shortplc != "")
		df->setAttribute("zky_datatooshortplc", shortplc);
	df->setAttribute("zky_offset", offset);
	df->setAttribute("zky_length", len);

	dr.setField(df);
	return true;
}


bool
AosStreamDataProc::isStreaming(JSONValue json)
{
	if(!json)
		return false;

	aos_assert_r(json, false);

	bool streamingFlag = false; 
	if (json.isMember("streaming"))
	{
		streamingFlag = json["streaming"].asBool();
	}

	return streamingFlag;
}
*/

////////////////////////////////////////////////////////////
//    Data sort, merge, ..... methods
////////////////////////////////////////////////////////////
bool
AosStreamDataProc::addComp(const OmnString &outputName, const AosXmlTagPtr &compDoc)
{
	AosCompareFunPtr comp;

	comp = AosCompareFun::getCompareFunc(compDoc);
	mCompMap[outputName] = comp;
	return true;
}

bool
AosStreamDataProc::addComp(const OmnString &outputName, const boost::shared_ptr<AosConf::CompFun> &confComp)
{
	//translate into AosCompFun type
	AosXmlTagPtr compDoc;

	compDoc = AosStr2Xml(mRundata.getPtr(), confComp->getConfig() AosMemoryCheckerArgs);
	return addComp(outputName, compDoc);
}

bool
AosStreamDataProc::procData(AosRundata* rdata, const AosRDDPtr &rdd)
{
	AosRecordsetObjPtr rs = rdd->getData();
	AosRecordsetObj* rs_raw = rs.getPtr();

	AosDataRecordObj* rcd = 0;
	rs_raw->resetOffset();

	start(rdata);

	/*
	if (mInputRecordWithDocid)
	{
		rs_raw->replaceRawRecord(mInputRecordWithDocid);
	}
	*/

	for(int i=0; i<rs_raw->size(); i++)
	{
		rs_raw->nextRecord(rdata, rcd);
		if(!rcd)
		{
			break;
		}
		procData(rdata, &rcd, NULL);
	}	

	finish(rdata);
	return true;

}



AosDataProcStatus::E 
AosStreamDataProc::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_record,
		AosDataRecordObj **output_record)
{
	OmnShouldNeverComeHere;
	return AosDataProcStatus::eContinue;
}

bool
AosStreamDataProc::startRDD(
					const AosRDDPtr &rdd, 
					const AosRundataPtr &rdata)
{
	AosTransPtr trans = OmnNew AosUpdateRDDStatusTrans(
			AosTaskStatus::eStart, 
			mService->getId(), 
			rdd->getDataId(), 
			mName,
			rdd->getRDDId(), 
			0, 
			0);
	bool timeout = false;
	AosBuffPtr resp;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt && resp && resp->getU8(0), false);
	return true;
}

bool
AosStreamDataProc::finishRDD(
					const AosRDDPtr &rdd, 
					const int remain,
					const AosRundataPtr &rdata)
{
	AosTransPtr trans = OmnNew AosUpdateRDDStatusTrans(
			AosTaskStatus::eFinish, 
			mService->getId(), 
			rdd->getDataId(), 
			mName,
			rdd->getRDDId(), 
			remain, 
			0);
	bool timeout = false;
	AosBuffPtr resp;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt && resp && resp->getU8(0), false);
	return true;
}
