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
#include "JimoJob/JimoDataEngine.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "JimoJob/JimoTask.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>


AosJimoDataEngine::AosJimoDataEngine()
:
mType("dataengine_scan_singlercd"),
mIsIILBatchOpr(false),
mIsMultiRcd(false)
{
}

bool 
AosJimoDataEngine::isValidType(const OmnString type)
{
	if (type == "dataengine_scan_singlercd") return true;
	else if (type == "dataengine_join") return true;	
	else if (type == "dataengine_join_new") return true;
	else if (type == "dataengine_scan2") return true;
	else if (type == "dataengine_join2") return true;	
	return false;
}

void
AosJimoDataEngine::reset() 
{
	mDataProcs.clear(); 
	mIsIILBatchOpr = false;
}


bool 
AosJimoDataEngine::isJoinEngine()
{
	if (mType == "dataengine_join" || mType == "dataengine_join_new" || mType == "dataengine_join2")
		return true;
	return false;
}

bool
AosJimoDataEngine::isMultiRcd()
{
	return mIsMultiRcd;
}

bool 
AosJimoDataEngine::isIILBatchOpr() 
{
	return mIsIILBatchOpr;
}

bool 
AosJimoDataEngine::setAttr(
		const string &name, 
		const string &value)
{
	if (name != "") mAttrs[name] = value;	
	return true;
}

bool 
AosJimoDataEngine::setType(const OmnString &type) 
{
	aos_assert_r(isValidType(type), false);
	setAttr("zky_type", type);
	mType = type;
	return true;
}


bool 
AosJimoDataEngine::setGroup(const JSONValue &group)
{
	mGroup = group;
	return true;
}

OmnString
AosJimoDataEngine::generateGroupConf(
		const AosRundataPtr &rdata, 
		const AosJimoProgObjPtr &prog)
{
	// mGroup json struct like this:
	//
	// [{                                    
	// 		"reocrd_name": rcd1,             
	//		"dataprocs": ["dp3", "dp4"]      
	// },                                   
	// {                                    
	//		"reocrd_name": rcd2,             
	//		"dataprocs": ["dp3", "dp4"]      
	// }]                                       
	//

	OmnString conf;
	try
	{
		conf << "<data_engine";
		for (mItr_t itr = mAttrs.begin(); itr != mAttrs.end(); itr++)
		{
			conf << " " << itr->first << "=\"" << itr->second << "\"";
		}
		conf << ">";

		JSONValue dps;
		OmnString record_name;
		vector<OmnString> data_procs;
		for (u32 i = 0 ; i < mGroup.size(); i++)
		{
			data_procs.clear();
			record_name = mGroup[i]["record_name"].asString();
			dps = mGroup[i]["dataprocs"];
			for (u32 k = 0; k < dps.size(); k++)        
			{                                                  
				data_procs.push_back(dps[k].asString());
			}
			conf << "<input_data_record zky_input_record_name=\"" << record_name << "\" >" 
				 << getDataProcsConf(rdata, data_procs, prog)
				 << "</input_data_record>";
		}
		conf << "</data_engine>";
		return conf;
	}
	catch(...)
	{
		OmnAlarm << __func__ << enderr;
		return conf;
	}
	return conf;
}

OmnString 
AosJimoDataEngine::getConfig(
		const AosRundataPtr &rdata, 
		const AosJimoProgObjPtr &prog)
{
	OmnString conf;

	if (isMultiRcd()) 
		return generateGroupConf(rdata, prog);

	if (mDataProcs.empty())
	{
		AosSetErrorUser(rdata, __func__) << "Not found DataProc" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	//conf << "<data_engine zky_type=\"" << mType << "\" " << ">";
	conf << "<data_engine";
	for (map<string, string>::iterator itr = mAttrs.begin(); 
			itr != mAttrs.end(); itr++)
	{
		conf << " " << itr->first << "=\"" << itr->second << "\"";
	}
	conf << ">" << getDataProcsConf(rdata, mDataProcs, prog) << "</data_engine>";
	return conf;
}

OmnString
AosJimoDataEngine::getDataProcsConf(
		const AosRundataPtr &rdata,
		vector<OmnString> &data_procs, 
		const AosJimoProgObjPtr &prog)
{
	aos_assert_r(prog, "");
	OmnString conf, objid;
	AosXmlTagPtr doc, datasets, dataset, dataproc;
	for (u32 i = 0; i < data_procs.size(); i++)
	{
		objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, data_procs[i]);
		//doc = AosGetDocByObjid(objid, rdata);
		doc = prog->getLogicDoc(rdata.getPtr(), objid);
		if (!doc)
		{
			OmnAlarm << __func__ << enderr;
			return "";
		}

		dataproc = doc->getFirstChild("dataproc");
		aos_assert_r(dataproc, "");

		datasets = doc->getFirstChild("datasets");
		if (datasets)
		{
			dataset = datasets->getFirstChild(true);
			while (dataset)
			{
				AosXmlTagPtr ds = dataset->clone(AosMemoryCheckerArgsBegin);
				mDataSets.push_back(ds);
				dataset = datasets->getNextChild();
			}
		}
		OmnString jimoObjid = dataproc->getAttrStr("jimo_objid");
		if (jimoObjid == "dataprociilbatchopr_jimodoc_v0") mIsIILBatchOpr = true; 
		conf << dataproc->toString();
	}
	return conf;
}



