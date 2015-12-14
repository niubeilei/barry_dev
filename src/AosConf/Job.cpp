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
#include "AosConf/Job.h"
#include <string>
using AosConf::Job;


Job::Job()
{
	init();
}

Job::~Job()
{
}


void
Job::init()
{
	mSchedulerAttr["zky_jobschedulerid"] = "norm";
	mSchedulerAttr["zky_numslots"] = "1";
}


void 
Job::setJobTask(const string &jobTaskConf)
{
	mTaskConfs.push_back(jobTaskConf);
}


void
Job::setJobTask(const boost::shared_ptr<JobTask> &task)
{
	mTasks.push_back(task);	
}

void
Job::setTaskName(const string &name)
{
	mTaskNames.push_back(name);
}

void
Job::setSchedulerAttr(const string &name, const string &value)
{
	mSchedulerAttr[name] = value;
}


string	
Job::getConfig()
{
	//	job format
	//	<job ...>
	//		<scheduler ...></scheduler>
	//		<tasks>
	//			<task>...</task>
	//			...
	//		</tasks>
	//	</job>

	// 1. job node 
	string config = "<job";
	for (map<string, string>::iterator itr=mAttrs.begin(); 
			itr!=mAttrs.end(); itr++) {
		config += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	config += ">";

	// 2. scheduler node 

	config += "<scheduler ";
	for (map<string, string>::iterator itr=mSchedulerAttr.begin(); 
			itr!=mSchedulerAttr.end(); itr++) {
		config += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	config += " />";


	// 3. tasks node 
	config += "<tasks>";
	for (size_t i=0; i<mTasks.size(); i++) {
		string task_conf = mTasks[i]->getConfig();
		config += task_conf;
	}

	for (size_t i=0; i<mTaskConfs.size(); i++)
	{
		config += mTaskConfs[i];
	}

	for (u32 i = 0; i < mTaskNames.size(); i++) {
		config += "<task zky_task_name=\"" + mTaskNames[i] + "\"/>";
	}
	config += "</tasks>";
	config += "</job>";	
	return config;
}

