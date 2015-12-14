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
#ifndef Aos_AosConf_Job_h
#define Aos_AosConf_Job_h

#include "AosConf/JobTask.h"
#include "AosConf/AosConfig.h"

#include <map>
#include <vector>

namespace AosConf
{
class Job: public AosConf::AosConfBase
{
private:
	vector<boost::shared_ptr<JobTask> >	mTasks;
	map<string, string> 				mSchedulerAttr;
	vector<string>						mTaskNames;
	vector<string>						mTaskConfs;

public:
	Job();
	~Job();

	void	init();
	void	setJobTask(const string &jobTaskConf);
	void	setJobTask(const boost::shared_ptr<JobTask> &task);
	void 	setTaskName(const string &name);	
	void	setSchedulerAttr(const string &name, const string &value);
	string	getConfig();

};
}


#endif



