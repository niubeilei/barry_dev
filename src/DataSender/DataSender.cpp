////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSender/DataSender.h"

#include "DataSender/DataSenderNorm.h"
#include "Debug/ExitHandler.h"
#include "Debug/Debug.h"

AosDataSender::AosDataSender(
		const OmnString &name, 
		const AosDataSenderType::E type,
		const bool regflag)
:
AosDataSenderObj(name, type, regflag)
{
}


AosDataSender::~AosDataSender()
{
}


bool
AosDataSender::init()
{
	static AosDataSenderNorm			lsDataSenderNorm(true);

	if (!AosDataSenderType::check())
	{
		OmnExitApp("Some data senders were not initialized");
		return false;
	}

	OmnScreen << "All data senders are initialized" << endl;
	return true;
}
