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
//
// Modification History:
// 09/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ProcMonitor/ProcMtrMgr.h"


AosProcMtrMgr::AosProcMtrMgr()
{
}


AosProcMtrMgr::~AosProcMtrMgr()
{
}


bool
AosProcMtrMgr::config(const AosXmlTagPtr &config)
{
	// 'config' should be in the form:
	// 	<monitored>
	// 		<proc .../>
	// 		...
	// 	</monitored>
	aos_assert_r(!config.isNull(), false);

	AosXmlTagPtr def = config->getFirstChild();
	mNumMonitored = 0;
	AosProcMtrMgrPtr thisptr(this, false);
	while (def && mNumMonitored < eMaxMonitored)
	{
		mMonitored[mNumMonitored] = OmnNew AosProcMonitor(def, thisptr);
		def = config->getNextChild();
	}
	return true;
}


bool
AosProcMtrMgr::monitoredFailed(const AosProcMonitorPtr &monitored)
{
	OmnNotImplementedYet;
	return false;
}


