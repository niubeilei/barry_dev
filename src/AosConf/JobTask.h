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
#ifndef Aos_AosConf_JobTask_h
#define Aos_AosConf_JobTask_h

#include "AosConf/Task.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataProc.h"
#include "AosConf/MapTask.h"
#include "AosConf/ReduceTask.h"
#include "AosConf/AosConfig.h"

#include <map>
#include <vector>
using AosConf::Task;

namespace AosConf
{
class JobTask: public AosConf::AosConfBase
{
private:
	vector<boost::shared_ptr<DataSet> >	mInputs;
	vector<boost::shared_ptr<DataSet> >	mOutputs;
	boost::shared_ptr<Task>				mMapTask;
	boost::shared_ptr<Task>				mReduceTask;
	string 								mInputDatasetStr;
	string								mOutputDatasetStr;
	string 								mMapTaskStr;
	string								mReduceTaskStr;
	string								mSplitType;

	vector<string>						mInputNames;
	vector<string>						mOutputNames;
	vector<string>						mDataProcNames;
	vector<string>						mActionNames;
	
public:
	JobTask();
	~JobTask();

	void	init();
	void 	setInputDataset(const boost::shared_ptr<DataSet> &dataset);
	void 	setOutputDataset(const boost::shared_ptr<DataSet> &dataset);
	void	setMapTask(const boost::shared_ptr<MapTask> maptask);
	void	setReduceTask(const boost::shared_ptr<ReduceTask> reducetask);
	void	setInputDataset(const string &str);
	void 	setOutputDataset(const string &str);
	void 	setMapTask(const string &str);
	void 	setReduceTask(const string &str);
	void 	setSplitType(const string &type){mSplitType = type;}

	void setInputName(const string &name){
		mInputNames.push_back(name);
	}

	void setOutputName(const string &name){
		mOutputNames.push_back(name);
	}

	void setDataProcName(const string &name){
		mDataProcNames.push_back(name);
	}

	void setActionName(const string &name){
		mActionNames.push_back(name);
	}

	string	getConfig();

};
}


#endif



