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
#include "StreamEngine/JqlService.h"
#include "StreamEngine/Service.h"

#include "SEInterfaces/GenericObjMgrObj.h" 
//#include "SEInterfaces/JimoProgObj.h" 
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
#include "Util/StrSplit.h"
#include <boost/make_shared.hpp>
#include "StreamEngine/RecordsetStream.h"
#include "StreamEngine/Ptrs.h"
#include "StreamEngine/ServiceMgr.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

/////////////////////////////////////////////////////
//   constructor/destructor/config
/////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJqlService_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJqlService(version);
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

AosJqlService::AosJqlService(const int ver)
:AosGenericObj(ver)
{
	mId = "";

	//register the trans for now. Later on, change to jimocall
	AosSendStreamDataTrans::registerSelf();
	
	//clear
	mDataProcMap.clear();
	mDataProcObjMap.clear();
	mParamMap.clear();
}


AosJqlService::~AosJqlService()
{

}

AosJimoPtr 
AosJqlService::cloneJimo() const
{
	try 
	{
		return OmnNew AosJqlService(*this);
	}
	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}

	return NULL;
}


//////////////////////////////////////////////
//      JQL methods
//////////////////////////////////////////////

//
//create the service doc: wrap up the json params with xml root tags
//
//
//<service name="xxx" objid="xxx">
//    <![CDATA[
//    	{	
//    		"params"   : [ "$ds1", "$keylist", "$statid" ],
//    		
//			"dataflow" :  {	
//					"$ds1" : [ "dp1.in1", "dp2.in1" ], 
//					"dp1.out1" : [ "dp2.in2" ],
//					"dp1.out2" : [ "dp3.in1" ],
//					"dp2.out1" : [ "dp3.in2" ]	
//			 },
//
// 			"running": {
//					"dp1" : {
// 						"type" : "dataprocindex",
//					 },
// 
//					 "dp2" : {
//						"type" : "dataprociilbatchopr",
//						"keys"  : "$keylist"
//					 },
//
// 					 "dp3" : { 
//						"type" : "dataprocdocbatchopr",
// 						"stat_objid"  : "$statid"
// 					 },
// 			}
//    	}]]>
//</service>
//
bool
AosJqlService::createByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	OmnScreen << "Start to create service: " << objName << endl;
	
	mName = objName;
	try
	{
		AosJimoProgObjPtr jimo_prog;

		if (prog)
			jimo_prog = prog;
		else
			jimo_prog = Jimo::jimoCreateJimoProg(rdata, "AosJimoProgService", 1);
		aos_assert_r(jimo_prog, false);


		JSONReader reader;
		JSONValue jsonObj;
		reader.parse(jsonstr, jsonObj);
	
		if (jsonObj["running"].isNull())
		{
			OmnAlarm << "service has no running" << enderr;
			return false;
		}

		if (jsonObj["datasets"].isNull())
		{
			OmnAlarm << "service has no datasets" << enderr;
			return false;
		}

		OmnString datasets_str;
		OmnString dataprocs_str;
		JSONValue ds = jsonObj["datasets"];
		OmnString objid;
		AosXmlTagPtr doc;
		for(u32 i=0; i< ds.size(); i++)
		{
			objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDatasetDoc, ds[i].asString());
			aos_assert_r(objid != "", false);

			doc = jimo_prog->getLogicDoc(rdata, objid);
			if (!doc)
			{
				doc = AosGetDocByObjid(objid, rdata);
			}
			aos_assert_r(doc, false);

			datasets_str << doc->toString();
		}

		bool rslt = createDataProcDoc(objName, jsonObj, dataprocs_str, jimo_prog, rdata);
		aos_assert_r(rslt, false);
		
		objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, objName);
		OmnString dp_str = "";
		dp_str 
			<< "<service " << AOSTAG_CTNR_PUBLIC << "=\"true\" "
			<< 		AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\">"
			<< 			"<jsonstr><![CDATA[" << jsonstr << "]]></jsonstr>"
			<<			"<dataprocs>" << dataprocs_str << "</dataprocs>"
			<< 			"<datasets>"  << datasets_str << "</datasets>"
			<< "</service>";

		return AosCreateDoc(dp_str, true, rdata);
	}
	catch (...)
	{
		OmnScreen << "JSONException... " << endl;
	}

	return false;
}

//
//run a Service with provided json params
//run service t1_service 
//{
//  "id"	:  "load_t1",
//	"$ds1"  :   "ds_t1",
//	"$statid" :  "db_t1_stat_t1_stat_model_internal_0",
//	"$keylist" : ["k1", "k2", "k3", "k4" ]
//}
//
bool
AosJqlService::runByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr)
{
	JSONValue jsonRunning;
	JSONValue jsonService;
	JSONReader reader;
	u64 interval = 0;
	u64 volume = 10000000;

	//set mName
	mName = objName;

	//get running parameters
	bool rslt = reader.parse(jsonstr, jsonRunning);
	aos_assert_r(rslt, false);
	FastWriter writer;             
	
	try {
		//save running parameters into a map
		Value::Members members = jsonRunning.getMemberNames();
		std::string name;
		for (u32 i = 0; i < members.size(); i++)
		{
			name = members[i];
			if (name == "running_name")
			{
				//instance id is not part of the parms
				mId = jsonRunning[name].asString();
				continue;
			}
			else if(name == "interval")
			{
				//time interval
				interval = jsonRunning[name].asInt();
			}
			else if(name == "volume")
			{
				//cache data
				volume = jsonRunning[name].asInt();
				aos_assert_r(volume > 0, false);
			}
			mParamMap[name] = jsonRunning[name];
		}

		rslt = createServiceDoc(objName, rdata);
		aos_assert_r(rslt, false);
		//create an service instance doc to hold service-level information
		//including snapshot, RDD monitoring info, etc
		//
		//Next time run the service with the same Id, there is no need to
		//create the instance again
		OmnString svrRunningStr = "";
		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, mId);
		svrRunningStr	
			<< "<serviceRunning " << AOSTAG_CTNR_PUBLIC << "=\"true\" "
			<< "zky_objname = " <<"\"" << objName << "_" << mId << "\" "
			<< "zky_interval = " <<"\"" << interval << "\" "
			<< "zky_volume = " <<"\"" << volume << "\" "
			<< AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\">"
			<< "<![CDATA[" << jsonstr << "]]>"
			<< "</serviceRunning>";

		AosXmlTagPtr conf = AosGetDocByObjid(objid, rdata);
		if (conf)
		{
			//The service running doc already exists, no need to create again
		}
		else
		{
			//create a new one
			aos_assert_r(AosCreateDoc(svrRunningStr, true, rdata), false);
		}

		//get the running docid
		conf = AosGetDocByObjid(objid, rdata);
		aos_assert_r(conf, false);
		mDocid = conf->getAttrU64(AOSTAG_DOCID, 0);
		
		//create doc once and start service
		//rslt = createDataProcDoc(objName, rdata);
		//aos_assert_r(rslt, false);
		AosServiceMgrObj::addServiceStatic(mDocid,rdata);

	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
    return true;
}


bool
AosJqlService::showByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr)
{
	bool rslt = AosServiceMgr::getSelf()->getAllServiceInfo(rdata);
    aos_assert_r(rslt, false);
	return rslt;
}

bool
AosJqlService::stopByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr)
{
	return true;
}
	
bool
AosJqlService::dropByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr)
{
	return true;
}

bool
AosJqlService::createServiceDoc(
		const OmnString &objName,
		AosRundata *rdata)
{
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, objName);
	aos_assert_r(objid != "", false);
	
	AosXmlTagPtr serviceConf = AosGetDocByObjid(objid, rdata);
	aos_assert_r(serviceConf, false);

	AosXmlTagPtr cloneConf = serviceConf->clone(AosMemoryCheckerArgsBegin);

	OmnString newName = objName;
	newName << "_" << mId;

	objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, newName);
	aos_assert_r(objid != "", false);

	cloneConf->setAttr(AOSTAG_OBJID, objid);
	cloneConf->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	cloneConf->setAttr(AOSTAG_PUBLIC_DOC, "true");
	cloneConf->removeAttr(AOSTAG_DOCID);

	AosXmlTagPtr jsonConf = cloneConf->getFirstChild("jsonstr");
	aos_assert_r(jsonConf, false);
	
	OmnString jsonstr = jsonConf->getNodeText();
	JSONReader reader;
	JSONValue jsonObj;
	
	reader.parse(jsonstr, jsonObj);
	JSONValue jsonDataProcs;                                          

	std::string name;
	map<OmnString, JSONValue> DataProcMap;
	map<OmnString, OmnStringList> DataflowMap;
	bool rslt;
	
	//config dataflow
	JSONValue jsonDataflow;
	rslt = replaceJsonParams(jsonObj["dataflow"], jsonDataflow);
	aos_assert_r(rslt, false);

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

		DataflowMap[name] = dataList; 
		dataList.clear();
	}
	
	rslt = replaceJsonParams(jsonObj["running"], jsonDataProcs);                
	aos_assert_r(rslt, false);          

	Value::Members dpMembers = jsonDataProcs.getMemberNames();
	for (u32 i = 0; i < jsonDataProcs.size(); i++)            
	{                                                         
		name = dpMembers[i];                                  
		DataProcMap[name] = jsonDataProcs[name];             
	} 

	//update input and output
	rslt = updateDataProcInputOutput(DataProcMap, DataflowMap);
	aos_assert_r(rslt, false);
	
	//create dataprocs
	AosGenericObjMgrObjPtr mgr = AosGenericObjMgrObj::getSelf(rdata);
	map<OmnString, JSONValue>::iterator itr; 
	OmnString dpName;
	OmnString fullName;
	JSONValue dpJson;
	OmnString dpType;
	OmnString dpObjid, dpJsonStr;
	AosXmlTagPtr conf;
	rslt = false;

	AosXmlTagPtr dataProcsConf = cloneConf->getFirstChild("dataprocs");
	aos_assert_r(dataProcsConf, false);

	AosXmlTagPtr dpConf = dataProcsConf->getFirstChild();
	while(dpConf)
	{
		conf = dpConf->getFirstChild("dataproc");
		aos_assert_r(conf, false);

		dpName = conf->getAttrStr("zky_dataproc_name");
		aos_assert_r(dpName != "", false);

		itr = DataProcMap.find(dpName);
		aos_assert_r(itr != DataProcMap.end(), false);
	
		fullName = mId;
		fullName << "_" << dpName;

		dpJson = itr->second;
		dpJson["streaming"] = "true";
		FastWriter writer;
		dpJsonStr = writer.write(dpJson);
		
		conf->setAttr(AOSTAG_NAME, fullName);
		conf->setNodeText(dpJsonStr, true);
	
		dpConf = dataProcsConf->getNextChild();
	}
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();   
	aos_assert_r(docclient, 0);
	AosXmlTagPtr doc = docclient->getDocByObjid(objid, rdata);
	if(doc)
		return true;
	else
		return AosCreateDoc(cloneConf->toString(), true, rdata);


}

bool
AosJqlService::createDataProcDoc(
		const OmnString &objName,
		JSONValue json,
		OmnString &dataprocs_str,
		const AosJimoProgObjPtr &prog,
		AosRundata *rdata) 
{
	try{
		std::string name;
		map<OmnString, JSONValue> DataProcMap;
		map<OmnString, OmnStringList> DataflowMap;
		bool rslt;
/*
		//config dataflow
		JSONValue jsonDataflow;
		rslt = replaceJsonParams(json["dataflow"], jsonDataflow);
		aos_assert_r(rslt, false);

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

			DataflowMap[name] = dataList; 
			dataList.clear();
		}

*/
		//config dataprocs
		JSONValue jsonDataProcs;                                          
		rslt = replaceJsonParams(json["running"], jsonDataProcs);                
		aos_assert_r(rslt, false);          

		Value::Members dpMembers = jsonDataProcs.getMemberNames();
		for (u32 i = 0; i < jsonDataProcs.size(); i++)            
		{                                                         
			name = dpMembers[i];                                  
			DataProcMap[name] = jsonDataProcs[name];             
		} 

/*
		//update input and output
		rslt = updateDataProcInputOutput(DataProcMap, DataflowMap);
		aos_assert_r(rslt, false);
*/
		//create dataprocs
		AosGenericObjMgrObjPtr mgr = AosGenericObjMgrObj::getSelf(rdata);
		map<OmnString, JSONValue>::iterator itr; 
		OmnString dpName;
		OmnString fullName;
		JSONValue dpJson;
		OmnString dpType;
		OmnString dpObjid;
		AosXmlTagPtr conf;
		rslt = false;

		//create all the dataproc objects 
		itr = DataProcMap.begin();
		while (itr != DataProcMap.end())
		{
			//get dataproc type from jsonValue
			dpName = itr->first;
			dpJson = DataProcMap[dpName];
			FastWriter writer;
			OmnScreen <<  "dpJson=" << writer.write(dpJson) << endl;
			dpType = dpJson["type"].asString();

			//if dataproc with the same name in the same scope
			//exist, no need to create a new one
			//fullName = mId;
			//fullName = objName;
			//fullName << "_" << dpName;
			fullName = dpName;
			dpObjid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, fullName);
			conf = prog->getLogicDoc(rdata, dpObjid);
			if (conf) 
			{
				itr++;
				continue;
			}

			//generate the dataproc doc firstly
			dpJson["streaming"] = "true";
			rslt = mgr->proc(rdata, prog, "create", 
				dpType, fullName, writer.write(dpJson));
			aos_assert_r(rslt, false);

			conf = prog->getLogicDoc(rdata, dpObjid);
			aos_assert_r(conf, false);

			AosXmlTagPtr dp = conf->getFirstChild("dataproc");
			aos_assert_r(dp, false);

			dp->setAttr("zky_dataproc_name", dpName);
			dataprocs_str << conf->toString();
			//for next dataproc
			itr++;
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
AosJqlService::createDataProcDoc(
		const OmnString &objName,
		AosRundata *rdata) 
{
	try{
		//config dataprocs
		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, objName);
		AosXmlTagPtr serviceConf = AosGetDocByObjid(objid, rdata);
		aos_assert_r(serviceConf, false);

		AosXmlTagPtr json_node = serviceConf->getFirstChild("jsonstr");
		aos_assert_r(json_node, false);
		OmnString dp_json = json_node->getNodeText(); 

		JSONValue json;                         
		JSONReader reader;                      
		std::string name;
		map<OmnString, JSONValue> DataProcMap;
		map<OmnString, OmnStringList> DataflowMap;
		bool rslt = reader.parse(dp_json, json);
		aos_assert_r(rslt, false);  


		//config dataflow
		JSONValue jsonDataflow;
		rslt = replaceJsonParams(json["dataflow"], jsonDataflow);
		aos_assert_r(rslt, false);

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

			DataflowMap[name] = dataList; 
			dataList.clear();
		}

		//config dataprocs
		JSONValue jsonDataProcs;                                          
		rslt = replaceJsonParams(json["running"], jsonDataProcs);                
		aos_assert_r(rslt, false);          

		Value::Members dpMembers = jsonDataProcs.getMemberNames();
		for (u32 i = 0; i < jsonDataProcs.size(); i++)            
		{                                                         
			name = dpMembers[i];                                  
			DataProcMap[name] = jsonDataProcs[name];             
		} 


		//update input and output
		rslt = updateDataProcInputOutput(DataProcMap, DataflowMap);
		aos_assert_r(rslt, false);

		//create dataprocs
		AosGenericObjMgrObjPtr mgr = AosGenericObjMgrObj::getSelf(rdata);
		map<OmnString, JSONValue>::iterator itr; 
		OmnString dpName;
		OmnString fullName;
		JSONValue dpJson;
		OmnString dpType;
		OmnString dpObjid;
		AosXmlTagPtr conf;
		rslt = false;

		//create all the dataproc objects 
		itr = DataProcMap.begin();
		while (itr != DataProcMap.end())
		{
			//get dataproc type from jsonValue
			dpName = itr->first;
			dpJson = DataProcMap[dpName];
			FastWriter writer;
			OmnScreen <<  "dpJson=" << writer.write(dpJson) << endl;
			dpType = dpJson["type"].asString();

			//if dataproc with the same name in the same scope
			//exist, no need to create a new one
			fullName = mId;
			fullName << "_" << dpName;
			dpObjid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, fullName);
			conf = AosGetDocByObjid(dpObjid, rdata);
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

        }
		catch (...)                                 
		{                                           
		    OmnScreen << "JSONException..." << endl;
		    return false;                           
		}
		return true;
	    
}


bool
AosJqlService::replaceJsonParams(
		JSONValue &json, 
		JSONValue &jsonNew)
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
AosJqlService::updateDataProcInputOutput(
		map<OmnString, JSONValue> &DataProcMap,   
		map<OmnString, OmnStringList> &DataflowMap)
{
	map<OmnString, JSONValue>::iterator dpItr;
	map<OmnString, OmnStringList>::iterator dfItr;
	map<OmnString, OmnString> dpInputMap;
	map<OmnString, OmnString> dpOutputMap;
		
	//init each dataproc's input out json string
	dpItr = DataProcMap.begin();
	OmnString dpName;
	while (dpItr != DataProcMap.end())
	{
		dpName = dpItr->first;
		dpInputMap[dpName] = "";
		dpOutputMap[dpName] = "";

		dpItr++;
	}

	//go throught the dataflow map to decide dataproc's input/output
	dfItr = DataflowMap.begin();
	OmnString dfLeft;
	OmnString fullDfLeft;  //dfLeft with full dataproc name
	OmnStringList dfRight;
	OmnString dfRightOne;
	while (dfItr != DataflowMap.end())
	{
		int len = 0;
		int idx = 0;
		dfLeft = dfItr->first;
		dfRight = dfItr->second;

		//handle dfLeft for dataproc's output
		fullDfLeft = dfLeft;
		if (dfLeft.indexOf(0, '.', true) >= 0)
		{
			len = dfLeft.length();
			idx = dfLeft.indexOf(0, '.', true);
			dpName = dfLeft.substr(0, idx-1);
			if (dpOutputMap[dpName] != "")
				dpOutputMap[dpName] << ", "; 

			dpOutputMap[dpName] << "\"" << dfLeft.substr(idx+1, len-1) << "\"";

			//fullDfLeft need to have service_id prepended to 
			//dataproc name
			fullDfLeft = "";
			fullDfLeft << mId << "_" << dfLeft;
			//fullDfLeft << mName << "_" << dfLeft;
		}

		//handle dfRight for dataproc's input 
		for (u32 i = 0; i < dfRight.size(); i++)
		{
			dfRightOne = dfRight[i];
			len = dfRightOne.length();
			idx = dfRightOne.indexOf(0, '.', true);
			//"." before
			dpName = dfRightOne.substr(0, idx-1);
			if (dpInputMap[dpName] != "")
				dpInputMap[dpName] << ", "; 

			dpInputMap[dpName] << "\"" << fullDfLeft << 
				"\" : \""  << dfRightOne.substr(idx+1, len-1) << "\"";
		}

		dfItr++;
	}

	//add start and end symbols
	//init each dataproc's input out json string
	dpItr = DataProcMap.begin();
	OmnString jsonStrInput;
	JSONValue jsonObjInput; 
	OmnString jsonStrOutput;
	JSONValue jsonObjOutput; 
	JSONReader reader;
	FastWriter writer;
	bool rslt;
	while (dpItr != DataProcMap.end())
	{
		dpName = dpItr->first;

		jsonStrInput = "";
		jsonStrInput << "{ " << dpInputMap[dpName] << " }";

		//jsonStr -> jsonValue;
		rslt = reader.parse(jsonStrInput, jsonObjInput);
		aos_assert_r(rslt, false);
		//insert jsonValue into dpItr->second 
		dpItr->second["inputListeners"] = jsonObjInput;

		jsonStrOutput = "";
		//jsonStr -> jsonValue;
		jsonStrOutput << "[ "<< dpOutputMap[dpName] << " ]";
		rslt = reader.parse(jsonStrOutput, jsonObjOutput);
		//insert jsonValue into dpItr->second 
		dpItr->second["outputSpeakers"] = jsonObjOutput;

		//print the new dataproc json str
		OmnScreen << "dataproc json =" << writer.write(dpItr->second) << endl;
		dpItr++;
	}


	return true;
}


