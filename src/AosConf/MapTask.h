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
#ifndef Aos_AosConf_MapTask_h
#define Aos_AosConf_MapTask_h

#include "AosConf/Task.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataProc.h"
#include "AosConf/DataEngine.h"

#include <map>
#include <vector>

namespace AosConf
{
class MapTask : public Task
{
private:
	boost::shared_ptr<DataEngine> 			mDataEngine;

public:
	MapTask();
	~MapTask();

	virtual string	getConfig();

	void	init();
	void	setDataEngine(const boost::shared_ptr<DataEngine> &dataengine);
};
}


#endif



