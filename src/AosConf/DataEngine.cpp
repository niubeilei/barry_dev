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

#include "AosConf/DataEngine.h"

using AosConf::DataEngine;

void 
DataEngine::setDataProc(const boost::shared_ptr<DataProc> &proc) 
{
	mDataProcs.push_back(proc);
	setDataProc(proc->getConfig());
}

void 
DataEngine::setDataProc(const string &conf)
{
	mDataProcsConf.push_back(conf);
}
