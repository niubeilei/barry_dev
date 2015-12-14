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
#include "AosConf/JobTask.h"

#include <string>
using AosConf::JobTask;


JobTask::JobTask()
{
	init();
}

JobTask::~JobTask()
{
}


void
JobTask::init()
{
}

string	
JobTask::getConfig()
{
	// 	This is "JobTask" format
	// 	<task ...>
	//		<input_datasets>...</input_datasets>
	//		<output_datasets>...</output_datasets>
	//		<map_task>...</map_task>
	//		<reduce_task>...</reduce_task>
	//	</task>
	
	// 1. task node 
	string config = "<task ";
	for (map<string, string>::iterator itr=mAttrs.begin(); 
			itr!=mAttrs.end(); itr++)
	{
		config += itr->first + "=\"" + itr->second +  "\" ";
	}
	config += ">";

	// 2. input_datasets node
	config += "<input_datasets>";
	if (mInputDatasetStr != "") 
	{
		config += mInputDatasetStr;
	}
	else
	{
		for (size_t i=0; i<mInputs.size(); i++)
		{
			string datasetconf = mInputs[i]->getConfig();
			config += datasetconf;
		}
	}
	if(mSplitType != "")
	{
		config += "<dataset_spliter split_type=\"" + mSplitType + "\"/>";
	}

	for (u32 i = 0; i < mInputNames.size(); i++) {
		config += "<input_dataset zky_dataset_name=\"" + mInputNames[i] + "\"/>";
	}

	config += "</input_datasets>";

	// 3. output_datasets node
	if (mOutputDatasetStr != "")
	{
		config += mOutputDatasetStr;
	}
	else
	{
		config += "<output_datasets>";
		for (size_t i=0; i<mOutputs.size(); i++)
		{
			string datasetconf = mOutputs[i]->getConfig();
			config += datasetconf;
		}

		for (u32 i = 0; i < mOutputNames.size(); i++) {
			config += "<output_dataset zky_dataset_name=\"" + mOutputNames[i] + "\"/>";
		}
		config += "</output_datasets>";
	}

	// 4. map_task node
	if (mMapTaskStr != "")
	{
		config += mMapTaskStr;
	}
	else if (!mDataProcNames.empty())
	{
		config += "<map_task>";
		config += "<actions>";
		config += "<action zky_type=\"dataengine_scan_singlercd\">";
		for (u32 i = 0; i < mDataProcNames.size(); i++) {
			config += "<dataproc zky_dataproc_name=\"" + mDataProcNames[i] + "\"/>";
		}
		config += "</action>";
		config += "</actions>";
		config += "</map_task>";
	}
	else
	{
		string map_conf = mMapTask->getConfig();
		config += map_conf;
	}

	// 5. reduce_task node
	if (mReduceTaskStr != "")
	{
		config += mReduceTaskStr;
	}
	else if (!mActionNames.empty())
	{
		config += "<reduce_task>";
		config += "<actions>";
		for (u32 i = 0; i < mActionNames.size(); i++) {
			config += "<dataproc zky_dataproc_name=\"" + mActionNames[i] + "\"/>";
		}
		config += "</actions>";
		config += "</reduce_task>";
	}
	else
	{
		string reduce_conf = mReduceTask->getConfig();
		config += reduce_conf;
	}
	
	config += "</task>";

	return config;
}


void	
JobTask::setMapTask(const boost::shared_ptr<MapTask> task)
{
	mMapTask = task;
}


void	
JobTask::setReduceTask(const boost::shared_ptr<ReduceTask> task)
{
	mReduceTask = task;
}


void 	
JobTask::setInputDataset(const boost::shared_ptr<DataSet> &dataset)
{
	mInputs.push_back(dataset);
}


void 	
JobTask::setOutputDataset(const boost::shared_ptr<DataSet> &dataset)
{
	mOutputs.push_back(dataset);
}
void	
JobTask::setInputDataset(const string &str)
{
	mInputDatasetStr = str;
}


void 	
JobTask::setOutputDataset(const string &str)
{
	mOutputDatasetStr += str;
}


void 	
JobTask::setMapTask(const string &str)
{
	mMapTaskStr += str;	
}


void 	
JobTask::setReduceTask(const string &str)
{
	mReduceTaskStr += str;
}
