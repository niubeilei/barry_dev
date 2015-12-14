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
// 2015/01/07 Created by Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "JimoJob/JimoTask.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobMgrObj.h"
#include <string>

AosJimoTask::AosJimoTask(
	const JSONValue &jsonObj,
	AosJimoJob* job)
:
mJob(job)
{
	config(jsonObj);
}

AosJimoTask::AosJimoTask(
	const OmnString &jsonstr, 
	AosJimoJob* job)
:
mJob(job)
{
	init(jsonstr);
}

bool 
AosJimoTask::init(const OmnString &jsonstr)
{
	JSONValue jsonObj;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, jsonObj);
	aos_assert_r(rslt, false);
	return config(jsonObj);
}

void 
AosJimoTask::reset()
{
	mInputs.clear();
	mOutputs.clear();
	mDataEngine.reset();
}


bool 
AosJimoTask::config(const JSONValue &jsonObj)
{
	// Task's config
	// {
	// 		"name": "t1",
	// 		"dataengine_type": "scanengine",
	// 		"dataprocs": [
	//			{
	//				"dataproc": "dp1",
	//				"inputs": ["t1_ds"],
	//				"outputs": ["dp1.index"]
	//			},
	//			{
	//				"dataproc": "dp2",
	//				"inputs": "t1_ds"
	//			}
	// 		]
	// }
	//
	//  or
	//
	//	[
	//		"name": "t2",
	//		"dataengine_type": "dataengine_scan_multircd",
	//		"dataprocgoups": {
	//			"inputs": ["dp1_output"],
	//			"group": [
	//				{
	//					"reocrd_name": rcd1,
	//					"dataprocs": ["dp3", "dp4"]
	//				},
	//				{
	//					"reocrd_name": rcd2,
	//					"dataprocs": ["dp3", "dp4"]
	//				}
	//			],
	//			"outputs" : [dp3_ouput1,dp4_output1]
	//		}
	//	]


	try 
	{
		mTaskName = jsonObj["name"].asString();
		string engine_type = jsonObj["dataengine_type"].asString();
		mDataEngine.setType(engine_type);
		bool flag = jsonObj["multircd"].asBool();
		mDataEngine.isMultiRcd(flag);
		
		if (mDataEngine.isJoinEngine())
		{
			string jointype = jsonObj["join_type"].asString();
			mDataEngine.setAttr("join_type", jointype);

			string lstr = jsonObj["lhs_field"].asString();
			mDataEngine.setAttr("LHS", lstr);
			string rstr = jsonObj["rhs_field"].asString();
			mDataEngine.setAttr("RHS", rstr);

			JSONValue dataprocs = jsonObj["dataprocs"];
			bool rslt = configDataprocs(dataprocs);
			aos_assert_r(rslt, false);
		}
		else if (mDataEngine.isMultiRcd())
		{
			JSONValue dp = jsonObj["dataprocgoups"];
			bool rslt = configMultiRcdDataproc(dp);
			aos_assert_r(rslt, false);
		}
		else
		{
			JSONValue dataprocs = jsonObj["dataprocs"];
			bool rslt = configDataprocs(dataprocs);
			aos_assert_r(rslt, false);

		}
		return true;
	}
	catch (...)
	{
		OmnAlarm << enderr;
		return false;	
	}
}

bool
AosJimoTask::configMultiRcdDataproc(const JSONValue &dp)
{
	// dataprocs json struct like this:
	// "dataprocgoups": {
	//		"inputs": ["dp1_output"],
	//		"group": [
	//			{
	//				"reocrd_name": rcd1,
	//				"dataprocs": ["dp3", "dp4"]
	//			},
	//			{
	//				"reocrd_name": rcd2,
	//				"dataprocs": ["dp3", "dp4"]
	//			}
	//		],
	//		"outputs": ["dp3_ouput1", "dp4_output1"]
	//	}

	try
	{
		JSONValue group = dp["group"];
		mDataEngine.setGroup(group);

		JSONValue inputsJson = dp["inputs"]; 
		for (size_t j=0; j<inputsJson.size(); j++)
		{
			string inputname = inputsJson[j].asString();
			mInputs[inputname] = inputname;
		}

		JSONValue outputsJson = dp["outputs"];
		for (size_t k=0; k<outputsJson.size(); k++)
		{
			string outputname = outputsJson[k].asString();
			mOutputs[outputname] = outputname;
		}
	}
	catch(...)
	{
		OmnAlarm << __func__ << enderr;
		return false;
	}
	return true;
}

bool
AosJimoTask::configDataprocs(const JSONValue &dps)
{
	/* dataprocs json struct like this:
	 *
	 * [
	 *   {
	 *     "dataproc": "dp1",
	 *     "inputs": [
	 *       "t1_ds"
	 *     ],
	 *     "outputs": [
	 *       "dp1.index"
	 *     ]
	 *   },
	 *   {
	 *     "dataproc": "dp2",
	 *     "inputs": [
	 *       "t1_ds"
	 *     ],
	 *     "outputs": [
	 *       "dp1.index"
	 *     ]
	 *   }
	 * ]
	 *
	 */

	try
	{
		for (size_t i=0; i<dps.size(); i++)
		{
			OmnString dpname = dps[i]["dataproc"].asString();
			aos_assert_r(dpname != "", false);
			mDataEngine.appendDataProc(dpname);

			JSONValue inputsJson = dps[i]["inputs"];
			for (size_t j=0; j<inputsJson.size(); j++)
			{
				string inputname = inputsJson[j].asString();
				mInputs[inputname] = inputname;
			}
			try 
			{
				JSONValue outputsJson = dps[i]["outputs"];
				for (size_t k=0; k<outputsJson.size(); k++)
				{
					string outputname = outputsJson[k].asString();
					mOutputs[outputname] = outputname;
				}
			}
			catch (...)
			{
				OmnAlarm << __func__ << enderr;
				return false;
			}
		}
	}
	catch (...)
	{
		OmnAlarm << __func__ << enderr;
		return false;
	}
	return true;
}


OmnString 
AosJimoTask::getConfig(const AosRundataPtr &rdata)
{
	// 1. data engine node 
	OmnString dataengineConf = mDataEngine.getConfig(rdata);
	aos_assert_r(dataengineConf != "", "");

	// 3. output datasets node
	OmnString outputDatasetsConf = "";
	outputDatasetsConf << "<"  << AOSTAG_OUTPUT_DATASETS << ">";
	vector<AosXmlTagPtr> datasets = mDataEngine.getDatasets();
	map<OmnString, AosXmlTagPtr> datasetsMap;
	for (size_t i=0; i<datasets.size(); i++)
	{
		OmnString name = datasets[i]->getAttrStr("zky_name");
		aos_assert_r(name != "", "");
		datasetsMap[name] = datasets[i];
		outputDatasetsConf << datasets[i]->toString();
	}

	for (map<string, string>::iterator itr = mOutputs.begin();
			itr != mOutputs.end(); itr++)
	{
		if (datasetsMap.count(itr->first))
		{
			mJob->convertToInput(itr->first, datasetsMap[itr->first]);
		}
	}
	outputDatasetsConf << "</"  << AOSTAG_OUTPUT_DATASETS << ">";

	// 2. input datasets node
	// Get dataset from mDatasets. If not found, get doc
	// from db, then put config into mDatasets
	OmnString inputDatasetsConf = "";
	inputDatasetsConf << "<" << AOSTAG_INPUT_DATASETS_TAG << ">";
	for (map<OmnString, OmnString>::iterator itr = mInputs.begin(); 
			itr != mInputs.end(); itr++)
	{
		if (mJob->mDatasets.count(itr->first))
		{
			inputDatasetsConf << mJob->mDatasets[itr->first];   
		}
		else
		{
			// if not found, put dataset config into map
			OmnString objid = AosGenericObj::getObjid("dataset", itr->second);
			AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
			if (!doc)
			{
				AosSetErrorUser(rdata, __func__)
					<< "Input \"" << itr->first << "\" Is NULL." << enderr;
				OmnAlarm << __func__ << enderr;
				return "";
			}

			inputDatasetsConf << doc->toString();
			mJob->mDatasets[itr->first] = doc->toString(); 
		}
	}
	inputDatasetsConf << "</" << AOSTAG_INPUT_DATASETS_TAG << ">";

	// 4. construct task config
	OmnString conf = "";
	conf << "<" << AOSTAG_TASK_TAG << " zky_name=\"" << mTaskName << "\" type=\"map\">";
	conf << inputDatasetsConf << outputDatasetsConf << dataengineConf;
	conf << "</" << AOSTAG_TASK_TAG << ">"; 

	return conf;
}

