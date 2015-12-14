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
// 2013/12/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Valueset/ValuesetRandom.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "Value/Value.h"
#include "XmlUtil/XmlTag.h"



AosValuesetRandom::AosValuesetRandom(const OmnString &version)
:
AosValueset(AOS_VALUESET_TYPE_RANDOM, version)
{
	init();
}


AosValuesetRandom::~AosValuesetRandom()
{
}


void
AosValuesetRandom::init()
{
}


bool
AosValuesetRandom::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	init();

	if (!worker_doc)
	{
		AosSetErrorUser(rdata, "valuesetrandom_missing_worker_doc") << enderr;
		return false;
	}

	return true;	
}


AosValue *
AosValuesetRandom::nextValue(const AosRundataPtr &rdata)
{
	// It randomly generates a value. 
	OmnNotImplementedYet;
	return 0;
}


bool
AosValuesetRandom::resetReadLoop()
{
	mReadLoop = 0;
	return true;
}


bool
AosValuesetRandom::setData(
		const AosRundataPtr &rdata, 
		const AosValuePtr &data)
{
	return true;
}


AosJimoPtr
AosValuesetRandom::cloneJimo() const
{
	try
	{
		return OmnNew AosValuesetRandom(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}

