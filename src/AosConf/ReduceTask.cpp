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
#include "AosConf/ReduceTask.h"
using AosConf::ReduceTask;


ReduceTask::ReduceTask()
{
}

ReduceTask::~ReduceTask()
{
}


string	
ReduceTask::getConfig()
{
	// 	This is "ReduceTask" format
	//	<reduce_task>
	//		<actions>
	//			<action>
	//				<dataproc>...</dataproc>
	//				<dataproc>...</dataproc>
	//				<dataproc>...</dataproc>
	//				...
	//			</action>
	//		</actions>
	//	</reduce_task>
	
	// 1. ReduceTask node 
	string config = "<reduce_task";
	for (map<string, string>::iterator itr=mAttrs.begin(); 
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	config += ">";

	// 2. actions node
	config += "<actions>";
	for (size_t i=0; i<mDataProcs.size(); i++)
	{
		string dataproc_conf = mDataProcs[i]->getConfig();
		config += dataproc_conf;
	}

	if (mDataProcsConf != "") 
	{
		config += mDataProcsConf;	
	}

	config += "</actions>";

	config += "</reduce_task>";
	return config;
}

void	
ReduceTask::setDataProc(const boost::shared_ptr<DataProc> &dataproc)
{
	mDataProcs.push_back(dataproc);
}


void 
ReduceTask::setDataProc(const string &dpconf)
{
	mDataProcsConf += dpconf;
}
