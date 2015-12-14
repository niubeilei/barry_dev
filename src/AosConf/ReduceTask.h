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
#ifndef Aos_AosConf_ReduceTask_h
#define Aos_AosConf_ReduceTask_h

#include "AosConf/Task.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataProc.h"

#include <map>
#include <vector>

namespace AosConf
{
class ReduceTask : public Task
{
private:
	vector<boost::shared_ptr<DataProc> >	mDataProcs;
	string 									mDataProcsConf;

public:
	ReduceTask();
	~ReduceTask();

	virtual string	getConfig();
	void setDataProc(const boost::shared_ptr<DataProc> &dataproc);
	void setDataProc(const string &dpconf); 

};
}


#endif



