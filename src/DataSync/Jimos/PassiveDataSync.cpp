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
// A Passive Data Sync runs a thread. The thread periodically 
// wakes up to check whether the input dataset has changes. If 
// yes, it retrieves the changes and processes them.
//
// Modification History:
// 2014/01/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSync/Jimos/PassiveDataSync.h"

#include "API/AosApi.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosPassiveDataSync_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosPassiveDataSync(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosPassiveDataSync::AosPassiveDataSync(const int version)
:
AosDataset(AOSDATASYNC_PASSIVE, version)
{
}


AosPassiveDataSync::~AosPassiveDataSync()
{
}


bool
AosPassiveDataSync::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	// 	<conf ..>
	// 	</conf>

	return true;
}


