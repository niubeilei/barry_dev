////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/02/15 Created by Phil
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/Service.h"

#include "SEInterfaces/GenericObjMgrObj.h" 
#include "AosConf/DataSet.h"
#include "DataProc/DataProc.h"
#include "JSON/JSON.h"
#include "JSON/value.h"
#include "API/AosApi.h"
#include "StreamEngine/StreamDataProc.h"
#include "DataRecord/Recordset.h"
#include "StreamEngine/RDD.h"
#include "StreamEngine/SendStreamDataTrans.h"
#include "StreamEngine/Ptrs.h"
#include "DataRecord/StreamRecordset.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include <boost/make_shared.hpp>
#include "StreamEngine/RecordsetStream.h"
#include "StreamEngine/Ptrs.h"
#include "StreamEngine/ServiceMgr.h"

/////////////////////////////////////////////////////
//   constructor/destructor/config
/////////////////////////////////////////////////////
AosService::AosService()
:
mDocid(0),
mId(""),
mName(""),
mLock(OmnNew OmnMutex())
{
	//register the trans for now. Later on, change to jimocall

	
	//clear
	mParamMap.clear();
	mDataProcMap.clear();
	mDataProcObjMap.clear();
	mDataflowMap.clear();
	mInputDataProcMap.clear();
	AosSendStreamDataTrans::registerSelf();
}


AosService::~AosService()
{

}

bool 
AosService::config(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &def)
{
	try {
		aos_assert_r(def, false);
		AosXmlTagPtr json_node = def->getFirstChild("jsonstr");
		aos_assert_r(json_node, false);
		OmnString dp_json = json_node->getNodeText();

		JSONValue json;
		JSONReader reader;
		std::string name;
		bool rslt = reader.parse(dp_json, json);
		aos_assert_r(rslt, false);

		OmnScreen << "service config json is: " << dp_json << endl;

		//check params section
		FastWriter writer;
		if(json.isMember("params"))
		{
			aos_assert_r(json.isMember("params"), false);
			mJsonParams = json["params"];
			OmnScreen << "paramsstr=" << writer.write(mJsonParams) << endl;
		}

		//replace dataflow parameters
		aos_assert_r(json.isMember("dataflow"), false);
		JSONValue jsonDataflow;
		OmnScreen << "dataflowstr=" << writer.write(json["dataflow"]) << endl;
		replaceJsonParams(json["dataflow"], jsonDataflow);
		OmnScreen << "dataflowstrnew=" << writer.write(jsonDataflow) << endl;

		//config dataflow section
		vector<OmnString> dataList;
		JSONValue jsonDataList;
		Value::Members dataMembers = jsonDataflow.getMemberNames();
		OmnString downStreamName;
		for (u32 i = 0; i < jsonDataflow.size(); i++)
		{
			name = dataMembers[i];
			jsonDataList = jsonDataflow[name]; 

			for (u32 j = 0; j < jsonDataList.size(); j++)
			{
				downStreamName = jsonDataList[j].asString();
				dataList.push_back(downStreamName);
			}

			mDataflowMap[name] = dataList; 
			dataList.clear();
		}
		
		AosXmlTagPtr dps = def->getFirstChild("dataprocs");
		aos_assert_r(dps, false);

		AosXmlTagPtr conf = dps->getFirstChild();
		AosXmlTagPtr dp;
		OmnString jsonstr, dpName;
		while(conf)
		{
			dp = conf->getFirstChild();
			aos_assert_r(dp, false);

			dpName = dp->getAttrStr("zky_dataproc_name");
			aos_assert_r(dpName != "", false);

			jsonstr = dp->getNodeText();
			
			JSONValue jsonRunning;                         
			JSONReader reader;                      
			rslt = reader.parse(jsonstr, jsonRunning);
			aos_assert_r(rslt, false); 

			mDataProcMap[dpName] = jsonRunning;
			conf = dps->getNextChild();
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
AosService::configParam(const AosRundataPtr &rdata,
		                const AosXmlTagPtr &def)
{
	try {                                       
	    aos_assert_r(def, false);               
		OmnString jsonStr = def->getNodeText(); 
				                                            
		JSONValue jsonRunning;                         
		JSONReader reader;                      
		std::string name;                       
    	bool rslt = reader.parse(jsonStr, jsonRunning);
		aos_assert_r(rslt, false); 

		FastWriter writer;
		Value::Members members = jsonRunning.getMemberNames();
            
		for (u32 i = 0; i < members.size(); i++)
		{                                       
			name = members[i];
			if (name == "running_name")               
			{                                         
			   //instance id is not part of the parms
			   mId = jsonRunning[name].asString();   
		       continue;                             
	        }                                         
			mParamMap[name] = jsonRunning[name];
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
AosService::runService(
		AosRundata *rdata,
		const u64 &serviceDocid)
{
	//get the service job configuration & config the service and param
	mDocid = serviceDocid;

	AosXmlTagPtr serviceConf = AosGetDocByDocid(serviceDocid, rdata);
    aos_assert_r(configParam(rdata,serviceConf),false);

	mName = serviceConf->getAttrStr("zky_objname");
	OmnString objid =  AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, mName);
	serviceConf =AosGetDocByObjid(objid, rdata);
	bool rslt = config(rdata, serviceConf);
	aos_assert_r(rslt, false);

	runDataProcs(serviceConf, rdata);

	AosServicePtr thisptr(this,false);
	AosServiceMgr::getSelf()->addService(mId, thisptr);
	return true;
}

////////////////////////////////////////////////////////////
//   Internal/helper methods
////////////////////////////////////////////////////////////
bool
AosService::replaceJsonParams(JSONValue &json, JSONValue &jsonNew)
{
	FastWriter writer;
	OmnString strjson=writer.write(json);
	OmnScreen << "strjson=" << strjson << endl;
	map<OmnString, JSONValue>::iterator it;

	for(it = mParamMap.begin(); it != mParamMap.end(); ++it){
		OmnString strKey = "";
		OmnString strValue = ""; 
		int index;

		strKey << "\"" << it->first << "\"";
		strValue = writer.write(it->second);
		index = strjson.findSubString(it->first,0);

		OmnScreen << "key=" << strKey << endl;
		OmnScreen << "value=" << strValue << endl;
		OmnScreen << "index=" << index << endl;
		if(index != -1)
		{
			strjson.replace(strKey,strValue,true);
			OmnScreen << "strjsonreplace=" << strjson << endl;
		}
	}

	JSONReader reader;
	bool rslt = reader.parse(strjson, jsonNew);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosService::createDataProcs(AosRundata* rdata)
{
	AosGenericObjMgrObjPtr mgr = AosGenericObjMgrObj::getSelf(rdata);
	map<OmnString, JSONValue>::iterator itr; 
	OmnString dpName;
	OmnString fullName;
	JSONValue dpJson;
	OmnString dpType;
	OmnString objid;
	AosXmlTagPtr conf;
	bool rslt;
	
	//create all the dataproc objects 
	itr = mDataProcMap.begin();
	while (itr != mDataProcMap.end())
	{
		//get dataproc type from jsonValue
		dpName = itr->first;
		dpJson = mDataProcMap[dpName];
		FastWriter writer;
		OmnScreen <<  "dpJson=" << writer.write(dpJson) << endl;
		dpType = dpJson["type"].asString();

		//if dataproc with the same name in the same scope
		//exist, no need to create a new one
		fullName = mId;
		fullName << "_" << dpName;
		objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, fullName);                                                                                
		conf = AosGetDocByObjid(objid, rdata);
		if (conf) 
		{
			itr++;
			continue;
		}

		//generate the dataproc doc firstly
		dpJson["streaming"] = "true";
		rslt = mgr->proc(rdata, 0, "create", 
				dpType, fullName, writer.write(dpJson));
		aos_assert_r(rslt, false);

		//for next dataproc
		itr++;
	}

	return true;
}

bool
AosService::runDataProcs(
		AosXmlTagPtr &service_conf, 
		AosRundata *rdata)
{
	aos_assert_r(service_conf, false);

	AosXmlTagPtr dataprocs = service_conf->getFirstChild("dataprocs");
	aos_assert_r(dataprocs, false);

	OmnString fullName;
	AosDataProcObjPtr dp;
	AosStreamDataProc *streamDP;
	AosXmlTagPtr dpConf;
	AosXmlTagPtr conf = dataprocs->getFirstChild();
	while (conf)
	{
		dpConf = conf->getFirstChild("dataproc");
		aos_assert_r(dpConf, false);

		fullName = dpConf->getAttrStr("zky_name");
		aos_assert_r(fullName != "", false);

		dp = AosDataProcObj::createDataProcStatic(dpConf, rdata);
		aos_assert_r(dp, 0);
		dp->setIsStreaming(true);

		AosXmlTagPtr dsConf = conf->getFirstChild("datasets");
		mDataProcObjMap[fullName] = dp;

		streamDP = dynamic_cast<AosStreamDataProc*>(dp.getPtr());
		streamDP->setService(this);
		streamDP->init(rdata, dpConf, dsConf);
		streamDP->run(rdata);
		
		conf = dataprocs->getNextChild();
	}
	return true;
}

bool
AosService::stopDataProcs(AosRundata *rdata)
{
	AosDataProcObjPtr dp;
	map<OmnString, AosDataProcObjPtr>::iterator itr; 

	itr = mDataProcObjMap.begin();
	while (itr != mDataProcObjMap.end())
	{
		dp = itr->second;
		dp->finish(rdata);
		
		itr++;
	}

	return true;
}


bool 
AosService::serviceDataArrived(
		const AosRundataPtr &rdata,
		const OmnString &dsName, 
		const AosRDDPtr &rdd)
{
	map<OmnString, vector<AosDataProcObj*> >::iterator itr;
	vector<AosDataProcObj*> dpList;
	AosStreamDataProc *dp;
	itr = mInputDataProcMap.find(dsName);
	if (itr != mInputDataProcMap.end())
	{
		dpList = (itr->second);
		for (u32 i = 0; i < dpList.size(); i++)
		{
			AosRDDPtr input_rdd = rdd;
			dp = (AosStreamDataProc*)(dpList)[i];
			if (dpList.size() > 1)
			{
				input_rdd = rdd->clone(rdata);
			}
			dp->inputHandler(input_rdd);
		}
	}

	return true;
}

//
//an RDD from upstream dataproc arrived
//
bool 
AosService::dataArrived(
		const AosRundataPtr &rdata,
		const OmnString &dsName, 
		const AosRDDPtr &rdd)
{
	//Jozhi 2015-08-31
	//this function may be there are mutil thread problem
	aos_assert_r(rdd, false);
	map<OmnString, AosServicePtr> mServiceMap = AosServiceMgr::getSelf()->getServiceMap();

	map<OmnString, AosServicePtr>::iterator itr;
	AosServicePtr service;
	OmnString serviceName = rdd->getServiceId();

	if (AosRecordsetStream::isDebug())
	{
		//if debug save the arrived data for tracing purpose
		AosRecordsetStream::addDebugData(dsName, rdd->getData());
	}

	if (serviceName.isEmpty())
	{
OmnScreen << "Alarm will be delete this case" << endl;
		//this means all the service need to consume thedata
		itr = mServiceMap.begin();
		while (itr != mServiceMap.end())
		{
			service = itr->second;
			aos_assert_r(service, false);
			service->serviceDataArrived(rdata, dsName, rdd);

			itr++;
		}

		return true;
	}

	itr = mServiceMap.find(serviceName);
	if (itr != mServiceMap.end())
	{
		service = itr->second;
		aos_assert_r(service, false);
		service->serviceDataArrived(rdata, dsName, rdd);
	}

	return true;
}

bool 
AosService::registerListeners(const OmnString &inputName, AosDataProcObj *dp)
{
	if (mInputDataProcMap.find(inputName) == mInputDataProcMap.end())
	{
		vector<AosDataProcObj*> vv;
		vv.push_back(dp);
		mInputDataProcMap[inputName] = vv;
	}
	else
	{
		mInputDataProcMap[inputName].push_back(dp);
	}
	return true;
}


bool 
AosService::findLastRemain(const OmnString &downstream_name, int &remain)
{
	map<OmnString, vector<AosDataProcObj*> >::iterator itr;
	vector<AosDataProcObj*> dpList;
	itr = mInputDataProcMap.find(downstream_name);
	if (itr != mInputDataProcMap.end())
	{
		dpList = (itr->second);
		for (u32 i = 0; i < dpList.size(); i++)
		{
			remain++;
			((AosStreamDataProc*)(dpList)[i])->numOutputs(remain);
		}
	}
	return true;
}

